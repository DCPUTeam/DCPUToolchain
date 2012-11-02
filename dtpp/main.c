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

char next = '\0';

bool has_input()
{
    return !feof(stdin);
    //return next != '!';
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

    if (dsethalt())
    {
        errval = derrinfo();
        printf("error occurred while preprocessing:\n");
        printf(derrstr[errval->errid], errval->errdata);
        getc(stdin);
        getc(stdin);
        return 1;
    }

    printf("! terminates input.\n\n");
    ppimpl(bautofree(bfromcstr("asm")), has_input, input, output);
    getc(stdin);
    getc(stdin);
    return 0;
}
