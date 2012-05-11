#include <stdio.h>
#include <stdlib.h>
#include <ddata.h>

int main(int argc, char** argv) {
	struct dbg_sym_basic_payload* pld = dbgfmt_basic_payload("ohai", 0xabad, 0x1dea);
	struct dbg_sym* sym = dbgfmt_debugging_symbol(DBGFMT_BASIC, (void*) pld);
	printf("%s %x %x\n", pld->path, pld->lineno, pld->address);
	dbgfmt_write_to_file("test.bin", 1, sym);
	
	return 0;
}