/**

    File:       ppimpl.c

    Project:    DCPU-16 Toolchain
    Component:  LibDCPU-PP

**/

#define PREPROCESSOR_INTERNALS

#include <assert.h>
#include <bstring.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ppimpl.h"
#include "directives/asm.h"
#include "directives/c.h"

///
/// Returns whether there is cached or provided input available.
///
bool ppimpl_has_input(state_t* state)
{
    return (list_size(&state->cached_input) > 0 || state->has_input());
}

///
/// Retrieves a character from either cached or provided input.
///
char ppimpl_get_input(state_t* state)
{
    assert(ppimpl_has_input(state));
    if (list_size(&state->cached_input) > 0)
        return (char)list_extract_at(&state->cached_input, 0);
    else
    {
        char c = state->input();

        // If this is a newline, increment the current line count.
        if (c == '\n')
            state->current_line += 1;

        return c;
    }
}

///
/// Retrieves the current line number.
///
unsigned int ppimpl_get_current_line(state_t* state)
{
    return state->current_line;
}

///
/// Retrieves the current filename, or "<unknown>" if not previously set.
///
bstring ppimpl_get_current_filename(state_t* state)
{
    if (state->current_filename == NULL)
        return state->default_filename;
    else
        return state->current_filename;
}

///
/// Generates a char* that contains location information for use with
/// warnings or errors.  Since the location string is generated, you
/// should free it if possible (but this is not the case when using
/// dhalt).
///
char* ppimpl_get_location(state_t* state)
{
    bstring b = bformat("line %u of '%s'", ppimpl_get_current_line(state), ppimpl_get_current_filename(state)->data);
    return b->data;
}

///
/// Prints a formatted string to the input cache of the current state.
///
void ppimpl_printf(state_t* state, const char* fmt, ...)
{
    char* store;
    int count, i;
    va_list argptr;
#ifndef WIN32
    va_list argptrc;
#endif
    va_start(argptr, fmt);
#ifndef WIN32
    va_copy(argptrc, argptr);
#endif
    count = vsnprintf(NULL, 0, fmt, argptr);
#ifndef WIN32
    count++;
#endif
    store = malloc(count + 1);
    memset(store, '\0', count + 1);
#ifndef WIN32
    vsnprintf(store, count, fmt, argptrc);
#else
    vsnprintf(store, count, fmt, argptr);
#endif
    va_end(argptr);
    for (i = count - 1; i >= 0; i--)
    {
        if (store[i] == '\0')
            continue;
        list_insert_at(&state->cached_input, (void*)store[i], 0);
    }
    free(store);
}

///
/// Prints a formatted string at the specified index in the input cache.
///
void ppimpl_pprintf(state_t* state, size_t index, const char* fmt, ...)
{
    char* store;
    int count, i;
    va_list argptr;
#ifndef WIN32
    va_list argptrc;
#endif
    va_start(argptr, fmt);
#ifndef WIN32
    va_copy(argptrc, argptr);
#endif
    count = vsnprintf(NULL, 0, fmt, argptr);
#ifndef WIN32
    count++;
#endif
    store = malloc(count + 1);
    memset(store, '\0', count + 1);
#ifndef WIN32
    vsnprintf(store, count, fmt, argptrc);
#else
    vsnprintf(store, count, fmt, argptr);
#endif
    va_end(argptr);
    for (i = count - 1; i >= 0; i--)
    {
        if (store[i] == '\0')
            continue;
        list_insert_at(&state->cached_input, (void*)store[i], index);
    }
    free(store);
}

///
/// Returns whether the character is considered to isolate
/// an identifier from it's surroundings.
///
bool ppimpl_isolates(char c, bool at_start)
{
    if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (!at_start && c >= '0' && c <= '9') ||
            c == '_')
        return false;
    else
        return true;
}

///
/// Checks to see if the current output cache
/// has a match against a specified match definition.
///
bool ppimpl_match_test(state_t* state, match_t* match)
{
    size_t i;
    int32_t j;

    // Check if the output cache is even long enough to hold
    // the match.
    if (list_size(&state->cached_output) < (size_t)blength(match->text.ref))
        return false;

    // Check each character until we find one that doesn't
    // match, going backwards through the list (remember that
    // we're matching against the end of the list).
    for (i = 0; i < (size_t)blength(match->text.ref); i++)
    {
        if (!match->case_insensitive)
        {
            if ((char)list_get_at(&state->cached_output, list_size(&state->cached_output) - i - 1) !=
                    (char)match->text.ref->data[blength(match->text.ref) - i - 1])
                return false;
        }
        else
        {
            if (tolower((char)list_get_at(&state->cached_output, list_size(&state->cached_output) - i - 1)) !=
                    tolower((char)match->text.ref->data[blength(match->text.ref) - i - 1]))
                return false;
        }
    }

    // We have a match, ensure that it is at the start of the line
    // if required.
    if (match->line_start_only)
    {
        // We have to make sure that we only have spaces or tabs between
        // the first character of the text and a \n or the start of the output.
        j = (int32_t)(list_size(&state->cached_output) - (size_t)blength(match->text.ref) - 1);
        while (j >= 0)
        {
            switch ((char)list_get_at(&state->cached_output, j))
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
            j--;
        }

        // If we reach here, check if i == 0, which means that we've
        // matched the start of the input (which also counts as a match).
        if (j <= 0)
            return true;
        else
            assert(false); // Something is not right.
    }

    // We have a match, ensure it is a proper identifier.
    if (match->identifier_only)
    {
        // We have to get the surrounding characters.
        bool has_next = ppimpl_has_input(state);
        bool has_previous = (list_size(&state->cached_output) >= (size_t)blength(match->text.ref) + 1);
        char next;
        char previous;
        bool is_identifier = false;
        if (has_next)
        {
            next = ppimpl_get_input(state);
            list_insert_at(&state->cached_input, (void*)next, 0);
        }
        if (has_previous)
            previous = (char)list_get_at(&state->cached_output, list_size(&state->cached_output) - blength(match->text.ref) - 1);

        // Check to see if the preceeding character (before the
        // match) and the proceeding character (after the match)
        // both isolate this match as an identifier.
        if (has_next && has_previous)
            is_identifier = ppimpl_isolates(next, false) && ppimpl_isolates(previous, true);
        else if (has_next)
            is_identifier = ppimpl_isolates(next, false);
        else if (has_previous)
            is_identifier = ppimpl_isolates(previous, true);
        else
            is_identifier = true;

        return is_identifier;
    }

    // We have a match that can be matched anywhere.
    return true;
}

///
/// Tracks the last character in the output buffer and
/// determines whether or not the current context is
/// inside a string.
///
void ppimpl_track_strings(state_t* state)
{
    char current;
    char previous;
    bool has_previous = false;

    // Get the character just pushed into the buffer.
    current = (char)list_get_at(&state->cached_output, list_size(&state->cached_output) - 1);

    // If the buffer is big enough, also get the character before
    // that so we can check for escaping.
    has_previous = (list_size(&state->cached_output) >= 2);
    if (has_previous)
        previous = (char)list_get_at(&state->cached_output, list_size(&state->cached_output) - 2);

    // Update the string states.
    if (!state->in_single_string && !state->in_double_string)
    {
        // We are not currently in either string.
        switch (current)
        {
            case '\'':
                // Now entering a single string.
                state->in_single_string = true;
                return;
            case '"':
                // Now entering a double string.
                state->in_double_string = true;
                return;
            default:
                // Some other character was processed, ignore.
                return;
        }
    }
    else if (state->in_single_string)
    {
        // We are in a single string.
        switch (current)
        {
            case '\'':
                // Potentially exiting a single string; check the previous
                // character.
                if (!has_previous || (has_previous && previous != '\\'))
                    state->in_single_string = false;
                return;
            case '\n':
                // A dangling string.
                state->in_single_string = false;
                return;
            default:
                // Some other character was processed, ignore.
                return;
        }
    }
    else if (state->in_double_string)
    {
        // We are in a double string.
        switch (current)
        {
            case '"':
                // Potentially exiting a single string; check the previous
                // character.
                if (!has_previous || (has_previous && previous != '\\'))
                    state->in_double_string = false;
                return;
            case '\n':
                // A dangling string.
                state->in_double_string = false;
                return;
            default:
                // Some other character was processed, ignore.
                return;
        }
    }
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
    scope_t* current;

    while (ppimpl_has_input(state))
    {
        // Fetch a character from the input.
        c = ppimpl_get_input(state);

        // Push it onto the output.
        list_append(&state->cached_output, (void*)c);

        // Update string tracking states.
        ppimpl_track_strings(state);

        // If we are in a single or double string, no matching
        // operations can be performed.
        if (state->in_single_string || state->in_double_string)
            continue;

        // We keep process once unless we're asked to restart
        // by a match.  This is used when a match changes the
        // handlers (such as appending a new handler or removing
        // an old one), and we need to restart the matching process.
        do
        {
            reprocess = false;

            // Iterate through all of our match handlers.
            for (i = 0; i < list_size(&state->handlers); i++)
            {
                m = (match_t*)list_get_at(&state->handlers, i);

                // Test for a match and if so, handle it.
                if (ppimpl_match_test(state, m))
                {
                    // Remove the characters from the cached input
                    // depending on the match text length.
                    list_delete_range(&state->cached_output,
                                      list_size(&state->cached_output) - blength(m->text.ref),
                                      list_size(&state->cached_output) - 1);

                    // Call the match handler.
                    m->handler(state, m, &reprocess);
                    if (reprocess)
                        break;
                }
            }
        }
        while (reprocess);
    }

    // We have finished processing; everything is stored in the cached output,
    // so now we push everything that was in the cached output.
    while (list_size(&state->cached_output) > 0)
        state->output((char)list_extract_at(&state->cached_output, 0));
}

///
/// Pushes a new scope onto the stack.
///
void ppimpl_push_scope(state_t* state, bool active)
{
    scope_t* scope = malloc(sizeof(scope_t));
    scope->active = active;
    scope->start_index = list_size(&state->cached_output);
    list_append(&state->scopes, scope);
    list_init(&scope->old_handlers);
    list_init(&scope->new_handlers);
}

///
/// Flips the current scope on the stack.
///
void ppimpl_flip_scope(state_t* state)
{
    scope_t* scope = list_get_at(&state->scopes, list_size(&state->scopes) - 1);
    bool active = scope->active;
    ppimpl_pop_scope(state);
    ppimpl_push_scope(state, !active);
}

///
/// Adds a handler inside a scope, dealing with overridding
/// existing handlers.
///
void ppimpl_register(state_t* state, match_t* match)
{
    scope_t* scope;
    match_t* other;
    size_t a;

    if (list_size(&state->scopes) == 0)
    {
        // Global registration.
        list_append(&state->handlers, match);
        return;
    }
    scope = list_extract_at(&state->scopes, list_size(&state->scopes) - 1);

    // Check to see if this is already defined.
    for (a = 0; a < list_size(&state->handlers); a++)
    {
        other = list_get_at(&state->handlers, a);
        if (biseq(other->text.ref, match->text.ref))
        {
            // Collision; move the existing handler into
            // the scope's "old_handlers" list.
            list_append(&scope->old_handlers, other);
            list_extract_at(&state->handlers, a);
            a--;
        }
    }

    // Append to handlers and add the name to the scope's
    // new_handles so it can be cleared when the scope is
    // popped.
    list_append(&state->handlers, match);
    list_append(&scope->new_handlers, bstrcpy(match->text.ref));
}

///
/// Pops a scope from the stack.
///
void ppimpl_pop_scope(state_t* state)
{
    scope_t* scope;
    match_t* match;
    match_t* old;
    bstring name;
    size_t a, i;
    if (list_size(&state->scopes) == 0)
        return;
    scope = list_extract_at(&state->scopes, list_size(&state->scopes) - 1);

    // Delete text if this scope was silenced.
    if (!scope->active)
    {
        list_delete_range(&state->cached_output,
                scope->start_index,
                list_size(&state->cached_output) - 1);
    }

    // Delete handlers that were defined this scope.
    list_iterator_start(&scope->new_handlers);
    while (list_iterator_hasnext(&scope->new_handlers))
    {
        name = list_iterator_next(&scope->new_handlers);

        for (i = 0; i < list_size(&state->handlers); i++)
        {
            old = list_get_at(&state->handlers, i);
            if (biseq(name, old->text.ref))
            {
                // We need remove the old handler.
                // FIXME: Free the old handler.
                list_delete_at(&state->handlers, i);
                i--;
            }
        }

        bdestroy(name);
    }
    list_iterator_stop(&scope->new_handlers);
    list_destroy(&scope->new_handlers);

    // Restore handlers.
    for (a = 0; a < list_size(&scope->old_handlers); a++)
    {
        match = list_get_at(&scope->old_handlers, a);

        // Restore the old handler.
        list_append(&state->handlers, match);
        list_extract_at(&scope->old_handlers, a);
        a--;
    }
    list_destroy(&scope->old_handlers);

    // Free memory.
    free(scope);
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
void ppimpl(freed_bstring filename, int line, freed_bstring lang, has_t has_input, pop_t input, push_t output)
{
    state_t state;
    list_init(&state.cached_input);
    list_init(&state.cached_output);
    list_init(&state.handlers);
    list_init(&state.scopes);
    list_attributes_comparator(&state.cached_input, list_comparator_char_t);
    list_attributes_comparator(&state.cached_output, list_comparator_char_t);
    list_attributes_copy(&state.cached_input, list_meter_char_t, false);
    list_attributes_copy(&state.cached_output, list_meter_char_t, false);
    list_attributes_hash_computer(&state.cached_input, list_hashcomputer_char_t);
    list_attributes_hash_computer(&state.cached_output, list_hashcomputer_char_t);
    state.has_input = has_input;
    state.input = input;
    state.output = output;
    state.current_line = line;
    state.current_filename = bstrcpy(filename.ref);
    state.default_filename = bfromcstr("<unknown>");
    state.in_single_string = false;
    state.in_double_string = false;
    if (biseqcstrcaseless(lang.ref, "asm"))
    {
        ppimpl_asm_line_register(&state);
        ppimpl_asm_expr_register(&state);
        ppimpl_asm_define_register(&state);
        ppimpl_asm_include_register(&state);
	ppimpl_asm_init(&state);
    }
    else if (biseqcstrcaseless(lang.ref, "c"))
    {
        ppimpl_c_line_register(&state);
        ppimpl_c_expr_register(&state);
        ppimpl_c_define_register(&state);
        ppimpl_c_include_register(&state);
        ppimpl_c_init(&state);
    }
    ppimpl_process(&state);
    bautodestroy(lang);
    bautodestroy(filename);
}
