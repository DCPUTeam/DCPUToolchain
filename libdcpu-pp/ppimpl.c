/**

    File:       ppimpl.c
    
    Project:    DCPU-16 Toolchain
    Component:  LibDCPU-PP

**/

#define PREPROCESSOR_INTERNALS

#include <assert.h>
#include <bstring.h>
#include "ppimpl.h"
#include "ppimpl_test.h"

///
/// Retrieves a character from either cached or provided input.
///
char ppimpl_get_input(state_t* state)
{
    if (list_size(&state->cached_input) > 0)
        return (char)list_extract_at(&state->cached_input, 0);
    else
        return state->input();
}

///
/// Checks to see if the current output cache
/// has a match against a specified match definition.
///
bool ppimpl_match_test(list_t* output_cache, match_t* match)
{
    size_t i;

    // Check if the output cache is even long enough to hold
    // the match.
    if (list_size(output_cache) < (size_t)blength(match->text.ref))
        return false;
    
    // Check each character until we find one that doesn't
    // match, going backwards through the list (remember that
    // we're matching against the end of the list).
    for (i = 0; i < (size_t)blength(match->text.ref); i++)
    {
        if ((char)list_get_at(output_cache, list_size(output_cache) - i - 1) !=
            (char)match->text.ref->data[blength(match->text.ref) - i - 1])
            return false;
    }

    // We have a match, ensure that it is at the start of the line
    // if required.
    if (match->line_start_only)
    {
        // We have to make sure that we only have spaces or tabs between
        // the first character of the text and a \n or the start of the output.
        i = list_size(output_cache) - (size_t)blength(match->text.ref) - 1;
        while (i >= 0)
        {
            switch ((char)list_get_at(output_cache, i))
            {
            case ' ':
            case '\t':
                // Permitted; continue.
                break;
            case '\n':
                // This means that it is at the start of a line.
                return true;
            default:
                // Anything else means that it's not at the start.
                return false;
            }
            i--;
        }

        // If we reach here, check if i == 0, which means that we've
        // matched the start of the input (which also counts as a match).
        if (i == 0)
            return true;
        else
            assert(false); // Something is not right.
    }

    // We have a match that can be matched anywhere.
    return true;
}

///
/// Performs preprocessing using the internally
/// constructed state object.
///
void ppimpl_process(state_t* state)
{
    char c;
    match_t* m;
    bool reprocess = false;
    size_t i;

    while (state->has_input())
    {
        // Fetch a character from the input.
        c = ppimpl_get_input(state);

        // Push it onto the output.
        list_append(&state->cached_output, (void*)c);

        // We keep process once unless we're asked to restart
        // by a match.  This is used when a match changes the
        // handlers (such as appending a new handler or removing
        // an old one), and we need to restart the matching process.
        do
        {
            // Iterate through all of our match handlers.
            for (i = 0; i < list_size(&state->handlers); i++)
            {
                m = (match_t*)list_get_at(&state->handlers, i);
            
                // Test for a match and if so, handle it.
                if (ppimpl_match_test(&state->cached_output, m))
                {
                    // Remove the characters from the cached input
                    // depending on the match text length.
                    list_delete_range(&state->cached_output,
                        list_size(&state->cached_output) - blength(m->text.ref),
                        list_size(&state->cached_output) - 1);

                    // Call the match handler.
                    reprocess = false;
                    m->handler(state, m, &reprocess);
                    if (reprocess)
                        break;
                }
            }
        } while (reprocess);
    }

    // We have finished processing; everything is stored in the cached output,
    // so now we push everything that was in the cached output.
    while (list_size(&state->cached_output) > 0)
        state->output((char)list_extract_at(&state->cached_output, 0));
}

///
/// Maps the list_*_int8_t definitions as character definitions.
///
#define list_comparator_char_t list_comparator_int8_t
#define list_meter_char_t list_meter_int8_t
#define list_hashcomputer_char_t list_hashcomputer_int8_t

///
/// Performs preprocessing.
///
void ppimpl(has_t has_input, pop_t input, push_t output)
{
    state_t state;
    list_init(&state.cached_input);
    list_init(&state.cached_output);
    list_init(&state.handlers);
    list_attributes_comparator(&state.cached_input, list_comparator_char_t);
    list_attributes_comparator(&state.cached_output, list_comparator_char_t);
    list_attributes_copy(&state.cached_input, list_meter_char_t, false);
    list_attributes_copy(&state.cached_output, list_meter_char_t, false);
    list_attributes_hash_computer(&state.cached_input, list_hashcomputer_char_t);
    list_attributes_hash_computer(&state.cached_output, list_hashcomputer_char_t);
    state.has_input = has_input;
    state.input = input;
    state.output = output;
    ppimpl_test_register(&state);
    ppimpl_process(&state);
}