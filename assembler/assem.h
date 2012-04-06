//
// DCPU Assembler by James Rhodes
//
// Assembler code.
//

#ifndef __DCPU_ASM_ASSEM_H
#define __DCPU_ASM_ASSEM_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "node.h"
#include "dcpu.h"

void reverse_lines(struct ast_node_lines* lines);
void process_root(struct ast_node_root* root);


#endif