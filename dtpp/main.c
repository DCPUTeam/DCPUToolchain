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
#include <ppfind.h>

char next = '\0';

bool has_input()
{
    return !feof(stdin) && next != '~';
}

char_t input()
{
    char_t c;
    c.value = (char)getc(stdin);
    c.file = bfromcstr("<stdin>");
    c.line = 0;
    c.column = 0;
    return c;
}

void output(char_t o)
{
    printf("%c (%s:%i:%i)\n", o.value, o.file->data, o.line, o.column);
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

    ppfind_add_path(bautofree(bfromcstr("../0x10c/stdlib-c/include")));

    ppimpl(bautofree(bfromcstr("<stdin>")), 0, bautofree(bfromcstr(argv[1])), has_input, input, output);
    return 0;
}
