// API file for stubsys.

void _stubapi_free(void* ptr)
{
	__asm
	{
		.IMPORT free
		SET A, <ptr>
		JSR free
	}
}

void* _stubapi_malloc(unsigned int size)
{
	void* result;
	void** store = &result;
	__asm
	{
		.IMPORT malloc
		SET A, <size>
		JSR malloc
		SET <store>, A
	}
	return result;
}

unsigned int _stubapi_errno()
{
    // We are not in control of anything that can fail.
    return 0;
}

void _stubapi_exit(unsigned int code)
{
    __asm
    {
        .IMPORT _kern_exit
        SET PC, _kern_exit
    }
}

