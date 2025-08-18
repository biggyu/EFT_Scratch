#include "shadow_memory.hpp"

ShadowMemory::ShadowMemory() {
    ShadowMemory::gloabl_ts_ = 0;
}

smem_entry& ShadowMemory::on_store(void* addr, double value, fp_op op = fp_op::STORE, size_t linenum = 0, smem_entry* lhs = nullptr, smem_entry* rhs = nullptr) {
    auto& e = table[static_cast<uintptr_t>(addr)];
    e.value = value;
    e.error = 0.0;
    e.timestamp = ++gloabl_ts_;
    e.opcode = op;
    e.linenum = linenum;
    e.lhs = lhs;
    e.rhs = rhs;
    return e;
}

smem_entry& ShadowMemory::on_load(void* addr, double program_value, fp_op op = fp_op::LOAD, size_t linenum = 0) {
    auto it = table.find(static_cast<uintptr_t>(addr));
    if (it != table.end() && it->second.value == program_value) {
        return it->second;
    } else {
        auto& e = table[static_cast<uintptr_t>(addr)];
        e.value = program_value;
        e.error = 0.0;
        e.timestamp = ++gloabl_ts_;
        e.opcode = op;
        e.linenum = linenum;
        e.lhs = nullptr;
        e.rhs = nullptr;
        return e;
    }
}

// bool ShadowMemory::has(void* addr) const {
//     return table.count(addr) != 0;
// }

void ShadowMemory::dump_summary(const char* header = "Shadow memory summary") const {
    printf("=== %s ===\n", header);
    printf("entries: %zu\n", table.size());
    size_t count = 0;
    for (auto& kv : table) {
        if (count++ >= 8) break; // print a few sample entries
        const auto& e = kv.second;
        printf("  addr=0x%zx  value=%.17g  err=%.3e  ts=%zu\n",
                    kv.first, e.computed, e.error, e.timestamp);
    }
}