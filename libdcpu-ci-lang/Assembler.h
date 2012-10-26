/**

    File:       Assembler.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang

    Authors:    James Rhodes

    Description:    Declares the Assembler class.

**/

#ifndef __DCPU_LIBDCC_ASSEMBLER_H
#define __DCPU_LIBDCC_ASSEMBLER_H

#include <string>
#include <map>

class Assembler
{
public:
    // Loads all of the assembler definitions, if they
    // are not already loaded.
    static void loadAll();

    // Returns whether the specified assembler exists.
    static bool hasAssembler(std::string name);

    // Gets a reference to an assembler by it's name.
    static const Assembler* getAssembler(std::string name);

    // Whether the assembler supports the .BOUNDARY directive.
    bool supportsSafetyBoundary;

    // Whether the assembler supports .SECTION and .OUTPUT directives.
    bool supportsSections;

    // Whether the assembler supports the DAT instruction.
    bool supportsDataInstruction;

    // Whether the assembler supports next-value addresses of
    // the order [REG+ADDR] (in addition to [ADDR+REG])
    bool supportsReorderedAddresses;

    // Whether the assembler supports reading characters as
    // memory addresses using the 'a' notation.
    bool supportsDirectCharacters;

    // Whether the assembler supports intermediate output and
    // the .IMPORT directive.
    bool supportsLinkedImportDirective;

    // Whether the assembler supports intermediate output and
    // the .EXPORT directive.
    bool supportsLinkedExportDirective;

private:
    // Things to make the assembler list work.  If you're adding
    // a new assembler definition, look inside "asmtypes.cpp".
    typedef std::pair<std::string, const Assembler*> entry;
    static bool m_HasLoaded;
    static std::map<std::string, const Assembler*> m_Assemblers;
    Assembler(
        bool supportsSafetyBoundary,
        bool supportsSections,
        bool supportsDataInstruction,
        bool supportsReorderedAddresses,
        bool supportsDirectCharacters,
        bool supportsLinkedImportDirective,
        bool supportsLinkedExportDirective
    ) : supportsSafetyBoundary(supportsSafetyBoundary),
        supportsSections(supportsSections),
        supportsDataInstruction(supportsDataInstruction),
        supportsReorderedAddresses(supportsReorderedAddresses),
        supportsDirectCharacters(supportsDirectCharacters),
        supportsLinkedImportDirective(supportsLinkedImportDirective),
        supportsLinkedExportDirective(supportsLinkedExportDirective) { };
};

#endif
