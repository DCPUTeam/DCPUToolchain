This is the folder under which support for kernels is defined.  Each kernel can be
specified to support a certain functionality set, of which only 'malloc' and 'free'
are absolutely required.

Each kernel defines the available functions according to it's config.lua file, which
is read in during compilation of DCPU-16 programs.  Where functionality is not provided
by the kernel natively, the C standard library will emulate it.  Note that this may
conflict with the kernel if the kernel does actually handle a given functionality but
is not exposing it, so it important that if something is under the kernel's control
(such as filesystem access), that the appropriate handles are provided for the C
system library.

When defining functions, the function names should be preceded with `_stubapi_`
to ensure global uniqueness and no conflicts.  That is, when defining the `malloc`
function, in the config.lua file you would refer to it as `malloc`, but in the
actual C definition you would name it `_stubapi_malloc` The full list of handles are:

Memory Management
======================

void\* malloc(int size)
--------------------------
*Desciption:* Allocates a block of memory and returns the address.
*Semantics:* If there is no available memory, this function should return a pointer
to 0x0.  Thus 0x0 can never be allocated as memory to a C program.

void free(void\* ptr)
--------------------------
*Description:* Frees previously allocated memory.
*Semantics:* If the passed pointer is within the program's address space, in the heap,
but not the start of an allocated block, the kernel may handle the request however it
deems best necessary, including ignoring the request, or free'ing the block the pointer
is in.

If the passed pointer is within the program's address space, but not in the heap, this
request must fail and the kernel must silently ignore it.

If the passed pointer is outside the program's address space, the kernel may either
silently ignore the request or initiate a segmentation fault in the program (recommended).

File Management
==================
*Notice:* If the kernel does not provide these functions, the C system library will
directly provide them by talking to the appropriate hardware and constructing an
appropriate filesystem (not specified).  If the kernel is handling disk devices,
it must expose these APIs or risk corrupting data on the disk.

void\* open(const char\* path, int path\_size)
------------------------------------------------
*Description:* Opens a handle to a path and returns a pointer to kernel-specific data.
*Semantics:* The `path_size` parameter provides the length of the path string.  This allows
either C strings or Pascal strings to be used inside the kernel.

When the application closes, the kernel must close any handles previously opened by the
process that were not already explicitly closed.

If there is an error opening the handle, the kernel must return NULL.

TODO: Rest of functions.
