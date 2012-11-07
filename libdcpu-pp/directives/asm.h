#ifndef __DCPU_LIBDCPUPP_DIRECTIVES_ASM_H
#define __DCPU_LIBDCPUPP_DIRECTIVES_ASM_H

#include "ppimpl.h"

void ppimpl_asm_line_register(state_t* state);
void ppimpl_asm_expr_register(state_t* state);
void ppimpl_asm_define_register(state_t* state);
void ppimpl_asm_include_register(state_t* state);
void ppimpl_asm_lua_register(state_t* state);
void ppimpl_asm_init(state_t* state);

#endif
