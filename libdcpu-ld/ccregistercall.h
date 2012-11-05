///
/// @addtogroup LibDCPU-LD
/// @{
///
/// @file
/// @brief The header for the "register-call" calling convention.
/// @author James Rhodes
///
/// The "register-call" calling convention is one where the first 3 arguments
/// are put into the A, B and C registers respectively and then any additional
/// arguments are pushed onto the stack.  The result of the kernel call is
/// placed into the top of the stack and thus, if there are less than 4 arguments,
/// a dummy value is pushed to hold the result.
///

#ifndef __DCPU_LD_CALL_CONVENTION_REGISTER_CALL_H
#define __DCPU_LD_CALL_CONVERTION_REGISTER_CALL_H

#include <simclist.h>
#include <policy.h>
#include "ldbin.h"
#include "ldconv.h"

void generate_ccregistercall(policies_t* policies, list_t* all, struct ldbin* bin, struct lconv_entry* call);

#endif

///
/// @}
///
