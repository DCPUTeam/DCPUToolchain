#include <stdio.h>
#include <stdlib.h>
#include <ddata.h>

int main(int argc, char** argv)
{
	struct dbg_sym_basic_payload* pld = dbgfmt_basic_payload("ohai", 0xabad, 0x1dea);
	struct dbg_sym* sym = dbgfmt_debugging_symbol(DBGFMT_BASIC, (void*) pld);
	struct dbg_sym_file* file;

	printf("%s %x %x %d %d\n", pld->path, pld->lineno, pld->address, sym->length, sym->type);
	dbgfmt_write_to_file("test.bin", 1, sym);

	file = dbgfmt_read_file("test.bin");
	printf("reading: %x %x\n", file->magic, file->num_symbols);

	return 0;
}