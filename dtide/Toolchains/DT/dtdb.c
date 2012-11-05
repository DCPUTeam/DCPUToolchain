#include "dtdb.h"

extern list_t* symbols;
struct dbg_sym_payload_line* found_line = NULL;

int dtdb_read_symbols(const char* path)
{
    ddbg_load_symbols(bfromcstr(path));
    return (symbols != NULL);
}

int32_t dtdb_get_line_address(const char* file, int line)
{
    return ddbg_file_to_address(bfromcstr(file), line);
}

bool dtdb_line_at_address(uint16_t address)
{
    int i;
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

    found_line = NULL;
    return false;
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



