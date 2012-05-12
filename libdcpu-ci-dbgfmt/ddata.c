/**

	File:		ddata.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-CI-DbgFmt

	Authors:	James Rhodes
				José Manuel Díez

	Description:

**/

#include "ddata.h"

struct dbg_sym_file* dbgfmt_header(uint32_t num_symbols);
struct dbg_sym* dbgfmt_debugging_symbol(uint8_t type, void* payload);

int dbgfmt_write(bstring path, uint32_t num_symbols, struct dbg_sym* symbols)
{
	FILE* out;
	struct dbg_sym_file* hdr = dbgfmt_header(num_symbols);
	size_t i = 0;

	out = fopen(path->data, "wb");
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

struct dbg_sym_file* dbgfmt_read(bstring path)
{
	FILE* in;
	struct dbg_sym_file* file = malloc(sizeof(struct dbg_sym_file));
	size_t i;
	uint16_t tmp;

	in = fopen(path->data, "rb");
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

struct dbgfmt_serialization_result* dbgfmt_serialize_payload_line(struct dbg_sym_payload_line* payload)
{
	struct dbgfmt_serialization_result* ser_res = malloc(sizeof(struct dbgfmt_serialization_result));

	uint16_t length = strlen(payload->path->data) + 1 + sizeof(payload->lineno) + sizeof(payload->address);
	uint8_t* result = malloc(length);
	uint8_t* origin = result;

	strcpy(result, payload->path->data);
	result += strlen(payload->path->data) + 1;
	memcpy(result, &payload->lineno, sizeof(payload->lineno));
	result += sizeof(payload->lineno);
	memcpy(result, &payload->address, sizeof(payload->address));

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

struct dbg_sym* dbgfmt_create_symbol(uint8_t type, void* payload)
{
	struct dbg_sym* symbol = malloc(sizeof(struct dbg_sym));
	struct dbgfmt_serialization_result* result;

	symbol->type = type;
	switch (symbol->type)
	{
		case DBGFMT_SYMBOL_LINE:
			result = dbgfmt_serialize_payload_line(payload);

			symbol->payload = malloc(result->length);
			memcpy(symbol->payload, result->bytestream, result->length);
			symbol->length = result->length;
			break;
	}

	return symbol;
}

struct dbg_sym_payload_line* dbgfmt_create_symbol_line(bstring path, uint16_t lineno, uint16_t address)
{
	struct dbg_sym_payload_line* payload = malloc(sizeof(struct dbg_sym_payload_line));

	payload->path = malloc(blength(path) + 1);
	bassign(payload->path, path);
	payload->lineno = lineno;
	payload->address = address;

	return payload;
}