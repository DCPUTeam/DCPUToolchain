#ifndef _IDE_DCPUTOOLCHAIN_EMU
#define _IDE_DCPUTOOLCHAIN_EMU

#ifdef __cplusplus
extern "C" {
#endif

#include <dcpu.h>
#include <stdio.h>
#include <dcpuhook.h>

vm_t* get_vm(void);    
bool start_emulation(const char* filename, vm_hook cyclehook, vm_hook writehook, vm_hook interrupthook, vm_hook hardwarehook, vm_hook sixtyhz, void* toolchain);
void cycle_emulation();
void stop_emulation();

#ifdef __cplusplus
}
#endif

#endif
