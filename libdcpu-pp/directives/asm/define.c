#include "../asm.h"
#include <stdlib.h>
#include <simclist.h>
#include <derr.h>

struct replace_info
{
    bstring full; //< The full name of the replacement, including macro parameters.
    bstring replacement; //< The replacement text.
};

static void replace_handle(state_t* state, match_t* match, bool* reprocess)
{
    struct replace_info* info = match->userdata;

    // Print out the definition.
    ppimpl_printf(state, "%s", info->replacement->data);
}

static void macro_handle(state_t* state, match_t* match, bool* reprocess)
{
    bstring name;
    bstring temp;
    list_t parameters;
    list_t arguments;
    bool getting_name = true;
    bool getting_parameters = false;
    bool getting_arguments = false;
    struct replace_info* info = match->userdata;
    int i = 0;
    int argument_brackets = 0;
    char c;
    char* start_loc;
    match_t* new_match;
    struct replace_info* new_info;

    // Parse the arguments out of the name.
    list_init(&parameters);
    temp = bfromcstr("");
    for (i = 0; i < blength(info->full); i++)
    {
        c = info->full->data[i];
        if (getting_name)
        {
            if (c == '(')
            {
                getting_name = false;
                getting_parameters = true;
                name = bstrcpy(temp);
                bassigncstr(temp, "");
            }
            else
                bconchar(temp, c);
        }
        else if (getting_parameters)
        {
            if (c == ',' || c == ')')
            {
                list_append(&parameters, bstrcpy(temp));
                bassigncstr(temp, "");
                if (c == ')')
                {
                    getting_parameters = false;
                    break;
                }
            }
            else
                bconchar(temp, c);
        }
    }

    // Attempt to accept an open bracket.
    c = ppimpl_get_input(state);
    while (c == '\1')
    {
        // Consume macro termination.
        i = 0;
        while (i < strlen("\1MACROTERMINATE\1"))
        {
            if (c != "\1MACROTERMINATE\1"[i++])
                dhalt(ERR_PP_EXPECTED_OPEN_BRACKET, ppimpl_get_location(state));
            c = ppimpl_get_input(state);
        }
        ppimpl_pop_scope(state);
    }
    if (c != '(')
        dhalt(ERR_PP_EXPECTED_OPEN_BRACKET, ppimpl_get_location(state));
    
    // Read arguments.
    getting_arguments = true;
    list_init(&arguments);
    start_loc = ppimpl_get_location(state);
    bassigncstr(temp, "");
    while (ppimpl_has_input(state) && getting_arguments)
    {
        c = ppimpl_get_input(state);

        if (c == '(')
        {
            argument_brackets++;
            bconchar(temp, c);
        }
        else if (c == ')' && argument_brackets != 0)
        {
            argument_brackets--;
            bconchar(temp, c);
        }
        else if (c == ')' && argument_brackets == 0)
        {
            list_append(&arguments, bstrcpy(temp));
            bassigncstr(temp, "");
            getting_arguments = false;
            break;
        }
        else if (c == ',' && argument_brackets == 0)
        {
            list_append(&arguments, bstrcpy(temp));
            bassigncstr(temp, "");
        }
        else
            bconchar(temp, c);
    }
    if (getting_arguments)
        dhalt(ERR_PP_NO_TERMINATING_BRACKET, start_loc);

    // Check to see if the argument count is correct.
    if (list_size(&arguments) > list_size(&parameters))
        dhalt(ERR_PP_TOO_MANY_PARAMETERS, start_loc);
    else if (list_size(&arguments) < list_size(&parameters))
        dhalt(ERR_PP_NOT_ENOUGH_PARAMETERS, start_loc);
    free(start_loc);

    // Create a new scope for macro evaluation.
    ppimpl_push_scope(state, true);

    // Define the new handlers.
    for (i = 0; i < list_size(&parameters); i++)
    {
        new_info = malloc(sizeof(struct replace_info));
        new_info->full = list_get_at(&parameters, i);
        new_info->replacement = list_get_at(&arguments, i);
        if (biseq(new_info->full, new_info->replacement))
        {
            free(new_info);
            continue;
        }
        new_match = malloc(sizeof(match_t));
        new_match->text = bautofree(list_get_at(&parameters, i));
        new_match->handler = replace_handle;
        new_match->line_start_only = false;
        new_match->identifier_only = true;
        new_match->userdata = new_info;
        ppimpl_register(state, new_match);
    }
    
    // Print out the macro evaluation and terminator.
    ppimpl_printf(state, "%s\1MACROTERMINATE\1", info->replacement->data);
}

static void macro_term_handle(state_t* state, match_t* match, bool* reprocess)
{
    // Pop the macro state.
    ppimpl_pop_scope(state);
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
    struct replace_info* info;

    // Get the first word.
    while (getting_word)
    {
        char c = ppimpl_get_input(state);
        bconchar(word, c);
        bltrimws(word);

        // Handle termination.
        if (blength(word) > 0 && (c == ' ' || c == '\t'))
        {
            // End of word.
            btrimws(word);
            getting_word = false;
        }
        else if (blength(word) > 0 && c == '(')
        {
            // This form of macro definition is not permitted
            // in assembly.
            dhalt(ERR_PP_LONG_FORM_MACRO_ONLY, ppimpl_get_location(state));
        }
        else if (blength(word) == 0 && c == '\n')
            dhalt(ERR_PP_ASM_DEFINE_PARAMETERS_INCORRECT, ppimpl_get_location(state));
        else if (blength(word) > 0 && c == '\n')
        {
            // End of word.
            btrimws(word);
            getting_word = false;
            getting_definition = false;
            ppimpl_printf(state, "\n");
        }
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
                ppimpl_printf(state, "\n");
            }
        }
        else if (blength(definition) == 0 && c == '\n')
        {
            // By default, replace with 1.
            bassigncstr(definition, "1");
            getting_definition = false;
            ppimpl_printf(state, "\n");
        }
    }

    // Create the new replacement handler.
    info = malloc(sizeof(struct replace_info));
    info->full = word;
    info->replacement = definition;
    if (biseq(info->full, info->replacement))
    {
        free(info);
        return;
    }
    new_match = malloc(sizeof(match_t));
    new_match->text = bautofree(word);
    new_match->handler = replace_handle;
    new_match->line_start_only = false;
    new_match->identifier_only = true;
    new_match->userdata = info;
    ppimpl_register(state, new_match);
    *reprocess = true;
}

static void macro_def_handle(state_t* state, match_t* match, bool* reprocess)
{
    // We need to parse this manually because we're interested in getting
    // the first word and then all of the content until a line that has
    // .ENDMACRO on it.
    bstring name = bfromcstr("");
    bstring word = bfromcstr("");
    bstring definition = bfromcstr("");
    bool getting_word = true;
    bool getting_definition = true;
    bool is_macro = false;
    match_t* new_match;
    struct replace_info* info;
    const char* term_match = ".ENDMACRO";
    const char* term_match_alt = "#ENDMACRO";
    int term_match_count = 0;

    // Get the first word.
    while (getting_word)
    {
        char c = ppimpl_get_input(state);
        bconchar(word, c);
        if (!is_macro && c != '(')
            bconchar(name, c);
        bltrimws(word);

        // Handle termination.
        if (blength(word) > 0 && (c == ' ' || c == '\t') && !is_macro)
        {
            // End of word.
            btrimws(word);
            btrimws(name);
            getting_word = false;
        }
        else if (blength(word) > 0 && c == '(' && !is_macro)
        {
            // Start of macro.
            is_macro = true;
        }
        else if (blength(word) > 0 && c == '(' && is_macro)
        {
            // Second ( in a macro; error.
            dhalt(ERR_PP_MACRO_MALFORMED, ppimpl_get_location(state));
        }
        else if (blength(word) > 0 && c == ')' && is_macro)
        {
            // End of macro name.
            btrimws(word);
            btrimws(name);
            getting_word = false;
        }
        else if (blength(word) == 0 && c == '\n')
            dhalt(ERR_PP_ASM_MACRO_PARAMETERS_INCORRECT, ppimpl_get_location(state));
    }

    // Check to make sure this is a macro.
    if (!is_macro)
        dhalt(ERR_PP_ASM_MACRO_PARAMETERS_EXPECTED, ppimpl_get_location(state));

    // Get the definition.
    while (getting_definition && ppimpl_has_input(state))
    {
        char c = ppimpl_get_input(state);
        bconchar(definition, c);
        bltrimws(definition);

        if (tolower(c) == tolower(term_match[term_match_count]) ||
                tolower(c) == tolower(term_match_alt[term_match_count]))
        {
            bool okay = true;
            if (term_match_count == 0 && blength(definition) >= 2)
            {
                int ip = blength(definition) - 2;
                while (ip > 0)
                {
                    char p = definition->data[ip];
                    if (p != ' ' && p != '\n')
                    {
                        okay = false;
                        break;
                    }
                    else if (p == '\n')
                        break;
                    ip--;
                }
            }
            if (okay)
            {
                term_match_count++;
                if (term_match_count >= strlen(term_match))
                {
                    btrimws(definition);
                    bdelete(definition, blength(definition) - strlen(term_match), strlen(term_match));
                    getting_definition = false;
                }
            }
        }
        else
            term_match_count = 0;
    }
    if (getting_definition)
        dhalt(ERR_PP_ASM_MACRO_TERMINATION_NOT_FOUND, ppimpl_get_location(state));

    // Create the new replacement handler.
    info = malloc(sizeof(struct replace_info));
    info->full = word;
    info->replacement = definition;
    if (biseq(info->full, info->replacement))
    {
        free(info);
        return;
    }
    new_match = malloc(sizeof(match_t));
    new_match->text = bautofree(name);
    new_match->handler = macro_handle;
    new_match->line_start_only = false;
    new_match->identifier_only = true;
    new_match->userdata = info;
    ppimpl_register(state, new_match);
    *reprocess = true;
}

void ppimpl_asm_define_register(state_t* state)
{
    // Register .DEFINE directive.
    match_t* match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr(".DEFINE "));
    match->handler = define_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);
    
    // Register .EQUATE directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr(".EQUATE "));
    match->handler = define_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);
    
    // Register .EQU directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr(".EQU "));
    match->handler = define_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register .MACRO directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr(".MACRO "));
    match->handler = macro_def_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register #DEFINE directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#DEFINE "));
    match->handler = define_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register #MACRO directive.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("#MACRO "));
    match->handler = macro_def_handle;
    match->userdata = NULL;
    match->line_start_only = true;
    match->identifier_only = false;
    match->case_insensitive = true;
    ppimpl_register(state, match);

    // Register macro terminator.
    match = malloc(sizeof(match_t));
    match->text = bautofree(bfromcstr("\1MACROTERMINATE\1"));
    match->handler = macro_term_handle;
    match->userdata = NULL;
    match->line_start_only = false;
    match->identifier_only = false;
    match->case_insensitive = false;
    ppimpl_register(state, match);
}
