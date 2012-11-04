///
/// @addtogroup LibDCPU
/// @{
///
/// @file
/// @brief API for buffered file access.
/// @author James Rhodes
/// 
/// The standalone file functions provided in the C standard library do not reliably
/// check for EOF ahead of time.
///
/// This buffered file access API provides a BFILE structure that will check one character
/// ahead of time whenever performing reads so that bfeof() will reliably return whether
/// the end of the file has been reached.
///
/// This library also deals with platform quirks such as the 14880 hole that occurs
/// on Windows.
///

#ifndef __DCPU_BFILE_H
#define __DCPU_BFILE_H

#include <stdio.h>
#include "dcpu.h"

///
/// @brief The BFILE structure, representing a buffered file handle.
///
/// This structure should for the most part be considered opaque and
/// not modified directly by users.
///
typedef struct
{
    FILE* file; //< A reference to the underlying FILE* handle which we operate on.
    size_t pos; //< The current read position in the file.
    bool readable; //< Whether the file handle is readable.
    bool wrapped; //< Whether this is a buffered wrapping around an already open file.
    int last; //< The last character read from the FILE* handle.
    int eof; //< Whether we have reached the end of the file.
} BFILE;

BFILE* bfopen(const char* path, const char* mode);
BFILE* bfwrap(FILE* _file, const char* mode);
int bfgetc(BFILE* file);
int bfputc(int chr, BFILE* file);
long bftell(BFILE* file);
int bfseek(BFILE* file, long offset, int origin);
int bfeof(BFILE* file);
void bfclose(BFILE* file);
size_t bfread(void* dest, size_t elem, size_t count, BFILE* file);
size_t bfwrite(const void* src, size_t elem, size_t count, BFILE* file);
size_t bfiread(uint16_t* dest, BFILE* file);
size_t bfiwrite(const uint16_t* src, BFILE* file);

#endif

///
/// @}
///