/**

    File:       luaglob.h

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU

    Authors:    James Rhodes

    Description:    Defines an inline function which loads in all of
            the DCPU constants for Lua.

**/

#ifndef __DCPU_LUAGLOB_H
#define __DCPU_LUAGLOB_H

// We have to define this as a macro to ensure we do not
// create a dependency on Lua for the libdcpu library.

#define dcpu_lua_set_constants(L) \
{ \
    lua_pushnumber(L, REG_A); \
    lua_setglobal(L, "REG_A"); \
    lua_pushnumber(L, REG_B); \
    lua_setglobal(L, "REG_B"); \
    lua_pushnumber(L, REG_C); \
    lua_setglobal(L, "REG_C"); \
    lua_pushnumber(L, REG_X); \
    lua_setglobal(L, "REG_X"); \
    lua_pushnumber(L, REG_Y); \
    lua_setglobal(L, "REG_Y"); \
    lua_pushnumber(L, REG_Z); \
    lua_setglobal(L, "REG_Z"); \
    lua_pushnumber(L, REG_I); \
    lua_setglobal(L, "REG_I"); \
    lua_pushnumber(L, REG_J); \
    lua_setglobal(L, "REG_J"); \
 \
    lua_pushnumber(L, VAL_A); \
    lua_setglobal(L, "VAL_A"); \
    lua_pushnumber(L, VAL_B); \
    lua_setglobal(L, "VAL_B"); \
    lua_pushnumber(L, VAL_C); \
    lua_setglobal(L, "VAL_C"); \
    lua_pushnumber(L, VAL_X); \
    lua_setglobal(L, "VAL_X"); \
    lua_pushnumber(L, VAL_Y); \
    lua_setglobal(L, "VAL_Y"); \
    lua_pushnumber(L, VAL_Z); \
    lua_setglobal(L, "VAL_Z"); \
    lua_pushnumber(L, VAL_I); \
    lua_setglobal(L, "VAL_I"); \
    lua_pushnumber(L, VAL_J); \
    lua_setglobal(L, "VAL_J"); \
 \
    lua_pushnumber(L, NXT_VAL_A); \
    lua_setglobal(L, "NXT_VAL_A"); \
    lua_pushnumber(L, NXT_VAL_B); \
    lua_setglobal(L, "NXT_VAL_B"); \
    lua_pushnumber(L, NXT_VAL_C); \
    lua_setglobal(L, "NXT_VAL_C"); \
    lua_pushnumber(L, NXT_VAL_X); \
    lua_setglobal(L, "NXT_VAL_X"); \
    lua_pushnumber(L, NXT_VAL_Y); \
    lua_setglobal(L, "NXT_VAL_Y"); \
    lua_pushnumber(L, NXT_VAL_Z); \
    lua_setglobal(L, "NXT_VAL_Z"); \
    lua_pushnumber(L, NXT_VAL_I); \
    lua_setglobal(L, "NXT_VAL_I"); \
    lua_pushnumber(L, NXT_VAL_J); \
    lua_setglobal(L, "NXT_VAL_J"); \
 \
    lua_pushnumber(L, PUSH_POP); \
    lua_setglobal(L, "PUSH_POP"); \
    lua_pushnumber(L, PEEK); \
    lua_setglobal(L, "PEEK"); \
    lua_pushnumber(L, PICK); \
    lua_setglobal(L, "PICK"); \
    lua_pushnumber(L, SP); \
    lua_setglobal(L, "SP"); \
    lua_pushnumber(L, PC); \
    lua_setglobal(L, "PC"); \
    lua_pushnumber(L, EX); \
    lua_setglobal(L, "EX"); \
    lua_pushnumber(L, NXT); \
    lua_setglobal(L, "NXT"); \
    lua_pushnumber(L, NXT_LIT); \
    lua_setglobal(L, "NXT_LIT"); \
 \
    lua_pushnumber(L, POS_A); \
    lua_setglobal(L, "POS_A"); \
    lua_pushnumber(L, POS_B); \
    lua_setglobal(L, "POS_B"); \
 \
    lua_pushnumber(L, OP_NONBASIC); \
    lua_setglobal(L, "OP_NONBASIC"); \
    lua_pushnumber(L, OP_SET); \
    lua_setglobal(L, "OP_SET"); \
    lua_pushnumber(L, OP_ADD); \
    lua_setglobal(L, "OP_ADD"); \
    lua_pushnumber(L, OP_SUB); \
    lua_setglobal(L, "OP_SUB"); \
    lua_pushnumber(L, OP_MUL); \
    lua_setglobal(L, "OP_MUL"); \
    lua_pushnumber(L, OP_MLI); \
    lua_setglobal(L, "OP_MLI"); \
    lua_pushnumber(L, OP_DIV); \
    lua_setglobal(L, "OP_DIV"); \
    lua_pushnumber(L, OP_MOD); \
    lua_setglobal(L, "OP_MOD"); \
    lua_pushnumber(L, OP_MDI); \
    lua_setglobal(L, "OP_MDI"); \
    lua_pushnumber(L, OP_AND); \
    lua_setglobal(L, "OP_AND"); \
    lua_pushnumber(L, OP_BOR); \
    lua_setglobal(L, "OP_BOR"); \
    lua_pushnumber(L, OP_XOR); \
    lua_setglobal(L, "OP_XOR"); \
    lua_pushnumber(L, OP_SHR); \
    lua_setglobal(L, "OP_SHR"); \
    lua_pushnumber(L, OP_ASR); \
    lua_setglobal(L, "OP_ASR"); \
    lua_pushnumber(L, OP_SHL); \
    lua_setglobal(L, "OP_SHL"); \
    lua_pushnumber(L, OP_IFB); \
    lua_setglobal(L, "OP_IFB"); \
    lua_pushnumber(L, OP_IFC); \
    lua_setglobal(L, "OP_IFC"); \
    lua_pushnumber(L, OP_IFE); \
    lua_setglobal(L, "OP_IFE"); \
    lua_pushnumber(L, OP_IFN); \
    lua_setglobal(L, "OP_IFN"); \
    lua_pushnumber(L, OP_IFG); \
    lua_setglobal(L, "OP_IFG"); \
    lua_pushnumber(L, OP_IFA); \
    lua_setglobal(L, "OP_IFA"); \
    lua_pushnumber(L, OP_IFL); \
    lua_setglobal(L, "OP_IFL"); \
    lua_pushnumber(L, OP_IFU); \
    lua_setglobal(L, "OP_IFU"); \
    lua_pushnumber(L, OP_ADX); \
    lua_setglobal(L, "OP_ADX"); \
    lua_pushnumber(L, OP_SBX); \
    lua_setglobal(L, "OP_SBX"); \
    lua_pushnumber(L, OP_STI); \
    lua_setglobal(L, "OP_STI"); \
    lua_pushnumber(L, OP_STD); \
    lua_setglobal(L, "OP_STD"); \
 \
    lua_pushnumber(L, NBOP_RESERVED); \
    lua_setglobal(L, "NBOP_RESERVED"); \
    lua_pushnumber(L, NBOP_JSR); \
    lua_setglobal(L, "NBOP_JSR"); \
    lua_pushnumber(L, NBOP_INT); \
    lua_setglobal(L, "NBOP_INT"); \
    lua_pushnumber(L, NBOP_IAG); \
    lua_setglobal(L, "NBOP_IAG"); \
    lua_pushnumber(L, NBOP_IAS); \
    lua_setglobal(L, "NBOP_IAS"); \
    lua_pushnumber(L, NBOP_RFI); \
    lua_setglobal(L, "NBOP_RFI"); \
    lua_pushnumber(L, NBOP_IAQ); \
    lua_setglobal(L, "NBOP_IAQ"); \
    lua_pushnumber(L, NBOP_HWN); \
    lua_setglobal(L, "NBOP_HWN"); \
    lua_pushnumber(L, NBOP_HWQ); \
    lua_setglobal(L, "NBOP_HWQ"); \
    lua_pushnumber(L, NBOP_HWI); \
    lua_setglobal(L, "NBOP_HWI"); \
 \
    lua_pushnumber(L, AR_NOFLOW); \
    lua_setglobal(L, "AR_NOFLOW"); \
    lua_pushnumber(L, AR_OVERFLOW); \
    lua_setglobal(L, "AR_OVERFLOW"); \
    lua_pushnumber(L, AR_UNDERFLOW); \
    lua_setglobal(L, "AR_UNDERFLOW"); \
    lua_pushnumber(L, AR_MAX); \
    lua_setglobal(L, "AR_MAX"); \
}

#endif
