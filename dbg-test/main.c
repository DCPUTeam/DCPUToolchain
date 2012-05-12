#include <stdio.h>
#include <stdlib.h>
#include <ddata.h>
#include <simclist.h>

int main(int argc, char** argv)
{
	size_t i = 0;
	struct dbg_sym* sym;
	struct dbg_sym_payload_line* payload_line;
	
	// Read in data.
	list_t* result = dbgfmt_read(bfromcstr("test.dsym16"));
	
	// Display data.
	printf("Loaded %i symbols.\n", list_size(result));
	for (i = 0; i < list_size(result); i++)
	{
		sym = list_get_at(result, i);
		switch (sym->type)
		{
			case DBGFMT_SYMBOL_LINE:
				payload_line = (struct dbg_sym_payload_line*)sym->payload;
				printf("Line information: %s:%u is at 0x%04X\n", payload_line->path->data, payload_line->lineno, payload_line->address);
				break;
			default:
				printf("Unknown\n");
				break;
		}
	}
	
	return 0;
}