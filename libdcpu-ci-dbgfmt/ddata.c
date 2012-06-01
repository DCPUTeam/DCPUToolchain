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
struct dbgfmt_serialization_result* dbgfmt_serialize_payload_string(struct dbg_sym_payload_string* payload);
struct dbg_sym_payload_line* dbgfmt_deserialize_symbol_line(void* data, uint16_t length);
struct dbg_sym_payload_string* dbgfmt_deserialize_symbol_string(void* data, uint16_t length);

int dbgfmt_write(bstring path, list_t* symbols)
{
	FILE* out;
	uint32_t total = (uint32_t)list_size(symbols);
	struct dbg_sym_file* hdr = dbgfmt_header(total);
	size_t i = 0;
	struct dbgfmt_serialization_result* result;
	void* rawdata;
	uint16_t rawlength;

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
			case DBGFMT_SYMBOL_STRING:
				result = dbgfmt_serialize_payload_string((struct dbg_sym_payload_string*)sym->payload);

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

list_t* dbgfmt_read(bstring path)
{
	FILE* in;
	struct dbg_sym_file headers;
	struct dbg_sym* symbol;
	void* payload;
	list_t* result = malloc(sizeof(list_t));
	uint32_t i;

	// Open the file.
	in = fopen(path->data, "rb");

	// FIXME: Do some checking here to see if the file
	// was loaded successfully.

	// Initialize the list of symbols.
	list_init(result);

	// Read in the header information.
	fread(&headers.magic, sizeof(headers.magic), 1, in);
	fread(&headers.num_symbols, sizeof(headers.num_symbols), 1, in);

	// Now read until EOF.
	for (i = 0; i < headers.num_symbols; i++)
	{
		// Allocate space for a new symbol.
		symbol = malloc(sizeof(struct dbg_sym));

		// Read in it's header information.
		fread(&symbol->length, sizeof(symbol->length), 1, in);
		fread(&symbol->type, sizeof(symbol->type), 1, in);

		// Now allocate and read in the payload.
		payload = malloc(symbol->length);
		fread(payload, symbol->length, 1, in);

		// Deserialize the payload.
		switch (symbol->type)
		{
			case DBGFMT_SYMBOL_LINE:
				payload = dbgfmt_deserialize_symbol_line(payload, symbol->length);
				break;
			case DBGFMT_SYMBOL_STRING:
				payload = dbgfmt_deserialize_symbol_string(payload, symbol->length);
				break;
			default:
				assert(0 /* unable to write out unknown debugging symbol */);
				break;
		}

		// Add the deserialized structure to the symbol.
		symbol->payload = payload;

		// Add the symbol to the list.
		list_append(result, symbol);
	}

	// Close the file.
	fclose(in);

	// Return the result list.
	return result;
}

struct dbg_sym_payload_line* dbgfmt_get_symbol_line(struct dbg_sym* bytes)
{
	struct dbg_sym_payload_line* result = malloc(sizeof(struct dbg_sym_payload_line));
	size_t str_length = bytes->length - 2 * sizeof(uint16_t);

	result->path = malloc(str_length + 1001);
	//memcpy(result->path, bytes->payload, str_length);
	return result;
}

struct dbg_sym_payload_string* dbgfmt_get_symbol_string(struct dbg_sym* bytes)
{
	struct dbg_sym_payload_string* result = malloc(sizeof(struct dbg_sym_payload_string));
	size_t str_length = bytes->length - 2 * sizeof(uint16_t);

	result->data = malloc(str_length + 1001);
	//memcpy(result->path, bytes->payload, str_length);
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

struct dbgfmt_serialization_result* dbgfmt_serialize_payload_string(struct dbg_sym_payload_string* payload)
{
	struct dbgfmt_serialization_result* ser_res = malloc(sizeof(struct dbgfmt_serialization_result));

	uint16_t length = strlen(payload->data->data) + 1 + sizeof(payload->address);
	uint8_t* result = malloc(length);
	uint8_t* origin = result;

	strcpy(result, payload->data->data);
	result += strlen(payload->data->data) + 1;
	memcpy(result, &payload->address, sizeof(payload->address));

	ser_res->bytestream = origin;
	ser_res->length = length;

	return ser_res;
}

struct dbg_sym_payload_line* dbgfmt_deserialize_symbol_line(void* data, uint16_t length)
{
	struct dbg_sym_payload_line* payload = malloc(sizeof(struct dbg_sym_payload_line));
	size_t pathlen = length - 2 * sizeof(uint16_t); // FIXME: There should be a better way of handling string length.

	// Read the string data into storage, then assign
	// the bstring.
	char* storage = malloc(pathlen); // Length includes NULL terminator.
	memcpy(storage, data, pathlen);
	payload->path = bfromcstr(storage);
	free(storage);

	// Read in other properties.
	memcpy(&payload->lineno, (char*)data + pathlen, sizeof(uint16_t));
	memcpy(&payload->address, (char*)data + pathlen + sizeof(uint16_t), sizeof(uint16_t));

	// Free the passed data.
	free(data);

	return payload;
}

struct dbg_sym_payload_string* dbgfmt_deserialize_symbol_string(void* data, uint16_t length)
{
	struct dbg_sym_payload_string* payload = malloc(sizeof(struct dbg_sym_payload_string));
	size_t pathlen = length - 1 * sizeof(uint16_t); // FIXME: There should be a better way of handling string length.

	// Read the string data into storage, then assign
	// the bstring.
	char* storage = malloc(pathlen); // Length includes NULL terminator.
	memcpy(storage, data, pathlen);
	payload->data = bfromcstr(storage);
	free(storage);

	// Read in other properties.
	memcpy(&payload->address, (char*)data + pathlen, sizeof(uint16_t));

	// Free the passed data.
	free(data);

	return payload;
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

struct dbg_sym_payload_string* dbgfmt_create_symbol_string(bstring data, uint16_t address)
{
	struct dbg_sym_payload_string* payload = malloc(sizeof(struct dbg_sym_payload_string));

	payload->data = bfromcstr("");
	bassign(payload->data, data);
	payload->address = address;

	return payload;
}

void dbgfmt_update_symbol(struct dbg_sym * (*symbols)[4], uint16_t symbols_count, uint16_t address)
{
	int i;

	for (i = 0; i < symbols_count; i++)
	{
		if ((*symbols)[i] == NULL)
			continue;

		switch ((*symbols)[i]->type)
		{
			case DBGFMT_SYMBOL_LINE:
				((struct dbg_sym_payload_line*)(*symbols)[i]->payload)->address = address;
				break;
			case DBGFMT_SYMBOL_STRING:
				((struct dbg_sym_payload_string*)(*symbols)[i]->payload)->address = address;
				break;
			default:
				assert(0 /* debugging symbol wasn't of any known type during update */);
		}
	}
}

void dbgfmt_finalize_symbol(struct dbg_sym* symbol, uint16_t address)
{
	switch (symbol->type)
	{
		case DBGFMT_SYMBOL_LINE:
			if (((struct dbg_sym_payload_line*)symbol->payload)->address == DBGFMT_UNDETERMINED)
				((struct dbg_sym_payload_line*)symbol->payload)->address = address;
			break;
		case DBGFMT_SYMBOL_STRING:
			if (((struct dbg_sym_payload_string*)symbol->payload)->address == DBGFMT_UNDETERMINED)
				((struct dbg_sym_payload_string*)symbol->payload)->address = address;
			break;
		default:
			assert(0 /* debugging symbol wasn't of any known type during update */);
	}
}