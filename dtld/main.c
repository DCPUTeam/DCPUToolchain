/**

    File:       main.c

    Project:    DCPU-16 Tools
    Component:  Linker

    Authors:    James Rhodes

    Description:    Main entry point.

**/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <argtable2.h>
#include <version.h>
#include <debug.h>
#include <osutil.h>
#include <iio.h>
#include <derr.h>
#include <policy.h>
#include "objfile.h"
#include "lprov.h"
#include "ldata.h"
#include "ldbins.h"
#include "ldtarget.h"
#include "ldpolicy.h"

struct tagbstring default_target = bsStatic("image");

int main(int argc, char* argv[])
{
    // Define our variables.
    int nerrors, i;
    int32_t saved = 0; // The number of words saved during compression and optimization.
    const char* warnprefix = "no-";
    int target;
    policies_t* loaded_policies = NULL;
    const char* policy_kernel = NULL;
    FILE* policy_file_io = NULL;
    bstring policy_path = NULL;
    bstring policy_kernel_path = NULL;
    bstring policy_jumplist_path = NULL;
    bstring policy_target = NULL;
    bstring policy_direct_target = NULL;
    bstring policy_direct_default = NULL;
    bool policy_direct = false;
    bool policy_use_kernel = false;

    // Define arguments.
    struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
    struct arg_str* target_arg = arg_str0("l", "link-as", "target", "Link as the specified object, can be 'static', 'kernel' or policy target.");
    struct arg_file* symbol_file = arg_file0("s", "symbols", "<file>", "Produce a combined symbol file (~triples memory usage!).");
    struct arg_str* symbol_ext = arg_str0(NULL, "symbol-extension", "ext", "When -s is used, specifies the extension for symbol files.  Defaults to \"dsym16\".");
    struct arg_file* input_files = arg_filen(NULL, NULL, "<file>", 1, 100, "The input object files.");
    struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file.");
    struct arg_str* kernel_name = arg_str0("k", "kernel", "<name>", "Target the specified kernel.");
    struct arg_file* policy_file = arg_file0("p", "policy", "<file>", "Use the specified policy file instead of the kernel policy.");
    struct arg_file* jumplist_file = arg_file0(NULL, "jumplist", "<file>", "The path to output the jumplist to (only used for building kernels.");
    struct arg_lit* direct_link = arg_lit0(NULL, "direct", "Perform a direct link against the kernel, overriding the kernel default.");
    struct arg_lit* no_direct_link = arg_lit0(NULL, "no-direct", "Do not perform a direct link against the kernel, overriding the kernel default.");
    struct arg_str* warning_policies = arg_strn("W", NULL, "policy", 0, _WARN_COUNT * 2 + 10, "Modify warning policies.");
    struct arg_lit* keep_output_arg = arg_lit0(NULL, "keep-outputs", "Keep the .OUTPUT entries in the final static library (used for stdlib).");
    struct arg_lit* little_endian_mode = arg_lit0(NULL, "little-endian", "Use little endian serialization (for compatibility with older versions).");
    struct arg_lit* no_short_literals_arg = arg_lit0(NULL, "no-short-literals", "Do not compress literals to short literals.");
    struct arg_int* opt_level = arg_int0("O", NULL, "<level>", "The optimization level.");
    struct arg_lit* opt_mode = arg_lit0("S", NULL, "Favour runtime speed over size when optimizing.");
    struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
    struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
    struct arg_end* end = arg_end(20);
    void* argtable[] = { show_help, target_arg, keep_output_arg, little_endian_mode, opt_level, opt_mode, direct_link, no_direct_link,
                         no_short_literals_arg, symbol_ext, symbol_file, kernel_name, policy_file, jumplist_file, warning_policies, output_file,
                         input_files, verbose, quiet, end
                       };

    // Parse arguments.
    nerrors = arg_parse(argc, argv, argtable);

    if (nerrors != 0 || show_help->count != 0)
    {
        if (show_help->count != 0)
            arg_print_errors(stdout, end, "linker");

        printd(LEVEL_DEFAULT, "syntax:\n    dtld");
        arg_print_syntax(stderr, argtable, "\n");
        printd(LEVEL_DEFAULT, "options:\n");
        arg_print_glossary(stderr, argtable, "    %-25s %s\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Set verbosity level.
    debug_setlevel(LEVEL_DEFAULT + verbose->count - quiet->count);
    
    // Show version information.
    version_print(bautofree(bfromcstr("Linker")));

    // Set global path variable.
    osutil_setarg0(bautofree(bfromcstr(argv[0])));

    // Set endianness.
    isetmode(little_endian_mode->count == 0 ? IMODE_BIG : IMODE_LITTLE);

    // Set up warning policies.
    dsetwarnpolicy(warning_policies);

    // Set up error handling.
    if (dsethalt())
    {
        // Handle the error.
        dautohandle();
        printd(LEVEL_ERROR, "linker: error occurred.\n");

        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Check to make sure target is correct.
    if (target_arg->count == 0)
        target = IMAGE_APPLICATION;
    else
    {
        if (strcmp(target_arg->sval[0], "static") == 0)
            target = IMAGE_STATIC_LIBRARY;
        else if (strcmp(target_arg->sval[0], "kernel") == 0)
            target = IMAGE_KERNEL;
        else
            target = IMAGE_APPLICATION;
    }
    
    // If the jumplist parameter is being used, we must be linking
    // in kernel mode.
    if (jumplist_file->count > 0 && target != IMAGE_KERNEL)
        dhalt(ERR_JUMPLIST_WITHOUT_KERNEL, NULL);
    else if (target == IMAGE_KERNEL && policy_file->count > 0)
        dhalt(ERR_POLICY_WITH_KERNEL, NULL);

    // If this is an application, we need to handle linker policies.
    if (target == IMAGE_APPLICATION)
    {
        bstring find = NULL;
        bstring repl = NULL;
        
        // Calculate what policies to use.
        if (kernel_name->count == 0)
            policy_kernel = osutil_getkerneldefault();
        else
            policy_kernel = kernel_name->sval[0];
        
        // If we are using the "none" kernel, we use no kernel
        // at all.
        policy_use_kernel = strcmp(policy_kernel, "none") != 0;
        
        if (policy_use_kernel)
        {
            if (policy_file->count > 0)
            {
                // Override the policy file that is being used.
                policy_path = bfromcstr(policy_file->filename[0]);
            }
            else
            {
                // Determine the path to the policy file.
                policy_path = osutil_getkernelpath();
                if (policy_path == NULL)
                    dhalt(ERR_POLICY_PATH_UNKNOWN, NULL);
                bconchar(policy_path, '/');
                bcatcstr(policy_path, policy_kernel);
                bcatcstr(policy_path, "/policy");
            }
            
            // Read in the policy file.
            policy_file_io = fopen(policy_path->data, "r");
            if (policy_file_io == NULL)
                dhalt(ERR_POLICY_FILE_NOT_FOUND, policy_path->data);
            loaded_policies = policies_load(policy_file_io);
            fclose(policy_file_io);

            // Read the settings.
            policy_kernel_path = policies_get_setting(loaded_policies, bautofree(bfromcstr("kernel")));
            policy_jumplist_path = policies_get_setting(loaded_policies, bautofree(bfromcstr("jumplist")));
            policy_target = policies_get_setting(loaded_policies, bautofree(bfromcstr("target")));
            policy_direct_target = policies_get_setting(loaded_policies, bautofree(bfromcstr("direct-target")));
            policy_direct_default = policies_get_setting(loaded_policies, bautofree(bfromcstr("direct-default")));
            
            // Set the defaults if a setting isn't specified.
            if (policy_target == NULL)
                policy_target = &default_target;
            if (target_arg->count > 0)
                policy_target = bfromcstr(target_arg->sval[0]);
            if (policy_direct_default == NULL)
                policy_direct = false;
            else
                policy_direct = biseqcstrcaseless(policy_direct_default, "true");
            if (direct_link->count > 0)
                policy_direct = true;
            else if (no_direct_link->count > 0)
                policy_direct = false;
            if (policy_direct && target_arg->count == 0 && policy_direct_target != NULL)
                policy_target = policy_direct_target;
            
            // Replace @KERNEL_FOLDER@ with the kernel's folder.
            find = bfromcstr("@KERNEL_FOLDER@");
            repl = osutil_getkernelpath();
            bconchar(repl, '/');
            bcatcstr(repl, policy_kernel);
            bfindreplace(policy_kernel_path, find, repl, 0);
            bfindreplace(policy_jumplist_path, find, repl, 0);
            bdestroy(find);
            bdestroy(repl);
            find = NULL;
            repl = NULL;
        }
    }

    // Load all passed objects and use linker bin system to
    // produce result.
    bins_init();
    if (policy_use_kernel && target != IMAGE_KERNEL)
    {
        // Import the jumplist.
        if (policy_direct && policy_jumplist_path != NULL && policy_kernel_path != NULL)
            bins_load_kernel(bautocpy(policy_jumplist_path), bautocpy(policy_kernel_path));
        else if (policy_jumplist_path != NULL)
            bins_load_jumplist(bautocpy(policy_jumplist_path));
    }
    for (i = 0; i < input_files->count; i++)
        if (!bins_load(bautofree(bfromcstr(input_files->filename[i])), symbol_file->count > 0, (symbol_file->count > 0 && symbol_ext->count > 0) ? symbol_ext->sval[0] : "dsym16"))
            // Failed to load one of the input files.
            dhalt(ERR_BIN_LOAD_FAILED, input_files->filename[i]);
    bins_associate();
    bins_sectionize();
    bins_flatten(bautofree(bfromcstr("output")));
    if (target == IMAGE_KERNEL)
        bins_write_jump();
    saved = bins_optimize(
                opt_mode->count == 0 ? OPTIMIZE_SIZE : OPTIMIZE_SPEED,
                opt_level->count == 0 ? OPTIMIZE_NONE : opt_level->ival[0]);
    if (no_short_literals_arg->count == 0 && target != IMAGE_STATIC_LIBRARY)
        saved += bins_compress();
    else if (no_short_literals_arg->count == 0)
        dwarn(WARN_SKIPPING_SHORT_LITERALS_TYPE, NULL);
    else
        dwarn(WARN_SKIPPING_SHORT_LITERALS_REQUEST, NULL);
    if (target == IMAGE_APPLICATION && policy_use_kernel)
        bins_resolve_kernel(loaded_policies);
    bins_resolve(
        target == IMAGE_STATIC_LIBRARY,
        target == IMAGE_STATIC_LIBRARY,
        target == IMAGE_STATIC_LIBRARY || target == IMAGE_KERNEL);
    if (target == IMAGE_APPLICATION && policy_use_kernel)
    {
        bins_apply_policy(
            bautofree(policy_path),
            bautocpy(policy_target),
            bautofree(bfromcstr("output")),
            bautofree(bfromcstr("output_applied"))
            );
    }
    bins_save(
        (target == IMAGE_APPLICATION && policy_use_kernel) ? bautofree(bfromcstr("output_applied")) : bautofree(bfromcstr("output")),
        bautofree(bfromcstr(output_file->filename[0])),
        target,
        keep_output_arg->count > 0,
        symbol_file->count > 0 ? symbol_file->filename[0] : NULL,
        (jumplist_file->count > 0 && target == IMAGE_KERNEL) ? jumplist_file->filename[0] : NULL);
    bins_free();
    if (saved > 0)
        printd(LEVEL_DEFAULT, "linker: saved %i words during optimization.\n", saved);
    else if (saved < 0)
        printd(LEVEL_DEFAULT, "linker: increased by %i words during optimization.\n", -saved);
    if (loaded_policies != NULL)
        policies_free(loaded_policies);

    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 0;
}

