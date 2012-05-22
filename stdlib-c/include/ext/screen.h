/**
 * 
 *	File:		screen.h
 * 
 *	Project:	DCPU-16 Toolchain
 *	Component:	C Extended Library
 * 
 *	Authors:	James Rhodes
 * 
 *	Description:	Provides access to the screen.
 * 
 **/

#ifndef __EXTLIB_SCREEN_H
#define __EXTLIB_SCREEN_H

// Printing functions:
void eputc(char chr, int x, int y);
void eputs(const char* string, int x, int y);
void eputxc(char chr, int x, int y, int modifier);
void eputxs(const char* string, int x, int y, int modifier);

#endif
