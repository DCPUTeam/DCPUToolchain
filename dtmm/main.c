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
#include <simclist.h>
#include <version.h>
#include <dirent.portable.h>
#include <argtable2.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <sys/stat.h>
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
	DIR* dir;
	bool printed;
	CURLcode res;
	FILE* fp;
	list_t installed;
	struct bStream* stream;
	long httpcode;
	bstring buffer, fname, sstr;
	bstring ext = bfromcstr(".lua");
	bstring url = bfromcstr("http://dms.dcputoolcha.in/modules/search?q=");
	bstring modpath = osutil_getmodulepath();
	struct dirent* entry;
	list_init(&installed);
	list_attributes_copy(&installed, list_meter_string, 1);
	list_attributes_comparator(&installed, list_comparator_string);

	// Attempt to open the modules directory.
	dir = opendir(modpath->data);
	if (dir == NULL)
	{
		printd(LEVEL_ERROR, "unable to query local repository.\n");
		return 1;
	}

	// Append the temporary search file name.
	bcatcstr(modpath, "/.search");
	bconcat(url, name);

	// Open the file and do the cURL transfer.
	printd(LEVEL_DEFAULT, "querying module repository...\n");
	fp = fopen(modpath->data, "wb");
	curl_easy_setopt(curl, CURLOPT_URL, url->data);
	curl_easy_setopt(curl, CURLINFO_HTTP_CODE, &httpcode);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(curl);
	if (res != 0 || httpcode != 200)
	{
		bdestroy(url);
		bdestroy(name);
		bdestroy(modpath);
		printd(LEVEL_ERROR, "curl failed with error code %i.\n", res);
		return 1;
	}
	fclose(fp);

	// Print the local results.
	printd(LEVEL_DEFAULT, "search results for %s:\n", name->data);
	while ((entry = readdir(dir)) != NULL)
	{
		fname = bfromcstr(&entry->d_name[0]);
		if (binstr(fname, blength(fname) - 4, ext) == BSTR_ERR)
		{
			bdestroy(fname);
			continue;
		}
		if (entry->d_type != DT_REG)
		{
			bdestroy(fname);
			continue;
		}
		sstr = bmidstr(fname, 0, blength(fname) - 4);
		printd(LEVEL_DEFAULT, "	 %s (installed)\n", sstr->data);
		list_append(&installed, sstr->data);
		bdestroy(sstr);
		bdestroy(fname);
	}

	// Print the online results.
	fp = fopen(modpath->data, "r");
	stream = bsopen(&read_data, fp);
	buffer = bfromcstr("");
	printed = false;
	while (bsreadln(buffer, stream, '\n') != BSTR_ERR)
	{
		btrimws(buffer);
		sstr = bmidstr(buffer, 0, blength(buffer) - 4);
		if (!list_contains(&installed, sstr->data))
			printd(LEVEL_DEFAULT, "	 %s\n", sstr->data);
		printed = true;
		bdestroy(sstr);
	}
	if (!printed)
		printd(LEVEL_DEFAULT, "	 <no online results>\n");
	bsclose(stream);
	fclose(fp);
	
	// Clean up.
	curl_easy_cleanup(curl);
	return 0;
}

bool do_install(CURL* curl, bstring name)
{
	FILE* fp;
	CURLcode res;
	long httpcode;
	struct stat buffer;
	bstring url = bfromcstr("http://dms.dcputoolcha.in/modules/download?name=");
	bstring modpath = osutil_getmodulepath();

	// Append the file name.
	bconchar(modpath, '/');
	bconcat(modpath, name);
	bcatcstr(modpath, ".lua");
	bconcat(url, name);
	bcatcstr(url, ".lua");

	// Check to see if the module is already installed.
	if (stat(modpath->data, &buffer) == 0)
	{
		if (unlink(modpath->data) == 0)
			printd(LEVEL_WARNING, "removed existing %s module.\n", name->data);
		else
		{
			printd(LEVEL_ERROR, "unable to remove existing %s module.\n", name->data);
			return 1;
		}
	}

	// Open the file and do the cURL transfer.
	printd(LEVEL_DEFAULT, "querying module repository...\n");
	fp = fopen(modpath->data, "wb");
	curl_easy_setopt(curl, CURLOPT_URL, url->data);
	curl_easy_setopt(curl, CURLINFO_HTTP_CODE, &httpcode);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	res = curl_easy_perform(curl);
	if (res != 0 && httpcode != 200)
	{
		bdestroy(url);
		bdestroy(name);
		bdestroy(modpath);
		printd(LEVEL_ERROR, "curl failed with error code %i.\n", res);
		return 1;
	}
	fclose(fp);
	printd(LEVEL_DEFAULT, "module %s installed.\n", name->data);

	return 0;
}

bool do_uninstall(CURL* curl, bstring name)
{
	bstring modpath = osutil_getmodulepath();
	struct stat buffer;

	// Append the file name.
	bconchar(modpath, '/');
	bconcat(modpath, name);
	bcatcstr(modpath, ".lua");
	
	// Check to see if the module is installed.
	if (stat(modpath->data, &buffer) == 0)
	{
		if (unlink(modpath->data) == 0)
			printd(LEVEL_DEFAULT, "removed existing %s module.\n", name->data);
		else
		{
			printd(LEVEL_ERROR, "unable to remove existing %s module.\n", name->data);
			return 1;
		}
	}
	else
		printd(LEVEL_WARNING, "module %s is not installed.\n", name->data);

	return 0;
}

int main(int argc, char* argv[])
{
	CURL* curl;
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

	if (biseqcstrcaseless(command, "search") || biseqcstrcaseless(command, "se"))
		return do_search(curl, name);
	else if (biseqcstrcaseless(command, "install") || biseqcstrcaseless(command, "in"))
		return do_install(curl, name);
	else if (biseqcstrcaseless(command, "uninstall") || biseqcstrcaseless(command, "rm"))
		return do_uninstall(curl, name);
	else
	{
		printd(LEVEL_ERROR, "unknown command (must be search, install or uninstall).");
		return 1;
	}

	return 0;
}
