#include "../c.h"

static void print_track(state_t* state, bstring file, int line)
{
    ppimpl_oprintf(state, "# %i %s\n", line, file->data);
}

void ppimpl_c_track_register(state_t* state)
{
    // Register tracking handler.
    state->print_tracking = print_track;
}
