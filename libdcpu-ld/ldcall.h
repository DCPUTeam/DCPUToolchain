///
/// @addtogroup LibDCPU-LD
/// @{
///
/// @file
/// @brief Library for producing code for kernel calling conventions.
/// @author James Rhodes
///
/// The linker permits a general .CALL directive that initiates a call back to
/// the kernel from user code.  Since some kernels use interrupts and others use
/// jumps, radically different code must be produced to call into each kernel.
///

#ifndef __DCPU_LD_CALL_H
#define __DCPU_LD_CALL_H

#include <policy.h>
#include <simclist.h>
#include "ldbin.h"
#include "ldconv.h"

void ldcall_generate(policies_t* policies, list_t* all, struct ldbin* bin, struct lconv_entry* call);

#endif

///
/// @}
///
