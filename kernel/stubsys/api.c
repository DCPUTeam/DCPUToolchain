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

void* _stubapi_malloc(size_t size)
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

int _stubapi_errno()
{
    // We are not in control of anything that can fail.
    return 0;
}

void _stubapi_exit(int code)
{
    __asm
    {
        .IMPORT _kern_exit
        SET PC, _kern_exit
    }
}
