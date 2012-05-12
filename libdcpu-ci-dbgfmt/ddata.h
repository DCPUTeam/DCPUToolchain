/**

	File:		ddata.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-CI-DbgFmt

	Authors:	James Rhodes
			José Manuel Díez

	Description:	Defines the API for reading and writing to debugging
			symbol formats.

**/

#ifndef __DCPU_DBG_DDATA_H
#define __DCPU_DBG_DDATA_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <bstring.h>

#define DBGFMT_MAGIC		0xc0debeaf

#define DBGFMT_SYMBOL_LINE	0x1

struct dbg_sym_file
{
	uint32_t magic;
	uint32_t num_symbols;
	struct dbg_sym* symbols;
};

struct dbg_sym
{
	uint16_t length;
	uint8_t type;
	uint8_t* payload;
};

struct dbg_sym_payload_line
{
	bstring path;
	uint16_t lineno;
	uint16_t address;
};

struct dbgfmt_serialization_result
{
	uint8_t* bytestream;
	uint16_t length;
};

int dbgfmt_write(bstring path, uint32_t num_symbols, struct dbg_sym* symbols);
struct dbg_sym_file* dbgfmt_read(bstring path);

struct dbg_sym* dbgfmt_create_symbol(uint8_t type, void* payload);
struct dbg_sym_payload_line* dbgfmt_create_symbol_line(bstring path, uint16_t lineno, uint16_t address);

#endif