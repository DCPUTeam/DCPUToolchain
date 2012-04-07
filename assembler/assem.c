//
// DCPU Assembler by James Rhodes
//
// Assembler code.
//

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "node.h"
#include "dcpu.h"
#include "imap.h"
#include "aout.h"
#include "aerr.h"

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
	uint16_t v_extra;
	bool v_extra_used;
	char* v_label;
};

struct process_parameters_results
{
	uint16_t a;
	uint16_t b;
	uint16_t a_extra;
	uint16_t b_extra;
	bool a_extra_used;
	bool b_extra_used;
	char* a_label;
	char* b_label;
};

struct process_parameter_results process_address(struct ast_node_address* param)
{
	struct process_parameter_results result;
	struct register_mapping* registr;
	if (param->bracketed && param->added)
	{
		// This is of the form [0x1000+I].
		registr = get_register_by_name_next(param->addcmpt);
		if (registr->value == VALUE_NEXT_UNSUPPORTED)
		{
			// Attempt to use a register in brackets that can't be.
			printf("\n");
			ahalt(ERR_NEXTED_REGISTER_UNSUPPORTED, param->addcmpt);
		}
		printf("[0x%04X+%s]", param->value, registr->name);
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
			printf("[0x%04X]", param->value);
			result.v = NXT;
		}
		else
		{
			printf("0x%04X", param->value);
			result.v = NXT_LIT;
		}
		result.v_extra = param->value;
		result.v_extra_used = true;
		result.v_label = NULL;
	}
	return result;
}

struct process_parameter_results process_register(struct ast_node_register* param)
{
	struct process_parameter_results result;
	struct register_mapping* registr;
	printf("%s", param->value);
	registr = get_register_by_name(param->value, param->bracketed);
	if (registr == NULL)
	{
		// Must be a label.
		result.v = 0x0;
		result.v_extra = 0x0;
		result.v_extra_used = false;
		result.v_label = param->value;
	}
	else if (registr->value == BRACKETS_UNSUPPORTED)
	{
		// Attempt to use a register in brackets that can't be.
		printf("\n");
		ahalt(ERR_BRACKETED_REGISTER_UNSUPPORTED, param->value);
	}
	else
	{
		// Must be a register.
		result.v = registr->value;
		result.v_extra = 0x0;
		result.v_extra_used = false;
		result.v_label = NULL;
	}
	return result;
}

struct process_parameter_results process_parameter(struct ast_node_parameter* param)
{
	struct process_parameter_results result;
	result.v = 0x0;
	result.v_extra = 0x0;
	result.v_extra_used = false;
	printf(" ");
	switch (param->type)
	{
	case type_address:
		return process_address(param->address);
	case type_register:
		return process_register(param->registr);
	}
	return result;
}

struct process_parameters_results process_parameters(struct ast_node_parameters* params)
{
	struct process_parameters_results result;
	struct process_parameter_results t;
	reverse_parameters(params);
	if (params->last != NULL)
	{
		t = process_parameter(params->last);
		result.a = t.v;
		result.a_extra = t.v_extra;
		result.a_extra_used = t.v_extra_used;
		result.a_label = t.v_label;
		if (params->last->prev != NULL)
		{
			t = process_parameter(params->last->prev);
			result.b = t.v;
			result.b_extra = t.v_extra;
			result.b_extra_used = t.v_extra_used;
			result.b_label = t.v_label;
		}
		else
		{
			result.b = 0x0;
			result.b_extra = 0x0;
			result.b_extra_used = false;
			result.b_label = NULL;
		}
	}
	else
	{
		result.a = 0x0;
		result.a_extra = 0x0;
		result.a_extra_used = false;
		result.a_label = NULL;
		result.b = 0x0;
		result.b_extra = 0x0;
		result.b_extra_used = false;
		result.b_label = NULL;
	}
	return result;
}

void process_line(struct ast_node_line* line)
{
	struct instruction_mapping* insttype;
	struct process_parameters_results ppresults;

	// Change depending on the type of line.
	switch (line->type)
	{
	case type_instruction:
		// Handle instruction.
		insttype = get_instruction_by_name(line->instruction->instruction);
		printf("EMIT %s", insttype->name);
		ppresults = process_parameters(line->instruction->parameters);

		// Force the parameter value to be NXT if it's a label.
		if (ppresults.a_label != NULL) ppresults.a = NXT_LIT;
		if (ppresults.b_label != NULL) ppresults.b = NXT_LIT;

		// Output the initial opcode.
		if (insttype->opcode != OP_NONBASIC)
			aout_emit(aout_create_opcode(insttype->opcode, ppresults.a, ppresults.b));
		else
			aout_emit(aout_create_opcode(insttype->opcode, insttype->nbopcode, ppresults.a));
		
		// If the parameter is a label or requires an extra word, output them.
		if (ppresults.a_label != NULL)
			aout_emit(aout_create_label_replace(ppresults.a_label));
		else if (ppresults.a_extra_used)
			aout_emit(aout_create_raw(ppresults.a_extra));
		if (ppresults.b_label != NULL)
			aout_emit(aout_create_label_replace(ppresults.b_label));
		else if (ppresults.b_extra_used)
			aout_emit(aout_create_raw(ppresults.b_extra));

		printf("\n");
		break;
	case type_label:
		// Handle label definition.
		printf(":%s\n", line->label->name);
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