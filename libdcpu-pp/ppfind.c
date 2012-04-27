/**

	File:			ppfind.c

	Project:		DCPU-16 Tools
	Component:		LibDCPU-PP

	Authors:		James Rhodes

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
	ppfind_add_path(bfromcstr("."));
}

void ppfind_add_path(bstring path)
{
	// Initialize if needed.
	if (!ppfind_initialized)
		ppfind_init();
	
	// Add path to list.
	list_append(&ppfind_include_paths, path);
}

void ppfind_add_autopath(bstring file)
{
	ppfind_add_path(osutil_dirname(file));
}

void ppfind_remove_path(bstring path)
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
		if (biseq(data, path))
		{
			list_delete(&ppfind_include_paths, data);
			bdestroy(data);
			return;
		}
	}
}

bstring ppfind_locate(bstring path)
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
		if (bconcat(result, path) == BSTR_ERR)
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
		return result;
	}

	// No such file was found.
	return NULL;
}