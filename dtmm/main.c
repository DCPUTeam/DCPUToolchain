/**

	File:		main.c

	Project:	DCPU-16 Tools
	Component:	Module Manager

	Authors:	James Rhodes

	Description:	Main entry point.

**/

#include <stdio.h>
#include <stdlib.h>
#include <bstring.h>
#include <version.h>
#include <argtable2.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <osutil.h>
#include <debug.h>

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
	return fwrite(ptr, size, nmemb, stream);
}

size_t read_data(void* buff, size_t elsize, size_t nelem, void* param)
{
	return fread(buff, elsize, nelem, (FILE*)param);
}

bool do_search(CURL* curl, bstring name)
{
	bool printed;
	CURLcode res;
	FILE* fp;
	struct bStream* stream;
	bstring buffer;
	bstring url = bfromcstr("http://dms.dcputoolcha.in/modules/search?q=");
	bstring modpath = osutil_getmodulepath();
	// TODO: Search existing module path to see
	//       if there is a match already in the
	//       module folder.
	
	// Append the temporary search file name.
	// FIXME: Is there a better way to do this?  Can
	//        we write into a temporary buffer instead
	//        for the search results?
	bcatcstr(modpath, "/.search");
	bconcat(url, name);

	// Open the file and do the cURL transfer.
	printd(LEVEL_DEFAULT, "querying module repository...\n");
	fp = fopen(modpath->data, "wb");
	curl_easy_setopt(curl, CURLOPT_URL, url->data);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(curl);
	if (res != 0)
	{
		bdestroy(url);
		bdestroy(name);
		bdestroy(modpath);
		printd(LEVEL_ERROR, "curl failed with error code %i.\n", res);
		return 1;
	}
	fclose(fp);

	// Print the results.
	printd(LEVEL_DEFAULT, "search results for %s:\n", name->data);
	fp = fopen(modpath->data, "r");
	stream = bsopen(&read_data, fp);
	buffer = bfromcstr("");
	printed = false;
	while (bsreadln(buffer, stream, '\n') != BSTR_ERR)
	{
		printd(LEVEL_DEFAULT, "  %s", buffer->data);
		printed = true;
	}
	if (!printed)
		printd(LEVEL_DEFAULT, "  <no results>\n");
	bsclose(stream);
	fclose(fp);
	
	// Clean up.
	curl_easy_cleanup(curl);
	return 0;
}

bool do_install(bstring name)
{
}

bool do_uninstall(bstring name)
{
	printd(LEVEL_ERROR, "not implemented.\n");
	return 1;
}

int main(int argc, char* argv[])
{
	FILE* fp;
	CURL* curl;
	CURLcode res;
	bstring command;
	bstring name;
	bstring modpath;

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_str* cmdopt = arg_str1(NULL, NULL, "<command>", "The command; either 'search', 'install' or 'uninstall'.");
	struct arg_str* nameopt = arg_str1(NULL, NULL, "<name>", "The name of the module to search for, install or uninstall.");
	struct arg_end* end = arg_end(20);
	void* argtable[] = { show_help, cmdopt, nameopt, end };

	// Parse arguments.
	int nerrors = arg_parse(argc, argv, argtable);

	version_print(bautofree(bfromcstr("Module Manager")));
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stderr, end, "mm");

		fprintf(stderr, "syntax:\n    dtmm");
		arg_print_syntax(stderr, argtable, "\n");
		fprintf(stderr, "options:\n");
		arg_print_glossary(stderr, argtable, "	  %-25s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	// Set argument 0 and convert parameters.
	osutil_setarg0(bautofree(bfromcstr(argv[0])));
	command = bfromcstr(cmdopt->sval[0]);
	name = bfromcstr(nameopt->sval[0]);

	// Initialize curl or exit.
	curl = curl_easy_init();
	if (!curl)
	{
		printd(LEVEL_ERROR, "unable to initialize curl.\n");
		return 1;
	}

	// Ensure module path exists.
	modpath = osutil_getmodulepath();
	if (modpath == NULL)
	{
		printd(LEVEL_ERROR, "module path does not exist (searched TOOLCHAIN_MODULES and modules/).\n");
		return 1;
	}
	bdestroy(modpath);

	// Command: search (se)
	if (biseqcstrcaseless(command, "search") || biseqcstrcaseless(command, "se"))
		return do_search(curl, name);	
	else
	{
		printd(LEVEL_ERROR, "unknown command (must be search, install or uninstall).");
		return 1;
	}

/*
	printf("initializing curl...\n");
	curl = curl_easy_init();
	if (curl)
	{
		printf("opening file...\n");
		fp = fopen("test.txt", "wb");
		printf("setting curl options...\n");
		curl_easy_setopt(curl, CURLOPT_URL, "http://dms.dcputoolcha.in/modules/download?name=rng.lua");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		printf("performing curl download...\n");
		res = curl_easy_perform(curl);
		// TODO: Do something with result.
		printf("cleaning up...\n");
		curl_easy_cleanup(curl);
		printf("closing file...\n");
		fclose(fp);
		printf("done.\n");
	}
	else
		printf("unable to initialize.\n");
*/
	return 0;
}
