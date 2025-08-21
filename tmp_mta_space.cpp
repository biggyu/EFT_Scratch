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
size_t TmpMtaSpc::inc_ts() {
    global_ts++;
}

size_t inst_id_hash() {
    string key = string(__FILE__) + ":" + to_string(__LINE__) + ":" + __func__;
    return std::hash<std::string>{}(key);
}

tms_entry* t_const(double v, TempContext& ctx, size_t site_id, size_t linenum) {
    tms_entry* t = ctx.queue.alloc();
    t->error = 0.0;
    t->value = v;
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
    PropSumError(a->value, b->value, x, dx);

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
    PropSumError(a->value, -b->value, x, dx);

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
    PropProdError(a->value, b->value, x, dx);

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
    PropDivError(a->value, b->value, x, dx);

    z->value = x;
    z->error = dx;
    z->opcode = fp_op::DIV;
    z->linenum = linenum;
    z->site_id = site_id;
    ctx.lwr[site_id] = {z, z->timestamp};
    return z;
}

void t_store(void* addr, double program_value, TempContext& ctx, size_t site_id, size_t linenum) {
    tms_entry* y_addr = ctx.lwm[site_id].addr;
    size_t y_ts = ctx.lwm[site_id].ts;

    smem_entry& m = ctx.smem->on_store(addr, program_value, fp_op::STORE, linenum)
    //TODO: For tracing, smem needs lhs, rhs
    m.lhs = y_addr->lhs;
    m.rhs = y_addr->rhs;
    m.error = y_addr->error;
    ctx.queue.inc_ts();
}

tms_entry* t_load(void* addr, double program_value, TempContext& ctx, size_t site_id, size_t linenum) {
    tms_entry* y = ctx.queue.alloc();
    smem_entry& m = ctx.smem->on_load(addr, program_value, fp_op::LOAD, linenum);
    if(m.value == program_value) {
        y->error = e.error;
        y->value = e.value;
        y->lhs = e.lhs;
        y->rhs = e.rhs;
    } else {
        y->error = 0;
        y->value = program_value;
        y->lhs = nullptr;
        y->rhs = nullptr;
    }
    y->opcode = e.opcode;
    y->linenum = e.linenum;
    y->site_id = site_id;
    ctx.lwm[site_id] = {y, y->timestamp};
    return y;
}