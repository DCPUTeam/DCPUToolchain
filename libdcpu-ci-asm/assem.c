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
#include <assert.h>
#include <bstring.h>
#include <ppexpr.h>
#include <simclist.h>
#include <ddata.h>
#include <debug.h>
#include <derr.h>
#include "node.h"
#include "dcpu.h"
#include "imap.h"
#include "aout.h"
#include "parser.h"

list_t* assem_dbg_symbols = NULL;

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
	struct ast_node_parameter* current = NULL;
	struct ast_node_parameter* nprev = NULL;
	struct ast_node_parameter* cnext = NULL;
	nlines->last = NULL;
	if (params == NULL)
	{
		free(nlines);
		return;
	}
	current = params->last;

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
			// Attempt to use a label in square brackets.  Convert this
			// to an expression and then reinvoke ourselves with the
			// evaluated value.
			param->value = expr_new(expr_new_label(bautofree(bfromcstr(param->addcmpt))), EXPR_OP_ADD, param->value);
			param->addcmpt = "";
			param->added = 0;
			param->bracketed = 0;

			bdestroy(btmp);
			return process_address(param);
		}
		else if (registr->value == VALUE_NEXT_UNSUPPORTED)
		{
			// Attempt to use a register in brackets that can't be.
			printd(LEVEL_VERBOSE, "\n");
			dhalt(ERR_NEXTED_REGISTER_UNSUPPORTED, param->addcmpt);
		}

		printd(LEVEL_VERBOSE, "[%s+%s]", btmp->data, registr->name);
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
			printd(LEVEL_VERBOSE, "[%s]", btmp->data);
			result.v = NXT;
		}
		else
		{
			printd(LEVEL_VERBOSE, "%s", btmp->data);
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
		printd(LEVEL_VERBOSE, "[%s]", param->value);
	else
		printd(LEVEL_VERBOSE, "%s", param->value);

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
		printd(LEVEL_VERBOSE, "\n");
		dhalt(ERR_BRACKETED_REGISTER_UNSUPPORTED, param->value);
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
	printd(LEVEL_VERBOSE, " ");

	switch (param->type)
	{
		case type_address:
			return process_address(param->address);

		case type_register:
			return process_register(param->registr);

		case type_raw:
			result.v_raw = param->raw;
	    break;

	default:
	    assert(false);
	}

	return result;
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
			printd(LEVEL_VERBOSE, "\n");
			dhalt(ERR_GEN_UNSUPPORTED_PARAMETER, NULL);
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
				printd(LEVEL_VERBOSE, "\n");
				dhalt(ERR_GEN_UNSUPPORTED_PARAMETER, NULL);
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

list_t newsyms;

void process_line(struct ast_node_line* line)
{
	struct instruction_mapping* insttype;
	struct process_parameters_results ppresults;
	struct process_parameter_results dparam;
	struct ast_node_parameter* dcurrent;
	uint32_t dchrproc;
	uint16_t i, flimit, fchar, opos;
	struct aout_byte* result = NULL;
	struct dbg_sym* newsym;

	// Change depending on the type of line.
	switch (line->type)
	{
		case type_keyword:
			switch (line->keyword)
			{
				case SYMBOL:
					printd(LEVEL_VERBOSE, ".SYMBOL %s", bstr2cstr(line->keyword_data_string, '0'));

					// Emit debugging symbol.
					list_append(&newsyms, dbgfmt_create_symbol(DBGFMT_SYMBOL_STRING, dbgfmt_create_symbol_string(line->keyword_data_string, DBGFMT_UNDETERMINED)));

					break;

				case SECTION:
					printd(LEVEL_VERBOSE, ".SECTION %s", bstr2cstr(line->keyword_data_string, '0'));

					// Emit section metadata.
					aout_emit(aout_create_metadata_section(bstr2cstr(line->keyword_data_string, '0')));

					break;

				case OUTPUT:
					printd(LEVEL_VERBOSE, ".OUTPUT %s", bstr2cstr(line->keyword_data_string, '0'));

					// Emit output metadata.
					aout_emit(aout_create_metadata_output(bstr2cstr(line->keyword_data_string, '0')));

					break;

				case BOUNDARY:
					printd(LEVEL_VERBOSE, ".BOUNDARY");

					// Emit safety boundary of 16 NULL words.
					for (i = 0; i < 16; i += 1)
						aout_emit(aout_create_raw(0));

					break;

				case FILL:
					printd(LEVEL_VERBOSE, ".FILL");

					if (line->keyword_data_expr_1 == NULL || line->keyword_data_expr_2 == NULL)
					{
						if (line->keyword_data_string != NULL)
							dhalt(ERR_LABEL_RESOLUTION_NOT_PERMITTED, line->keyword_data_string->data);
						else
							dhalt(ERR_LABEL_RESOLUTION_NOT_PERMITTED, "");
					}

					// Emit N words with value X
					flimit = expr_evaluate(line->keyword_data_expr_1, &dhalt_label_resolution_not_permitted, &dhalt_expression_exit_handler);
					fchar = expr_evaluate(line->keyword_data_expr_2, &dhalt_label_resolution_not_permitted, &dhalt_expression_exit_handler);
					for (i = 0; i < flimit; i++)
						aout_emit(aout_create_raw(fchar));

					break;

				case EXTENSION:
					printd(LEVEL_VERBOSE, ".EXTENSION %s", bstr2cstr(line->keyword_data_string, '0'));

					// Emit extension metadata.
					aout_emit(aout_create_metadata_extension(bstr2cstr(line->keyword_data_string, '0')));

					break;

				case INCBIN:
					printd(LEVEL_VERBOSE, ".INCBIN %s", bstr2cstr(line->keyword_data_string, '0'));

					// Emit binary include metadata.
					aout_emit(aout_create_metadata_incbin(bstr2cstr(line->keyword_data_string, '0')));

					break;

				case ORIGIN:
					if (line->keyword_data_expr_1 == NULL)
					{
						if (line->keyword_data_string != NULL)
							dhalt(ERR_LABEL_RESOLUTION_NOT_PERMITTED, line->keyword_data_string->data);
						else
							dhalt(ERR_LABEL_RESOLUTION_NOT_PERMITTED, "");
					}

					opos = expr_evaluate(line->keyword_data_expr_1, &dhalt_label_resolution_not_permitted, &dhalt_expression_exit_handler);
					printd(LEVEL_VERBOSE, ".ORIGIN 0x%04X", opos);

					// Emit origin set metadata.
					aout_emit(aout_create_metadata_origin(opos));

					break;

		case SEEK:
					if (line->keyword_data_expr_1 == NULL)
					{
						if (line->keyword_data_string != NULL)
							dhalt(ERR_LABEL_RESOLUTION_NOT_PERMITTED, line->keyword_data_string->data);
						else
							dhalt(ERR_LABEL_RESOLUTION_NOT_PERMITTED, "");
					}

					opos = expr_evaluate(line->keyword_data_expr_1, &dhalt_label_resolution_not_permitted, &dhalt_expression_exit_handler);
					printd(LEVEL_VERBOSE, ".SEEK 0x%04X", opos);

					// Emit seek metadata.
					aout_emit(aout_create_metadata_seek(opos));

					break;

				case EXPORT:
					printd(LEVEL_VERBOSE, ".EXPORT %s", bstr2cstr(line->keyword_data_string, '0'));

					// Emit export metadata.
					aout_emit(aout_create_metadata_export(bstr2cstr(line->keyword_data_string, '0')));

					break;

				case IMPORT:
					printd(LEVEL_VERBOSE, ".IMPORT %s", bstr2cstr(line->keyword_data_string, '0'));

					// Emit import metadata.
					aout_emit(aout_create_metadata_import(bstr2cstr(line->keyword_data_string, '0')));

					break;

		case IMPORT_OPTIONAL:
					printd(LEVEL_VERBOSE, ".IMPORT OPTIONAL %s", bstr2cstr(line->keyword_data_string, '0'));

					// Emit import metadata.
					aout_emit(aout_create_metadata_import_optional(bstr2cstr(line->keyword_data_string, '0')));

					break;

				case JUMP:
		    if (line->keyword_data_string == NULL)
			printd(LEVEL_VERBOSE, ".JUMP <table>");
		    else
					printd(LEVEL_VERBOSE, ".JUMP %s", bstr2cstr(line->keyword_data_string, '0'));

					// Emit jump metadata.
		    if (line->keyword_data_string == NULL)
					aout_emit(aout_create_metadata_jump(NULL));
		    else
			aout_emit(aout_create_metadata_jump(bstr2cstr(line->keyword_data_string, '0')));

					break;

				default:
					printd(LEVEL_VERBOSE, "?? UNKNOWN KEYWORD\n");
					dhalt(ERR_UNSUPPORTED_KEYWORD, NULL);
			}

			printd(LEVEL_VERBOSE, "\n");
			break;

		case type_instruction:

			// Check to see if this is DAT.
			if (strcmp(line->instruction->instruction, "DAT") == 0)
			{
				// Handle data.
				printd(LEVEL_VERBOSE, "EMIT DAT");

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
						printd(LEVEL_VERBOSE, " \"%s\"", dparam.v_raw->data);

						for (dchrproc = 0; dchrproc < (uint32_t)blength(dparam.v_raw); dchrproc++)
							aout_emit(aout_create_raw(dparam.v_raw->data[dchrproc]));
					}
					else if (dparam.v_extra_used == true) // Just a single address.
						aout_emit(aout_create_expr(dparam.v_extra));
					else // Something that isn't handled by DAT.
					{
						printd(LEVEL_VERBOSE, "\n");
						dhalt(ERR_DAT_UNSUPPORTED_PARAMETER, NULL);
					}

					dcurrent = dcurrent->prev;
				}
			}
			else
			{
				// Handle instruction.
				insttype = get_instruction_by_name(line->instruction->instruction);

				if (insttype == NULL)
					dhalt(ERR_UNKNOWN_OPCODE, line->instruction->instruction);

				printd(LEVEL_VERBOSE, "EMIT %s", insttype->name);

				// Check parameter count.
				if (line->instruction->parameters == NULL && strcmp(line->instruction->instruction, "RFI") == 0)
				{
					// Handle RFI (which can accept no parameters).
					result = aout_emit(aout_create_opcode(insttype->opcode, insttype->nbopcode, 0x21 /* 0 literal */));
					printd(LEVEL_VERBOSE, "\n");
					break;
				}
				else if (line->instruction->parameters == NULL)
				{
					// Halt and error.
					dhalt(ERR_INVALID_PARAMETER_COUNT, NULL);
				}

				// Process parameters normally.
				ppresults = process_parameters(line->instruction->parameters);

				// Force the parameter value to be NXT if it's a label.
				if (ppresults.a_label != NULL) ppresults.a = NXT_LIT;
				if (ppresults.b_label != NULL) ppresults.b = NXT_LIT;
				if (ppresults.a_label != NULL && ppresults.a_label_bracketed) ppresults.a = NXT;
				if (ppresults.b_label != NULL && ppresults.b_label_bracketed) ppresults.b = NXT;

		// Check for relative addressing.
		if ((insttype->opcode == OP_ADD || insttype->opcode == OP_SUB ||
		    insttype->opcode == OP_MUL || insttype->opcode == OP_DIV) && ppresults.a == PC)
		{
		    // Warn about relative addressing portability.
		    dwarn(WARN_RELATIVE_PC_ADDRESSING, NULL);
		}

				// Output the initial opcode.
				if (insttype->opcode != OP_NONBASIC)
					result = aout_emit(aout_create_opcode(insttype->opcode, ppresults.a, ppresults.b));
				else
					result = aout_emit(aout_create_opcode(insttype->opcode, insttype->nbopcode, ppresults.a));

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

			printd(LEVEL_VERBOSE, "\n");
			break;

		case type_label:
			// Handle label definition.
			list_append(&newsyms, dbgfmt_create_symbol(DBGFMT_SYMBOL_LABEL, dbgfmt_create_symbol_label(bfromcstr(line->label->name), DBGFMT_UNDETERMINED)));
			printd(LEVEL_VERBOSE, ":%s\n", line->label->name);
			aout_emit(aout_create_label(line->label->name));
			break;

	default:
	    assert(false);
	}

	// If we can associate debugging symbols with this instruction...
	if (result != NULL)
	{
		// While the new symbols list is not empty, copy those symbols
		// into the output and associate.
		while (list_size(&newsyms) > 0)
		{
			newsym = list_extract_at(&newsyms, 0);
			printd(LEVEL_DEBUG, "Debugging custom symbol: %i\n", newsym->length);
			list_append(&result->symbols, newsym);
			list_append(assem_dbg_symbols, newsym);
		}

		// If the line information is provided, output
		// debugging symbols.
		if (line != NULL && line->file != NULL)
		{
			// Output a file / line number debugging symbol here.
			newsym = dbgfmt_create_symbol(DBGFMT_SYMBOL_LINE, dbgfmt_create_symbol_line(line->file, line->line, DBGFMT_UNDETERMINED));
			list_append(&result->symbols, newsym);
			list_append(assem_dbg_symbols, newsym);

			printd(LEVEL_DEBUG, "Debugging symbol: %i %s\n", line->line, line->file->data);
		}

		// If the higher-language line information is
		// provided, output debugging symbols.
		if (line != NULL && line->ufile != NULL)
		{
			// Output a file / line number debugging symbol here.
			newsym = dbgfmt_create_symbol(DBGFMT_SYMBOL_LINE, dbgfmt_create_symbol_line(line->ufile, line->uline, DBGFMT_UNDETERMINED));
			list_append(&result->symbols, newsym);
			list_append(assem_dbg_symbols, newsym);

			printd(LEVEL_DEBUG, "High-level debugging symbol: %i %s\n", line->uline, line->ufile->data);
		}
	}
}

void process_lines(struct ast_node_lines* lines)
{
	struct ast_node_line* current;
	struct dbg_sym* newsym;

	// First reverse the lines.
	reverse_lines(lines);

	// Now navigate the linked list (although it says prev / last
	// here, we are actually now navigating it forwards because
	// we reversed it).
	current = lines->last;

	list_init(&newsyms);
	while (current != NULL)
	{
		// Process each line.
		process_line(current);
		current = current->prev;
	}
	while (list_size(&newsyms) > 0)
	{
		// Get each trailing debug symbol and store it in the list anyway.
		newsym = list_extract_at(&newsyms, 0);
		printd(LEVEL_DEBUG, "Debugging trailing symbol.\n");
		list_append(assem_dbg_symbols, newsym);
	}
	list_destroy(&newsyms);
}

void process_root(struct ast_node_root* root, list_t* symbols)
{
	// Associate the global symbols variable.
	assem_dbg_symbols = symbols;

	// Process all of the lines.
	process_lines(root->values);

	// Reset the global symbols variable.
	assem_dbg_symbols = NULL;
}
