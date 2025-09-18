#include "shadow_stack.hpp"

static size_t ShadowStack::push(size_t n_params, size_t call_site_id = 0, size_t ts = 0) {
    auto& st = stack();
    st.emplace_back();
    st.back().params.assign(n_params, nullptr);
    st.back().call_site_id = call_site_id;
    st.back().ts = ts;
    return st.size() - 1;
}

static ShadowFrame& ShadowStack::top() {
    assert(!s_stack.empty());
    return s_stack.back();
}
static ShadowFrame& ShadowStack::at(size_t idx){
    assert(idx < s_stack.size());
    return s_stack[idx];
}

static void ShadowStack::set_param(size_t idx, fp_entry* m) {
    top().params.at(idx) = m;
}
static void ShadowStack::set_param_at(size_t frame_idx, size_t param_idx, fp_entry* m) {
    at(frame_idx).params.at(param_idx) = m;
}
static fp_entry* ShadowStack::get_param(size_t idx) {
    return top().params.at(idx);
}

static void ShadowStack::set_ret(fp_entry* m) {
    top().ret = m;
}
static fp_entry* ShadowStack::pop_and_get_ret() {
    assert(!s_stack.empty());
    fp_entry* r = s_stack.back().ret;
    s_stack.pop_back();
    return r;
}

static size_t ShadowStack::depth() {
    return stack().size();
}