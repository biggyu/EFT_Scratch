#include "shadow_memory.hpp"

// ShadowMemory::ShadowMemory() {
//     ShadowMemory::gloabl_ts_ = 0;
// }

fp_entry& ShadowMemory::on_store(void* dest, fp_entry* org) {
    auto& e = table[reinterpret_cast<uintptr_t>(dest)];
    e.value = org->value;
    e.error = org->error;
    e.timestamp = org->timestamp;
    e.opcode = fp_op::STORE;
    e.linenum = org->linenum;
    e.lhs = org->lhs;
    e.rhs = org->rhs;
    return e;
}

fp_entry& ShadowMemory::on_load(void* addr, double program_value, size_t linenum = 0) {
    auto it = table.find(reinterpret_cast<uintptr_t>(addr));
    if (it != table.end()) {
        it->second.value = program_value;
        it->second.opcode = fp_op::LOAD;
        it->second.linenum = linenum;
        return it->second;
    } else {
        auto& e = table[reinterpret_cast<uintptr_t>(addr)];
        e.value = program_value;
        e.error = 0.0;
        // e.timestamp = ++gloabl_ts_;
        e.opcode = fp_op::LOAD;
        e.linenum = linenum;
        e.lhs = nullptr;
        e.rhs = nullptr;
        return e;
    }
}

fp_entry* ShadowMemory::peek(void* addr) {
    auto it = table.find(reinterpret_cast<uintptr_t>(addr));
    return (it == table.end()) ? nullptr : &it->second;
}

void ShadowMemory::dump_summary() const {
    cout << "=== Shadow memory summary ===" << endl << "entries: " << table.size() << endl;
    size_t count = 0;
    for (auto& kv : table) {
        const auto& e = kv.second;
        cout << "  addr=0x" << setw(15) << kv.first << " value=" << setw(17) << setprecision(10) << e.value << " err=" << e.error << "\tln=" << e.linenum << "\tts=" << e.timestamp << endl;
    }
}

// void ShadowMemory::inc_ts() {
//     gloabl_ts_++;
// }

// ShadowMemory::ShadowMemory() {
//     ShadowMemory::gloabl_ts_ = 0;
// } 

// smem_entry& ShadowMemory::on_store(void* addr, double value, fp_op op = fp_op::STORE, size_t linenum = 0, smem_entry* lhs = nullptr, smem_entry* rhs = nullptr) {
//     auto& e = table[reinterpret_cast<uintptr_t>(addr)];
//     e.value = value;
//     e.error = 0.0;
//     // e.timestamp = ++gloabl_ts_;
//     e.opcode = op;
//     e.linenum = linenum;
//     e.lhs = lhs;
//     e.rhs = rhs;
//     return e;
// }

// smem_entry& ShadowMemory::on_load(void* addr, double program_value, fp_op op = fp_op::LOAD, size_t linenum = 0) {
//     auto it = table.find(reinterpret_cast<uintptr_t>(addr));
//     if (it != table.end() && it->second.value == program_value) {
//         return it->second;
//     } else {
//         auto& e = table[reinterpret_cast<uintptr_t>(addr)];
//         e.value = program_value;
//         e.error = 0.0;
//         // e.timestamp = ++gloabl_ts_;
//         e.opcode = op;
//         e.linenum = linenum;
//         e.lhs = nullptr;
//         e.rhs = nullptr;
//         return e;
//     }
// }

// void ShadowMemory::dump_summary(const char* header = "Shadow memory summary") const {
//     printf("=== %s ===\n", header);
//     printf("entries: %zu\n", table.size());
//     size_t count = 0;
//     for (auto& kv : table) {
//         const auto& e = kv.second;
//         printf("  addr=0x%zx\tvalue=%.17g\terr=%.3e\tln=%zu\tts=%zu\n",
//                     kv.first, e.value, e.error, e.linenum, e.timestamp);
//     }
// }