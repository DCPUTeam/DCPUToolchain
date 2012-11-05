///
/// @addtogroup LibDCPU-LD
/// @{
///
/// @file
/// @brief The header for the "register-direct" calling convention.
/// @author James Rhodes
///
/// The "register-direct" calling convention is one where the first 3 arguments
/// are put into the A, B and C registers respectively and then any additional
/// arguments are pushed onto the stack.  The result of the kernel is placed
/// into the position on the stack where RFI will restore from and thus upon
/// returning from the interrupt, the DCPU-16 automatically sets the A register
/// to the corect value.
///

#ifndef __DCPU_LD_CALL_CONVENTION_REGISTER_DIRECT_H
#define __DCPU_LD_CALL_CONVERTION_REGISTER_DIRECT_H

#include <simclist.h>
#include <policy.h>
#include "ldbin.h"
#include "ldconv.h"

void generate_ccregisterdirect(policies_t* policies, list_t* all, struct ldbin* bin, struct lconv_entry* call);

#endif

///
/// @}
///
