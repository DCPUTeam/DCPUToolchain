#ifndef _IDE_DCPUTOOLCHAIN_ASM
#define _IDE_DCPUTOOLCHAIN_ASM

#ifdef __cplusplus
extern "C" {
#endif

#include <dcpu.h>
#include <stdio.h>
    
bool perform_assemble(const char* input_filename,
        const char* output_binary_filename,
        const char* output_symbols_filename);

#ifdef __cplusplus
}
#endif

#endif
