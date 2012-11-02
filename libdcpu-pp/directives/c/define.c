#include "../c.h"
#include <stdlib.h>
#include <derr.h>

static void replace_handle(state_t* state, match_t* match, bool* reprocess)
{
    // Print out the definition.
    ppimpl_printf(state, "%s", ((bstring)match->userdata)->data);
}

static void macro_handle(bstring word, state_t* state, match_t* match, bool* reprocess)
{

}

static void define_handle(state_t* state, match_t* match, bool* reprocess)
{
    // We need to parse this manually because we're interested in getting
    // the first word and then all of the content until a line that doesn't end
    // with "\".
    bstring word = bfromcstr("");
    bstring definition = bfromcstr("");
    bool getting_word = true;
    bool getting_definition = true;
    match_t* new_match;

    // Get the first word.
    while (getting_word)
    {
        char c = ppimpl_get_input(state);
        bconchar(word, c);
        bltrimws(word);

        // Handle termination.
        if (blength(word) > 0 && c == ' ')
        {
            // End of word.
            btrimws(word);
            getting_word = false;
        }
        else if (blength(word) == 0 && c == '\n')
            dhalt(ERR_PP_DEFINE_PARAMETERS_INCORRECT, ppimpl_get_location(state));
    }

    // Get the definition.
    while (getting_definition)
    {
        char c = ppimpl_get_input(state);
        bconchar(definition, c);
        bltrimws(definition);
        if (blength(definition) > 0 && c == '\n')
        {
            // Only terminate if not preceeded by a slash.
            if (definition->data[blength(definition) - 2] != '\\')
            {
                // End of definition.
                btrimws(definition);
                getting_definition = false;
            }
        }
        else if (blength(definition) == 0 && c == '\n')
        {
            // By default, replace with 1.
            bassigncstr(definition, "1");
            getting_definition = false;
        }
    }

    // Create the new replacement handler.
    new_match = malloc(sizeof(match_t));
    new_match->text = bautofree(word);
    new_match->handler = replace_handle;
    new_match->line_start_only = false;
    new_match->identifier_only = true;
    new_match->userdata = definition;
    ppimpl_register(state, new_match);
    *reprocess = true;
}

void ppimpl_c_define_register(state_t* state)
{
    // Register .DEFINE directive.
    match_t* match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#define "));
    match->handler = define_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    ppimpl_register(state, match);
}
