#include "../asm.h"

static void print_track(state_t* state, bstring file, int line)
{
    ppimpl_printf(state, "# %i %s\n", line, file->data);
    state->current_line--;
}

void ppimpl_asm_track_register(state_t* state)
{
    // Register tracking handler.
    state->print_tracking = print_track;
}

