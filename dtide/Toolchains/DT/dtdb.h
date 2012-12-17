#ifndef TOOLCHAIN_DTDB_H
#define TOOLCHAIN_DTDB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dbgaux.h>
#include <ddata.h>
#include <simclist.h>
// For bool and stdint
#include <dcpu.h>

int     dtdb_read_symbols(const char* path);
int32_t dtdb_get_line_address(const char* file, int line);
bool    dtdb_line_at_address(uint16_t address);

int     dtdb_get_line_number();
char*   dtdb_get_line_path();

#ifdef __cplusplus
}
#endif

#endif
