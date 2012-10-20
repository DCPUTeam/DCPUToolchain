#ifndef _IDE_DCPUTOOLCHAIN_EMU
#define _IDE_DCPUTOOLCHAIN_EMU

#ifdef __cplusplus
extern "C" {
#endif

#include <dcpu.h>
#include <stdio.h>
    
bool start_emulation(const char* filename);
void cycle_emulation();
void stop_emulation();

#ifdef __cplusplus
}
#endif

#endif
