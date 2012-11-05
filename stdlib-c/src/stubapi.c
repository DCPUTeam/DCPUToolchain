// Links to stubsys.

int main(int argc, char** argv);

void _stdlib_enter()
{
    int code = main(0, NULL);
    __asm
    {
        SET A, <code>
        .CALL _stubapi_exit
    }
    // Process is terminated by kernel after this
    // point.  No further code is executed.
}
