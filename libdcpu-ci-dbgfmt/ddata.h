/**

	File:		ddata.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-CI-DbgFmt

	Authors:	James Rhodes

	Description:

**/

#ifndef __DCPU_DBG_DDATA_H
#define __DCPU_DBG_DDATA_H
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DBGFMT_BASIC		0x1

#define DBGFMT_MAGIC		0xc0debeaf


struct dbg_sym_file_header {
	uint32_t magic;
	uint32_t num_symbols;
};

struct dbg_sym {
	uint16_t length;
	uint8_t type;
	uint8_t* payload;
};

struct dbg_sym_basic_payload {
	char* path;
	uint16_t lineno;
	uint16_t address;
};

struct dbgfmt_serialization_result {
	uint8_t* bytestream;
	uint16_t length;
};
int dbgfmt_write_to_file(char* path, uint32_t num_symbols, struct dbg_sym* symbols);
struct dbg_sym_file_header* dbgfmt_header(uint32_t num_symbols);
struct dbg_sym* dbgfmt_debugging_symbol(uint8_t type, void* payload);
struct dbg_sym_basic_payload* dbgfmt_basic_payload(char* path, uint16_t lineno, uint16_t address);

#endif