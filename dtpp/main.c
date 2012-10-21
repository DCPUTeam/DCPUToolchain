/**

	File:		main.c

	Project:	DCPU-16 Tools
	Component:	Preprocessor

	Authors:	James Rhodes

	Description:	Main entry point.

**/

#include <dcpu.h>
#include <ppimpl.h>

char next = '\0';

bool has_input()
{
    return next != '!';
}

char input()
{
    next = (char)getc(stdin);
    return next;
}

void output(char o)
{
    putc((int)o, stdout);
}

int main(int argc, char* argv[])
{
    printf("! terminates input.\n\n");
    ppimpl(has_input, input, output);
    getc(stdin);
    getc(stdin);
    return 0;
}
