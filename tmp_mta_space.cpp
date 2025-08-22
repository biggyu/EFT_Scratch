#include "tmp_mta_space.hpp"

void TwoSum(double a, double b, double& x, double& dx) {
    x = a + b;
    double bp = x - a;
    double ap = a - bp;
    double da = a - ap;
    double db = b - bp;
    dx = da + db;
}

void PropSumError(double a, double da, double b, double db, double& x, double& dx) {
    TwoSum(a, b, x, dx);
    dx = dx + da + db;
}

void TwoProd(double a, double b, double& x, double& dx) {
    x = a * b;
    dx = std::fma(a, b, -x);
}

void PropProdError(double a, double da, double b, double db, double& x, double& dx) {
    TwoProd(a, b, x, dx);
    dx = dx + a * db + b * da;
}

void TwoDiv(double a, double b, double& x, double& dx) {
    x = a / b;
    dx = std::fma(x, b, -a);
}

void PropDivError(double a, double da, double b, double db, double& x, double& dx) {
    TwoDiv(a, b, x, dx);
    dx = (da - dx - x * db) / (b + db);
}



TmpMtaSpc::TmpMtaSpc(size_t cap_ = 256) cap(cap_), circ_queue(new tms_entry[cap_]) {
    head = 0;
    global_ts = 0;
}

TmpMtaSpc::~TmpMtaSpc() {
    delete[] circ_queue;
}

void TmpMtaSpc::alloc() {
    tms_entry* t = &q_[head];
    head = (head + 1) % cap;
    t->timestamp = ++global_ts;
    return t;
}
void TmpMtaSpc::inc_ts() {
    global_ts++;
}

void TmpMtaSpc::get_ts() {
    return global_ts;
}

size_t inst_id_hash(const char* file, int line, const char* func) {
    string key = string(file) + ":" + to_string(line) + ":" + func;
    return std::hash<std::string>{}(key);
}

tms_entry* t_const(double program_value, TempContext& ctx, size_t site_id, size_t linenum) {
    tms_entry* t = ctx.queue.alloc();
    t->error = 0.0;
    t->value = program_value;
    t->lhs = nullptr;
    t->rhs = nullptr;
    t->opcode = fp_op::INIT;
    t->linenum = linenum;
    t->site_id = site_id;
    ctx.lwm[site_id] = {t, t->timestamp};
    return t;
}

tms_entry* t_add(tms_entry* a, tms_entry* b, TempContext& ctx, size_t site_id, size_t linenum) {
    tms_entry* z = ctx.queue.alloc();
    tms_entry* a_addr = ctx.lwm[a->site_id].addr;
    size_t a_ts = ctx.lwm[a->site_id].ts;
    tms_entry* b_addr = ctx.lwm[b->site_id].addr;
    size_t b_ts = ctx.lwm[b->site_id].ts;
    double a_error, b_error;
    if(a_ts == a_addr->timestamp) {
        z->lhs = a_addr;
        a_error = a_addr->error;
    }
    else {
        z->lhs = NULL;
        a_error = 0.0;
    }
    if(b_ts == b_addr->timestamp) {
        z->rhs = b_addr;
        b_error = b_addr->error;
    }
    else {
        z->rhs = NULL;
        b_error = 0.0;
    }
    double x, dx;
    PropSumError(a->value, a_error, b->value, b_error, x, dx);

    z->value = x;
    z->error = dx;
    z->opcode = fp_op::ADD;
    z->linenum = linenum;
    z->site_id = site_id;
    ctx.lwr[site_id] = {z, z->timestamp};
    return z;
}
tms_entry* t_sub(tms_entry* a, tms_entry* b, TempContext& ctx, size_t site_id, size_t linenum) {
    tms_entry* z = ctx.queue.alloc();
    tms_entry* a_addr = ctx.lwm[a->site_id].addr;
    size_t a_ts = ctx.lwm[a->site_id].ts;
    tms_entry* b_addr = ctx.lwm[b->site_id].addr;
    size_t b_ts = ctx.lwm[b->site_id].ts;
    double a_error, b_error;
    if(a_ts == a_addr->timestamp) {
        z->lhs = a_addr;
        a_error = a_addr->error;
    }
    else {
        z->lhs = NULL;
        a_error = 0.0;
    }
    if(b_ts == b_addr->timestamp) {
        z->rhs = b_addr;
        b_error = b_addr->error;
    }
    else {
        z->rhs = NULL;
        b_error = 0.0;
    }
    double x, dx;
    PropSumError(a->value, a_error, -b->value, b_error, x, dx);

    z->value = x;
    z->error = dx;
    z->opcode = fp_op::SUB;
    z->linenum = linenum;
    z->site_id = site_id;
    ctx.lwr[site_id] = {z, z->timestamp};
    return z;
}

tms_entry* t_mul(tms_entry* a, tms_entry* b, TempContext& ctx, size_t site_id, size_t linenum) {
    tms_entry* z = ctx.queue.alloc();
    tms_entry* a_addr = ctx.lwm[a->site_id].addr;
    size_t a_ts = ctx.lwm[a->site_id].ts;
    tms_entry* b_addr = ctx.lwm[b->site_id].addr;
    size_t b_ts = ctx.lwm[b->site_id].ts;
    double a_error, b_error;
    if(a_ts == a_addr->timestamp) {
        z->lhs = a_addr;
        a_error = a_addr->error;
    }
    else {
        z->lhs = NULL;
        a_error = 0.0;
    }
    if(b_ts == b_addr->timestamp) {
        z->rhs = b_addr;
        b_error = b_addr->error;
    }
    else {
        z->rhs = NULL;
        b_error = 0.0;
    }
    double x, dx;
    PropProdError(a->value, a_error, b->value, b_error, x, dx);

    z->value = x;
    z->error = dx;
    z->opcode = fp_op::MUL;
    z->linenum = linenum;
    z->site_id = site_id;
    ctx.lwr[site_id] = {z, z->timestamp};
    return z;
}

tms_entry* t_div(tms_entry* a, tms_entry* b, TempContext& ctx, size_t site_id, size_t linenum) {
    tms_entry* z = ctx.queue.alloc();
    tms_entry* a_addr = ctx.lwm[a->site_id].addr;
    size_t a_ts = ctx.lwm[a->site_id].ts;
    tms_entry* b_addr = ctx.lwm[b->site_id].addr;
    size_t b_ts = ctx.lwm[b->site_id].ts;
    double a_error, b_error;
    if(a_ts == a_addr->timestamp) {
        z->lhs = a_addr;
        a_error = a_addr->error;
    }
    else {
        z->lhs = NULL;
        a_error = 0.0;
    }
    if(b_ts == b_addr->timestamp) {
        z->rhs = b_addr;
        b_error = b_addr->error;
    }
    else {
        z->rhs = NULL;
        b_error = 0.0;
    }
    double x, dx;
    PropDivError(a->value, a_error, b->value, b_error, x, dx);

    z->value = x;
    z->error = dx;
    z->opcode = fp_op::DIV;
    z->linenum = linenum;
    z->site_id = site_id;
    ctx.lwr[site_id] = {z, z->timestamp};
    return z;
}

void t_store(void* addr, tms_entry* y, TempContext& ctx, size_t site_id, size_t linenum) {
    *reinterpret_cast<float*>(addr) = static_cast<float>(y->value); //TODO: use Template for generalization

    smem_entry& m = ctx.smem->on_store(addr, y->value, fp_op::STORE, linenum, nullptr, nullptr)
    m.error = y_addr->error;
    m.lhs = y_addr->lhs;
    m.rhs = y_addr->rhs;
    ctx.queue.inc_ts();
}

tms_entry* t_load(void* addr, TempContext& ctx, size_t site_id, size_t linenum) {
    double v = static_cast<double>(*reinterpret_cast<float*>(addr)); //TODO: use Template for generalization
    smem_entry* s = ctx->smem->peek(addr);

    tms_entry* y = ctx.queue.alloc();
    // smem_entry& m = ctx.smem->on_load(addr, program_value, fp_op::LOAD, linenum);
    if(s->value == v) {
        y->error = s->error;
        y->value = s->value;
        y->lhs = s->lhs;
        y->rhs = s->rhs;
        y->opcode = s->opcode;
        y->linenum = s->linenum;
    } else {
        y->error = 0;
        y->value = v;
        y->lhs = nullptr;
        y->rhs = nullptr;
        y->opcode = fp_op::LOAD;
        y->linenum = linenum ;
    }
    ctx.smem->onload(addr, v, fp_op::LOAD, linenum);
    y->site_id = site_id;
    ctx.lwm[site_id] = {y, y->timestamp};
    return y;
}