#ifndef TOOLCHAIN_DTDB_H
#define TOOLCHAIN_DTDB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dbgaux.h>
#include <ddata.h>
#include <simclist.h>

int dtdb_read_symbols(const char* path);
int32_t dtdb_get_line_address(const char* file, int line);

#ifdef __cplusplus
}
#endif

#endif
