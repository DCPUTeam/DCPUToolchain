/**

	File:           node.h

	Project:        DCPU-16 Tools
	Component:      Assembler

	Authors:        James Rhodes

	Description:    Declares node structures for the AST.

**/

#ifndef __DCPU_AST_NODE_H
#define __DCPU_AST_NODE_H

#include <stdint.h>
#include <stdlib.h>

// Address
typedef struct ast_node_address
{
	uint16_t value;
	uint8_t  bracketed;
	uint8_t  added;
	char*    addcmpt;
} ast_node_address_t;

// Register
struct ast_node_register
{
	char*    value;
	uint8_t  bracketed;
};

// Type enumeration
enum ast_node_type
{
	type_raw,
	type_keyword,
	type_address,
	type_register,
	type_parameter,
	type_parameters,
	type_instruction,
	type_label
};

// Parameter
struct ast_node_parameter
{
	enum ast_node_type type;
	struct ast_node_address* address;
	struct ast_node_register* registr;
	char* raw;
	struct ast_node_parameter* prev;
};

// Parameters
struct ast_node_parameters
{
	struct ast_node_parameter* last;
};

// Instruction
struct ast_node_instruction
{
	char* instruction;
	struct ast_node_parameters* parameters;
};

// Label
struct ast_node_label
{
	char* name;
};

// Line
struct ast_node_line
{
	enum ast_node_type type;
	int keyword;
	struct ast_node_label* label;
	struct ast_node_instruction* instruction;
	struct ast_node_line* prev;
	char* keyword_data_string;
	uint16_t keyword_data_numeric;
	uint16_t keyword_data_numeric_2;
};

// Lines
struct ast_node_lines
{
	struct ast_node_line* last;
};

// Root
struct ast_node_root
{
	struct ast_node_lines* values;
};

// External root reference.
extern struct ast_node_root ast_root;

#endif
