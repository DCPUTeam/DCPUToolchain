#include <stdio.h>
#include <bfile.h>
#include <node.h>
#include <parser.h>

void json_demo(struct ast_node_root ast_root)
{
    // Technically I could use a JSON library here to construct a proper
    // JSON output, but I'm just going to output JSON text.  Keep in mind
    // that this would be a lot more elegant using a JSON library...
    struct ast_node_line* line = ast_root.values->last;
    bool first = true;
    printf("[\n");
    while (line != NULL)
    {
        if (!first)
            printf(",\n");
        first = false;
        printf("{\n");
        printf("\"type\": ");
        switch (line->type)
        {
            case type_keyword:
                printf("\"keyword\",\n");
                printf("\"name\": ");
                switch (line->keyword)
                {
                    case BOUNDARY:
                        printf("\"BOUNDARY\",\n");
                        break;
                    case EXTENSION:
                        printf("\"EXTENSION\",\n");
                        break;
                    case SEEK:
                        printf("\"SEEK\",\n");
                        break;
                    case ORIGIN:
                        printf("\"ORIGIN\",\n");
                        break;
                    case INCBIN:
                        printf("\"INCBIN\",\n");
                        break;
                    case EXPORT:
                        printf("\"EXPORT\",\n");
                        break;
                    case IMPORT:
                        printf("\"IMPORT\",\n");
                        break;
                    case IMPORT_OPTIONAL:
                        printf("\"IMPORT OPTIONAL\",\n");
                        break;
                    case FILL:
                        printf("\"FILL\",\n");
                        break;
                    case SECTION:
                        printf("\"SECTION\",\n");
                        break;
                    case OUTPUT:
                        printf("\"OUTPUT\",\n");
                        break;
                    case JUMP:
                        printf("\"JUMP\",\n");
                        break;
                    case CALL:
                        printf("\"CALL\",\n");
                        break;
                    case SYMBOL:
                        printf("\"SYMBOL\",\n");
                        break;
                    default:
                        printf("null,\n");
                        break;
                }
                if (line->keyword_data_string == NULL)
                    printf("\"value_string\": null,\n");
                else
                    printf("\"value_string\": \"%s\",\n", line->keyword_data_string->data);
                // We could actually turn the expression tree into a JSON object if we
                // were using a JSON library to do it (otherwise it just gets messy for
                // this demo).
                if (line->keyword_data_expr_1 == NULL)
                    printf("\"value_expr1\": null,\n");
                else
                {
                    bstring bf = expr_representation(line->keyword_data_expr_1);
                    printf("\"value_expr1\": \"%s\",\n", bf->data);
                    bdestroy(bf);
                }
                if (line->keyword_data_expr_2 == NULL)
                    printf("\"value_expr2\": null,\n");
                else
                {
                    bstring bf = expr_representation(line->keyword_data_expr_2);
                    printf("\"value_expr2\": \"%s\",\n", bf->data);
                    bdestroy(bf);
                }
                break;
            case type_instruction:
                printf("\"instruction\",\n");
                if (line->instruction->instruction == NULL)
                    printf("\"instruction\": null,\n");
                else
                    printf("\"instruction\": \"%s\",\n", line->instruction->instruction);
                struct ast_node_parameter* parameter = NULL;
                if (line->instruction->parameters != NULL)
                    parameter = line->instruction->parameters->last;
                bool pfirst = true;
                printf("\"parameters\": [\n");
                while (parameter != NULL)
                {
                    if (!pfirst)
                        printf(",\n");
                    pfirst = false;
                    printf("{\n");
                    printf("\"type\": ");
                    switch (parameter->type)
                    {
                        case type_raw:
                            printf("\"raw\",\n");
                            printf("\"raw\": [\n");
                            if (blength(parameter->raw) > 0)
                                printf("%u\n", parameter->raw->data[0]);
                            for (int i = 1; i < blength(parameter->raw); i++)
                                printf(",%u\n", parameter->raw->data[i]);
                            printf("]\n");
                            break;
                        case type_address:
                            printf("\"address\",\n");
                            if (parameter->address == NULL)
                                printf("\"address\": null\n");
                            else
                            {
                                // same with above, we could easily turn this into a JSON tree
                                // if we were using a JSON library
                                bstring bf = expr_representation(parameter->address->value);
                                printf("\"address\": {\n");
                                printf("\"value\": \"%s\",\n", bf->data);
                                printf("\"bracketed\": %s,\n", parameter->address->bracketed == 0 ? "false" : "true");
                                printf("\"added\": %s,\n", parameter->address->added == 0 ? "false" : "true");
                                if (parameter->address->addcmpt == NULL)
                                    printf("\"addcmpt\": null\n");
                                else
                                    printf("\"addcmpt\": \"%s\"\n", parameter->address->addcmpt);
                                printf("}\n");
                                bdestroy(bf);
                            }
                            break;
                        case type_register:
                            printf("\"register\",\n");
                            if (parameter->registr == NULL)
                                printf("\"register\": null\n");
                            else
                            {
                                printf("\"register\": {\n");
                                printf("\"value\": \"%s\",\n", parameter->registr->value);
                                printf("\"bracketed\": %s\n", parameter->registr->bracketed == 0 ? "false" : "true");
                                printf("}\n");
                            }
                            break;
                        default:
                            printf("null,\n");
                            break;
                    }
                    printf("}");
                    parameter = parameter->prev;
                }
                printf("\n],\n");
                break;
            case type_label:
                printf("\"label\",\n");
                if (line->label->name == NULL)
                    printf("\"label\": null,\n");
                else
                    printf("\"label\": \"%s\",\n", line->label->name);
                break;
            default:
                printf("null,\n");
        }
        printf("\"line\": %u,\n", line->line);
        printf("\"line_upper\": %u,\n", line->uline);
        if (line->file == NULL)
            printf("\"filename\": null,\n");
        else
            printf("\"filename\": \"%s\",\n", line->file->data);
        if (line->ufile == NULL)
            printf("\"filename_upper\": null,\n");
        else
            printf("\"filename_upper\": \"%s\"\n", line->ufile->data);
        
        printf("}");
        line = line->prev;
    }
    printf("\n]\n");
}
