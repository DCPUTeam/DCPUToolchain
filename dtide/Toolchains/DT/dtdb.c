#include "dtdb.h"
#include <simclist.h>

extern list_t* symbols;
struct dbg_sym_payload_line* found_line = NULL;

struct dbg_sym_payload_line** adressToLineArray;

int dtdb_read_symbols(const char* path)
{
    unsigned int i;
    struct dbg_sym* sym;
    struct dbg_sym_payload_line* payload_line;
    uint16_t max_address = 0;
    
    // load symbols using the debugger library
    ddbg_load_symbols(bfromcstr(path));
    if (symbols == NULL) return 0;
    
    // search for maximum address:
    list_iterator_start(symbols);
    while (list_iterator_hasnext(symbols))
    {
        sym = (struct dbg_sym*) list_iterator_next(symbols);
        if(sym->type == DBGFMT_SYMBOL_LINE)
        {
            payload_line = (struct dbg_sym_payload_line*) sym->payload;
            if(payload_line->address > max_address)
            {
                max_address = payload_line->address;
            }
        }
    }
    list_iterator_stop(symbols);
    
    // init array for fast symbol lookup by address
    adressToLineArray = malloc(sizeof(struct dbg_sym_payload_line*)*(max_address+1));
    for (i = 0; i <= max_address; i++)
    {
        adressToLineArray[i] = NULL;
    }
    
    // fill lookup array with symbols
    list_iterator_start(symbols);
    while (list_iterator_hasnext(symbols))
    {
        sym = (struct dbg_sym*) list_iterator_next(symbols);
        if(sym->type == DBGFMT_SYMBOL_LINE)
        {
            payload_line = (struct dbg_sym_payload_line*) sym->payload;
            if(payload_line->address >= 0 && payload_line->address <= max_address)
            {
                if(adressToLineArray[payload_line->address] == NULL)
                {
                    adressToLineArray[payload_line->address] = sym->payload;
                }
            }
        }
    }
    list_iterator_stop(symbols);
    
    
    return 1;
}

int32_t dtdb_get_line_address(const char* file, int line)
{
    return ddbg_file_to_address(bfromcstr(file), line);
}

bool dtdb_line_at_address(uint16_t address)
{
    /*
    size_t i;
    struct dbg_sym* sym;
    struct dbg_sym_payload_line* payload_line;

    if(symbols == NULL) return false;

    for(i = 0; i < list_size(symbols); i++)
    {
        sym = list_get_at(symbols, i);
        if(sym->type == DBGFMT_SYMBOL_LINE)
        {
            payload_line = (struct dbg_sym_payload_line*) sym->payload;
            if(payload_line->address == address)
            {
                found_line = payload_line;
                return true;
            }
        }
    }
    */
    
    if (adressToLineArray[address] == NULL)
    {
        found_line = NULL;
        return false;
    }
    else
    {
        found_line = adressToLineArray[address];
        return true;
    }
}

int dtdb_get_line_number()
{
    if(found_line == NULL) return -1;
    return found_line->lineno;
}

char* dtdb_get_line_path()
{
    if(found_line == NULL) return NULL;
    return found_line->path->data;
}



