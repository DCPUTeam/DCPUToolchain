/**

    File:       main.c

    Project:    DCPU-16 Tools
    Component:  Linker

    Authors:    James Rhodes

    Description:    Main entry point.

**/

#include <stdio.h>
#include <bstring.h>
#include <policy.h>
#include <parser.h>

extern int yyparse();
extern FILE* yyin, *yyout;

void print_value(policy_value_t* value);

void print_function(policy_function_call_t* call)
{
    switch (call->function)
    {
        case FUNC_TOTAL:
            printf("total");
            break;
        case FUNC_FIELD:
            printf("field");
            break;
        case FUNC_CODE:
            printf("code");
            break;
        case FUNC_WORDS:
            printf("words");
            break;
    }
    
    if (call->parameters != NULL && list_size(call->parameters) > 0)
    {
        printf("(");
        for (int i = 0; i < list_size(call->parameters); i++)
        {
            if (i != 0)
                printf(", ");
            print_value(list_get_at(call->parameters, i));
        }
        printf(")");
    }
}

void print_value(policy_value_t* value)
{
    switch (value->type)
    {
        case NUMBER:
            printf("%i", value->number);
            break;
        case WORD:
        case VARIABLE:
            printf("%s", value->string->data);
            break;
        case TABLE:
            switch (value->table)
            {
                case TABLE_ADJUSTMENT:
                    printf("adjustment");
                    break;
                case TABLE_PROVIDED:
                    printf("provided");
                    break;
                case TABLE_REQUIRED:
                    printf("required");
                    break;
                case TABLE_SECTION:
                    printf("section");
                    break;
                case TABLE_OUTPUT:
                    printf("output");
                    break;
                case TABLE_JUMP:
                    printf("jump");
                    break;
                case TABLE_OPTIONAL:
                    printf("optional");
                    break;
                default:
                    printf("<unknown table>");
                    break;
            }
            break;
        case FIELD:
            switch (value->field)
            {
                case FIELD_LABEL_SIZE:
                    printf("label_size");
                    break;
                case FIELD_LABEL_TEXT:
                    printf("label_text");
                    break;
                case FIELD_ADDRESS:
                    printf("address");
                    break;
                default:
                    printf("<unknown field>");
                    break;
            }
            break;
        case LIST:
            printf(" {");
            for (int i = 0; i < list_size(value->list); i++)
            {
                if (i != 0)
                    printf(", ");
                print_value(list_get_at(value->list, i));
            }
            printf(" }");
            break;
        case FUNCTION:
            print_function(value->function);
            break;
        default:
            printf("<unknown value>");
            break;
    }
}

void print_instructions(int indent, list_t* instructions)
{
    freed_bstring indentstr = bautofree(bfromcstr(""));
    for (int i = 0; i < indent; i++)
        bconchar(indentstr.ref, ' ');
    
    list_iterator_start(instructions);
    while (list_iterator_hasnext(instructions))
    {
        policy_instruction_t* inst = list_iterator_next(instructions);
        
        printf("%s", indentstr.ref->data);
        switch (inst->type)
        {
            case INST_CHAIN:
                printf("chain ");
                print_value(inst->value);
                printf("\n");
                break;
            case INST_OFFSET:
                printf("offset ");
                print_value(inst->value);
                printf("\n");
                break;
            case INST_WRITE:
                printf("write ");
                print_value(inst->value);
                printf("\n");
                break;
            case KEYWORD_FOR:
                printf("for %s from ", inst->for_variable->data);
                print_value(inst->for_start);
                printf(" to ");
                print_value(inst->for_end);
                printf("\n");
                print_instructions(indent + 4, inst->for_instructions);
                printf("%sendfor\n", indentstr.ref->data);
                break;
            default:
                printf("<unknown> ...\n");
                break;
        }
    }
    list_iterator_stop(instructions);
    
    bautodestroy(indentstr);
}

list_t handle_code()
{
    list_t words;
    printf("code called\n");
    list_init(&words);
    list_append(&words, (void*)0x0001);
    list_append(&words, (void*)0x0002);
    list_append(&words, (void*)0x0003);
    list_append(&words, (void*)0x0004);
    list_append(&words, (void*)0x0005);
    list_append(&words, (void*)0x0006);
    return words;
}

list_t handle_field(int table, int index, int field)
{
    list_t words;
    printf("field called with %i, %i, %i\n", table, index, field);
    list_init(&words);
    return words;
}

void handle_offset(int position)
{
    printf("offset called with %i\n", position);
}

int handle_total_table(int table)
{
    printf("total called with %i\n", table);
    return 3;
}

void handle_write(list_t words)
{
    printf("write called with:\n");
    list_iterator_start(&words);
    while (list_iterator_hasnext(&words))
        printf("    %u\n", (uint16_t)list_iterator_next(&words));
    list_iterator_stop(&words);
}

void handle_error(freed_bstring error)
{
    printf("ERROR OCCURRED!\n");
    printf("%s\n", error.ref->data);
    bautodestroy(error);
}
        
int main(int argc, char* argv[])
{
    FILE* file = NULL;
    policies_t* policies = NULL;
    
    if (argc < 3)
    {
        printf("usage: dtpolicy <file> <mode> [<policy>]\n");
        return 1;
    }
    
    // Load policies.
    file = fopen(argv[1], "r");
    policies = policies_load(file);
    fclose(file);
    
    if (strcmp(argv[2], "print") == 0)
    {
        // Print out a list of settings.
        if (policies->settings == NULL)
            printf("== no settings ==\n");
        else
        {
            printf("== settings ==\n");
            list_iterator_start(policies->settings->settings);
            while (list_iterator_hasnext(policies->settings->settings))
            {
                policy_setting_t* setting = list_iterator_next(policies->settings->settings);
                printf("%s = %s\n", setting->key->data, setting->value->data);
            }
            list_iterator_stop(policies->settings->settings);
            printf("\n");
        }
        
        // Print out a list of policies.
        printf("== policies ==\n");
        list_iterator_start(&policies->policies);
        while (list_iterator_hasnext(&policies->policies))
        {
            policy_t* policy = list_iterator_next(&policies->policies);
            printf(" - %s\n", policy->name->data);
        }
        list_iterator_stop(&policies->policies);
        printf("\n");
        
        // Print out each policy.
        list_iterator_start(&policies->policies);
        while (list_iterator_hasnext(&policies->policies))
        {
            policy_t* policy = list_iterator_next(&policies->policies);
            
            printf("== policy: %s ==\n", policy->name->data);
            print_instructions(4, policy->instructions);
            printf("\n");
        }
        list_iterator_stop(&policies->policies);
    }
    else if (strcmp(argv[2], "execute") == 0)
    {
        if (argc < 4)
        {
            printf("need policy parameter for execution.\n");
            policies_free(policies);
            return 1;
        }
        
        policy_t* policy = policies_get_policy(policies, bautofree(bfromcstr(argv[3])));
        if (policy == NULL)
            printf("no such policy '%s'.\n", argv[3]);
        printf("== executing: %s ==\n", argv[3]);
        policy_state_t* state = state_from_policy(policy);
        state->call_code = handle_code;
        state->call_field = handle_field;
        state->call_offset = handle_offset;
        state->call_total_table = handle_total_table;
        state->call_write = handle_write;
        state->error = handle_error;
        state_execute(policies, state);
    }
    else   
    {
        printf("unknown mode; expected 'print' or 'execute'.\n");
    }
    
    // Free policies.
    policies_free(policies);
    
    return 0;
}

