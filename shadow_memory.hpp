#ifndef __SHADOW_MEMORY_H__
#define __SHADOW_MEMORY_H__
#pragma once
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <map>
using namespace std;

enum class fp_op : uint32_t { INIT, LOAD, STORE, ADD, SUB, MUL, DIV, UNKNOWN };

struct smem_entry {
    double error;
    double value;
    size_t timestamp;
    fp_op opcode;
    size_t linenum;
    smem_entry* lhs;
    smem_entry* rhs;
};


class ShadowMemory {
    public:
        ShadowMemory();
        smem_entry& on_store(void* addr, double value, fp_op op, size_t linenum, smem_entry* lhs, smem_entry* rhs);
        smem_entry& on_load(void* addr, double program_value, fp_op op, size_t linenum);
        // bool has(void* addr);
        void dump_summary(const char* header) const;
        
    private:
        // static uintptr_t key(void* p) { return reinterpret_cast<uintptr_t>(p); }
        map<uintptr_t, smem_entry> table;
        size_t gloabl_ts_ = 0;
};  
#endif