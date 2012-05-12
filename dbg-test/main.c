#include <stdio.h>
#include <stdlib.h>
#include <ddata.h>

int main(int argc, char** argv)
{
	struct dbg_sym_payload_line* pld = dbgfmt_create_symbol_line(bfromcstr("ohai"), 0xabad, 0x1dea);
	struct dbg_sym* sym = dbgfmt_create_symbol(DBGFMT_SYMBOL_LINE, (void*) pld);
	struct dbg_sym_file* file;

	printf("%s %x %x %d %d\n", pld->path, pld->lineno, pld->address, sym->length, sym->type);
	dbgfmt_write(bfromcstr("test.bin"), 1, sym);

	file = dbgfmt_read(bfromcstr("test.bin"));
	printf("reading: %x %x\n", file->magic, file->num_symbols);
	dbgfmt_get_basic(file->symbols);
	return 0;
}