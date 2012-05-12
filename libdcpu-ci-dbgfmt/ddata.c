/**

	File:		ddata.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-CI-DbgFmt

	Authors:	James Rhodes
				José Manuel Díez

	Description:

**/

#include "ddata.h"
#include <assert.h>

struct dbg_sym_file* dbgfmt_header(uint32_t num_symbols);
struct dbg_sym* dbgfmt_debugging_symbol(uint8_t type, void* payload);
struct dbgfmt_serialization_result* dbgfmt_serialize_payload_line(struct dbg_sym_payload_line* payload);

int dbgfmt_write(bstring path, list_t* symbols)
{
	FILE* out;
	uint32_t total = (uint32_t)list_size(symbols);
	struct dbg_sym_file* hdr = dbgfmt_header(total);
	size_t i = 0;
	struct dbgfmt_serialization_result* result;
	void* rawdata;
	uint32_t rawlength;
	
	out = fopen(path->data, "wb");
	fwrite(&hdr->magic, sizeof(hdr->magic), 1, out);
	fwrite(&total, sizeof(uint32_t), 1, out);
	for (i = 0; i < total; i++)
	{
		struct dbg_sym* sym = (struct dbg_sym*)list_get_at(symbols, i);
		
		switch (sym->type)
		{
			case DBGFMT_SYMBOL_LINE:
				result = dbgfmt_serialize_payload_line((struct dbg_sym_payload_line*)sym->payload);
				
				rawdata = malloc(result->length);
				memcpy(rawdata, result->bytestream, result->length);
				rawlength = result->length;
				break;
			default:
				assert(0 /* unable to write out unknown debugging symbol */);
				break;
		}
		
		fwrite(&rawlength, sizeof(rawlength), 1, out);
		fwrite(&sym->type, sizeof(sym->type), 1, out);
		fwrite(rawdata, 1, rawlength, out);
		
		free(rawdata);
	}
	fclose(out);
	
	return 0;
}

uint8_t* dbgfmt_deserialize_basic(FILE* stream, uint16_t length)
{
	size_t str_length = length - 2 * sizeof(uint16_t);
	uint16_t tmp;
	uint8_t* result = malloc(str_length + 2 * sizeof(uint16_t)), *origin;
	origin = result;

	fread(result, str_length, 1, stream);
	result += str_length;
	fread(result, sizeof(uint16_t), 1, stream);
	result += sizeof(uint16_t);
	fread(result, sizeof(uint16_t), 1, stream);
	result = origin;

	return result;
}

struct dbg_sym_payload_line* dbgfmt_get_symbol_line(struct dbg_sym* bytes)
{
	struct dbg_sym_payload_line* result = malloc(sizeof(struct dbg_sym_payload_line));
	size_t str_length = bytes->length - 2 * sizeof(uint16_t);

	result->path = malloc(str_length + 1001);
	//memcpy(result->path, bytes->payload, str_length);
	printf("get_basic: %s\n", bytes->payload);

	return result;
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

struct dbg_sym_file* dbgfmt_read(bstring path)
{
	FILE* in;
	struct dbg_sym_file* file = malloc(sizeof(struct dbg_sym_file));
	struct dbg_sym* tmp_symbol = malloc(sizeof(struct dbg_sym)), *origin;
	size_t i, symbols_start, symbols_end;
	struct dbg_sym_payload_line* very_tmp_smbl;
	uint16_t tmp;

	in = fopen(path->data, "rb");
	fread(&file->magic, sizeof(file->magic), 1, in);
	fread(&file->num_symbols, sizeof(file->symbols), 1, in);

	fseek(in, sizeof(file->symbols), SEEK_SET);
	symbols_start = ftell(in);
	fseek(in, 0, SEEK_END);
	symbols_end = ftell(in);
	fseek(in, symbols_start, SEEK_SET);
	file->symbols = malloc(symbols_end - symbols_start);
	origin = file->symbols;

	for (i = 0; i < file->num_symbols; i++)
	{
		fread(&tmp_symbol->length, sizeof(uint16_t), 1, in);
		fread(&tmp_symbol->type, sizeof(uint8_t), 1, in);

		switch (tmp_symbol->type)
		{
			case DBGFMT_SYMBOL_LINE:
				tmp_symbol->payload = dbgfmt_deserialize_basic(in, tmp_symbol->length);
				break;
		}

		memcpy(file->symbols, &tmp_symbol->length, sizeof(uint16_t));
		file->symbols += sizeof(uint16_t);
		memcpy(file->symbols, &tmp_symbol->type, sizeof(uint8_t));
		file->symbols += sizeof(uint8_t);
		memcpy(file->symbols, tmp_symbol->payload, tmp_symbol->length);
		file->symbols += tmp_symbol->length;
	}

	file->symbols = origin;

	fclose(in);

	return file;
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
	symbol->payload = payload;

	return symbol;
}

struct dbg_sym_payload_line* dbgfmt_create_symbol_line(bstring path, uint16_t lineno, uint16_t address)
{
	struct dbg_sym_payload_line* payload = malloc(sizeof(struct dbg_sym_payload_line));

	payload->path = bfromcstr("");
	bassign(payload->path, path);
	payload->lineno = lineno;
	payload->address = address;

	return payload;
}

void dbgfmt_update_symbol(struct dbg_sym* symbol, uint16_t address)
{
	if (symbol == NULL)
		return;
	
	switch (symbol->type)
	{
		case DBGFMT_SYMBOL_LINE:
			((struct dbg_sym_payload_line*)symbol->payload)->address = address;
			break;
		default:
			assert(0 /* debugging symbol wasn't of any known type during update */);
	}
}