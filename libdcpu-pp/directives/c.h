#ifndef __DCPU_LIBDCPUPP_DIRECTIVES_C_H
#define __DCPU_LIBDCPUPP_DIRECTIVES_C_H

#include "ppimpl.h"

void ppimpl_c_line_register(state_t* state);
void ppimpl_c_expr_register(state_t* state);
void ppimpl_c_define_register(state_t* state);
void ppimpl_c_include_register(state_t* state);
void ppimpl_c_init(state_t* state);

#endif
