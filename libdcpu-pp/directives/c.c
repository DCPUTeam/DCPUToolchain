#include "../ppimpl.h"
#include "c.h"

void ppimpl_c_init(state_t* state)
{
    // We need to output the initial # 1 filename directive
    // so the compiler knows where we are.
    //ppimpl_printf(state, "# %i %s\n", state->current_line, state->current_filename->data);
}