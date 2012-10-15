/**

	File:		assem.h

	Project:	DCPU-16 Tools
	Component:	Assembler

	Authors:	James Rhodes

	Description:	Declares public functions to traverse and
			process the AST.

**/

#ifndef __DCPU_ASM_ASSEM_H
#define __DCPU_ASM_ASSEM_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <simclist.h>
#include "node.h"
#include "dcpu.h"

void reverse_lines(struct ast_node_lines* lines);
void process_root(struct ast_node_root* root, list_t* symbols);


#endif
