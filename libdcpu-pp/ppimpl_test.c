#define PREPROCESSOR_INTERNALS

#include <bstring.h>
#include <stdlib.h>
#include "ppimpl.h"

void my_match(state_t* state, match_t* match, bool* reprocess)
{
    printf(">> match detected.\n");
}

void ppimpl_test_register(state_t* state)
{
    match_t* match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr(".TEST"));
    match->handler = my_match;
    match->line_start_only = true;
    list_append(&state->handlers, match);
}
