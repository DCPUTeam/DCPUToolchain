/**

    File:       ddata.c

    Project:    DCPU-16 Tools
    Component:  LibDCPU-CI-DbgFmt

    Authors:    James Rhodes
                José Manuel Díez

    Description:

**/

#include <assert.h>
#include <debug.h>
#include "ddata.h"

struct dbg_sym_file* dbgfmt_header(uint32_t num_symbols);
struct dbg_sym* dbgfmt_debugging_symbol(uint8_t type, void* payload);
struct dbgfmt_serialization_result* dbgfmt_serialize_payload_line(struct dbg_sym_payload_line* payload);
struct dbgfmt_serialization_result* dbgfmt_serialize_payload_string(struct dbg_sym_payload_string* payload);
struct dbgfmt_serialization_result* dbgfmt_serialize_payload_label(struct dbg_sym_payload_label* payload);
struct dbg_sym_payload_line* dbgfmt_deserialize_symbol_line(void* data, uint16_t length);
struct dbg_sym_payload_string* dbgfmt_deserialize_symbol_string(void* data, uint16_t length);
struct dbg_sym_payload_label* dbgfmt_deserialize_symbol_label(void* data, uint16_t length);

size_t dbgsym_meter(const void* el)
{
    return sizeof(struct dbg_sym);
}

int dbgsym_comparator(const void* a, const void* b)
{
    struct dbg_sym* ea = (struct dbg_sym*)a;
    struct dbg_sym* eb = (struct dbg_sym*)b;
    if (dbgfmt_get_symbol_address(ea) > dbgfmt_get_symbol_address(eb))
        return -1;
    else if (dbgfmt_get_symbol_address(ea) == dbgfmt_get_symbol_address(eb))
        return 0;
    else
        return 1;
}

list_t* dbgfmt_create_list()
{
    list_t* result = malloc(sizeof(list_t));
    list_init(result);
    list_attributes_copy(result, &dbgsym_meter, 1);
    list_attributes_comparator(result, &dbgsym_comparator);
    return result;
}

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
    free(hdr);
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
                free(result->bytestream);
                free(result);
                break;
            case DBGFMT_SYMBOL_STRING:
                result = dbgfmt_serialize_payload_string((struct dbg_sym_payload_string*)sym->payload);

                rawdata = malloc(result->length);
                memcpy(rawdata, result->bytestream, result->length);
                rawlength = result->length;
                free(result->bytestream);
                free(result);
                break;
            case DBGFMT_SYMBOL_LABEL:
                result = dbgfmt_serialize_payload_label((struct dbg_sym_payload_label*)sym->payload);

                rawdata = malloc(result->length);
                memcpy(rawdata, result->bytestream, result->length);
                rawlength = result->length;
                free(result->bytestream);
                free(result);
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

list_t* dbgfmt_read(bstring path, uint8_t null_on_read_failure)
{
    FILE* in;
    struct dbg_sym_file headers;
    struct dbg_sym* symbol;
    void* payload;
    list_t* result = dbgfmt_create_list();
    uint32_t i;

    // Open the file.
    in = fopen(path->data, "rb");

    // Check to ensure the file is open.
    if (in == NULL)
    {
        // Fail.
        if (null_on_read_failure)
        {
            dbgfmt_free(result);
            return NULL;
        }
        else
            return result;
    }

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
            case DBGFMT_SYMBOL_LABEL:
                payload = dbgfmt_deserialize_symbol_label(payload, symbol->length);
                break;
            default:
                assert(0 /* unable to write out unknown debugging symbol */);
                break;
        }

        // Add the deserialized structure to the symbol.
        symbol->payload = payload;
        //printd(LEVEL_DEFAULT, "ALLOC %p.\n", symbol->payload);

        // Add the symbol to the list.
        list_append(result, symbol);

        // Free symbol and data (it is copied into the list).
        free(symbol);
    }

    // Close the file.
    fclose(in);

    // Return the result list.
    return result;
}

void dbgfmt_free(list_t* symbols)
{
    struct dbg_sym* symbol;

    if (symbols == NULL)
        return;

    list_iterator_start(symbols);
    while (list_iterator_hasnext(symbols))
    {
        symbol = list_iterator_next(symbols);

        // Free payload.
        dbgfmt_free_symbol(symbol);

        // No need to free the symbol, it will be
        // free'd by simclist when we clear / destroy
        // the list.
    }
    list_iterator_stop(symbols);
    list_clear(symbols);
    list_destroy(symbols);
    free(symbols);
}

struct dbg_sym_payload_line* dbgfmt_get_symbol_line(struct dbg_sym* bytes)
{
    struct dbg_sym_payload_line* result = malloc(sizeof(struct dbg_sym_payload_line));
    size_t str_length = bytes->length - 2 * sizeof(uint16_t);

    result->path = bfromcstralloc(str_length + 1, "");
    return result;
}

struct dbg_sym_payload_string* dbgfmt_get_symbol_string(struct dbg_sym* bytes)
{
    struct dbg_sym_payload_string* result = malloc(sizeof(struct dbg_sym_payload_string));
    size_t str_length = bytes->length - 2 * sizeof(uint16_t);

    result->data = bfromcstralloc(str_length + 1, "");
    return result;
}

struct dbg_sym_payload_label* dbgfmt_get_symbol_label(struct dbg_sym* bytes)
{
    struct dbg_sym_payload_label* result = malloc(sizeof(struct dbg_sym_payload_label));
    size_t str_length = bytes->length - 2 * sizeof(uint16_t);

    result->label = bfromcstralloc(str_length + 1, "");
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

struct dbgfmt_serialization_result* dbgfmt_serialize_payload_label(struct dbg_sym_payload_label* payload)
{
    struct dbgfmt_serialization_result* ser_res = malloc(sizeof(struct dbgfmt_serialization_result));

    uint16_t length = strlen(payload->label->data) + 1 + sizeof(payload->address);
    uint8_t* result = malloc(length);
    uint8_t* origin = result;

    strcpy(result, payload->label->data);
    result += strlen(payload->label->data) + 1;
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
    size_t datalen = length - 1 * sizeof(uint16_t); // FIXME: There should be a better way of handling string length.

    // Read the string data into storage, then assign
    // the bstring.
    char* storage = malloc(datalen); // Length includes NULL terminator.
    memcpy(storage, data, datalen);
    payload->data = bfromcstr(storage);
    free(storage);

    // Read in other properties.
    memcpy(&payload->address, (char*)data + datalen, sizeof(uint16_t));

    // Free the passed data.
    free(data);

    return payload;
}

struct dbg_sym_payload_label* dbgfmt_deserialize_symbol_label(void* data, uint16_t length)
{
    struct dbg_sym_payload_label* payload = malloc(sizeof(struct dbg_sym_payload_label));
    size_t labellen = length - 1 * sizeof(uint16_t); // FIXME: There should be a better way of handling string length.

    // Read the string data into storage, then assign
    // the bstring.
    char* storage = malloc(labellen); // Length includes NULL terminator.
    memcpy(storage, data, labellen);
    payload->label = bfromcstr(storage);
    free(storage);

    // Read in other properties.
    memcpy(&payload->address, (char*)data + labellen, sizeof(uint16_t));

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

struct dbg_sym* dbgfmt_copy_symbol(struct dbg_sym* other)
{
    struct dbg_sym_payload_line* payload_line;
    struct dbg_sym_payload_string* payload_string;
    struct dbg_sym_payload_label* payload_label;

    switch (other->type)
    {
        case DBGFMT_SYMBOL_LINE:
            payload_line = other->payload;
            return dbgfmt_create_symbol(other->type, dbgfmt_create_symbol_line(payload_line->path, payload_line->lineno, payload_line->address));
        case DBGFMT_SYMBOL_STRING:
            payload_string = other->payload;
            return dbgfmt_create_symbol(other->type, dbgfmt_create_symbol_string(payload_string->data, payload_string->address));
        case DBGFMT_SYMBOL_LABEL:
            payload_label = other->payload;
            return dbgfmt_create_symbol(other->type, dbgfmt_create_symbol_label(payload_label->label, payload_label->address));
        default:
            assert(0 /* debugging symbol wasn't of any known type during address get */);
    }

    return NULL;
}

void dbgfmt_free_symbol(struct dbg_sym* symbol)
{
    struct dbg_sym_payload_line* payload_line;
    struct dbg_sym_payload_string* payload_string;
    struct dbg_sym_payload_label* payload_label;

    // Free payload.
    switch (symbol->type)
    {
        case DBGFMT_SYMBOL_LINE:
            payload_line = symbol->payload;
            bdestroy(payload_line->path);
            free(payload_line);
            break;
        case DBGFMT_SYMBOL_STRING:
            payload_string = symbol->payload;
            bdestroy(payload_string->data);
            free(payload_string);
            break;
        case DBGFMT_SYMBOL_LABEL:
            payload_label = symbol->payload;
            bdestroy(payload_label->label);
            free(payload_label);
            break;
        default:
            assert(0 /* debugging symbol wasn't of any known type during address get */);
    }
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

struct dbg_sym_payload_label* dbgfmt_create_symbol_label(bstring label, uint16_t address)
{
    struct dbg_sym_payload_label* payload = malloc(sizeof(struct dbg_sym_payload_label));

    payload->label = bfromcstr("");
    bassign(payload->label, label);
    payload->address = address;

    return payload;
}

uint16_t dbgfmt_get_symbol_address(struct dbg_sym* symbol)
{
    switch (symbol->type)
    {
        case DBGFMT_SYMBOL_LINE:
            return ((struct dbg_sym_payload_line*)symbol->payload)->address;
        case DBGFMT_SYMBOL_STRING:
            return ((struct dbg_sym_payload_string*)symbol->payload)->address;
        case DBGFMT_SYMBOL_LABEL:
            return ((struct dbg_sym_payload_label*)symbol->payload)->address;
        default:
            assert(0 /* debugging symbol wasn't of any known type during address get */);
    }

    return 0;
}

void dbgfmt_update_symbol(struct dbg_sym* symbol, uint16_t address)
{
    switch (symbol->type)
    {
        case DBGFMT_SYMBOL_LINE:
            ((struct dbg_sym_payload_line*)symbol->payload)->address = address;
            break;
        case DBGFMT_SYMBOL_STRING:
            ((struct dbg_sym_payload_string*)symbol->payload)->address = address;
            break;
        case DBGFMT_SYMBOL_LABEL:
            ((struct dbg_sym_payload_label*)symbol->payload)->address = address;
            break;
        default:
            assert(0 /* debugging symbol wasn't of any known type during update */);
    }
}

void dbgfmt_finalize_symbol(struct dbg_sym* symbol, uint16_t address)
{
    if (dbgfmt_get_symbol_address(symbol) == DBGFMT_UNDETERMINED)
        dbgfmt_update_symbol(symbol, address);
}

void dbgfmt_update_symbol_list(list_t* symbols, uint16_t address)
{
    unsigned int i;

    for (i = 0; i < list_size(symbols); i++)
        dbgfmt_update_symbol((struct dbg_sym*)list_get_at(symbols, i), address);
}