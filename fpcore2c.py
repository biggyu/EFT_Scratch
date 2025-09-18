#!/usr/bin/env python3
# fpcore2c.py — very small FPCore → C translator for a common subset.
# Usage: python3 fpcore2c.py benchmark.fpcore > benchmark.c

import sys, re, math

# ---------- simple s-expr tokenizer & parser ----------
def tokenize(s):
    # keep parentheses, symbols, numbers; remove comments ;...
    s = re.sub(r";.*", "", s)
    tokens = re.findall(r'\(|\)|"(?:\\.|[^"])*"|[^\s()]+', s)
    return tokens

def parse(tokens):
    def _parse(i):
        tok = tokens[i]
        if tok == '(':
            lst = []
            i += 1
            while i < len(tokens) and tokens[i] != ')':
                node, i = _parse(i)
                lst.append(node)
            if i >= len(tokens) or tokens[i] != ')':
                raise SyntaxError("missing )")
            return lst, i+1
        elif tok == ')':
            raise SyntaxError("unexpected )")
        else:
            return tok, i+1
    ast, i = _parse(0)
    if i != len(tokens):
        raise SyntaxError("extra tokens")
    return ast

# ---------- AST → C code ----------
math_funcs = {
    'sqrt':'sqrt', 'exp':'exp', 'log':'log', 'sin':'sin', 'cos':'cos',
    'tan':'tan', 'asin':'asin', 'acos':'acos', 'atan':'atan',
    'sinh':'sinh', 'cosh':'cosh', 'tanh':'tanh', 'pow':'pow',
    'fabs':'fabs', 'abs':'fabs'
}
bin_ops = {'+':'+', '-':'-', '*':'*', '/':'/'}
cmp_ops = {'<':'<','>':'>','<=':'<=','>=':'>=','==':'==','!=':'!='}

def is_number(tok):
    try:
        float(tok)
        return True
    except:
        return False

def c_number(tok, double=True):
    # Preserve special constants if they appear (inf, nan)
    low = tok.lower()
    if low in ('inf','+inf'):
        return '(INFINITY)'
    if low in ('-inf'):
        return '(-INFINITY)'
    if low in ('nan'):
        return '(NAN)'
    # numeric literal with suffix
    if double:
        return tok if ('.' in tok or 'e' in low) else (tok + '.0')
    else:
        return tok  # we generate double anyway

def gensym(prefix=['t']):
    prefix[0] += '1'
    return prefix[0]

def emit_expr(e, env, lines):
    """
    Returns (c_expr_string, optional_decls_lines)
    env: dict for let-bound variables
    """
    # symbol
    if isinstance(e, str):
        if e in env: return env[e], []
        if is_number(e): return c_number(e), []
        # plain variable
        return e, []
    # list form
    if not e: raise ValueError("empty s-expr")
    head = e[0]

    # (let ((x e1) (y e2)) body)
    if head == 'let':
        _, bindings, body = e
        new_env = env.copy()
        decls = []
        for pair in bindings:
            if not (isinstance(pair, list) and len(pair)==2 and isinstance(pair[0], str)):
                raise ValueError("bad let binding")
            name = pair[0]
            c_rhs, d2 = emit_expr(pair[1], new_env, lines)
            decls += d2
            tmp = name
            decls += [f"double {tmp} = {c_rhs};"]
            new_env[name] = tmp
        c_body, d3 = emit_expr(body, new_env, lines)
        return c_body, decls + d3

    # (if cond then else)
    if head == 'if':
        _, cond, thenp, elsep = e
        c_cond, d1 = emit_expr(cond, env, lines)
        c_then, d2 = emit_expr(thenp, env, lines)
        c_else, d3 = emit_expr(elsep, env, lines)
        return f"(({c_cond}) ? ({c_then}) : ({c_else}))", d1 + d2 + d3

    # n-ary arithmetic: (+ a b c ...) → ((a+b)+c) …
    if head in bin_ops:
        op = bin_ops[head]
        if len(e) < 3:
            raise ValueError(f"{head} needs >=2 args")
        c_args = []
        decls = []
        for arg in e[1:]:
            ca, dd = emit_expr(arg, env, lines)
            c_args.append(f"({ca})")
            decls += dd
        # left-assoc fold
        expr = c_args[0]
        for a in c_args[1:]:
            expr = f"({expr} {op} {a})"
        return expr, decls

    # comparisons: we allow binary comparisons
    if head in cmp_ops:
        if len(e) != 3: raise ValueError(f"{head} needs 2 args")
        a, b = e[1], e[2]
        ca, d1 = emit_expr(a, env, lines)
        cb, d2 = emit_expr(b, env, lines)
        return f"(({ca}) {cmp_ops[head]} ({cb}))", d1 + d2

    # math functions
    if head in math_funcs:
        fname = math_funcs[head]
        args = e[1:]
        if len(args)==1:
            ca, d1 = emit_expr(args[0], env, lines)
            return f"{fname}({ca})", d1
        elif len(args)==2 and fname=='pow':
            ca, d1 = emit_expr(args[0], env, lines)
            cb, d2 = emit_expr(args[1], env, lines)
            return f"pow({ca}, {cb})", d1 + d2
        else:
            raise ValueError(f"unsupported arity for {head}")

    # constants like PI, E, etc. (if written as symbols)
    if isinstance(head, str) and head.upper() in ('PI','M_PI'):
        return 'M_PI', []

    # unknown head: treat as function application f(a,b,...)
    if isinstance(head, str):
        fname = head
        c_args = []
        decls = []
        for arg in e[1:]:
            ca, dd = emit_expr(arg, env, lines)
            c_args.append(ca)
            decls += dd
        return f"{fname}(" + ", ".join(c_args) + ")", decls

    raise ValueError(f"unsupported form: {e}")

def emit_function(form, idx):
    # form like: (FPCore (x y) body ...) ; may include :name :precision metadata
    if not form or form[0] != 'FPCore':
        raise ValueError("top-level must be FPCore")
    # parse header
    # FPCore (<args>) [metadata pairs]* body
    # We scan to find the first list after 'FPCore' which is the arg list
    if len(form) < 3 or not isinstance(form[1], list):
        raise ValueError("FPCore header malformed")
    args = [a for a in form[1] if isinstance(a, str)]
    # metadata + body
    meta_i = 2
    # collect :name if present
    name = f"fpcore_func_{idx}"
    while meta_i < len(form) - 1 and isinstance(form[meta_i], str) and form[meta_i].startswith(':'):
        key = form[meta_i]; val = form[meta_i+1]
        if key == ':name' and isinstance(val, str):
            # sanitize name to C identifier
            name = re.sub(r'[^a-zA-Z0-9_]', '_', val)
        meta_i += 2
    body = form[meta_i]

    # generate C body
    lines = []
    env = {}
    c_expr, decls = emit_expr(body, env, lines)

    # signature: all doubles
    sig_args = ", ".join(f"double {a}" for a in args)
    fun = []
    fun.append("#include <math.h>")
    fun.append("")
    fun.append(f"double {name}({sig_args})" + " {")
    for d in decls:
        fun.append(f"  {d}")
    fun.append(f"  return ({c_expr});")
    fun.append("}")
    return "\n".join(fun), name, args

def main():
    if len(sys.argv) != 2:
        print("usage: fpcore2c.py <file.fpcore>", file=sys.stderr)
        sys.exit(1)
    src = open(sys.argv[1], 'r', encoding='utf-8').read()
    toks = tokenize(src)
    ast = parse(toks)

    # The file may contain one or more FPCore forms. Normalize to list.
    forms = ast if isinstance(ast, list) and ast and ast[0] != 'FPCore' else [ast]
    out = []
    exported = []
    idx = 0
    for form in forms:
        if isinstance(form, list) and form and form[0] == 'FPCore':
            code, name, args = emit_function(form, idx)
            out.append(code)
            exported.append((name, args))
            idx += 1

    # Emit all functions + a tiny test main (optional)
    print("// Generated by fpcore2c.py")
    print("\n\n".join(out))

    # Optional main for quick smoke (comment out if you don’t want it)
    if exported:
        name, args = exported[0]
        call_args = ", ".join("0.0" for _ in args)
        print("\n#ifdef FPCORE_MAIN\n#include <stdio.h>\nint main(){")
        print(f"  printf(\"%0.17g\\n\", {name}({call_args}));")
        print("  return 0;\n}\n#endif")

if __name__ == '__main__':
    main()
