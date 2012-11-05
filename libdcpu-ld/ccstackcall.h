///
/// @addtogroup LibDCPU-LD
/// @{
///
/// @file
/// @brief The header for the "stack-call" calling convention.
/// @author James Rhodes
///
/// The "stack-call" calling convention is one where all of the arguments to a
/// kernel call are pushed onto the stack and the interrupt is triggered.  The
/// result of the kernel call is in the position of the first argument when the
/// interrupt returns and thus, a dummy value be pushed onto the stack if there
/// are no arguments to the kernel call.
///

#ifndef __DCPU_LD_CALL_CONVENTION_STACK_CALL_H
#define __DCPU_LD_CALL_CONVERTION_STACK_CALL_H

#include <simclist.h>
#include <policy.h>
#include "ldbin.h"
#include "ldconv.h"

void generate_ccstackcall(policies_t* policies, list_t* all, struct ldbin* bin, struct lconv_entry* call);

#endif

///
/// @}
///
