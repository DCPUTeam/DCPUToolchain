///
/// @addtogroup LibDCPU-VM
///
/// @file
/// @brief  Declares macros and functions used to host the interrupt clock (generic clock).
/// @sa     http://dcpu.com/dcpu-16/
/// @author James Rhodes
/// @author Aaron Miller
///

#define PRIVATE_VM_ACCESS

#include <stdio.h>
#include <stdlib.h>
#include <debug.h>
#include "vm.h"
#include "dcpuops.h"
#include "dcpuhook.h"
#include "hw.h"

#define VM_CHECK_ARITHMETIC_FLOW(op, val_a, val_b) \
    if ((int32_t)val_a op (int32_t)val_b < (int32_t)0) \
    vm->ex = AR_UNDERFLOW; \
    else if ((int32_t)val_a op (int32_t)val_b > (int32_t)AR_MAX) \
    vm->ex = AR_OVERFLOW; \
    else \
    vm->ex = AR_NOFLOW;

#define VM_CHECK_ARITHMETIC_FLOW_EX(op, val_a, val_b, val_ex) \
    if ((int32_t)val_a op (int32_t)val_b + (int32_t)val_ex < (int32_t)0) \
    vm->ex = AR_UNDERFLOW; \
    else if ((int32_t)val_a op (int32_t)val_b + (int32_t)val_ex > (int32_t)AR_MAX) \
    vm->ex = AR_OVERFLOW; \
    else \
    vm->ex = AR_NOFLOW;

#define VM_SKIP_RESET if(vm->skip) {vm->skip = false; return;}
#define VM_BRANCHING_SKIP if(vm->skip) { return; }

#define VM_HOOK_FIRE(var) \
    if (var >= (uint16_t*)&vm->ram && var < (uint16_t*)&vm->ram + 0x10000) \
    vm_hook_fire(vm, (uint16_t)(var - (uint16_t*)&vm->ram), HOOK_ON_WRITE, NULL);

#define OP_NUM_CYCLES(count) vm->sleep_cycles += (count - 1);

uint16_t* vm_internal_get_store(vm_t* vm, uint16_t loc, uint8_t pos)
{
    uint16_t t;

    // Don't modify the state of the program if
    // we're skipping.
    if (vm->skip)
    {
        if ((loc >= NXT_VAL_A && loc <= NXT_VAL_J) || loc == NXT)
            vm_consume_word(vm);

        return &vm->dummy;
    }

    // Otherwise, run normally.
    if (loc >= REG_A && loc <= REG_J)
        return &vm->registers[loc];
    else if (loc >= VAL_A && loc <= VAL_J)
        return &vm->ram[vm->registers[loc - VAL_A]];
    else if (loc >= NXT_VAL_A && loc <= NXT_VAL_J)
    {
        vm->sleep_cycles += 1; // Resolving this costs an additional cycle
        return &vm->ram[(uint16_t)(vm->registers[loc - NXT_VAL_A] + vm_consume_word(vm))];
    }
    else if (loc == PUSH_POP)
    {
        if (pos == POS_A)
            t = vm->sp++;
        else if (pos == POS_B)
            t = --vm->sp;
        else
            t = vm->sp;

        return &vm->ram[t];
    }
    else if (loc == PEEK)
        return &vm->ram[vm->sp];
    else if (loc == PICK)
    {
        vm->sleep_cycles += 1; // Resolving this costs an additional cycle
        return &vm->ram[(uint16_t)(vm->sp + vm_consume_word(vm))];
    }
    else if (loc == IA)
        return &vm->ia;
    else if (loc == SP)
        return &vm->sp;
    else if (loc == PC)
        return &vm->pc;
    else if (loc == EX)
        return &vm->ex;
    else if (loc == NXT)
    {
        vm->sleep_cycles += 1; //Resolving this costs an additional cycle
        return &vm->ram[vm_consume_word(vm)];
    }
    else
        return &vm->dummy; // Dummy position for assignments that silently fail.
}

// Sometimes an instruction will get the value of 'a' for a second
// time (such as in the case of ADD).  We need to make sure that
// if the value of 'a' modified the VM state in vm_internal_get_store
// that we don't modify the state again (for example, not incrementing
// PC for NXT).
uint16_t vm_resolve_value_once(vm_t* vm, uint16_t val, uint8_t pos)
{
    switch (val)
    {
        case NXT:
            return vm->ram[(uint16_t)vm->ram[(uint16_t)(vm->pc - 1)]];

        case PUSH_POP:
            return vm->ram[vm->sp];

        case PICK:
            return vm->ram[(uint16_t)(vm->sp + vm->ram[(uint16_t)(vm->pc - 1)])];

        case NXT_VAL_A:
        case NXT_VAL_B:
        case NXT_VAL_C:
        case NXT_VAL_X:
        case NXT_VAL_Y:
        case NXT_VAL_Z:
        case NXT_VAL_I:
        case NXT_VAL_J:
            return vm->ram[(uint16_t)(vm->registers[val - NXT_VAL_A] + vm->ram[(uint16_t)(vm->pc - 1)])];

        default:
            return vm_resolve_value(vm, val, pos);
    }
}

void vm_op_fire(vm_t* vm)
{
    static int screen_id = -1;
    uint16_t count = vm_hw_count(vm), i = 0;
    hw_t* device;

    if (screen_id == -1)
    {
        for (i = 0; i < count; i++)
        {
            device = vm_hw_get_device(vm, i);
            if (device != NULL && device->id == 0x7349F615)
            {
                vm->registers[REG_A] = 0;
                vm->registers[REG_B] = 0x1000;
                vm_hw_interrupt(vm, i);

                vm->registers[REG_A] = 1;
                vm->registers[REG_B] = 0x1000 + 0x17F;
                vm_hw_interrupt(vm, i);

                screen_id = i;
                break;
            }
        }
    }

    if (vm->fire_cycles < vm->fire_cycles_target)
    {
        vm->fire_cycles++;
        return;
    }
    else
    {
        uint16_t pos = 0x1000, length = 0, fill = 0, idx = 0;

        vm->fire_cycles = 0;
        vm->fire_cycles_target = DCPU_TICKS_KHZ * 1000;

        while (pos < 0x1000 + 0x300)
        {
            length = rand() % 100;
            fill = rand() % 0x10000;

            for (idx = 0; idx < length; idx++)
            {
                vm->ram[pos + idx] = fill;
                vm_hook_fire(vm, pos + idx, HOOK_ON_WRITE, NULL);
            }

            if (pos + length < pos)
                break;
            pos += length;
        }
    }
}

void vm_op_radiation(vm_t* vm)
{
    if (vm->radiation_cycles < vm->radiation_cycles_target)
    {
        vm->radiation_cycles++;
        return;
    }
    else
    {
        uint16_t pos = rand() % 0x10000, bit = rand() % 15;
        vm->radiation_cycles = 0;
        vm->radiation_cycles_target = DCPU_TICKS_KHZ * vm->radiation_factor * (rand() % 10);

        vm->ram[pos] = vm->ram[pos] ^ (1 << bit);
        vm_hook_fire(vm, pos, HOOK_ON_WRITE, NULL);
    }
}

void vm_op_set(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    OP_NUM_CYCLES(1);

    VM_SKIP_RESET;
    *store_b = val_a;
    VM_HOOK_FIRE(store_b);
    vm->skip = false;
}

void vm_op_add(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(2);

    VM_SKIP_RESET;
    *store_b = val_b + val_a;
    VM_CHECK_ARITHMETIC_FLOW(+, val_b, val_a);
    VM_HOOK_FIRE(store_b);
}

void vm_op_sub(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(2);

    VM_SKIP_RESET;
    *store_b = val_b - val_a;
    VM_CHECK_ARITHMETIC_FLOW(-, val_b, val_a);
    VM_HOOK_FIRE(store_b);
}

void vm_op_mul(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(2);

    VM_SKIP_RESET;
    *store_b = val_b * val_a;
    vm->ex = ((val_b * val_a) >> 16) & 0xffff;
    VM_HOOK_FIRE(store_b);
}

void vm_op_mli(vm_t* vm, uint16_t b, uint16_t a)
{
    int16_t val_a;
    int16_t val_b;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(2);

    VM_SKIP_RESET;
    *store_b = val_b * val_a;
    vm->ex = ((val_b * val_a) >> 16) & 0xffff;
    VM_HOOK_FIRE(store_b);
}

void vm_op_div(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(3);

    VM_SKIP_RESET;

    if (val_a != 0)
    {
        *store_b = val_b / val_a;
        vm->ex = ((val_b << 16) / val_a) & 0xffff;
    }
    else
    {
        *store_b = 0;
        vm->ex = 0;
    }

    VM_HOOK_FIRE(store_b);
}

void vm_op_dvi(vm_t* vm, uint16_t b, uint16_t a)
{
    int16_t val_a;
    int16_t val_b;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(3);

    VM_SKIP_RESET;

    if (val_a != 0)
    {
        *store_b = val_b / val_a;
        vm->ex = ((val_b << 16) / val_a) & 0xffff;
    }
    else
    {
        *store_b = 0;
        vm->ex = 0;
    }

    VM_HOOK_FIRE(store_b);
}

void vm_op_mdi(vm_t* vm, uint16_t b, uint16_t a)
{
    int16_t val_a;
    int16_t val_b;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(3);

    VM_SKIP_RESET;

    if (val_a == 0)
        *store_b = 0;
    else
        *store_b = val_b % val_a;

    VM_HOOK_FIRE(store_b);
}

void vm_op_mod(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(3);

    VM_SKIP_RESET;

    if (val_a != 0)
        *store_b = val_b % val_a;
    else
        *store_b = 0;

    VM_HOOK_FIRE(store_b);
}


void vm_op_and(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(1);

    VM_SKIP_RESET;
    *store_b = val_b & val_a;
    VM_HOOK_FIRE(store_b);
}

void vm_op_bor(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(1);

    VM_SKIP_RESET;
    *store_b = val_b | val_a;
    VM_HOOK_FIRE(store_b);
}

void vm_op_xor(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(1);

    VM_SKIP_RESET;
    *store_b = val_b ^ val_a;
    VM_HOOK_FIRE(store_b);
}

void vm_op_shr(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(1);

    VM_SKIP_RESET;
    *store_b = val_b >> val_a;
    vm->ex = ((val_b << 16) >> val_a) & 0xffff;
    VM_HOOK_FIRE(store_b);
}

void vm_op_asr(vm_t* vm, uint16_t b, uint16_t a)
{
    // TODO: This may not infact be correct.  C uses
    // arithmetic shifts if the left-hand value is
    // signed, however, we still need to make sure that the
    // excess register is completely compliant.
    int16_t val_b;
    int16_t val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(1);

    VM_SKIP_RESET;
    *store_b = val_b >> val_a;
    vm->ex = ((val_b << 16) >> val_a) & 0xffff;
    VM_HOOK_FIRE(store_b);
}

void vm_op_shl(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(1);

    VM_SKIP_RESET;
    *store_b = val_b << val_a;
    vm->ex = ((val_b << val_a) >> 16) & 0xffff;
    VM_HOOK_FIRE(store_b);
}

void vm_op_ifb(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    val_a = vm_resolve_value(vm, a, POS_A);
    val_b = vm_resolve_value(vm, b, POS_B);

    VM_BRANCHING_SKIP;
    vm->skip = !((val_b & val_a) != 0);
}

void vm_op_ifc(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    val_a = vm_resolve_value(vm, a, POS_A);
    val_b = vm_resolve_value(vm, b, POS_B);

    VM_BRANCHING_SKIP;
    vm->skip = !((val_b & val_a) == 0);
}

void vm_op_ife(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    val_a = vm_resolve_value(vm, a, POS_A);
    val_b = vm_resolve_value(vm, b, POS_B);

    VM_BRANCHING_SKIP;
    vm->skip = !(val_b == val_a);
}

void vm_op_ifn(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    val_a = vm_resolve_value(vm, a, POS_A);
    val_b = vm_resolve_value(vm, b, POS_B);

    VM_BRANCHING_SKIP;
    vm->skip = !(val_b != val_a);
}

void vm_op_ifg(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    val_a = vm_resolve_value(vm, a, POS_A);
    val_b = vm_resolve_value(vm, b, POS_B);

    VM_BRANCHING_SKIP;
    vm->skip = !(val_b > val_a);
}

void vm_op_ifa(vm_t* vm, uint16_t b, uint16_t a)
{
    int16_t val_b, val_a;
    val_a = (int16_t)vm_resolve_value(vm, a, POS_A);
    val_b = (int16_t)vm_resolve_value(vm, b, POS_B);

    VM_BRANCHING_SKIP;
    vm->skip = !(val_b > val_a);
}

void vm_op_ifl(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a;
    val_b = vm_resolve_value(vm, b, POS_B);
    val_a = vm_resolve_value(vm, a, POS_A);

    VM_BRANCHING_SKIP;
    vm->skip = !(val_b < val_a);
}

void vm_op_ifu(vm_t* vm, uint16_t b, uint16_t a)
{
    int16_t val_b, val_a;
    val_a = (int16_t)vm_resolve_value(vm, a, POS_A);
    val_b = (int16_t)vm_resolve_value(vm, b, POS_B);

    VM_BRANCHING_SKIP;
    vm->skip = !(val_b < val_a);
}

void vm_op_adx(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a, val_ex;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(3);

    val_ex = vm->ex;
    VM_SKIP_RESET;
    *store_b = val_b + val_a + val_ex;
    VM_CHECK_ARITHMETIC_FLOW_EX(+, val_b, val_a, val_ex);
    VM_HOOK_FIRE(store_b);
}

void vm_op_sbx(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_b, val_a, val_ex;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);
    val_b = *store_b;
    OP_NUM_CYCLES(3);

    val_ex = vm->ex;
    VM_SKIP_RESET;
    *store_b = val_b - val_a + val_ex;
    VM_CHECK_ARITHMETIC_FLOW_EX(-, val_b, val_a, val_ex);
    VM_HOOK_FIRE(store_b);
}

void vm_op_sti(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);

    VM_SKIP_RESET;
    *store_b = val_a;
    VM_HOOK_FIRE(store_b);
    vm->registers[REG_I]++;
    vm->registers[REG_J]++;
    VM_HOOK_FIRE(&vm->registers[REG_I]);
    VM_HOOK_FIRE(&vm->registers[REG_J]);
}

void vm_op_std(vm_t* vm, uint16_t b, uint16_t a)
{
    uint16_t val_a;
    uint16_t* store_b;
    val_a = vm_resolve_value(vm, a, POS_A);
    store_b = vm_internal_get_store(vm, b, POS_B);

    VM_SKIP_RESET;
    *store_b = val_a;
    VM_HOOK_FIRE(store_b);
    vm->registers[REG_I]--;
    vm->registers[REG_J]--;
    VM_HOOK_FIRE(&vm->registers[REG_I]);
    VM_HOOK_FIRE(&vm->registers[REG_J]);
}

void vm_op_jsr(vm_t* vm, uint16_t a)
{
    uint16_t val_a = vm_resolve_value(vm, a, POS_A);
    uint16_t t;
    OP_NUM_CYCLES(3);

    VM_SKIP_RESET;
    t = --vm->sp;
    vm->ram[t] = vm->pc;
    vm->pc = val_a;
}

void vm_op_hcf(vm_t* vm, uint16_t a)
{
    uint16_t val_a = vm_resolve_value(vm, a, POS_A);
    (void)val_a;
    OP_NUM_CYCLES(9);
    vm->halted = true;
}

void vm_op_int(vm_t* vm, uint16_t a)
{
    uint16_t val_a = vm_resolve_value(vm, a, POS_A);
    OP_NUM_CYCLES(4);

    printd(LEVEL_DEBUG, "sending interrupt %u\n", val_a);
    vm_interrupt(vm, val_a);
}

void vm_op_iag(vm_t* vm, uint16_t a)
{
    uint16_t* store_a = vm_internal_get_store(vm, a, POS_A);
    VM_SKIP_RESET;
    *store_a = vm->ia;
    VM_HOOK_FIRE(store_a);
}

void vm_op_ias(vm_t* vm, uint16_t a)
{
    uint16_t val_a = vm_resolve_value(vm, a, POS_A);
    VM_SKIP_RESET;
    vm->ia = val_a;
}

void vm_op_rfi(vm_t* vm, uint16_t a)
{
    uint16_t val_a = vm_resolve_value(vm, a, POS_A);
    (void)val_a;
    OP_NUM_CYCLES(3);
    VM_SKIP_RESET;
    vm->registers[REG_A] = vm->ram[vm->sp++];
    vm->pc = vm->ram[vm->sp++];
    printd(LEVEL_DEBUG, "turning off interrupt queue\n");
    vm->queue_interrupts = false;
    printd(LEVEL_DEBUG, "returning from interrupt.\n");
    VM_HOOK_FIRE(&vm->registers[REG_A]);
}

void vm_op_iaq(vm_t* vm, uint16_t a)
{
    uint16_t val_a = vm_resolve_value(vm, a, POS_A);
    OP_NUM_CYCLES(2);

    VM_SKIP_RESET;

    printd(LEVEL_DEBUG, "IAQ CALLED WITH %u\n", val_a);

    if (val_a == 0)
    {
        printd(LEVEL_DEBUG, "turning off interrupt queue\n");
        vm->queue_interrupts = false;
    }
    else
    {
        printd(LEVEL_DEBUG, "turning on interrupt queue\n");
        vm->queue_interrupts = true;
    }
}

void vm_op_hwn(vm_t* vm, uint16_t a)
{
    uint16_t* store_a = vm_internal_get_store(vm, a, POS_A);
    OP_NUM_CYCLES(2);

    VM_SKIP_RESET;

    *store_a = vm_hw_count(vm);

    VM_HOOK_FIRE(store_a);
    vm->skip = false;
}

void vm_op_hwq(vm_t* vm, uint16_t a)
{
    hw_t* queried_device;
    uint16_t* store_a = vm_internal_get_store(vm, REG_A, POS__);
    uint16_t* store_b = vm_internal_get_store(vm, REG_B, POS__);
    uint16_t* store_c = vm_internal_get_store(vm, REG_C, POS__);
    uint16_t* store_x = vm_internal_get_store(vm, REG_X, POS__);
    uint16_t* store_y = vm_internal_get_store(vm, REG_Y, POS__);

    uint16_t val_a = vm_resolve_value(vm, a, POS_A);
    OP_NUM_CYCLES(4);

    VM_SKIP_RESET;
    if (val_a < vm_hw_count(vm))
    {
        queried_device = vm_hw_get_device(vm, val_a);
        if (queried_device == NULL)
        {
            vm_halt(vm, "queried hw is null (%d)", val_a);
            vm->skip = false;
            return;
        }

        printd(LEVEL_DEBUG, "hwq: index %d %08X\n", val_a, queried_device->id);

        *store_a = (queried_device->id & 0x0000FFFF) >>  0;
        *store_b = (queried_device->id & 0xFFFF0000) >> 16;
        *store_c = queried_device->version;
        *store_x = (queried_device->manufacturer & 0x0000FFFF) >>  0;
        *store_y = (queried_device->manufacturer & 0xFFFF0000) >> 16;

        VM_HOOK_FIRE(store_a);
        VM_HOOK_FIRE(store_b);
        VM_HOOK_FIRE(store_c);
        VM_HOOK_FIRE(store_x);
        VM_HOOK_FIRE(store_y);
    }
    vm->skip = false;
}

void vm_op_hwi(vm_t* vm, uint16_t a)
{
    uint16_t val_a = vm_resolve_value(vm, a, POS_A);
    OP_NUM_CYCLES(4);

    VM_SKIP_RESET;
    vm_hw_interrupt(vm, val_a);
}
