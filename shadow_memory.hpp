#ifndef __SHADOW_MEMORY_H__
#define __SHADOW_MEMORY_H__
#pragma once
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <unordered_map>
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


class ShadMem {
    public:
        ShadMem();
        smem_entry& on_store(void* addr, double value, fp_op op = fp_op::STORE, size_t linenum = 0, smem_entry* lhs = nullptr, smem_entry* rhs = nullptr);
        smem_entry& on_load(void* addr, double program_value, fp_op op = fp_op::LOAD, size_t linenum = 0);
        // bool has(void* addr);
        void dump_summary(const char* header = "Shadow memory summary");
        
    private:
        // static uintptr_t key(void* p) { return reinterpret_cast<uintptr_t>(p); }
        unordered_map<uintptr_t, smem_entry> table;
        size_t timestamp = 0;
}
#endif