///
/// @addtogroup LibDCPU-VM
/// @{
///
/// @file
/// @brief General API access for linker policies.
/// @author Jakob Bornecrantz
/// 
/// This header provides access to vm_execute function.
///

#ifndef __DCPUEXEC_H
#define __DCPUEXEC_H

#include "vm.h"

void vm_execute(vm_t* vm, const char* execution_dump);

#endif

/// 
/// @}
///
