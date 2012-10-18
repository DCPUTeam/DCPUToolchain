/**
 *
 *	File:		ppexpr.c
 *
 *	Project:	DCPU-16 Toolchain
 *	Component:	LibDCPU-PP-Expr
 *
 *	Authors:	James Rhodes
 *
 *	Description:	Defines the public API for using the compile-time
 *			expression system in programs.
 *
 **/

#include <bstring.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "ppexpr.h"

struct expr* expr_new(struct expr* a, int op, struct expr* b)
{
	struct expr* n = malloc(sizeof(struct expr));
	n->type = EXPR_EXPR;
	n->a = a;
	n->op = op;
	n->b = b;
	n->data = NULL;
	return n;
}

struct expr* expr_new_number(uint16_t val)
{
	struct expr* n = malloc(sizeof(struct expr));
	n->type = EXPR_NUMBER;
	n->a = NULL;
	n->op = EXPR_OP_INVALID;
	n->b = NULL;
	n->data = (void*)val;
	return n;
}

struct expr* expr_new_label(freed_bstring label)
{
	struct expr* n = malloc(sizeof(struct expr));
	n->type = EXPR_LABEL;
	n->a = NULL;
	n->op = EXPR_OP_INVALID;
	n->b = NULL;
	n->data = (void*)label.ref;
	assert(n->data != NULL);
	return n;
}

#define recursive_evaluate(e) expr_evaluate(e, labelreplace, errexit)

uint16_t expr_evaluate(struct expr* e, uint16_t(*labelreplace)(bstring label), void(*errexit)(int code, void* data))
{
	uint16_t t, t2;

	switch (e->type)
	{
		case EXPR_NUMBER:
			return (uint16_t)(intptr_t)e->data;
		case EXPR_LABEL:
			if (labelreplace == NULL)
			{
				errexit(EXPR_EXIT_LABEL_NOT_FOUND, (bstring)e->data);
				return 0;
			}
			else
				return labelreplace((bstring)e->data);
		case EXPR_EXPR:
			switch (e->op)
			{
				case EXPR_OP_ADD:
					return recursive_evaluate(e->a) + recursive_evaluate(e->b);
				case EXPR_OP_SUBTRACT:
					return recursive_evaluate(e->a) - recursive_evaluate(e->b);
				case EXPR_OP_MULTIPLY:
					return recursive_evaluate(e->a) * recursive_evaluate(e->b);
				case EXPR_OP_DIVIDE:
					t = recursive_evaluate(e->b);
					if (t == 0)
						errexit(EXPR_EXIT_DIVIDE_BY_ZERO, expr_representation(e->b));
					return recursive_evaluate(e->a) / t;
				case EXPR_OP_MODULUS:
					t = recursive_evaluate(e->b);
					if (t == 0)
						errexit(EXPR_EXIT_DIVIDE_BY_ZERO, expr_representation(e->b));
					return recursive_evaluate(e->a) % t;
				case EXPR_OP_EQUALS:
					return recursive_evaluate(e->a) == recursive_evaluate(e->b);
				case EXPR_OP_NOT_EQUALS:
					return recursive_evaluate(e->a) != recursive_evaluate(e->b);
				case EXPR_OP_LESS_THAN:
					return recursive_evaluate(e->a) < recursive_evaluate(e->b);
				case EXPR_OP_LESS_EQUALS:
					return recursive_evaluate(e->a) <= recursive_evaluate(e->b);
				case EXPR_OP_GREATER_THAN:
					return recursive_evaluate(e->a) > recursive_evaluate(e->b);
				case EXPR_OP_GREATER_EQUALS:
					return recursive_evaluate(e->a) >= recursive_evaluate(e->b);
				case EXPR_OP_AND:
					return recursive_evaluate(e->a) & recursive_evaluate(e->b);
				case EXPR_OP_BOR:
					return recursive_evaluate(e->a) | recursive_evaluate(e->b);
				case EXPR_OP_XOR:
					return recursive_evaluate(e->a) ^ recursive_evaluate(e->b);
                case EXPR_OP_BOOLEAN_NOT:
                    return ! recursive_evaluate(e->b);
                case EXPR_OP_BOOLEAN_AND:
                    return recursive_evaluate(e->a) && recursive_evaluate(e->b);
                case EXPR_OP_BOOLEAN_OR:
                    return recursive_evaluate(e->a) || recursive_evaluate(e->b);
                case EXPR_OP_BOOLEAN_XOR:
                    t = recursive_evaluate(e->a);
                    t2 = recursive_evaluate(e->b);
                    return ((t || t2) && !(t && t2));
                case EXPR_OP_LEFT_SHIFT:
                    return recursive_evaluate(e->a) << recursive_evaluate(e->b);
                case EXPR_OP_RIGHT_SHIFT:
                    return recursive_evaluate(e->a) >> recursive_evaluate(e->b);
				default:
					return 0;
			}
		default:
			return 0;
	}
}

bstring expr_representation(struct expr* e)
{
	bstring a, b, op, result;
	switch (e->type)
	{
		case EXPR_NUMBER:
			return bformat("%u", (uint16_t)(intptr_t)e->data);
		case EXPR_LABEL:
			return bstrcpy((bstring)e->data);
		case EXPR_EXPR:
			a = expr_representation(e->a);
			b = expr_representation(e->b);
			switch (e->op)
			{
				case EXPR_OP_ADD:
					op = bfromcstr("+");
					break;
				case EXPR_OP_SUBTRACT:
					op = bfromcstr("-");
					break;
				case EXPR_OP_MULTIPLY:
					op = bfromcstr("*");
					break;
				case EXPR_OP_DIVIDE:
					op = bfromcstr("/");
					break;
				case EXPR_OP_MODULUS:
					op = bfromcstr("%");
					break;
				case EXPR_OP_EQUALS:
					op = bfromcstr("==");
					break;
				case EXPR_OP_NOT_EQUALS:
					op = bfromcstr("!=");
					break;
				case EXPR_OP_LESS_THAN:
					op = bfromcstr("<");
					break;
				case EXPR_OP_LESS_EQUALS:
					op = bfromcstr("<=");
					break;
				case EXPR_OP_GREATER_THAN:
					op = bfromcstr(">");
					break;
				case EXPR_OP_GREATER_EQUALS:
					op = bfromcstr(">=");
					break;
				case EXPR_OP_AND:
					op = bfromcstr("&");
					break;
				case EXPR_OP_BOR:
					op = bfromcstr("|");
					break;
				case EXPR_OP_XOR:
					op = bfromcstr("^");
					break;
				default:
					op = bfromcstr("???");
					break;
			}
			result = bfromcstr("");
			bconchar(result, '(');
			bconcat(result, a);
			bconchar(result, ' ');
			bconcat(result, op);
			bconchar(result, ' ');
			bconcat(result, b);
			bconchar(result, ')');
			bdestroy(a);
			bdestroy(b);
			bdestroy(op);
			return result;
		default:
			return bfromcstr("???");
	}
}

void expr_delete(struct expr* e)
{
	// Handle the expression components.
	switch (e->type)
	{
		case EXPR_NUMBER:
			// Don't need to free any children components.
			break;
		case EXPR_LABEL:
			// Need to free the bstring.
			bdestroy((bstring)e->data);
			break;
		case EXPR_EXPR:
			// Need to free each child expression recursively.
			expr_delete(e->a);
			expr_delete(e->b);
			break;
	}

	// Free the expression itself.
	free(e);
}
