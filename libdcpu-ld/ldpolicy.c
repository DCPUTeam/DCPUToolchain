///
/// @addtogroup LibDCPU-LD
/// @{
///
/// @file
/// @brief Implements the API for using linker policies on linker bins.
/// @author James Rhodes
///

#include <stdlib.h>
#include <stdio.h>
#include <dcpu.h>
#include <debug.h>
#include <derr.h>
#include <policy.h>
#include <policyast.h>
#include <parser.h>
#include "ldbins.h"
#include "ldbin.h"
#include "ldconv.h"

struct policy_userdata
{
    policy_state_t* state;
    bstring code_bin;
    struct ldbin* output_bin;
    uint16_t previous_offset;
};

///
/// @brief Convenience function to get the designated code bin.
///
static struct ldbin* policy_get_source_bin(void* userdata)
{
    struct ldbin* bin;
    list_iterator_start(&ldbins.bins);
    while (list_iterator_hasnext(&ldbins.bins))
    {
        bin = list_iterator_next(&ldbins.bins);
        
        if (biseq(bin->name, ((struct policy_userdata*)userdata)->code_bin))
        {
            list_iterator_stop(&ldbins.bins);
            return bin;
        }
    }
    list_iterator_stop(&ldbins.bins);
    dwarn(WARN_POLICY_CODE_BIN_NOT_FOUND, NULL);
    return NULL;
}

///
/// @brief Handles the code() callback from the policy execution.
///
static list_t policy_code(void* userdata)
{
    list_t words;
    struct ldbin* bin = policy_get_source_bin(userdata);
    list_init(&words);
    
    // Get all of the words out of the bin.
    list_iterator_start(&bin->words);
    while (list_iterator_hasnext(&bin->words))
        list_append(&words, (void*)(*(uint16_t*)list_iterator_next(&bin->words)));
    list_iterator_stop(&bin->words);
    
    // Return the words.
    return words;
}

///
/// @brief Handles the field() callback from the policy execution.
///
static list_t policy_field(void* userdata, int table, int index, int field)
{
    int i;
    list_t words;
    list_t* table_list;
    struct lconv_entry* entry;
    struct ldbin* bin = policy_get_source_bin(userdata);
    list_init(&words);
    
    // Get the references to the correct table.
    switch (table)
    {
        case TABLE_ADJUSTMENT:
            table_list = bin->adjustment;
            break;
        case TABLE_PROVIDED:
            table_list = bin->provided;
            break;
        case TABLE_REQUIRED:
            table_list = bin->required;
            break;
        case TABLE_SECTION:
            table_list = bin->section;
            break;
        case TABLE_OUTPUT:
            table_list = bin->output;
            break;
        case TABLE_JUMP:
            table_list = bin->jump;
            break;
        case TABLE_OPTIONAL:
            table_list = bin->optional;
            break;
        default:
            dhalt(ERR_POLICY_UNKNOWN_TABLE, NULL);
            return words;
    }
    
    // If the index is outside the bounds, just return the
    // empty list.
    if (index < 0 || index > (int)list_size(table_list))
        return words;
    
    // Get the entry in the table.
    entry = list_get_at(table_list, index);
    
    // Now create the words based on the field.
    switch (field)
    {
        case FIELD_ADDRESS:
            list_append(&words, (void*)entry->address);
            return words;
        case FIELD_LABEL_SIZE:
            if (entry->label == NULL)
                list_append(&words, (void*)(uint16_t)0);
            else
                list_append(&words, (void*)(uint16_t)blength(entry->label));
            return words;
        case FIELD_LABEL_TEXT:
            if (entry->label == NULL)
                return words;
            for (i = 0; i < blength(entry->label); i++)
                list_append(&words, (void*)(uint16_t)entry->label->data[i]);
            return words;
        default:
            dhalt(ERR_POLICY_UNKNOWN_FIELD, NULL);
            return words;
    }
}

///
/// @brief Handles the total(table) callback from the policy execution.
///
static int policy_total_table(void* userdata, int table)
{
    list_t* table_list;
    struct ldbin* bin = policy_get_source_bin(userdata);
    
    // Get the references to the correct table.
    switch (table)
    {
        case TABLE_ADJUSTMENT:
            table_list = bin->adjustment;
            break;
        case TABLE_PROVIDED:
            table_list = bin->provided;
            break;
        case TABLE_REQUIRED:
            table_list = bin->required;
            break;
        case TABLE_SECTION:
            table_list = bin->section;
            break;
        case TABLE_OUTPUT:
            table_list = bin->output;
            break;
        case TABLE_JUMP:
            table_list = bin->jump;
            break;
        case TABLE_OPTIONAL:
            table_list = bin->optional;
            break;
        default:
            dhalt(ERR_POLICY_UNKNOWN_TABLE, NULL);
            return 0;
    }
    
    // Return the length of the table.
    return (int)list_size(table_list);
}

///
/// @brief Handles the offset() callback from the policy execution.
///
static void policy_offset(void* userdata, int position)
{
    uint16_t* word;
    struct lconv_entry* entry;
    struct ldbin* bin = policy_get_source_bin(userdata);
    
    list_iterator_start(bin->adjustment);
    while (list_iterator_hasnext(bin->adjustment))
    {
        entry = list_iterator_next(bin->adjustment);
        word = list_get_at(&bin->words, entry->address);
        
        printd(LEVEL_EVERYTHING, "%u -> %u\n", *word, 
               *word + (position - ((struct policy_userdata*)userdata)->previous_offset));
        *word = *word + (position - ((struct policy_userdata*)userdata)->previous_offset);
    }
    list_iterator_stop(bin->adjustment);
    ((struct policy_userdata*)userdata)->previous_offset = position;
}

///
/// @brief Handles the write() callback from the policy execution.
///
static void policy_write(void* userdata, list_t words)
{
    list_iterator_start(&words);
    while (list_iterator_hasnext(&words))
        bin_write(((struct policy_userdata*)userdata)->output_bin, (uint16_t)list_iterator_next(&words));
    list_iterator_stop(&words);
}

///
/// @brief Handles the error callback from the policy execution.
///
static void policy_error(freed_bstring error)
{
    dhalt(ERR_POLICY_EXECUTION, error.ref->data);
    bautodestroy(error);
}

///
/// @brief Applies the linker policy to the current set of bins.
///
/// @param path Path to the policy file.
/// @param target The target type (policy) to apply.
/// @param code The name of the bin that holds the final code.
/// @param output The name of the new bin that will hold the result.
///
void bins_apply_policy(freed_bstring path, freed_bstring target, freed_bstring code, freed_bstring output)
{
    policies_t* policies = NULL;
    policy_t* policy = NULL;
    policy_state_t* state = NULL;
    FILE* file = NULL;
    struct policy_userdata* ud = NULL;
    
    // Attempt to open the file.
    file = fopen(path.ref->data, "r");
    if (file == NULL)
        dhalt(ERR_POLICY_FILE_NOT_FOUND, path.ref->data);
    policies = policies_load(file);
    fclose(file);
    
    // Find the desired policy.
    policy = policies_get_policy(policies, bautocpy(target.ref));
    if (policy == NULL)
        dhalt(ERR_POLICY_TARGET_NOT_FOUND, target.ref->data);
    if (policy->type == POLICY_INTERNAL)
        dhalt(ERR_POLICY_IS_INTERNAL, target.ref->data);
    
    // Create the state and execute it.
    state = state_from_policy(policy);
    ud = malloc(sizeof(struct policy_userdata));
    ud->state = state;
    ud->code_bin = code.ref;
    ud->output_bin = bins_add(output, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    ud->previous_offset = 0;
    state->userdata = ud;
    state->call_code = policy_code;
    state->call_field = policy_field;
    state->call_offset = policy_offset;
    state->call_total_table = policy_total_table;
    state->call_write = policy_write;
    state->error = policy_error;
    state_execute(policies, state);
    free(ud);
    
    bautodestroy(code);
    bautodestroy(path);
    bautodestroy(target);
}

///
/// @}
///
