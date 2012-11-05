#include "dtdb.h"

extern list_t* symbols;

int dtdb_read_symbols(const char* path)
{
    ddbg_load_symbols(bfromcstr(path));
    return (symbols != NULL);
}

int32_t dtdb_get_line_address(const char* file, int line)
{
    return ddbg_file_to_address(bfromcstr(file), line);
}


