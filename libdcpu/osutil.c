/**

	File:		osutil.c

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU

	Authors:	James Rhodes

	Description:	Defines functions for interacting with the
			local operating system (such as getting
			directory names, etc.)

**/

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <libgen.h>
#endif
// FIXME: How portable are these headers?  Do they work on
//	  both Windows and Linux?
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <assert.h>
#include "osutil.h"

bstring osutil_arg0 = NULL;
#ifdef _WIN32
char* dirname(char* path)
{
	// FIXME: This assumes the resulting path will always
	// be shorter than the original (which it should be
	// given that we're only returning a component of it, right?)
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	_splitpath(path, drive, dir, NULL, NULL);
	strcpy(path, drive);
	strcpy(path + strlen(path), dir);
	return path;
}
#endif

///
/// Returns the directory name component of the specified path.
///
/// Returns the directory component of the specified path in a
/// cross-platform manner.
///
/// @param path The path to retrieve the directory name of.
/// @return The directory name component.
///
bstring osutil_dirname(bstring path)
{
	bstring bpath;
	char* cpath;
	char* opath;

	cpath = bstr2cstr(path, '0');
	// needs to return in case of dirname implementation not changing parameter
	// in particular this doesn't work on OS X check $ man 3 dirname
	opath = dirname(cpath);
	bpath = bfromcstr(opath);
	bcstrfree(cpath);

	return bpath;
}

///
/// Sets the specified file handler to binary mode.
///
/// On some platforms (Windows), file descriptors are opened in text
/// mode by default.  In particular, on these platforms, standard
/// input, output and error are all text mode by default which causes
/// issues when outputting emulator bytecode to these descriptors.
/// This function sets a file descriptor into binary mode in a
/// cross-platform manner.
///
/// @param fd The file descriptor to switch into binary mode.
///
void osutil_makebinary(FILE* fd)
{
#ifdef _WIN32
	_setmode(_fileno(stdout), _O_BINARY);
#endif
}

///
/// Stores the application path in a global context.
///
/// Stores the application path (arg0) in a global context so that
/// libraries can retrieve it later using osutil_getarg0.  All applications
/// using a DCPU-16 Toolchain library should invoke this method after
/// parsing their arguments.  If this value is not set, some libraries
/// may be unable to resolve their required runtime components.
///
/// @param arg0 The string containing argument 0.
///
void osutil_setarg0(freed_bstring arg0)
{
	if (osutil_arg0 != NULL)
		bdestroy(osutil_arg0);
	osutil_arg0 = bstrcpy(arg0.ref);
	bautodestroy(arg0);
}

///
/// Retrieves the current path to the application.
///
/// Retrieves the previously stored path to the application and
/// returns a copy of the internal string.
///
/// @return A copy of the current path to the application.  The result
///	    must be freed manually.
///
bstring osutil_getarg0()
{
	assert(osutil_arg0 != NULL);

	return bstrcpy(osutil_arg0);
}

///
/// Retrieves the current directory that the application executable is located in.
///
/// Retrieves the previously stored path to the application, resolves the directory
/// using os_dirname and returns a copy of the directory the application is contained in.
///
/// @return The path of the directory containing the application.  The result must be freed manually.
///
bstring osutil_getarg0path()
{
	bstring arg0 = osutil_getarg0();
	bstring path = osutil_dirname(arg0);
#ifdef WIN32
	// Special handling for Windows where the path
	// can be blank if argument 0 is just the executable
	// name.
	if (biseqcstr(path, ""))
	{
		bdestroy(path);
		bdestroy(arg0);
		arg0 = bfromcstr(".\\");
		bconcat(arg0, osutil_getarg0());
		path = osutil_dirname(arg0);
	}
#endif
	bdestroy(arg0);
	return path;
}

///
/// Retrieves the directory that contains toolchain modules.
///
/// Retrieves the directory that contains toolchain modules based on the existance of the
/// TOOLCHAIN_MODULES environment variable or defaulting to the modules/ folder in the
/// same directory as the current application.	This requires osutil_setarg0 to have been
/// previously called.
///
/// @return The path of the directory holding toolchain modules.  The result must be freed manually.
///
bstring osutil_getmodulepath()
{
	// FIXME: This function should return NULL
	//	  if the path does not exist or is
	//	  not a directory.
	bstring tmp;
	int result;
	struct stat buffer;
	char* env = getenv("TOOLCHAIN_MODULES");
	if (env == NULL)
	{
		tmp = osutil_getarg0path();
		bcatcstr(tmp, "/modules");
	}
	else
	{
		tmp = bfromcstr(env);
	}

	// Check if path exists.
	result = stat(tmp->data, &buffer);
	if (result != 0 || (buffer.st_mode & S_IFDIR) == 0)
	{
		bdestroy(tmp);
		return NULL;
	}
	else
		return tmp;
}
