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
**Desciption:** Allocates a block of memory and returns the address.<br/>
**Semantics:** If there is no available memory, this function should return a pointer
to 0x0.  Thus 0x0 can never be allocated as memory to a C program.

void free(void\* ptr)
--------------------------
**Description:** Frees previously allocated memory.<br/>
**Semantics:** If the passed pointer is within the program's address space, in the heap,
but not the start of an allocated block, the kernel may handle the request however it
deems best necessary, including ignoring the request, or free'ing the block the pointer
is in.

If the passed pointer is within the program's address space, but not in the heap, this
request must fail and the kernel must silently ignore it.

If the passed pointer is outside the program's address space, the kernel may either
silently ignore the request or initiate a segmentation fault in the program (recommended).

Error Handling
================

int errno()
-------------
**Description:** Returns the error number from the last kernel call.<br/>
**Semantics:** After each successful kernel call (except for this one), the error number
maintained by the kernel should be 0.  Error numbers should be maintained unique to each
process and the kernel should not expose, without good reason such as debugging purposes,
the ability for processes to change the error number of another process.

See the error code reference at the bottom of this document for a full list of error
codes that the kernel may return from this call.

File Management
==================
**Notice:** If the kernel does not provide these functions, the C system library will
directly provide them by talking to the appropriate hardware and constructing an
appropriate filesystem (not specified).  If the kernel is handling disk devices,
it must expose these APIs or risk corrupting data on the disk.

void\* open(const char\* path, int path\_size)
------------------------------------------------
**Description:** Opens a handle to a path and returns a pointer to kernel-specific data.<br/>
**Semantics:** The `path_size` parameter provides the length of the path string.  This allows
either C strings or Pascal strings to be used inside the kernel.

When the application closes, the kernel must close any handles previously opened by the
process that were not already explicitly closed.

If there is an error opening the handle, the kernel must return NULL.

uint16_t read(void* handle, void* out, uint16_t offset, uint16_t size)
-----------------------------------------------------------------------
**Description:** Reads data from the file handle and stores the data in the memory pointed to
by out.  The offset specifies the position in the file.  The size is the maximum number of
bytes to read and the call returns the actual number of bytes read.<br/>
**Semantics:** The kernel must read up to size bytes from the file, but MUST store them in
the out parameter as individual bytes in memory.  It is up to the kernel to handle unpacking
if stored on a disk format that deals with words instead of bytes, but the kernel should enable
access to all data on the disk.  The kernel should return the actual number of bytes read, which
may be less that the specified size but MUST NOT be more than the specified size.  If a read is
requested past the end of the file the kernel must return 0 and perform no reading.

uint16_t write(void* handle, void* in, uint16_t offset, uint16_t size)
-----------------------------------------------------------------------
**Description:** Writes data from memory pointed to by in to the file handle.  Writing begins
in the file at the specified offset.  The size is the maximum number of bytes the kernel
is permitted to write to the file from memory and the call returns the number of actual
bytes written.<br/>
**Semantics:** The kernel must write up to size bytes from the file, but MUST treat the memory
source as individual bytes in memory.  It is up to the kernel to handle packing of bytes
if stored on a disk format that deals with words instead of bytes, but the kernel should
enable writing to all data on disk (except where permissions otherwise disallow).  The kernel
should return the actual number of bytes written, which may be less than the specified size
but MUST NOT be more than the specified size.  If a write is requested past the end of a file,
the kernel must expand the file if disk space permits, otherwise the kernel should write up
until the disk is full.

void close(void* handle)
-------------------------
**Description:** Closes an existing file handle.<br/>
**Semantics:** If the address that is passed is not to a valid handle, the kernel may silently
ignore the request or initiate a segmentation fault in the program (recommended).

Other I/O Management
======================
**Notice:** If the kernel does not provide these functions, the C system library will
directly provide them by talking to the appropriate hardware and constructing an
appropriate filesystem (not specified).  If the kernel is handling disk devices,
it must expose these APIs or risk corrupting data on the disk.

void mkdir(const char\* path, int path\_size)
-----------------------------------------------
**Description:** Creates a new directory at the specified path.<br/>
**Semantics:** The `path_size` parameter provides the length of the path string.  This allows
either C strings or Pascal strings to be used inside the kernel.

void rmdir(const char\* path, int path\_size)
-----------------------------------------------
**Description:** Removes an empty directory at the specified path.<br/>
**Semantics:** The `path_size` parameter provides the length of the path string.  This allows
either C strings or Pascal strings to be used inside the kernel.

The kernel must not remove the directory if it is not empty.

Appendix: Error Code Reference
===============================
A full list of defined error codes is as follows.  Kernels must not define additional
error codes in a non-standard manner as this will conflict with future error code allocations.

This is derived from the command `perl -le 'print $!+0, "\t", $!++ for 0..127'` on Linux-based
systems, with error code omissions where deemed suitable.

  - 0: Success
  - 1: Operation not permitted
  - 2: No such file or directory
  - 3: No such process
  - 4: Interrupted system call
  - 5: Input/output error
  - 6: No such device or address
  - 7: (reserved/omitted)
  - 8: Exec format error
  - 9: Bad file descriptor
  - 10: No child processes
  - 11: Resource temporarily unavailable
  - 12: Cannot allocate memory
  - 13: Permission denied
  - 14: Bad address
  - 15: Block device required
  - 16: Device or resource busy
  - 17: File exists
  - 18: (reserved/omitted)
  - 19: No such device
  - 20: Not a directory
  - 21: Is a directory
  - 22: Invalid argument
  - 23: Too many open files in system
  - 24: Too many open files
  - 25: (reserved/omitted)
  - 26: (reserved/omitted)
  - 27: File too large
  - 28: No space left on device
  - 29: Illegal seek
  - 30: Read-only file system
  - 31: Too many links
  - 32: (reserved/omitted)
  - 33: (reserved/omitted)
  - 34: (reserved/omitted)
  - 35: (reserved/omitted)
  - 36: File name too long
  - 37: (reserved/omitted)
  - 38: Function not implemented
  - 39: Directory not empty
  - 40: Too many levels of symbolic links
  - 41 to 76: (reserved/omitted)
  - 77: File descriptor in bad state
  - 78 to 122: (reserved/omitted)
  - 123: No media found
  - 124: Wrong medium type
  - 125: Operation cancelled
  - 126: (reserved/omitted)
  - 127: (reserved/omitted)
