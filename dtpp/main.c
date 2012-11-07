/**

    File:       main.c

    Project:    DCPU-16 Tools
    Component:  Preprocessor

    Authors:    James Rhodes

    Description:    Main entry point.

**/

#include <dcpu.h>
#include <ppimpl.h>
#include <derr.h>
#include <osutil.h>

char next = '\0';

bool has_input()
{
    return !feof(stdin);
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
    struct errinfo* errval;

    if (argc != 2)
    {
        fprintf(stderr, "usage: dtpp lang < file\n");
        return 1;
    }

    osutil_setarg0(bautofree(bfromcstr(argv[0])));
    
    if (dsethalt())
    {
        errval = derrinfo();
        printf("error occurred while preprocessing:\n");
        printf(derrstr[errval->errid], errval->errdata);
        return 1;
    }

    ppimpl(bautofree(bfromcstr("<stdin>")), 0, bautofree(bfromcstr(argv[1])), has_input, input, output);
    return 0;
}
