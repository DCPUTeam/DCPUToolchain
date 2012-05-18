/**
 *
 *	File:		rparse.h
 *
 *	Project:	DCPU-16 Toolchain
 *	Component:	LibDCPU
 *
 *	Authors:	James Rhodes
 *
 *	Description:	Declares functions for parsing line and filename
 *			information at a lexing stage.
 *
 **/

#ifndef __DCPU_RPARSE_H
#define __DCPU_RPARSE_H

void rparse_do(char* yytext, int* out_line, bstring* out_filename);

#endif

