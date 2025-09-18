#pragma once
#include <vector>
#include <cstddef>
#include <cassert>
#include "eft/shadow_memory.hpp"
#include "eft/tmp_mta_space.hpp"
using namespace std;
namespace eft {
struct ShadowFrame {
    vector<fp_entry*> params;
    fp_entry* ret;
    size_t static_id;
    size_t ts;
};

class ShadowStack {
    public:
        static size_t push(size_t n_params, size_t call_site_id, size_t ts);

        static ShadowFrame& top();
        static ShadowFrame& at(size_t idx);

        static void set_param(size_t idx, fp_entry* m);
        static void set_param_at(size_t frame_idx, size_t param_idx, fp_entry* m);
        static fp_entry* get_param(size_t idx);

        static void set_ret(fp_entry* m);
        static fp_entry* pop_and_get_ret();

        static size_t depth();

    private:
        static vector<ShadowFrame> s_stack;
};

#define SHADOW_CALL_BEGIN(N)  size_t __shadow_frame = ShadowStack::push((N), SITE_ID())
#define SHADOW_PASS_ARG(I, META_PTR) ShadowStack::set_param_at(__shadow_frame, (I), (META_PTR))
#define SHADOW_CALL_END_RET(META_RET_PTR_VAR) fp_entry* META_RET_PTR_VAR = ShadowStack::pop_and_get_ret()

#define SHADOW_FUNC_ENTER() auto& __shadow_top = ShadowStack::top()
#define SHADOW_PARAM(I)     (__shadow_top.params.at(I))
#define SHADOW_RETURN(META_PTR) do { ShadowStack::set_ret((META_PTR)); return (META_PTR)->value; } while(0)

}