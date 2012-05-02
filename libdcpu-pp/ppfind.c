/**

	File:		ppfind.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU-pp

	Authors:	James Rhodes

	Description:	Defines the functions used to automatically
			locate files based on a set of inclusion paths.

**/

#include <stdio.h>
#include <bstrlib.h>
#include <simclist.h>
#include <osutil.h>
#include "ppfind.h"
#include "dcpu.h"

bool ppfind_initialized = false;
uint32_t ppfind_include_depth;
list_t ppfind_include_paths;

void ppfind_init()
{
	list_init(&ppfind_include_paths);
	ppfind_include_depth = 0;
	ppfind_initialized = true;
	ppfind_add_path(bautofree(bfromcstr(".")));
}

void ppfind_add_path(freed_bstring path)
{
	// Initialize if needed.
	if (!ppfind_initialized)
		ppfind_init();

	// Add path to list.
	list_append(&ppfind_include_paths, path.ref);

	// Free memory.
	bautodestroy(path);
}

void ppfind_add_autopath(freed_bstring file)
{
	ppfind_add_path(bautofree(osutil_dirname(file.ref)));
	bautodestroy(file);
}

void ppfind_remove_path(freed_bstring path)
{
	unsigned int i;

	// Initialize if needed.
	if (!ppfind_initialized)
		ppfind_init();

	// Find specified path and remove it
	// if it's in there.
	for (i = 0; i < list_size(&ppfind_include_paths); i++)
	{
		bstring data = (bstring)list_get_at(&ppfind_include_paths, i);

		if (biseq(data, path.ref))
		{
			list_delete(&ppfind_include_paths, data);
			bdestroy(data);
			return;
		}
	}

	// Free memory.
	bautodestroy(path);
}

bstring ppfind_locate(freed_bstring path)
{
	unsigned int i;
	FILE* test;

	// Initialize if needed.
	if (!ppfind_initialized)
		ppfind_init();

	// Go through all of the paths in the
	// include list and concatenate the
	// path onto them, then check if the file
	// exists.
	for (i = 0; i < list_size(&ppfind_include_paths); i++)
	{
		bstring data = (bstring)list_get_at(&ppfind_include_paths, i);
		bstring result = bstrcpy(data);

		if (bconchar(result, '/') == BSTR_ERR)
		{
			bdestroy(result);
			continue;
		}

		if (bconcat(result, path.ref) == BSTR_ERR)
		{
			bdestroy(result);
			continue;
		}

		// Check if file exists.
		test = fopen((const char*)(result->data), "rb");

		if (test == NULL)
			continue;

		// File was found, close it and return this
		// path as the result.
		fclose(test);
		bautodestroy(path);
		return result;
	}

	// No such file was found.
	bautodestroy(path);
	return NULL;
}
