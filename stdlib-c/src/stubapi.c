// Links to stubsys.

int main(int argc, char** argv);

void _stdlib_enter()
{
    int code = main(0, NULL);
    __asm
    {
        .IMPORT _stubapi_exit
		SET A, <code>
        SET PC, [_stubapi_exit]
    }
    // Process is terminated by kernel after this
    // point.  No further code is executed.
}
