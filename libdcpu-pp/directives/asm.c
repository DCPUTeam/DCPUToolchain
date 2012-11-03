#include "../ppimpl.h"
#include "asm.h"

void ppimpl_asm_init(state_t* state)
{
    // We need to output the initial # 1 filename directive
    // so the assembler knows where we are.
    ppimpl_printf(state, "# %i %s\n", state->current_line, state->current_filename->data);
}