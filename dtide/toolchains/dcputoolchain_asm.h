#ifndef _IDE_DCPUTOOLCHAIN_ASM
#define _IDE_DCPUTOOLCHAIN_ASM

#ifdef __cplusplus
extern "C" {
#endif

#include <dcpu.h>
#include <stdio.h>

bool perform_assemble(const char* input_filename,
        FILE** output_binary, FILE** output_symbols);

#ifdef __cplusplus
}
#endif

#endif
