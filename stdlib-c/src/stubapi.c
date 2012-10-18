// Links to stubsys.

int main(int argc, char** argv);
int _stubapi_exit(int code);

void stdlib_enter()
{
    int code = main(0, NULL);
    _stubapi_exit(code);
    // Process is terminated by kernel after this
    // point.  No further code is executed.
}
