#ifndef __SHADOW_MEMORY_H__
#define __SHADOW_MEMORY_H__
#pragma once
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdint>
#include <map>
using namespace std;

enum class fp_op : uint32_t { INIT, LOAD, STORE, ADD, SUB, MUL, DIV, UNKNOWN };

struct fp_entry {
    double error;
    double value;
    fp_op opcode;
    size_t linenum;
    fp_entry* lhs;
    fp_entry* rhs;
    size_t timestamp;
    size_t static_id;
};


class ShadowMemory {
    public:
        // ShadowMemory();
        fp_entry& on_store(void* dest, fp_entry* org);
        // fp_entry& on_store(void* addr, double value, size_t linenum);
        fp_entry& on_load(void* addr, double program_value, size_t linenum);
        fp_entry* peek(void* addr);
        void dump_summary() const;
        // void inc_ts();
        
    private:
        map<uintptr_t, fp_entry> table;
        // size_t gloabl_ts_ = 0;
};  
#endif