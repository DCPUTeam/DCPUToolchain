/**

    File:       dcpuops.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-vm

    Authors:    James Rhodes
            Tyrel Haveman

    Description:    Declares functions that handle individual
            opcodes in the virtual machine.

**/

#ifndef __DCPUOPS_H
#define __DCPUOPS_H

#include "vm.h"

#ifdef PRIVATE_VM_ACCESS
uint16_t* vm_internal_get_store(vm_t* vm, uint16_t loc, uint8_t pos);
#endif

void vm_op_fire(vm_t* vm);
void vm_op_radiation(vm_t* vm);
void vm_op_set(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_add(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_sub(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_mul(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_mli(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_div(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_dvi(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_mod(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_mdi(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_and(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_bor(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_xor(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_shr(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_asr(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_shl(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_mvi(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_ifb(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_ifc(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_ife(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_ifn(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_ifg(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_ifa(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_ifl(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_ifu(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_adx(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_sbx(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_sti(vm_t* vm, uint16_t b, uint16_t a);
void vm_op_std(vm_t* vm, uint16_t b, uint16_t a);

void vm_op_jsr(vm_t* vm, uint16_t a);
void vm_op_hcf(vm_t* vm, uint16_t a);
void vm_op_int(vm_t* vm, uint16_t a);
void vm_op_iag(vm_t* vm, uint16_t a);
void vm_op_ias(vm_t* vm, uint16_t a);
void vm_op_rfi(vm_t* vm, uint16_t a);
void vm_op_iap(vm_t* vm, uint16_t a);
void vm_op_iaq(vm_t* vm, uint16_t a);
void vm_op_hwn(vm_t* vm, uint16_t a);
void vm_op_hwq(vm_t* vm, uint16_t a);
void vm_op_hwi(vm_t* vm, uint16_t a);

#endif
