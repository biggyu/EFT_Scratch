#pragma once

#include "eft/shadow_memory.hpp"
#include "eft/tmp_mta_space.hpp"
#include "eft/shadow_stack.hpp"

// ---------------------------------------------------------------------------
// Convenience macros for EFT operations
// ---------------------------------------------------------------------------

#define T_CONST(val) tmpctx->t_const((val), SITE_ID(), __LINE__)
#define T_ADD(a, b) tmpctx->t_add((a), (b), SITE_ID(), __LINE__)
#define T_SUB(a, b) tmpctx->t_sub((a), (b), SITE_ID(), __LINE__)
#define T_MUL(a, b) tmpctx->t_mul((a), (b), SITE_ID(), __LINE__)
#define T_DIV(a, b) tmpctx->t_div((a), (b), SITE_ID(), __LINE__)
#define T_SQRT(a) tmpctx->t_sqrt((a), SITE_ID(), __LINE__)
#define T_LOAD(ptr) tmpctx->t_load((ptr), SITE_ID(), __LINE__)
#define T_STORE(ptr, val) tmpctx->t_store((ptr), (val), SITE_ID(), __LINE__)