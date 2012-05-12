/**

	File:		dbgaux.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-vm-dbg

	Authors:	James Rhodes
			José Manuel Díez

	Description:	Declares auxilary functions for the debugger.

**/

#include <dcpuhook.h>
#include <bstrlib.h>

void ddbg_cycle_hook(vm_t* vm, uint16_t pos);
void ddbg_load(bstring path);
void ddbg_load_symbols(bstring path);
void ddbg_inspect_symbols();
void ddbg_create_vm();
void ddbg_flash_vm();
void ddbg_run_vm();
void ddbg_attach(bstring hw);
void ddbg_add_breakpoint(bstring file, int index);
void ddbg_breakpoints_list();
void ddbg_dump_state();
void ddbg_dump_ram(int start, int difference);
void ddbg_disassemble(int start, int difference);
