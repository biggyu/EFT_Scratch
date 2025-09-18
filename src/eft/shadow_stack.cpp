#include "eft/shadow_stack.hpp"
namespace eft {
size_t ShadowStack::push(size_t n_params, size_t static_id = 0, size_t ts = 0) {
    auto& st = s_stack;
    st.emplace_back();
    st.back().params.assign(n_params, nullptr);
    st.back().static_id = static_id;
    st.back().ts = ts;
    return st.size() - 1;
}

ShadowFrame& ShadowStack::top() {
    assert(!s_stack.empty());
    return s_stack.back();
}
ShadowFrame& ShadowStack::at(size_t idx){
    assert(idx < s_stack.size());
    return s_stack[idx];
}

void ShadowStack::set_param(size_t idx, fp_entry* m) {
    top().params.at(idx) = m;
}
void ShadowStack::set_param_at(size_t frame_idx, size_t param_idx, fp_entry* m) {
    at(frame_idx).params.at(param_idx) = m;
}
fp_entry* ShadowStack::get_param(size_t idx) {
    return top().params.at(idx);
}

void ShadowStack::set_ret(fp_entry* m) {
    top().ret = m;
}
fp_entry* ShadowStack::pop_and_get_ret() {
    assert(!s_stack.empty());
    fp_entry* r = s_stack.back().ret;
    s_stack.pop_back();
    return r;
}

size_t ShadowStack::depth() {
    return s_stack.size();
}
}