/**

    File:       dbgaux.c

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU-vm-dbg

    Authors:    James Rhodes
            José Manuel Díez

    Description:    Defines auxilary functions for the debugger.

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bstring.h>
#include <simclist.h>
#include <iio.h>
#include <dcpu.h>
#include <dcpubase.h>
#include <dcpuhook.h>
#include <debug.h>
#include <imap.h>
#include <ddata.h>
#include <time.h>
#include <string.h>
#include "breakpoint.h"
#include "backtrace.h"
#include "dbgaux.h"
#include "dbglua.h"
#include "dcpudis.h"

uint16_t flash_size = 0;
uint16_t flash[0x10000];
list_t breakpoints;
list_t backtrace;
list_t* symbols;
extern vm_t* vm;
int ddbg_return_code;
bool ignore_next_breakpoint = false;

FILE* w;

int32_t min_int32(int32_t a, int32_t b)
{
    if (a < b)
        return a;
    else
        return b;
}

int32_t max_int32(int32_t a, int32_t b)
{
    if (a > b)
        return a;
    else
        return b;
}

void ddbg_set_write_pipe(uint16_t w_fd)
{
    w = fdopen((int)w_fd, "w");
    if (w == NULL)
        printf("error: %s\n", strerror(errno));
}

void ddbg_help(bstring section)
{
    if (biseq(section, bfromcstr("general")))
    {
        printd(LEVEL_DEFAULT, "Available commands:\n");
        printd(LEVEL_DEFAULT, "- break\n");
        printd(LEVEL_DEFAULT, "- load\n");
        printd(LEVEL_DEFAULT, "- run\n");
        printd(LEVEL_DEFAULT, "- continue\n");
        printd(LEVEL_DEFAULT, "- step\n");
        printd(LEVEL_DEFAULT, "- next\n");
        printd(LEVEL_DEFAULT, "- inspect\n");
        printd(LEVEL_DEFAULT, "- disasm\n");
        printd(LEVEL_DEFAULT, "- attach\n");
        printd(LEVEL_DEFAULT, "- detach\n");
        printd(LEVEL_DEFAULT, "- backtrace\n");
        printd(LEVEL_DEFAULT, "- quit\n");
        printd(LEVEL_DEFAULT, "Type `help <command>' to get help about a particular command.\n");
    }
    else if (biseq(section, bfromcstr("break")))
    {
        printd(LEVEL_DEFAULT, "Manipulates breakpoints.\n");
        printd(LEVEL_DEFAULT, "Available commands: add, delete, list\n");
        printd(LEVEL_DEFAULT, "Syntax: `break <command> <path>:<index>'\n");
        printd(LEVEL_DEFAULT, "Example: break add memory:0xbeef\n");
        printd(LEVEL_DEFAULT, "Example: break add test.c:23\n");
        printd(LEVEL_DEFAULT, "Note: `test.c:23` will only work correctly if you have previously loaded symbols with the `load symbols` command.\n");
    }
    else if (biseq(section, bfromcstr("load")))
    {
        printd(LEVEL_DEFAULT, "Loads words from a file to the VM.\n");
        printd(LEVEL_DEFAULT, "Syntax: `load [symbols] <file>'\n");
    }
    else if (biseq(section, bfromcstr("run")) || biseq(section, bfromcstr("continue")))
    {
        printd(LEVEL_DEFAULT, "Sets vm->halted to false, runs the VM.\n");
    }
    else if (biseq(section, bfromcstr("step")))
    {
        printd(LEVEL_DEFAULT, "Executes a single instruction, stepping into any jumps (i.e. step into).\n");
    }
    else if (biseq(section, bfromcstr("next")))
    {
        printd(LEVEL_DEFAULT, "Executes until the instruction directly following the current instruction is hit (i.e. step over).\n");
    }
    else if (biseq(section, bfromcstr("inspect")))
    {
        printd(LEVEL_DEFAULT, "Returns information about devices.\n");
        printd(LEVEL_DEFAULT, "Available commands: cpu, memory, symbols.\n");
        printd(LEVEL_DEFAULT, "Syntax: `inspect <command> [<arg1>] [<arg2>]'\n");
        printd(LEVEL_DEFAULT, "Note: `inspect memory' takes one or two arguments; the first argument is the start address and the second argument is the number of words to be dumped.\n");
    }
    else if (biseq(section, bfromcstr("disasm")))
    {
        printd(LEVEL_DEFAULT, "Disassembles loaded image.\n");
        printd(LEVEL_DEFAULT, "Syntax: `disasm [<start>] [<length>]'\n");
        printd(LEVEL_DEFAULT, "Note: If start is not provided, disassembles from 0x0.  If length is not provided, disassembles however many words were initially loaded.\n");
    }
    else if (biseq(section, bfromcstr("attach")) || biseq(section, bfromcstr("detach")))
    {
        printd(LEVEL_DEFAULT, "Attaches or detaches hardware from the debugger.\n");
        printd(LEVEL_DEFAULT, "Syntax: `attach hardware <name>'\n");
        printd(LEVEL_DEFAULT, "Syntax: `detach hardware <name>'\n");
        printd(LEVEL_DEFAULT, "Note: The supported hardware names are `lem1802', `keyboard' and `clock'.  Lua hardware can not yet be loaded.\n");
        printd(LEVEL_DEFAULT, "Note: When you specify a filename on the command-line, the debugger will automatically attach all devices.\n");
        printd(LEVEL_DEFAULT, "Note: Attaching hardware of the same kind may have unintended side effects.\n");
    }
    else if (biseq(section, bfromcstr("backtrace")))
    {
        printd(LEVEL_DEFAULT, "Experimental; shows an assembly backtrace.\n");
        printd(LEVEL_DEFAULT, "Syntax: `bt'\n");
        printd(LEVEL_DEFAULT, "Note: Due to the nature of assembly code, backtrace often generates lots of redundant entries.  It is likely to be moved into a module in the future.\n");
    }
    else if (biseq(section, bfromcstr("quit")))
    {
        printd(LEVEL_DEFAULT, "Gracefully quits the debugger.\n");
    }
}

list_t* ddbg_get_symbols(uint16_t addr)
{
    unsigned int i;
    struct dbg_sym* sym;
    struct dbg_sym_payload_line* payload_line;
    struct dbg_sym_payload_string* payload_string;
    struct dbg_sym_payload_label* payload_label;
    list_t* result = malloc(sizeof(list_t));
    list_init(result);

    if (symbols != NULL)
    {
        for (i = 0; i < list_size(symbols); i++)
        {
            sym = list_get_at(symbols, i);
            switch (sym->type)
            {
                case DBGFMT_SYMBOL_LINE:
                    payload_line = (struct dbg_sym_payload_line*)sym->payload;
                    if (payload_line->address == addr)
                        list_append(result, bformat("lineinfo:%s:%u", payload_line->path->data, payload_line->lineno));
                    break;
                case DBGFMT_SYMBOL_STRING:
                    payload_string = (struct dbg_sym_payload_string*)sym->payload;
                    if (payload_string->address == addr)
                        list_append(result, bfromcstr(payload_string->data->data));
                    break;
                case DBGFMT_SYMBOL_LABEL:
                    payload_label = (struct dbg_sym_payload_label*)sym->payload;
                    if (payload_label->address == addr)
                        list_append(result, bformat("label:%s", payload_label->label->data));
                    break;
                default:
                    break;
            }
        }
    }

    return result;
}

void ddbg_precycle_hook(vm_t* vm, uint16_t pos, void* ud)
{
    unsigned int i = 0;
    struct breakpoint* bk;
    uint16_t op, a, b;

    // Handle any symbols that are at this cycle.
    list_t* symbols = ddbg_get_symbols(vm->pc);
    list_iterator_start(symbols);
    while (list_iterator_hasnext(symbols))
        dbg_lua_handle_hook_symbol(&lstate, NULL, bautofree((bstring)list_iterator_next(symbols)));
    list_iterator_stop(symbols);
    list_empty(symbols);
    free(symbols);

    // Handle custom Lua commands.
    dbg_lua_handle_hook(&lstate, NULL, bautofree(bfromcstr("precycle")), pos);

    // Check to see if Lua halted the VM and return if it did.
    if (vm->halted)
        return;

    // Handle breakpoints.
    if (!ignore_next_breakpoint)
    {
        for (i = 0; i < list_size(&breakpoints); i++)
        {
            bk = (struct breakpoint*)list_get_at(&breakpoints, i);

            if (vm->pc == bk->addr)
            {
                vm->halted = true;
                ignore_next_breakpoint = true;
                vm_hook_break(vm); // Required for UI to update correctly.
                if (bk->temporary)
                    list_delete_at(&breakpoints, i--);
                if (!bk->silent)
                    ddbg_disassemble(max_int32((int32_t)vm->pc - 10, 0x0), min_int32((int32_t)vm->pc + 10, 0x10000) - vm->pc);
                printd(LEVEL_DEFAULT, "Breakpoint hit at 0x%04X.\n", bk->addr);
                return;
            }
        }
    }
    ignore_next_breakpoint = false;

    // Handle backtrace.
    op = INSTRUCTION_GET_OP(vm->ram[vm->pc]);
    a = INSTRUCTION_GET_A(vm->ram[vm->pc]);
    b = INSTRUCTION_GET_B(vm->ram[vm->pc]);
    if ((op == OP_SET && b == PC) || (op == OP_NONBASIC && b == NBOP_JSR))
    {
        // FIXME: This doesn't handle every valid value correctly..
        if (a == PUSH_POP)
            list_delete_at(&backtrace, list_size(&backtrace) - 1);
        else if (a == NXT_LIT)
        {
            printd(LEVEL_DEBUG, "jumping literally from 0x%04X to 0x%04X (0x%04X).\n", vm->pc, vm->ram[vm->pc + 1], vm->pc + 1);
            list_append(&backtrace, backtrace_entry_create(vm->pc, vm->ram[vm->pc + 1]));
        }
        else if (a == NXT)
        {
            //list_append(&backtrace, backtrace_entry_create(vm->pc, vm->ram[vm->ram[vm->pc+1]]));
        }
        else
        {
            // Unhandled.
            printd(LEVEL_DEBUG, "warning: unhandled backtrace jump occurred.\n");
        }
    }
}

void ddbg_postcycle_hook(vm_t* vm, uint16_t pos, void* ud)
{
    // Handle custom Lua commands.
    dbg_lua_handle_hook(&lstate, NULL, bautofree(bfromcstr("postcycle")), pos);
}

void ddbg_write_hook(vm_t* vm, uint16_t pos, void* ud)
{
    // Handle custom Lua commands.
    dbg_lua_handle_hook(&lstate, NULL, bautofree(bfromcstr("write")), pos);
}

void ddbg_break_hook(vm_t* vm, uint16_t pos, void* ud)
{
    // Handle custom Lua commands.
    dbg_lua_handle_hook(&lstate, NULL, bautofree(bfromcstr("break")), pos);
}

void ddbg_interrupt_hook(vm_t* vm, uint16_t pos, void* ud)
{
    // Handle custom Lua commands.
    dbg_lua_handle_hook(&lstate, NULL, bautofree(bfromcstr("interrupt")), pos);
}

void ddbg_send_vm_state(vm_t* vm, uint16_t pos, void* ud)
{
#ifdef JDIEZ_IS_THIS_NEEDED
    FILE* output = (w == NULL) ? stderr : w;

    fprintf(output, "vm:%u:%u:%u:%u:%u:%u:%u:%u:%u:%u:%u:%u",
            vm->registers[REG_A],
            vm->registers[REG_B],
            vm->registers[REG_C],
            vm->registers[REG_X],
            vm->registers[REG_Y],
            vm->registers[REG_Z],
            vm->registers[REG_I],
            vm->registers[REG_J],
            vm->pc,
            vm->sp,
            vm->ia,
            vm->ex);

    fflush(output);
#endif
}


void ddbg_hardware_change_hook(vm_t* vm, uint16_t id, void* ud)
{
#ifdef JDIEZ_IS_THIS_NEEDED
    FILE* output = (w == NULL) ? stderr : w;
    hw_t device = vm_hw_get_device(vm, id);

    // This switch uses curly braces because I need to be able to declare variables
    // there, since I don't know the type of the struct without checking the ID.

    fprintf(output, "hw:");
    switch (device.id)
    {
        case LEM1802_ID:
        {
            struct lem1802_hardware* hw = (struct lem1802_hardware*) ud;
            fprintf(output, "%d:%04x:%u:%u:%u:%u\n",
                    id, LEM1802_ID,
                    hw->palette_location,
                    hw->font_location,
                    hw->screen_location,
                    hw->border_color);
            break;
        }
        case TIMER_ID:
        {
            struct timer_hardware* hw = (struct timer_hardware*) ud;
            fprintf(output, "%d:%04x:%u:%u\n",
                    id, TIMER_ID,
                    hw->clock_target,
                    hw->message);
            break;
        }
    }


    fflush(output);
#endif
}

void ddbg_set(bstring object, bstring value)
{
    if (biseq(object, bfromcstr("vm_debug")))
    {
        vm->debug = (biseq(value, bfromcstr("on"))) ? true : false;
        printd(LEVEL_DEFAULT, "Debugging set to %d.\n", vm->debug);
    }
}

void ddbg_load(bstring path)
{
    FILE* load;
    unsigned int i = 0;
    bool uread = true;

    load = fopen(path->data, "rb");

    if (load == NULL)
    {
        printd(LEVEL_DEFAULT, "Unable to load %s from disk.\n", path->data);
        return;
    }

    for (i = 0; i < 0x10000 && !feof(load); i++)
        iread(&flash[i], load);
    fclose(load);

    printd(LEVEL_DEFAULT, "Loaded 0x%04X words from %s.\n", i, path->data);
    flash_size = i;
    ignore_next_breakpoint = false;
}

vm_t* _dbg_lua_get_vm()
{
    return vm;
}

void _dbg_lua_break()
{
    // Halt virtual machine.
    vm->halted = true;
    ignore_next_breakpoint = false;
}

void _dbg_lua_run()
{
    // Un-halt virtual machine.
    vm->halted = false;
    ignore_next_breakpoint = false;
}

list_t* _dbg_lua_get_symbols()
{
    return symbols;
}

void ddbg_init(host_context_t* context)
{
    // Initialize Lua.
    lstate.get_vm = _dbg_lua_get_vm;
    lstate.dbg_lua_break = _dbg_lua_break;
    lstate.dbg_lua_run = _dbg_lua_run;
    lstate.dbg_lua_get_symbols = _dbg_lua_get_symbols;

    w = NULL;

    dbg_lua_init();

    // Create VM.
    ddbg_create_vm();
    printf("%p\n", context);
    vm->host = context;
}

void ddbg_create_vm()
{
    breakpoints = breakpoint_list_create();
    list_init(&backtrace);
    list_attributes_copy(&backtrace, list_meter_uint16_t, 1);
    ignore_next_breakpoint = false;
    vm = vm_create();
    vm_hook_register(vm, &ddbg_precycle_hook, HOOK_ON_PRE_CYCLE, NULL);
    vm_hook_register(vm, &ddbg_postcycle_hook, HOOK_ON_POST_CYCLE, NULL);
    vm_hook_register(vm, &ddbg_write_hook, HOOK_ON_WRITE, NULL);
    vm_hook_register(vm, &ddbg_break_hook, HOOK_ON_BREAK, NULL);
    vm_hook_register(vm, &ddbg_interrupt_hook, HOOK_ON_INTERRUPT, NULL);

    vm_hook_register(vm, &ddbg_hardware_change_hook, HOOK_ON_HARDWARE_CHANGE, NULL);
    vm_hook_register(vm, &ddbg_send_vm_state, HOOK_ON_60HZ, NULL);
    printd(LEVEL_DEFAULT, "Created VM.\n");
}

void ddbg_flash_vm()
{
    vm_flash(vm, flash);
    ignore_next_breakpoint = false;
    printd(LEVEL_DEFAULT, "Flashed memory.\n");
}

void ddbg_run_vm()
{
    vm->halted = false;
    vm->sleep_cycles = 0;
    dbg_lua_handle_hook(&lstate, NULL, bautofree(bfromcstr("run")), 0);
    vm_execute(vm, NULL);
    printd(LEVEL_DEFAULT, "\n");
}

void ddbg_continue_vm()
{
    vm->halted = false;
    vm->sleep_cycles = 0;
    dbg_lua_handle_hook(&lstate, NULL, bautofree(bfromcstr("continue")), 0);
    vm_execute(vm, NULL);
    printd(LEVEL_DEFAULT, "\n");
}

void ddbg_attach(bstring hw)
{
    if (biseq(hw, bfromcstr("lem1802")))
        vm_hw_lem1802_init(vm);
    else if (biseq(hw, bfromcstr("keyboard")))
        // TODO: keyboard
        vm_hw_lem1802_init(vm);
    else if (biseq(hw, bfromcstr("clock")))
        vm_hw_timer_init(vm);
    else if (biseq(hw, bfromcstr("m35fd")))
        vm_hw_m35fd_init(vm);
    else
        printd(LEVEL_DEFAULT, "Unrecognized hardware.\n");
}

int32_t ddbg_file_to_address(bstring file, int index)
{
    unsigned int i;
    struct dbg_sym* sym;
    struct dbg_sym_payload_line* payload_line;

    if (symbols != NULL)
    {
        // Search through our debugging symbols.
        for (i = 0; i < list_size(symbols); i++)
        {
            sym = list_get_at(symbols, i);
            switch (sym->type)
            {
                case DBGFMT_SYMBOL_LINE:
                    payload_line = (struct dbg_sym_payload_line*)sym->payload;
                    if (binstr(payload_line->path, 0, file) != BSTR_ERR && payload_line->lineno == index)
                    {
                        // The filename and line number matches, we have found
                        // our symbol entry.
                        printd(LEVEL_DEFAULT, "Line information: %s:%u is at 0x%04X\n", payload_line->path->data, payload_line->lineno, payload_line->address);
                        return payload_line->address;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // If we don't find a memory address, we return -1.
    return -1;
}

int32_t ddbg_label_to_address(bstring label)
{
    unsigned int i;
    struct dbg_sym* sym;
    struct dbg_sym_payload_label* payload_label;

    if (symbols != NULL)
    {
        // Search through our debugging symbols.
        for (i = 0; i < list_size(symbols); i++)
        {
            sym = list_get_at(symbols, i);
            switch (sym->type)
            {
                case DBGFMT_SYMBOL_LABEL:
                    payload_label = (struct dbg_sym_payload_label*)sym->payload;
                    if (biseq(payload_label->label, label))
                    {
                        // The label matches, we have found our symbol entry.
                        printd(LEVEL_DEFAULT, "Label information: %s is at 0x%04X\n", payload_label->label->data, payload_label->address);
                        return payload_label->address;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // If we don't find a memory address, we return -1.
    return -1;
}

void ddbg_add_breakpoint(bstring file, int index)
{
    int32_t memory;

    if (!biseq(file, bfromcstr("memory")))
    {
        memory = ddbg_file_to_address(file, index);
    }
    else
    {
        if (index < 0)
        {
            printd(LEVEL_DEFAULT, "Memory address must be greater than 0.\n");
            memory = -1;
        }
        else
        {
            memory = index;
        }
    }

    // Did we get a valid result?
    if (memory == -1)
    {
        printd(LEVEL_DEFAULT, "Unable to resolve specified symbol.\n");
        return;
    }

    list_append(&breakpoints, breakpoint_create(memory, false, false));
    printd(LEVEL_DEFAULT, "Breakpoint added at 0x%04X.\n", memory);
}

void ddbg_delete_breakpoint(bstring file, int index)
{
    unsigned int i = 0;
    bool found = false;
    int32_t memory = ddbg_file_to_address(file, index);
    struct breakpoint* bk;

    // Did we get a valid result?
    if (memory == -1)
    {
        printd(LEVEL_DEFAULT, "Unable to resolve specified symbol.\n");
        return;
    }

    for (i = 0; i < list_size(&breakpoints); i++)
    {
        bk = (struct breakpoint*)list_get_at(&breakpoints, i);

        if (bk->addr == memory)
        {
            list_delete_at(&breakpoints, i--);
            found = true;
        }
    }

    if (found == true)
        printd(LEVEL_DEFAULT, "Breakpoint removed at 0x%04X.\n", memory);
    else
        printd(LEVEL_DEFAULT, "There was no breakpoint at %s:%d.\n", file->data, index);
}

void ddbg_add_breakpoint_identifier(bstring ident)
{
    // TODO: In the future when C functions are supported
    // in the debugging symbol format, we should probably
    // allow setting breakpoints by C function name as well.
    int32_t memory = ddbg_label_to_address(ident);

    // Did we get a valid result?
    if (memory == -1)
    {
        printd(LEVEL_DEFAULT, "Unable to resolve specified symbol.\n");
        return;
    }

    list_append(&breakpoints, breakpoint_create(memory, false, false));
    printd(LEVEL_DEFAULT, "Breakpoint added at 0x%04X.\n", memory);
}

void ddbg_delete_breakpoint_identifier(bstring ident)
{
    // TODO: In the future when C functions are supported
    // in the debugging symbol format, we should probably
    // allow removing breakpoints by C function name as well.
    unsigned int i = 0;
    bool found = false;
    int32_t memory = ddbg_label_to_address(ident);
    struct breakpoint* bk;

    // Did we get a valid result?
    if (memory == -1)
    {
        printd(LEVEL_DEFAULT, "Unable to resolve specified symbol.\n");
        return;
    }

    for (i = 0; i < list_size(&breakpoints); i++)
    {
        bk = (struct breakpoint*)list_get_at(&breakpoints, i);

        if (bk->addr == memory)
        {
            list_delete_at(&breakpoints, i--);
            found = true;
        }
    }

    if (found == true)
        printd(LEVEL_DEFAULT, "Breakpoint removed at 0x%04X.\n", memory);
    else
        printd(LEVEL_DEFAULT, "There was no breakpoint on label %s.\n", ident->data);
}

void ddbg_step_into()
{
    vm->sleep_cycles = 0;
    vm_cycle(vm);

    // Handle custom Lua commands.
    dbg_lua_handle_hook(&lstate, NULL, bautofree(bfromcstr("step")), 0);
}

void ddbg_step_over()
{
    uint16_t inst, op_a, op_b, offset = 1, bp;
    inst = INSTRUCTION_GET_OP(vm->ram[vm->pc]);
    op_a = INSTRUCTION_GET_A(vm->ram[vm->pc]);
    op_b = INSTRUCTION_GET_B(vm->ram[vm->pc]);
    vm->sleep_cycles = 0;

    if (op_a == NXT)
        offset += 1;
    if (op_a == NXT_LIT)
        offset += 1;

    if (op_b == NXT)
        offset += 1;
    if (op_b == NXT_LIT)
        offset += 1;

    if (inst == NBOP_RESERVED)
    {
        if (op_b == NBOP_JSR)
        {
            bp = op_a;
        }
    }
    else
    {
        bp = vm->pc + offset;
    }

    list_append(&breakpoints, breakpoint_create(bp, true, true));
    vm->halted = false;
    vm_execute(vm, NULL);

    // Handle custom Lua commands.
    dbg_lua_handle_hook(&lstate, NULL, bautofree(bfromcstr("next")), 0);
}

void ddbg_backtrace()
{
    int i;
    struct backtrace_entry* entry;

    printd(LEVEL_DEFAULT, "current backtrace:\n");

    for (i = list_size(&backtrace) - 1; i >= 0; i--)
    {
        entry = (struct backtrace_entry*)list_get_at(&backtrace, i);
        printd(LEVEL_DEFAULT, "  0x%04X called 0x%04X\n", entry->caller, entry->callee);
    }
}

void ddbg_breakpoints_list()
{
    unsigned int i = 0;

    printd(LEVEL_DEFAULT, "%d breakpoints loaded.\n", list_size(&breakpoints));

    for (i = 0; i < list_size(&breakpoints); i++)
        printd(LEVEL_DEFAULT, "- at 0x%04X\n", ((struct breakpoint*)list_get_at(&breakpoints, i))->addr);
}

void ddbg_dump_state()
{
    printd(LEVEL_DEFAULT, "A:   0x%04X   [A]:   0x%04X\n", vm->registers[REG_A], vm->ram[vm->registers[REG_A]]);
    printd(LEVEL_DEFAULT, "B:   0x%04X   [B]:   0x%04X\n", vm->registers[REG_B], vm->ram[vm->registers[REG_B]]);
    printd(LEVEL_DEFAULT, "C:   0x%04X   [C]:   0x%04X\n", vm->registers[REG_C], vm->ram[vm->registers[REG_C]]);
    printd(LEVEL_DEFAULT, "X:   0x%04X   [X]:   0x%04X\n", vm->registers[REG_X], vm->ram[vm->registers[REG_X]]);
    printd(LEVEL_DEFAULT, "Y:   0x%04X   [Y]:   0x%04X\n", vm->registers[REG_Y], vm->ram[vm->registers[REG_Y]]);
    printd(LEVEL_DEFAULT, "Z:   0x%04X   [Z]:   0x%04X\n", vm->registers[REG_Z], vm->ram[vm->registers[REG_Z]]);
    printd(LEVEL_DEFAULT, "I:   0x%04X   [I]:   0x%04X\n", vm->registers[REG_I], vm->ram[vm->registers[REG_I]]);
    printd(LEVEL_DEFAULT, "J:   0x%04X   [J]:   0x%04X\n", vm->registers[REG_J], vm->ram[vm->registers[REG_J]]);
    printd(LEVEL_DEFAULT, "PC:  0x%04X   SP:    0x%04X\n", vm->pc, vm->sp);
    printd(LEVEL_DEFAULT, "EX:  0x%04X   IA:    0x%04X\n", vm->ex, vm->ia);
    if (vm->queue_interrupts)
        printd(LEVEL_DEFAULT, "IRQ ENABLED:       true\n");
    else
        printd(LEVEL_DEFAULT, "IRQ ENABLED:      false\n");
    printd(LEVEL_DEFAULT, "IRQ COUNT:       0x%04X\n", vm->irq_count);
    printd(LEVEL_DEFAULT, "CYCLES TO SLEEP: 0x%04X\n", vm->sleep_cycles);
}

void ddbg_dump_ram(int start, int difference)
{
    int i = 0;

    if (start < 0 || difference < 0)
    {
        printd(LEVEL_DEFAULT, "Invalid parameters provided to 'inspect memory'.");
        return;
    }

    if (difference == 0)
        difference = 32;

    if ((start + difference) > 0xffff)
    {
        printd(LEVEL_DEFAULT, "Memory out of bounds.\n");
        return;
    }

    for (i = 0; i < difference; i++)
    {
        if (i % 8 == 0)
            printd(LEVEL_DEFAULT, "%04X: ", start + i);

        printd(LEVEL_DEFAULT, "%04X ", vm->ram[start + i]);

        if ((i + 1) % 8 == 0)
            printd(LEVEL_DEFAULT, "\n");
    }

    printd(LEVEL_DEFAULT, "\n");
}

void ddbg_disassemble_default()
{
    // The parser can't access the VM instance, so we use
    // this function to default to printing the assembly
    // around the current instruction.
    ddbg_disassemble(max_int32((int32_t)vm->pc - 10, 0x0), min_int32((int32_t)vm->pc + 10, 0x10000) - vm->pc);
}

void ddbg_disassemble(int start, int difference)
{
    int i = 0;
    unsigned int ii = 0;
    bool found = false;
    struct inst inst;
    struct dbg_sym* sym;
    struct dbg_sym_payload_line* payload_line;
    struct dbg_sym_payload_label* payload_label;

    if (start < 0 || difference < 0)
    {
        printd(LEVEL_DEFAULT, "Invalid parameters provided to 'disassemble'.");
        return;
    }

    if (difference == 0)
        difference = (flash_size - start);

    while (i < difference)
    {
        // Show symbols.
        if (symbols != NULL)
        {
            for (ii = 0; ii < list_size(symbols); ii++)
            {
                sym = list_get_at(symbols, ii);
                switch (sym->type)
                {
                    case DBGFMT_SYMBOL_LINE:
                        payload_line = (struct dbg_sym_payload_line*)sym->payload;
                        if (payload_line->address == start + i)
                        {
                            found = true;
                            printd(LEVEL_DEFAULT, "0x%04X (0x%04X) (%s:%d):\n", start + i, vm->ram[start + i], payload_line->path->data, payload_line->lineno);

                        }
                        break;
                    case DBGFMT_SYMBOL_LABEL:
                        payload_label = (struct dbg_sym_payload_label*)sym->payload;
                        if (payload_label->address == start + i)
                        {
                            found = true;
                            printd(LEVEL_DEFAULT, "0x%04X (0x%04X) (%s):\n", start + i, vm->ram[start + i], payload_label->label->data);

                        }
                        break;
                }
            }
            if (!found) printd(LEVEL_DEFAULT, "0x%04X (0x%04X):\n", start + i, vm->ram[start + i]);
        }
        else
            printd(LEVEL_DEFAULT, "0x%04X (0x%04X): ", start + i, vm->ram[start + i]);

        // Disassemble.
        inst = vm_disassemble(vm, start + i, true);
        if (symbols != NULL)
            printd(LEVEL_DEFAULT, "    ");
        if (vm->pc == start + i)
            printd(LEVEL_DEFAULT, " >>> ");
        else
            printd(LEVEL_DEFAULT, "     ");
        if (inst.original.full == 0x0)
            printd(LEVEL_DEFAULT, "<null>\n");
        else if (inst.pretty.op == NULL)
            printd(LEVEL_DEFAULT, "???\n");
        else if (inst.pretty.b == NULL)
            printd(LEVEL_DEFAULT, "%s %s\n", inst.pretty.op->data, inst.pretty.a->data);
        else
            printd(LEVEL_DEFAULT, "%s %s, %s\n", inst.pretty.op->data, inst.pretty.b->data, inst.pretty.a->data);
        i += inst.size;
    }

    printd(LEVEL_DEFAULT, "\n");
}


void ddbg_load_symbols(bstring path)
{
    // TODO: Free existing symbols if
    // symbols is not NULL.

    // Load symbols.
    symbols = dbgfmt_read(path, true);

    if (symbols != NULL)
        printd(LEVEL_DEFAULT, "Loaded symbols from %s.\n", path->data);
    else
        printd(LEVEL_DEFAULT, "Failed to load symbols from %s.\n", path->data);
}

void ddbg_inspect_symbols()
{
    unsigned int i;
    struct dbg_sym* sym;
    struct dbg_sym_payload_line* payload_line;
    struct dbg_sym_payload_string* payload_string;
    struct dbg_sym_payload_label* payload_label;

    // Check to see if no symbols are loaded.
    if (symbols == NULL)
        printd(LEVEL_DEFAULT, "No symbols are loaded.\n");
    else
    {
        // Print out a list of symbols.
        printd(LEVEL_DEFAULT, "%u symbols are loaded.\n", list_size(symbols));
        for (i = 0; i < list_size(symbols); i++)
        {
            sym = list_get_at(symbols, i);
            switch (sym->type)
            {
                case DBGFMT_SYMBOL_LINE:
                    payload_line = (struct dbg_sym_payload_line*)sym->payload;
                    printd(LEVEL_DEFAULT, "0x%04X: [  line] %s:%u\n", payload_line->address, payload_line->path->data, payload_line->lineno);
                    break;
                case DBGFMT_SYMBOL_STRING:
                    payload_string = (struct dbg_sym_payload_string*)sym->payload;
                    printd(LEVEL_DEFAULT, "0x%04X: [string] %s\n", payload_string->address, payload_string->data->data);
                    break;
                case DBGFMT_SYMBOL_LABEL:
                    payload_label = (struct dbg_sym_payload_label*)sym->payload;
                    printd(LEVEL_DEFAULT, "0x%04X: [ label] %s\n", payload_label->address, payload_label->label->data);
                    break;
                default:
                    break;
            }
        }
    }
}
