/**

	File:		ddata.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-CI-DbgFmt

	Authors:	James Rhodes
				José Manuel Díez

	Description:

**/

#include "ddata.h"

int dbgfmt_write_to_file(char* path, uint32_t num_symbols, struct dbg_sym* symbols)
{
	FILE* out;
	struct dbg_sym_file* hdr = dbgfmt_header(num_symbols);
	size_t i = 0;

	out = fopen(path, "wb");
	fwrite(&hdr->magic, sizeof(hdr->magic), 1, out);
	fwrite(&hdr->num_symbols, sizeof(hdr->num_symbols), 1, out);
	for (i = 0; i < num_symbols; i++)
	{
		fwrite(&symbols[i].length, sizeof(symbols[i].length), 1, out);
		fwrite(&symbols[i].type, sizeof(symbols[i].type), 1, out);
		fwrite(symbols[i].payload, 1, symbols[i].length, out);
	}
	fclose(out);

	return 0;
}

struct dbg_sym_file* dbgfmt_read_file(char* path)
{
	FILE* in;
	struct dbg_sym_file* file = malloc(sizeof(struct dbg_sym_file));
	size_t i;
	uint16_t tmp;

	in = fopen(path, "rb");
	fread(&file->magic, sizeof(file->magic), 1, in);
	fread(&file->num_symbols, sizeof(file->symbols), 1, in);
	printf("%d\n", sizeof(file->symbols));
	fseek(in, sizeof(file->symbols), SEEK_SET);
	for (i = 0; i < file->num_symbols; i++)
	{
		fread(&tmp, sizeof(uint16_t), 1, in);
		printf("%d\n", tmp);
	}

	fclose(in);

	return file;
}

struct dbgfmt_serialization_result* dbgfmt_serialize_basic(void* payload)
{
	struct dbg_sym_basic_payload* l_payload = (struct dbg_sym_basic_payload*) payload;
	struct dbgfmt_serialization_result* ser_res = malloc(sizeof(struct dbgfmt_serialization_result));

	uint16_t length = strlen(l_payload->path) + 1 + sizeof(l_payload->lineno) + sizeof(l_payload->address);
	uint8_t* result = malloc(length);
	uint8_t* origin = result;

	strcpy(result, l_payload->path);
	result += strlen(l_payload->path) + 1;
	memcpy(result, &l_payload->lineno, sizeof(l_payload->lineno));
	result += sizeof(l_payload->lineno);
	memcpy(result, &l_payload->address, sizeof(l_payload->address));

	ser_res->bytestream = origin;
	ser_res->length = length;

	return ser_res;
}

struct dbg_sym_file* dbgfmt_header(uint32_t num_symbols)
{
	struct dbg_sym_file* header = malloc(sizeof(struct dbg_sym_file));

	header->magic = DBGFMT_MAGIC;
	header->num_symbols = num_symbols;

	return header;
}

struct dbg_sym* dbgfmt_debugging_symbol(uint8_t type, void* payload)
{
	struct dbg_sym* symbol = malloc(sizeof(struct dbg_sym));
	struct dbgfmt_serialization_result* result;

	symbol->type = type;
	switch (symbol->type)
	{
		case DBGFMT_BASIC:
			result = dbgfmt_serialize_basic(payload);

			symbol->payload = malloc(result->length);
			memcpy(symbol->payload, result->bytestream, result->length);
			symbol->length = result->length;
			break;
	}

	return symbol;
}

struct dbg_sym_basic_payload* dbgfmt_basic_payload(char* path, uint16_t lineno, uint16_t address)
{
	struct dbg_sym_basic_payload* payload = malloc(sizeof(struct dbg_sym_basic_payload));

	payload->path = malloc(strlen(path) + 1);
	strcpy(payload->path, path);
	payload->lineno = lineno;
	payload->address = address;

	return payload;
}