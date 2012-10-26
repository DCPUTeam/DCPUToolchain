/**

    File:       dbgaux.h

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU-vm-dbg

    Authors:    James Rhodes
            José Manuel Díez

    Description:    Declares auxilary functions for the debugger.

**/

#include <dcpu.h>
#include <dcpuhook.h>
#include <bstrlib.h>

// import hardware
#include <hw.h>
#include <hwlem1802.h>
#include <hwtimer.h>
#include <hwsped3.h>
#include <hwm35fd.h>


///
/// The last return code from a debugging command (to indicate
/// success or failure).
///
extern int ddbg_return_code;

void ddbg_init();
void ddbg_help(bstring section);
void ddbg_set(bstring object, bstring value);
void ddbg_load(bstring path);
void ddbg_load_symbols(bstring path);
void ddbg_inspect_symbols();
void ddbg_create_vm();
void ddbg_flash_vm();
void ddbg_run_vm();
void ddbg_continue_vm();
void ddbg_attach(bstring hw);
void ddbg_add_breakpoint(bstring file, int index);
void ddbg_delete_breakpoint(bstring file, int index);
void ddbg_add_breakpoint_identifier(bstring ident);
void ddbg_delete_breakpoint_identifier(bstring ident);
void ddbg_step_into();
void ddbg_step_over();
void ddbg_backtrace();
void ddbg_breakpoints_list();
void ddbg_dump_state();
void ddbg_dump_ram(int start, int difference);
void ddbg_disassemble_default();
void ddbg_disassemble(int start, int difference);
