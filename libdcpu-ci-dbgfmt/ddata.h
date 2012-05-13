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
#include <simclist.h>

#define DBGFMT_MAGIC		0xC0DEBEAF
#define DBGFMT_UNDETERMINED	0xFFFF

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
	void* payload;
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

int dbgfmt_write(bstring path, list_t* symbols);
list_t* dbgfmt_read(bstring path);


void dbgfmt_update_symbol(struct dbg_sym * (*symbols)[4], uint16_t symbols_count, uint16_t address);

struct dbg_sym* dbgfmt_create_symbol(uint8_t type, void* payload);
struct dbg_sym_payload_line* dbgfmt_create_symbol_line(bstring path, uint16_t lineno, uint16_t address);


struct dbg_sym_payload_line* dbgfmt_get_symbol_line(struct dbg_sym* bytes);

#endif