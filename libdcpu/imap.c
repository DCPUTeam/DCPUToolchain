/**

    File:       imap.c

    Project:    DCPU-16 Tools
    Component:  LibDCPU

    Authors:    James Rhodes

    Description:    Defines instruction and register maps so that
            the assembler can turn character values into
            their internal representations.

**/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "posix.h"
#include "imap.h"
#include "dcpu.h"

#define OPCODE(op) { #op, OP_ ## op, 0 }
#define NBOPCODE(op) { #op, OP_NONBASIC, NBOP_ ## op }
#define REGISTER(v) { #v, REG_ ## v }
#define REGISTER_VALUE(v) { #v, VAL_ ## v }
#define REGISTER_VALUE_NEXT(v) { #v, NXT_VAL_ ## v }

struct instruction_mapping instruction_name_map[] =
{
    OPCODE(SET),
    OPCODE(ADD),
    OPCODE(SUB),
    OPCODE(MUL),
    OPCODE(MLI),
    OPCODE(DIV),
    OPCODE(DVI),
    OPCODE(MOD),
    OPCODE(MDI),
    OPCODE(AND),
    OPCODE(BOR),
    OPCODE(XOR),
    OPCODE(SHR),
    OPCODE(ASR),
    OPCODE(SHL),
    OPCODE(IFB),
    OPCODE(IFC),
    OPCODE(IFE),
    OPCODE(IFN),
    OPCODE(IFG),
    OPCODE(IFA),
    OPCODE(IFL),
    OPCODE(IFU),
    OPCODE(ADX),
    OPCODE(SBX),
    OPCODE(STI),
    OPCODE(STD),
    NBOPCODE(JSR),
    NBOPCODE(INT),
    NBOPCODE(IAG),
    NBOPCODE(IAS),
    NBOPCODE(RFI),
    NBOPCODE(IAQ),
    NBOPCODE(HWN),
    NBOPCODE(HWQ),
    NBOPCODE(HWI),
    NULL
};

struct register_mapping register_name_map[] =
{
    REGISTER(A),
    REGISTER(B),
    REGISTER(C),
    REGISTER(X),
    REGISTER(Y),
    REGISTER(Z),
    REGISTER(I),
    REGISTER(J),
    { "POP", PUSH_POP },
    { "PUSH", PUSH_POP },
    { "PEEK", PEEK },
    { "SP", SP },
    { "PC", PC },
    { "EX", EX },
    NULL
};

struct register_mapping register_value_name_map[] =
{
    REGISTER_VALUE(A),
    REGISTER_VALUE(B),
    REGISTER_VALUE(C),
    REGISTER_VALUE(X),
    REGISTER_VALUE(Y),
    REGISTER_VALUE(Z),
    REGISTER_VALUE(I),
    REGISTER_VALUE(J),
    { "POP", BRACKETS_UNSUPPORTED },
    { "PEEK", BRACKETS_UNSUPPORTED },
    { "PUSH", BRACKETS_UNSUPPORTED },
    { "SP", PEEK },
    { "PC", BRACKETS_UNSUPPORTED },
    { "EX", BRACKETS_UNSUPPORTED },
    NULL
};

struct register_mapping register_value_next_map[] =
{
    REGISTER_VALUE_NEXT(A),
    REGISTER_VALUE_NEXT(B),
    REGISTER_VALUE_NEXT(C),
    REGISTER_VALUE_NEXT(X),
    REGISTER_VALUE_NEXT(Y),
    REGISTER_VALUE_NEXT(Z),
    REGISTER_VALUE_NEXT(I),
    REGISTER_VALUE_NEXT(J),
    { "POP", VALUE_NEXT_UNSUPPORTED },
    { "PEEK", VALUE_NEXT_UNSUPPORTED },
    { "PUSH", VALUE_NEXT_UNSUPPORTED },
    { "SP", PICK },
    { "PC", VALUE_NEXT_UNSUPPORTED },
    { "EX", VALUE_NEXT_UNSUPPORTED },
    NULL
};

struct register_mapping register_disasm_map[] =
{
    { "NXT", NXT },
    { "NXT_LIT", NXT_LIT },
    NULL
};

struct instruction_mapping* get_instruction_by_value(uint16_t opcode, uint16_t nbopcode)
{
    uint16_t i = 0;

    while (instruction_name_map[i].name != NULL)
    {
        if ((opcode != OP_NONBASIC && instruction_name_map[i].opcode == opcode) ||
                (opcode == OP_NONBASIC && instruction_name_map[i].opcode == OP_NONBASIC && instruction_name_map[i].nbopcode == nbopcode))
            return &instruction_name_map[i];

        i += 1;
    }

    return NULL;
}

struct instruction_mapping* get_instruction_by_name(char* name)
{
    uint16_t i = 0;

    while (instruction_name_map[i].name != NULL)
    {
        if (strcmp(instruction_name_map[i].name, name) == 0)
            return &instruction_name_map[i];

        i += 1;
    }

    return NULL;
}

struct register_mapping* get_register_by_value(uint16_t value)
{
    uint16_t i = 0;

    while (register_value_name_map[i].name != NULL)
    {
        if (register_value_name_map[i].value == value)
            return &register_value_name_map[i];

        i += 1;
    }
    i = 0;
    while (register_value_next_map[i].name != NULL)
    {
        if (register_value_next_map[i].value == value)
            return &register_value_next_map[i];

        i += 1;
    }
    i = 0;
    while (register_name_map[i].name != NULL)
    {
        if (register_name_map[i].value == value)
            return &register_name_map[i];

        i += 1;
    }
    i = 0;
    while (register_disasm_map[i].name != NULL)
    {
        if (register_disasm_map[i].value == value)
            return &register_disasm_map[i];

        i += 1;
    }

    return NULL;
}

struct register_mapping* get_register_by_name(char* name, bool bracketed)
{
    uint16_t i = 0;

    if (bracketed)
    {
        while (register_value_name_map[i].name != NULL)
        {
            if (stricmp(register_value_name_map[i].name, name) == 0)
                return &register_value_name_map[i];

            i += 1;
        }
    }
    else
    {
        while (register_name_map[i].name != NULL)
        {
            if (stricmp(register_name_map[i].name, name) == 0)
                return &register_name_map[i];

            i += 1;
        }
    }

    return NULL;
}

struct register_mapping* get_register_by_name_next(char* name)
{
    uint16_t i = 0;

    while (register_value_next_map[i].name != NULL)
    {
        if (stricmp(register_value_next_map[i].name, name) == 0)
            return &register_value_next_map[i];

        i += 1;
    }

    return NULL;
}
