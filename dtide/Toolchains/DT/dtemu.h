#ifndef _IDE_DCPUTOOLCHAIN_EMU
#define _IDE_DCPUTOOLCHAIN_EMU

#ifdef __cplusplus
extern "C" {
#endif

#include <dcpu.h>
#include <stdio.h>
#include <dcpuhook.h>

    vm_t* get_vm(void);
    void lem_resize(void* ud, int w, int h);
    vm_t* start_emulation(
        const char* filename,

        vm_hook cyclehook,
        vm_hook writehook,
        vm_hook interrupthook,
        vm_hook hardwarehook,
        vm_hook sixtyhz,

        create_context_t create_context,
        activate_context_t activate_context,
        swap_buffers_t swap_buffers,
        destroy_context_t destroy_context,
        get_ud_t get_ud,

        void* toolchain);
    void cycle_emulation();
    void stop_emulation();

#ifdef __cplusplus
}
#endif

#endif
