/**

	File:		ddata.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-CI-DbgFmt

	Authors:	James Rhodes
				José Manuel Díez

	Description:

**/

#include "ddata.h"

int dbgfmt_write_to_file(char* path, uint32_t num_symbols, dbg_sym* symbols) {
	FILE *out;
	struct dbg_sym_file_header* hdr = dbgfmt_header(num_symbols);
	
	out = fopen(path, "wb");
	fwrite(&hdr->magic, sizeof(hdr->magic), 1, out);
	fwrite(&hdr->num_symbols, sizeof(hdr->num_symbols), 1, out);
	
	fclose(out);
	
	return 0;
}

struct dbgfmt_serialization_result* dbgfmt_serialize_basic(void* payload) {
	struct dbg_sym_basic_payload* l_payload = (struct dbg_sym_basic_payload*) payload;
	struct dbgfmt_serialization_result* ser_res = malloc(sizeof(struct dbgfmt_serialization_result));
	
	uint16_t length = strlen(l_payload->path) +1 + sizeof(l_payload->lineno) + sizeof(l_payload->address);
	uint8_t* result = malloc(length);
	uint8_t* origin = result;
	
	strcpy(result, l_payload->path);
	result += strlen(l_payload->path) +1;
	memcpy(result, &l_payload->lineno, sizeof(l_payload->lineno));
	result += sizeof(l_payload->lineno);
	memcpy(result, &l_payload->address, sizeof(l_payload->address));
	
	ser_res->bytestream = origin;
	ser_res->length = length;
	
	printf("length: %d\n", length);
	
	return ser_res;
}

struct dbg_sym_file_header* dbgfmt_header(uint32_t num_symbols) {
	struct dbg_sym_file_header* header = malloc(sizeof(struct dbg_sym_file_header));
	
	header->magic = DBGFMT_MAGIC;
	header->num_symbols = num_symbols;
	
	return header;
}

struct dbg_sym* dbgfmt_debugging_symbol(uint8_t type, void* payload) {
	struct dbg_sym* symbol = malloc(sizeof(struct dbg_sym));
	struct dbgfmt_serialization_result* result;
	
	symbol->type = type;
	switch(symbol->type) {
		case DBGFMT_BASIC:
			result = dbgfmt_serialize_basic(payload);
			
			symbol->payload = malloc(result->length);
			memcpy(symbol->payload, result->bytestream, result->length);
			symbol->length = result->length;
			break;
	}

	return symbol;
}

struct dbg_sym_basic_payload* dbgfmt_basic_payload(char* path, uint16_t lineno, uint16_t address) {
	struct dbg_sym_basic_payload* payload = malloc(sizeof(struct dbg_sym_basic_payload));
	
	payload->path = malloc(strlen(path) +1);
	strcpy(payload->path, path);
	payload->lineno = lineno;
	payload->address = address;

	return payload;
}