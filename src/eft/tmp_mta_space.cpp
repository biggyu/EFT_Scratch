#include "eft/tmp_mta_space.hpp"

// void TwoSum(double a, double b, double& x, double& dx) {
//     x = a + b;
//     double bp = x - a;
//     double ap = a - bp;
//     double da = a - ap;
//     double db = b - bp;
//     dx = da + db;
// }

// void PropSumError(double a, double da, double b, double db, double& x, double& dx) {
//     TwoSum(a, b, x, dx);
//     dx = dx + da + db;
// }

// void TwoProd(double a, double b, double& x, double& dx) {
//     x = a * b;
//     dx = std::fma(a, b, -x);
// }

// void PropProdError(double a, double da, double b, double db, double& x, double& dx) {
//     TwoProd(a, b, x, dx);
//     dx = dx + a * db + b * da;
// }

// void TwoDiv(double a, double b, double& x, double& dx) {
//     x = a / b;
//     dx = std::fma(x, b, -a);
// }

// void PropDivError(double a, double da, double b, double db, double& x, double& dx) {
//     TwoDiv(a, b, x, dx);
//     dx = (da - dx - x * db) / (b + db);
// }

// TmpMtaSpc::TmpMtaSpc(size_t cap_ = 256) {
//     cap = cap_;
//     circ_queue = new fp_entry[cap_];
//     head = 0;
//     global_ts = 0;
// }

// TmpMtaSpc::~TmpMtaSpc() {
//     delete[] circ_queue;
// }

// fp_entry* TmpMtaSpc::alloc() {
//     fp_entry* t = &circ_queue[head];
//     head = (head + 1) % cap;
//     t->timestamp = ++global_ts;
//     return t;
// }
// void TmpMtaSpc::inc_ts() {
//     global_ts++;
// }

// size_t TmpMtaSpc::get_ts() {
//     return global_ts;
// }
namespace eft {
size_t inst_id_hash(const char* file, int line, const char* func) {
    string key = string(file) + ":" + to_string(line) + ":" + func;
    return std::hash<std::string>{}(key);
}

TempContext::TempContext(ShadowMemory* s, size_t cap_=256) : smem(s) {
    cap = cap_;
    circ_queue = new fp_entry[cap_];
    head = 0;
    global_ts = 0;
}

TempContext::~TempContext() {
    delete[] circ_queue;
}

fp_entry* TempContext::alloc() {
    fp_entry* t = &circ_queue[head];
    head = (head + 1) % cap;
    t->timestamp = ++global_ts;
    return t;
}

void TempContext::inc_ts() {
    global_ts++;
}

// fp_entry* TempContext::t_const(float& program_value, size_t site_id, size_t linenum) {
//     fp_entry* t = alloc();
//     t->error = 0.0;
//     t->value = static_cast<double>(program_value);
//     t->lhs = nullptr;
//     t->rhs = nullptr;
//     t->opcode = fp_op::INIT;
//     t->linenum = linenum;
//     t->static_id = site_id;
//     lwm[site_id] = {t, t->timestamp};
//     return t;
// }

fp_entry* TempContext::t_add(fp_entry* a, fp_entry* b, size_t site_id, size_t linenum) {
    fp_entry* z = alloc();
    lwrm_value a_lwm = lwm[a->static_id];
    lwrm_value b_lwm = lwm[b->static_id];
    double a_error, b_error, x, dx;
    if(a_lwm.ts == a_lwm.addr->timestamp) {
        z->lhs = a_lwm.addr;
        a_error = a_lwm.addr->error;
    }
    else {
        z->lhs = nullptr;
        a_error = 0.0;
    }
    if(b_lwm.ts == b_lwm.addr->timestamp) {
        z->rhs = b_lwm.addr;
        b_error = b_lwm.addr->error;
    }
    else {
        z->rhs = nullptr;
        b_error = 0.0;
    }
    PropSumError(a->value, a_error, b->value, b_error, x, dx);
    z->value = x;
    z->error = dx;
    z->opcode = fp_op::ADD;
    z->linenum = linenum;
    z->static_id = site_id;
    lwm[site_id] = {z, z->timestamp};
    return z;
}

fp_entry* TempContext::t_sub(fp_entry* a, fp_entry* b, size_t site_id, size_t linenum) {
    fp_entry* z = alloc();
    lwrm_value a_lwm = lwm[a->static_id];
    lwrm_value b_lwm = lwm[b->static_id];
    double a_error, b_error, x, dx;
    if(a_lwm.ts == a_lwm.addr->timestamp) {
        z->lhs = a_lwm.addr;
        a_error = a_lwm.addr->error;
    }
    else {
        z->lhs = nullptr;
        a_error = 0.0;
    }
    if(b_lwm.ts == b_lwm.addr->timestamp) {
        z->rhs = b_lwm.addr;
        b_error = b_lwm.addr->error;
    }
    else {
        z->rhs = nullptr;
        b_error = 0.0;
    }
    PropSumError(a->value, a_error, -b->value, b_error, x, dx);
    z->value = x;
    z->error = dx;
    z->opcode = fp_op::SUB;
    z->linenum = linenum;
    z->static_id = site_id;
    lwm[site_id] = {z, z->timestamp};
    return z;
}

fp_entry* TempContext::t_mul(fp_entry* a, fp_entry* b, size_t site_id, size_t linenum) {
    fp_entry* z = alloc();
    lwrm_value a_lwm = lwm[a->static_id];
    lwrm_value b_lwm = lwm[b->static_id];
    double a_error, b_error, x, dx;
    if(a_lwm.ts == a_lwm.addr->timestamp) {
        z->lhs = a_lwm.addr;
        a_error = a_lwm.addr->error;
    }
    else {
        z->lhs = nullptr;
        a_error = 0.0;
    }
    if(b_lwm.ts == b_lwm.addr->timestamp) {
        z->rhs = b_lwm.addr;
        b_error = b_lwm.addr->error;
    }
    else {
        z->rhs = nullptr;
        b_error = 0.0;
    }
    PropProdError(a->value, a_error, b->value, b_error, x, dx);
    z->value = x;
    z->error = dx;
    z->opcode = fp_op::MUL;
    z->linenum = linenum;
    z->static_id = site_id;
    lwm[site_id] = {z, z->timestamp};
    return z;
}

fp_entry* TempContext::t_div(fp_entry* a, fp_entry* b, size_t site_id, size_t linenum) {
    fp_entry* z = alloc();
    lwrm_value a_lwm = lwm[a->static_id];
    lwrm_value b_lwm = lwm[b->static_id];
    double a_error, b_error, x, dx;
    if(a_lwm.ts == a_lwm.addr->timestamp) {
        z->lhs = a_lwm.addr;
        a_error = a_lwm.addr->error;
    }
    else {
        z->lhs = nullptr;
        a_error = 0.0;
    }
    if(b_lwm.ts == b_lwm.addr->timestamp) {
        z->rhs = b_lwm.addr;
        b_error = b_lwm.addr->error;
    }
    else {
        z->rhs = nullptr;
        b_error = 0.0;
    }
    PropDivError(a->value, a_error, b->value, b_error, x, dx);
    z->value = x;
    z->error = dx;
    z->opcode = fp_op::DIV;
    z->linenum = linenum;
    z->static_id = site_id;
    lwm[site_id] = {z, z->timestamp};
    return z;
}

fp_entry* TempContext::t_sqrt(fp_entry* a, size_t site_id, size_t linenum) {
    fp_entry* z = alloc();
    lwrm_value a_lwm = lwm[a->static_id];
    double a_error, b_error, x, dx;
    if(a_lwm.ts == a_lwm.addr->timestamp) {
        z->lhs = a_lwm.addr;
        a_error = a_lwm.addr->error;
    }
    else {
        z->lhs = nullptr;
        a_error = 0.0;
    }
    PropSqrtError(a->value, a_error, x, dx);
    z->value = x;
    z->error = dx;
    z->opcode = fp_op::SQRT;
    z->linenum = linenum;
    z->static_id = site_id;
    lwm[site_id] = {z, z->timestamp};
    return z;
}

void TempContext::t_store(void* addr, fp_entry* y, size_t site_id, size_t linenum) {
    lwrm_value y_lwm = lwm[y->static_id];
    fp_entry& m = smem->on_store(addr, y_lwm.addr);
    inc_ts();
}

// fp_entry* TempContext::t_load(void* addr, size_t site_id, size_t linenum) {
//     double v = static_cast<double>(*reinterpret_cast<float*>(addr)); //TODO: use Template for generalization
//     fp_entry* s = smem->peek(addr);
//     fp_entry* y = alloc();
//     if(s->value == v) {
//         y->value = s->value;
//         y->error = s->error;
//         y->lhs = s->lhs;
//         y->rhs = s->rhs;
//         y->opcode = s->opcode;
//         y->linenum = s->linenum;
//     }
//     else {
//         y->value = v;
//         y->error = 0.0;
//         y->lhs = nullptr;
//         y->rhs = nullptr;
//         y->opcode = fp_op::LOAD;
//         y->linenum = linenum;
//     }
//     y->static_id = site_id;
//     lwm[site_id] = {y, y->timestamp};
//     return y;
// }

void TempContext::dump_sum() {
    smem->dump_summary();
}

void TempContext::backtrack(fp_entry* x, int ind=0) {
    lwrm_value x_lwm = lwm[x->static_id];
    tracing[ind] = x;
    if(x->lhs != nullptr) {
        if(x->timestamp > x->lhs->timestamp) {
            backtrack(x->lhs, ind * 2 + 1);
        }
    }
    if(x->rhs != nullptr) {
        if(x->timestamp > x->rhs->timestamp) {
            backtrack(x->rhs, (ind + 1) * 2);
        }
    }
    return;
}

void TempContext::dump_tracing(fp_entry* x) {
    for(int i = 0; i < lwm.size(); i++) {
        tracing.push_back(nullptr);
    }
    backtrack(x);
    for(int i = 0; i < tracing.size(); i++) {
        if (tracing.at(i) != NULL) {
            cout << i << " : err= " << tracing.at(i)->error << " value= " << tracing.at(i)->value << " ts= " << tracing.at(i)->timestamp << " line= " << tracing.at(i)->linenum << endl;
        }
    }
}
}
// // template<typename F>
// // fp_entry* t_const(F& program_value, TempContext& ctx, size_t site_id, size_t linenum) {
// fp_entry* t_const(float& program_value, TempContext& ctx, size_t site_id, size_t linenum) {
//     fp_entry* t = ctx.queue.alloc();
//     t->error = 0.0;
//     // t->value = static_cast<double>(*(program_value));
//     t->value = static_cast<double>(program_value);
//     t->lhs = nullptr;
//     t->rhs = nullptr;
//     t->opcode = fp_op::INIT;
//     t->linenum = linenum;
//     t->static_id = site_id;
//     ctx.queue.inc_ts();
//     ctx.lwm[site_id] = {t, t->timestamp};
//     return t;
// }

// fp_entry* t_add(fp_entry* a, fp_entry* b, TempContext& ctx, size_t site_id, size_t linenum) {
//     fp_entry* z = ctx.queue.alloc();
//     fp_entry* a_addr = ctx.lwm[a->static_id].addr;
//     size_t a_ts = ctx.lwm[a->static_id].ts;
//     fp_entry* b_addr = ctx.lwm[b->static_id].addr;
//     size_t b_ts = ctx.lwm[b->static_id].ts;
//     double a_error, b_error;
//     if(a_ts == a_addr->timestamp) {
//         z->lhs = a_addr;
//         a_error = a_addr->error;
//     }
//     else {
//         z->lhs = NULL;
//         a_error = 0.0;
//     }
//     if(b_ts == b_addr->timestamp) {
//         z->rhs = b_addr;
//         b_error = b_addr->error;
//     }
//     else {
//         z->rhs = NULL;
//         b_error = 0.0;
//     }
//     double x, dx;
//     PropSumError(a->value, a_error, b->value, b_error, x, dx);

//     z->value = x;
//     z->error = dx;
//     z->opcode = fp_op::ADD;
//     z->linenum = linenum;
//     z->static_id = site_id;
//     ctx.queue.inc_ts();
//     ctx.lwm[site_id] = {z, z->timestamp};
//     return z;
// }
// fp_entry* t_sub(fp_entry* a, fp_entry* b, TempContext& ctx, size_t site_id, size_t linenum) {
//     fp_entry* z = ctx.queue.alloc();
//     fp_entry* a_addr = ctx.lwm[a->static_id].addr;
//     size_t a_ts = ctx.lwm[a->static_id].ts;
//     fp_entry* b_addr = ctx.lwm[b->static_id].addr;
//     size_t b_ts = ctx.lwm[b->static_id].ts;
//     double a_error, b_error;
//     if(a_ts == a_addr->timestamp) {
//         z->lhs = a_addr;
//         a_error = a_addr->error;
//     }
//     else {
//         z->lhs = NULL;
//         a_error = 0.0;
//     }
//     if(b_ts == b_addr->timestamp) {
//         z->rhs = b_addr;
//         b_error = b_addr->error;
//     }
//     else {
//         z->rhs = NULL;
//         b_error = 0.0;
//     }
//     double x, dx;
//     PropSumError(a->value, a_error, -b->value, b_error, x, dx);

//     z->value = x;
//     z->error = dx;
//     z->opcode = fp_op::SUB;
//     z->linenum = linenum;
//     z->static_id = site_id;
//     ctx.queue.inc_ts();
//     ctx.lwm[site_id] = {z, z->timestamp};
//     return z;
// }

// fp_entry* t_mul(fp_entry* a, fp_entry* b, TempContext& ctx, size_t site_id, size_t linenum) {
//     fp_entry* z = ctx.queue.alloc();
//     fp_entry* a_addr = ctx.lwm[a->static_id].addr;
//     size_t a_ts = ctx.lwm[a->static_id].ts;
//     fp_entry* b_addr = ctx.lwm[b->static_id].addr;
//     size_t b_ts = ctx.lwm[b->static_id].ts;
//     double a_error, b_error;
//     if(a_ts == a_addr->timestamp) {
//         z->lhs = a_addr;
//         a_error = a_addr->error;
//     }
//     else {
//         z->lhs = NULL;
//         a_error = 0.0;
//     }
//     if(b_ts == b_addr->timestamp) {
//         z->rhs = b_addr;
//         b_error = b_addr->error;
//     }
//     else {
//         z->rhs = NULL;
//         b_error = 0.0;
//     }
//     double x, dx;
//     PropProdError(a->value, a_error, b->value, b_error, x, dx);

//     z->value = x;
//     z->error = dx;
//     z->opcode = fp_op::MUL;
//     z->linenum = linenum;
//     z->static_id = site_id;
//     ctx.queue.inc_ts();
//     ctx.lwm[site_id] = {z, z->timestamp};
//     return z;
// }

// fp_entry* t_div(fp_entry* a, fp_entry* b, TempContext& ctx, size_t site_id, size_t linenum) {
//     fp_entry* z = ctx.queue.alloc();
//     fp_entry* a_addr = ctx.lwm[a->static_id].addr;
//     size_t a_ts = ctx.lwm[a->static_id].ts;
//     fp_entry* b_addr = ctx.lwm[b->static_id].addr;
//     size_t b_ts = ctx.lwm[b->static_id].ts;
//     double a_error, b_error;
//     if(a_ts == a_addr->timestamp) {
//         z->lhs = a_addr;
//         a_error = a_addr->error;
//     }
//     else {
//         z->lhs = NULL;
//         a_error = 0.0;
//     }
//     if(b_ts == b_addr->timestamp) {
//         z->rhs = b_addr;
//         b_error = b_addr->error;
//     }
//     else {
//         z->rhs = NULL;
//         b_error = 0.0;
//     }
//     double x, dx;
//     PropDivError(a->value, a_error, b->value, b_error, x, dx);

//     z->value = x;
//     z->error = dx;
//     z->opcode = fp_op::DIV;
//     z->linenum = linenum;
//     z->static_id = site_id;
//     ctx.queue.inc_ts();
//     ctx.lwm[site_id] = {z, z->timestamp};
//     return z;
// }

// void t_store(void* addr, fp_entry* y, TempContext& ctx, size_t site_id, size_t linenum) {
//     // *reinterpret_cast<float*>(addr) = static_cast<float>(y->value); //TODO: use Template for generalization //?

//     fp_entry* y_addr = ctx.lwm[y->static_id].addr;
//     size_t y_ts = ctx.lwm[y->static_id].ts;
//     fp_entry& m = ctx.smem->on_store(addr, y_addr->value, fp_op::STORE, linenum, nullptr, nullptr);
//     // memcpy(ctx.smem->peek(addr), y, sizeof(fp_entry)); //! SIZE
//     // m.error = y->error;
//     // m.lhs = y->lhs;
//     // m.rhs = y->rhs;
//     ctx.smem->inc_ts();
//     ctx.queue.inc_ts();
// }

// fp_entry* t_load(void* addr, TempContext& ctx, size_t site_id, size_t linenum) {
//     double v = static_cast<double>(*reinterpret_cast<float*>(addr)); //TODO: use Template for generalization
//     fp_entry* s = ctx.smem->peek(addr);

//     fp_entry* y = ctx.queue.alloc();
//     fp_entry& m = ctx.smem->on_load(addr, v, fp_op::LOAD, linenum);
//     if(s->value == v) {
//         // memcpy(y, s, sizeof(fp_entry)); //! SIZE
//         y->value = s->value;
//         y->error = s->error;
//         y->lhs = s->lhs;
//         y->rhs = s->rhs;
//         y->opcode = s->opcode;
//         y->linenum = s->linenum;
//     } else {
//         y->error = 0;
//         y->value = v;
//         y->lhs = nullptr;
//         y->rhs = nullptr;
//         y->opcode = fp_op::LOAD;
//         y->linenum = linenum ;
//     }
//     // ctx.smem->on_load(addr, v, fp_op::LOAD, linenum); //?
//     y->static_id = site_id;
//     ctx.queue.inc_ts();
//     ctx.lwm[site_id] = {y, y->timestamp};
//     return y;
// }