#ifndef __TEMP_MTA_SPACE_H__
#define __TEMP_MTA_SPACE_H__
#pragma once
#include <cmath>
#include <cstdint>
#include <cstring>
#include <map>
#include <vector>
#include "eft/shadow_memory.hpp"

using namespace std;
// void TwoSum(double a, double b, double& x, double &dx);
// void PropSumError(double a, double da, double b, double db, double& x, double& dx);
// void TwoProd(double a, double b, double& x, double& dx);
// void PropProdError(double a, double da, double b, double db, double& x, double& dx);
// void TwoDiv(double a, double b, double& x, double& dx);
// void PropDivError(double a, double da, double b, double db, double& x, double& dx);

// struct tms_entry{
//     // mpfr_t val;
//     double error;
//     double value;

//     size_t linenum;
//     enum fp_op opcode;
//     // bool is_init;

//     // size_t lock;
//     // size_t key;

//     // size_t op1_lock;
//     // size_t op1_key;
//     struct tms_entry* lhs;

//     // size_t op2_lock;
//     // size_t op2_key;
//     struct tms_entry* rhs;
//     size_t timestamp;
//     size_t static_id;
// };

// class TmpMtaSpc {
//     public:
//         TmpMtaSpc(size_t cap_);
//         ~TmpMtaSpc();
//         fp_entry* alloc();
//         void inc_ts();
//         size_t get_ts();
//     private:
//         size_t cap;
//         fp_entry* circ_queue;
//         size_t head;
//         size_t global_ts;

// };


// using LstWrtMap = map<size_t, lwrm_value>;
namespace eft {
struct lwrm_value {
    fp_entry* addr;
    size_t ts;
};

size_t inst_id_hash(const char* file, int line, const char* func);
#define SITE_ID() inst_id_hash(__FILE__, __LINE__, __func__)

class TempContext {
    public:
        TempContext(ShadowMemory* s, size_t cap_);
        ~TempContext();
        fp_entry* alloc();
        void inc_ts();
        template <typename T>
        fp_entry* t_const(T program_value, size_t site_id, size_t linenum) {
            fp_entry* t = alloc();
            t->error = 0.0;
            t->value = static_cast<double>(program_value);
            t->lhs = nullptr;
            t->rhs = nullptr;
            t->opcode = fp_op::INIT;
            t->linenum = linenum;
            t->static_id = site_id;
            lwm[site_id] = {t, t->timestamp};
            return t;
        }
        fp_entry* t_add(fp_entry* a, fp_entry* b, size_t site_id, size_t linenum);
        fp_entry* t_sub(fp_entry* a, fp_entry* b, size_t site_id, size_t linenum);
        fp_entry* t_mul(fp_entry* a, fp_entry* b, size_t site_id, size_t linenum);
        fp_entry* t_div(fp_entry* a, fp_entry* b, size_t site_id, size_t linenum);
        void t_store(void* addr, fp_entry* y, size_t site_id, size_t linenum);
        fp_entry* t_load(void* addr, size_t site_id, size_t linenum);
        void backtrack(fp_entry* x, int ind);
        void dump_sum();
        void dump_tracing(fp_entry* x);
    private:
        ShadowMemory* smem;
        size_t global_ts;

        // TmpMtaSpc queue;
        fp_entry* circ_queue;
        size_t cap;
        size_t head;

        map<size_t, lwrm_value> lwm;
        vector<fp_entry*> tracing;

        void TwoSum(double a, double b, double& x, double& dx) {
            x = a + b;
            double bp = x - a;
            double ap = x - bp;
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

        void SquareRoot(double a, double& x, double& dx) {
            x = sqrt(a);
            dx = std::fma(-x, x, a);
        }

        void PropDivError(double a, double da, double b, double db, double& x, double& dx) {
            TwoDiv(a, b, x, dx);
            dx = (da - dx - x * db) / (b + db);
        }
};

}


// // template <typename F>
// // fp_entry* t_const(F& program_value, TempContext& ctx, size_t site_id, size_t linenum);
// fp_entry* t_const(float& program_value, TempContext& ctx, size_t site_id, size_t linenum);
// fp_entry* t_add(fp_entry* x, fp_entry* y, TempContext& ctx, size_t site_id, size_t linenum);
// fp_entry* t_sub(fp_entry* x, fp_entry* y, TempContext& ctx, size_t site_id, size_t linenum);
// fp_entry* t_mul(fp_entry* x, fp_entry* y, TempContext& ctx, size_t site_id, size_t linenum);
// fp_entry* t_div(fp_entry* x, fp_entry* y, TempContext& ctx, size_t site_id, size_t linenum);
// void t_store(void* addr, fp_entry* y, TempContext& ctx, size_t site_id, size_t linenum);
// fp_entry* t_load(void* addr, TempContext& ctx, size_t site_id, size_t linenum);

// // bool valid_operand(fp_entry* e, const LastWriteInfo& lwi) {
// //     return e && (e->timestamp == lwi.ts);
// // }

#endif