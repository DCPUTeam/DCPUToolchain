///
/// @addtogroup LibDCPU
/// @{
///
/// @file
/// @brief The implementation of the buffered file access library.
/// @author James Rhodes
/// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bfile.h"
#include "iio.h"

///
/// @brief Open a file with the specified mode.
///
/// This function takes the same parameters as fopen().
///
/// @param path The path of the file to open.
/// @param mode The mode to open the file with.
/// @return The buffered file handle.
///
BFILE* bfopen(const char* path, const char* mode)
{
    BFILE* file = (BFILE*)malloc(sizeof(BFILE));
    file->file = fopen(path, mode);
    if (file->file == NULL)
    {
        free(file);
        return NULL;
    }
    file->readable = (mode[0] == 'r' || mode[strlen(mode) - 1] == '+');
    file->last = -1;
    file->eof = false;
    file->pos = 0;
    file->wrapped = false;
    if (file->readable)
    {
        file->last = fgetc(file->file);
        file->eof = feof(file->file);
    }
    return file;
}

///
/// @brief Wraps an existing FILE* handle for buffered access.
///
/// In some cases, you may want to wrap an existing FILE* handle
/// for buffered access (e.g. the stdin / stdout handles).
///
/// @note This internally keeps track that the BFILE* was wrapped and
///       therefore will not close the underlying file handle when bfclose()
///       is called.
///
/// @param _file The file handle to wrap.
/// @param mode The mode that the file handle is already open under.
/// @return The buffered file handle.
///
BFILE* bfwrap(FILE* _file, const char* mode)
{
    BFILE* file = (BFILE*)malloc(sizeof(BFILE));
    file->file = _file;
    if (file->file == NULL)
    {
        free(file);
        return NULL;
    }
    file->readable = (mode[0] == 'r' || mode[strlen(mode) - 1] == '+');
    file->last = -1;
    file->eof = false;
    file->wrapped = true;
    if (file->readable)
    {
        file->last = fgetc(file->file);
        file->eof = feof(file->file);
    }
    return file;
}

///
/// @brief Get a character from the buffered file stream.
///
/// @param file The buffered file handle.
/// @return The next character, or EOF if past the end of the stream.
///
int bfgetc(BFILE* file)
{
    int c;
    assert(file != NULL && file->readable);
    c = file->last;
    file->pos++;
    file->last = fgetc(file->file);
    file->eof = feof(file->file);
    return c;
}

///
/// @brief Put a character into the buffered file stream.
///
/// @param chr The character to write.
/// @param file The buffered file handle.
/// @return On success the written character is returned, otherwise EOF.
///
int bfputc(int chr, BFILE* file)
{
    assert(file != NULL);
    return fputc(chr, file->file);
}

///
/// @brief Returns the current read position in the stream.
///
/// @param file The buffered file handle.
/// @return The position in the stream used for reading.
///
long bftell(BFILE* file)
{
    assert(file != NULL);
    return file->pos;
}

///
/// @brief Seeks to the specified position in the stream.
///
/// @warning Due to read position caching (a workaround for a Windows
///          API bug), this function will set the position for both
///          reading and writing.  However, writes to the file will
///          not be reported in bftell() as it only provides the position
///          of the read point.
///
/// @param file The buffered file handle.
/// @param offset The offset relative to the origin.
/// @param origin The origin, either SEEK_SET or SEEK_CUR.
/// @return Zero on success, non-zero on failure.
///
int bfseek(BFILE* file, long offset, int origin)
{
    assert(file != NULL);
    if (origin == SEEK_SET)
        file->pos = offset;
    else if (origin == SEEK_CUR)
        file->pos += offset;
    else
        assert(origin != SEEK_END);
    return fseek(file->file, file->pos, SEEK_SET);
}

///
/// @brief Whether the end of the stream has been reached.
///
/// @return True if at the end of the stream, false otherwise.
///
int bfeof(BFILE* file)
{
    assert(file != NULL);
    return file->eof;
}

///
/// @brief Close the buffered file handle.
///
/// If this is not a wrapped handle, the underlying file handle
/// will also be closed.  If it is a wrapped handle however, it
/// is expected that the caller of bfwrap() will manually call
/// fclose() on the original file handle if required.
///
/// @param file The buffered file handle.
///
void bfclose(BFILE* file)
{
    assert(file != NULL);
    fflush(file->file);
    if (!file->wrapped)
        fclose(file->file);
    file->file = NULL;
    file->eof = true;
    file->last = -1;
    file->pos = 0;
    file->readable = false;
    free(file);
}

///
/// @brief Read data from the buffered file handle.
///
/// A compatible version of fread() for buffered file handles.
///
/// @sa The C standard library's documentation on fread().
///
size_t bfread(void* dest, size_t elem, size_t count, BFILE* file)
{
    size_t result;
    assert(file != NULL && file->readable);
    fseek(file->file, file->pos, SEEK_SET);
    result = fread(dest, elem, count, file->file);
    file->pos += result;
    file->last = fgetc(file->file);
    file->eof = feof(file->file);
    return result;
}

///
/// @brief Write data to the buffered file handle.
///
/// A compatible version of fwrite() for buffered file handles.
///
/// @sa The C standard library's documentation on fwrite().
///
size_t bfwrite(const void* src, size_t elem, size_t count, BFILE* file)
{
    return fwrite(src, elem, count, file->file);
}

///
/// @brief Read a single uint16_t from the buffered file handle.
///
/// Reads a uint16_t from the buffered file handle taking into account
/// the current global endianness setting.
///
/// @param dest The destination to store the uint16_t in.
/// @param file The buffered file handle.
/// @return The number of bytes read.
///
size_t bfiread(uint16_t* dest, BFILE* file)
{
    size_t result;
    assert(file != NULL && file->readable);
    fseek(file->file, file->pos, SEEK_SET);
    result = iread(dest, file->file);
    file->pos += result;
    file->last = fgetc(file->file);
    file->eof = feof(file->file);
    return result;
}

///
/// @brief Write a single uint16_t from the buffered file handle.
///
/// Writes a uint16_t to the buffered file handle taking into account
/// the current global endianness setting.
///
/// @param src The uint16_t to write.
/// @param file The buffered file handle.
/// @return The number of bytes written.
///
size_t bfiwrite(const uint16_t* src, BFILE* file)
{
    return iwrite(src, file->file);
}

///
/// @}
///