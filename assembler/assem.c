/**

	File:		assem.c

	Project:	DCPU-16 Tools
	Component:	Assembler

	Authors:	James Rhodes
			Aaron Miller

	Description:	Implements functions for traversing the AST
			and emitting bytecode instructions.

**/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <bstring.h>
#include <ppexpr.h>
#include "node.h"
#include "dcpu.h"
#include "imap.h"
#include "aout.h"
#include "aerr.h"
#include "parser.h"

void reverse_lines(struct ast_node_lines* lines)
{
	struct ast_node_lines* nlines = (struct ast_node_lines*)malloc(sizeof(struct ast_node_lines));
	struct ast_node_line* current = lines->last;
	struct ast_node_line* nprev = NULL;
	struct ast_node_line* cnext = NULL;
	nlines->last = NULL;

	while (current != NULL)
	{
		nprev = nlines->last;
		cnext = current->prev;
		nlines->last = current;
		current->prev = nprev;
		current = cnext;
	}

	lines->last = nlines->last;
	free(nlines);
}

void reverse_parameters(struct ast_node_parameters* params)
{
	struct ast_node_parameters* nlines = (struct ast_node_parameters*)malloc(sizeof(struct ast_node_parameters));
	struct ast_node_parameter* current = params->last;
	struct ast_node_parameter* nprev = NULL;
	struct ast_node_parameter* cnext = NULL;
	nlines->last = NULL;

	while (current != NULL)
	{
		nprev = nlines->last;
		cnext = current->prev;
		nlines->last = current;
		current->prev = nprev;
		current = cnext;
	}

	params->last = nlines->last;
	free(nlines);
}

struct process_parameter_results
{
	uint16_t v;
	struct expr* v_extra;
	bool v_extra_used;
	char* v_label;
	bool v_label_bracketed;
	bstring v_raw;
};

struct process_parameters_results
{
	uint16_t a;
	uint16_t b;
	struct expr* a_extra;
	struct expr* b_extra;
	bool a_extra_used;
	bool b_extra_used;
	bool a_label_bracketed;
	bool b_label_bracketed;
	char* a_label;
	char* b_label;
	bstring raw;
};

struct process_parameter_results process_address(struct ast_node_address* param)
{
	struct process_parameter_results result;
	struct register_mapping* registr;
	bstring btmp = NULL;
	result.v_raw = NULL;
	
	if (param->value != NULL)
		btmp = expr_representation(param->value);

	if (param->bracketed && param->added)
	{
		// This is of the form [0x1000+I].
		registr = get_register_by_name_next(param->addcmpt);

		if (registr == NULL)
		{
			// Attempt to use a label in brackets that can't be.
			fprintf(stderr, "\n");
			ahalt(ERR_NEXTED_LABEL_UNSUPPORTED, param->addcmpt);
		}
		else if (registr->value == VALUE_NEXT_UNSUPPORTED)
		{
			// Attempt to use a register in brackets that can't be.
			fprintf(stderr, "\n");
			ahalt(ERR_NEXTED_REGISTER_UNSUPPORTED, param->addcmpt);
		}

		fprintf(stderr, "[%s+%s]", btmp->data, registr->name);
		result.v = registr->value;
		result.v_extra = param->value;
		result.v_extra_used = true;
		result.v_label = NULL;
	}
	else
	{
		// This is either the form 0x1000 or [0x1000].
		if (param->bracketed)
		{
			fprintf(stderr, "[%s]", btmp->data);
			result.v = NXT;
		}
		else
		{
			fprintf(stderr, "%s", btmp->data);
			result.v = NXT_LIT;
		}

		result.v_extra = param->value;
		result.v_extra_used = true;
		result.v_label = NULL;
	}
	
	if (btmp != NULL)
		bdestroy(btmp);
	
	return result;
}

struct process_parameter_results process_register(struct ast_node_register* param)
{
	struct process_parameter_results result;
	struct register_mapping* registr;

	if (param->bracketed)
		fprintf(stderr, "[%s]", param->value);
	else
		fprintf(stderr, "%s", param->value);

	registr = get_register_by_name(param->value, param->bracketed);

	if (registr == NULL)
	{
		// Must be a label.
		result.v = 0x0;
		result.v_extra = 0x0;
		result.v_extra_used = false;
		result.v_label = param->value;
		result.v_label_bracketed = param->bracketed;
		result.v_raw = NULL;
	}
	else if (registr->value == BRACKETS_UNSUPPORTED)
	{
		// Attempt to use a register in brackets that can't be.
		fprintf(stderr, "\n");
		ahalt(ERR_BRACKETED_REGISTER_UNSUPPORTED, param->value);
	}
	else
	{
		// Must be a register.
		result.v = registr->value;
		result.v_extra = 0x0;
		result.v_extra_used = false;
		result.v_label = NULL;
		result.v_label_bracketed = false;
		result.v_raw = NULL;
	}

	return result;
}

struct process_parameter_results process_parameter(struct ast_node_parameter* param)
{
	struct process_parameter_results result;
	result.v = 0x0;
	result.v_extra = 0x0;
	result.v_extra_used = false;
	result.v_label = NULL;
	result.v_label_bracketed = false;
	result.v_raw = NULL;
	fprintf(stderr, " ");

	switch (param->type)
	{
		case type_address:
			return process_address(param->address);

		case type_register:
			return process_register(param->registr);

		case type_raw:
			result.v_raw = param->raw;
	}

	return result;
}

char* str_concat(char* a, char* b)
{
	// TODO: Actually find out how abuse of not memory managing this is.
	char* res;

	if (a == NULL)
		return b;
	else
	{
		res = malloc(strlen(a) + strlen(b) + 1);
		memcpy(res, a, strlen(a));
		memcpy(res + strlen(a), b, strlen(b));
		res[strlen(a) + strlen(b) + 1] = '\0';
		return res;
	}
}

struct process_parameters_results process_parameters(struct ast_node_parameters* params)
{
	struct process_parameters_results result;
	struct process_parameter_results t;
	reverse_parameters(params);
	result.raw = NULL;

	if (params->last != NULL)
	{
		t = process_parameter(params->last);

		if (t.v_raw)
		{
			fprintf(stderr, "\n");
			ahalt(ERR_GEN_UNSUPPORTED_PARAMETER, NULL);
		}

		result.a = t.v;
		result.a_extra = t.v_extra;
		result.a_extra_used = t.v_extra_used;
		result.a_label = t.v_label;
		result.a_label_bracketed = t.v_label_bracketed;

		if (params->last->prev != NULL)
		{
			t = process_parameter(params->last->prev);

			if (t.v_raw)
			{
				fprintf(stderr, "\n");
				ahalt(ERR_GEN_UNSUPPORTED_PARAMETER, NULL);
			}

			result.b = t.v;
			result.b_extra = t.v_extra;
			result.b_extra_used = t.v_extra_used;
			result.b_label = t.v_label;
			result.b_label_bracketed = t.v_label_bracketed;
		}
		else
		{
			result.b = 0x0;
			result.b_extra = 0x0;
			result.b_extra_used = false;
			result.b_label = NULL;
			result.b_label_bracketed = false;
		}
	}
	else
	{
		result.a = 0x0;
		result.a_extra = 0x0;
		result.a_extra_used = false;
		result.a_label = NULL;
		result.b_label_bracketed = false;
		result.b = 0x0;
		result.b_extra = 0x0;
		result.b_extra_used = false;
		result.b_label = NULL;
		result.b_label_bracketed = false;
	}

	return result;
}

void process_line(struct ast_node_line* line)
{
	struct instruction_mapping* insttype;
	struct process_parameters_results ppresults;
	struct process_parameter_results dparam;
	struct ast_node_parameter* dcurrent;
	uint32_t dchrproc;
	uint16_t i, flimit, fchar, opos;

	// Change depending on the type of line.
	switch (line->type)
	{
		case type_keyword:
			switch (line->keyword)
			{
				case BOUNDARY:
					fprintf(stderr, ".BOUNDARY");

					// Emit safety boundary of 16 NULL words.
					for (i = 0; i < 16; i += 1)
						aout_emit(aout_create_raw(0));

					break;

				case FILL:
					fprintf(stderr, ".FILL");

					// Emit N words with value X
					flimit = expr_evaluate(line->keyword_data_expr_1, &ahalt_label_resolution_not_permitted, &ahalt_expression_exit_handler);
					fchar = expr_evaluate(line->keyword_data_expr_2, &ahalt_label_resolution_not_permitted, &ahalt_expression_exit_handler);
					for (i = 0; i < flimit; i++)
						aout_emit(aout_create_raw(fchar));

					break;

				case EXTENSION:
					fprintf(stderr, ".EXTENSION %s", line->keyword_data_string);

					// Emit extension metadata.
					aout_emit(aout_create_metadata_extension(bstr2cstr(line->keyword_data_string, '0')));

					break;

				case INCBIN:
					fprintf(stderr, ".INCBIN %s", line->keyword_data_string);

					// Emit binary include metadata.
					aout_emit(aout_create_metadata_incbin(bstr2cstr(line->keyword_data_string, '0')));

					break;

				case ORIGIN:
					opos = expr_evaluate(line->keyword_data_expr_1, &ahalt_label_resolution_not_permitted, &ahalt_expression_exit_handler);
					fprintf(stderr, ".ORIGIN 0x%04X", opos);

					// Emit origin set metadata.
					aout_emit(aout_create_metadata_origin(opos));

					break;

				case EXPORT:
					fprintf(stderr, ".EXPORT %s", line->keyword_data_string);

					// Emit export metadata.
					aout_emit(aout_create_metadata_export(bstr2cstr(line->keyword_data_string, '0')));

					break;

				case IMPORT:
					fprintf(stderr, ".IMPORT %s", line->keyword_data_string);

					// Emit export metadata.
					aout_emit(aout_create_metadata_import(bstr2cstr(line->keyword_data_string, '0')));

					break;

				default:
					fprintf(stderr, "\n");
					ahalt(ERR_UNSUPPORTED_KEYWORD, NULL);
			}

			fprintf(stderr, "\n");
			break;

		case type_instruction:

			// Check to see if this is DAT.
			if (strcmp(line->instruction->instruction, "DAT") == 0)
			{
				// Handle data.
				fprintf(stderr, "EMIT DAT");

				// Process parameters as data.
				reverse_parameters(line->instruction->parameters);
				dcurrent = line->instruction->parameters->last;

				while (dcurrent != NULL)
				{
					// Process parameter normally.
					dparam = process_parameter(dcurrent);

					// Output depending on what kind of parameter it was.
					if (dparam.v_label != NULL) // If this is a label, output something that we need to replace.
						aout_emit(aout_create_expr(expr_new_label(bautofree(bfromcstr(dparam.v_label)))));
					else if (dparam.v_raw != NULL) // If the raw field is not null, get each character and output it.
					{
						fprintf(stderr, " \"%s\"", dparam.v_raw->data);

						for (dchrproc = 0; dchrproc < blength(dparam.v_raw); dchrproc++)
							aout_emit(aout_create_raw(dparam.v_raw->data[dchrproc]));
					}
					else if (dparam.v_extra_used == true) // Just a single address.
						aout_emit(aout_create_expr(dparam.v_extra));
					else // Something that isn't handled by DAT.
					{
						fprintf(stderr, "\n");
						ahalt(ERR_DAT_UNSUPPORTED_PARAMETER, NULL);
					}

					dcurrent = dcurrent->prev;
				}
			}
			else
			{
				// Handle instruction.
				insttype = get_instruction_by_name(line->instruction->instruction);

				if (insttype == NULL)
					ahalt(ERR_UNKNOWN_OPCODE, line->instruction->instruction);

				fprintf(stderr, "EMIT %s", insttype->name);

				// Process parameters normally.
				ppresults = process_parameters(line->instruction->parameters);

				// Force the parameter value to be NXT if it's a label.
				if (ppresults.a_label != NULL) ppresults.a = NXT_LIT;

				if (ppresults.b_label != NULL) ppresults.b = NXT_LIT;

				if (ppresults.a_label != NULL && ppresults.a_label_bracketed) ppresults.a = NXT;

				if (ppresults.b_label != NULL && ppresults.b_label_bracketed) ppresults.b = NXT;

				// Output the initial opcode.
				if (insttype->opcode != OP_NONBASIC)
					aout_emit(aout_create_opcode(insttype->opcode, ppresults.a, ppresults.b));
				else
					aout_emit(aout_create_opcode(insttype->opcode, insttype->nbopcode, ppresults.a));

				// If the parameter is a label or requires an extra word, output them.
				if (ppresults.b_label != NULL)
					aout_emit(aout_create_expr(expr_new_label(bautofree(bfromcstr(ppresults.b_label)))));
				else if (ppresults.b_extra_used)
					aout_emit(aout_create_expr(ppresults.b_extra));

				if (ppresults.a_label != NULL)
					aout_emit(aout_create_expr(expr_new_label(bautofree(bfromcstr(ppresults.a_label)))));
				else if (ppresults.a_extra_used)
					aout_emit(aout_create_expr(ppresults.a_extra));

			}

			fprintf(stderr, "\n");
			break;

		case type_label:
			// Handle label definition.
			fprintf(stderr, ":%s\n", line->label->name);
			aout_emit(aout_create_label(line->label->name));
			break;
	}
}

void process_lines(struct ast_node_lines* lines)
{
	struct ast_node_line* current;

	// First reverse the lines.
	reverse_lines(lines);

	// Now navigate the linked list (although it says prev / last
	// here, we are actually now navigating it forwards because
	// we reversed it).
	current = lines->last;

	while (current != NULL)
	{
		// Process each line.
		process_line(current);
		current = current->prev;
	}
}

void process_root(struct ast_node_root* root)
{
	// Process all of the lines.
	process_lines(root->values);
}
