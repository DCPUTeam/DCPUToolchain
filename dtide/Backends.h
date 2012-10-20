#ifndef BACKENDS_H
#define BACKENDS_H

#include <list>
#include <string>
#include <stdint.h>

enum CodeSyntax
{
    DCPUAssembly,
    C,
    Brainfuck
};

enum ConfigurationType
{
    Integer,
    Double,
    String,
    Directory,
    File,
    OptionList
};

union ConfigurationValue
{
    int Integer;
    double Double;
    std::string* String;
    std::string* Directory; // Like string except verified by IDE.
    std::string* File; // Like string except verified by IDE.
    std::list<std::string>* OptionList; // First option is selected one?
};

class ConfigurationOption
{
public:
    std::string Grouping;
    std::string Name;
    ConfigurationType Type;
    ConfigurationValue Value;
};

class BuildAPI
{
private:
    // Maintain state here to determine what file is currently
    // building.	You could use multiple BuildAPIs to do parallel
    // builds.
public:
    // Implemented by IDE.
    virtual void AddError(std::string message, std::string file, int line) = 0;
    virtual void AddWarning(std::string message, std::string file, int line) = 0;

    // langtarget is the language to handle the output file, regardless
    // of extension.  This way, dtcc can produce an assembly file, which
    // produces an object file, which can be finally linked (the object file
    // might simply be known as the "Object" language or w/e).  When the
    // result of a build is an output file and there are no more intermediate
    // files to process, then the build is done (success is determined by
    // whether there were errors).
    virtual void AddIntermediateFile(std::string path, std::string langtarget) = 0;
    virtual void AddOutputFile(std::string path) = 0;

    // Called when build is complete; this essentially means that the tool
    // is done.  Note that toolchains should only call AddIntermediateFile
    // when the file is fully generated as this can be used again to speed up
    // parallel builds.
    virtual void End() = 0;
};

class Language
{
public:
    virtual std::string GetName() = 0;
    virtual std::string GetDescription() = 0;
    // File extensions (can be empty for languages only produced by build outputs
    // such as object files).
    virtual std::list<std::string> GetExtensions() = 0;
    virtual void Build(std::string filename, std::string outputDir, BuildAPI& api) = 0;

    virtual std::string GetDefaultFileName() = 0;
    virtual CodeSyntax GetCodeSyntax() = 0;
};

class Device
{
public:
    // Used for custom devices since you get a reference to a single
    // instance of the device from GetCustomDevices.  This lets you clone
    // a device to get another one without the IDE knowing the type of device.
    Device& Clone();
    std::string GetName();
    std::string GetDescription();
    std::string GetDocumentationURL();
    std::string GetAuthors();
  
    // Configuration interface (e.g. setting filename for disk device...)
    std::list<ConfigurationOption> GetOptions();
    ConfigurationOption GetOption();
    void SetOption(std::string Name, ConfigurationType Type, ConfigurationValue Value);

    // Debugging interface.
    bool Has(int devid, std::string name); // Whether there is any data for this name.
    //template <class T>
    //T Get<T>(int devid, std::string name, ...)  // Each device has a known set of properties (like the screen
					                            // might expose the colour and and character in each frame with
					                            // different names).  Varargs are used to pass in information.
    void Hide(int devid); // Indicates the emulator / debugger should not show any visible reference of this device
		    // as the IDE wants to draw it itself (for LEM1802 / SPED3 screens).
    void Show(int devid); // Indicates the emulator should show this device again.
};

class DebuggingSession
{
public:
    // Implemented by IDE.
    void Stop(); // Execution has finished.
    void Break(int id, std::string filename, int line); // Break at a certain point (id used for more functions).
    void AddError(int id, std::string message); // Show error at breakpoint.
    void AddWarning(int id, std::string message); // Show warning at breakpoint.

    // Views.
    void SetMemory(uint16_t memory[0x10000]); // Sets memory for display in IDE.
    void PushCall(std::string call, std::string language, std::string file, int line); // Pushes a call onto the call stack for the call stack pane.
    void PopCall(std::string language); // Pop a call from the call stack.

    // Devices.
    void SetActiveDevice(int devid, Device& device); // Provides the IDE with a reference to an active device, such as
						                             // any LEM1802 screens, etc. which the IDE can then draw inline.
						                             // Device IDs are used to differentiate between two devices of the 
						                             // same type (e.g. two LEM1802 screens).
};

enum ModuleType
{
    Preprocessor,
    Debugger,
    Optimizer,
};

// Device modules are excluded from Module; they use Device instead.
class Module
{
public:
    std::string GetName();
    std::string GetDescription();
    std::string GetDocumentationURL();
    std::list<std::string> GetAuthors();
    ModuleType GetType();
};

class PreprocessorModule : Module
{
public:
    std::list<std::string> GetDirectives();
    std::list<ConfigurationType> GetDirectiveParameters();
    std::string GetDirectiveDescription();
    std::list<std::string> GetFutureRequiredDirective(); // (used to specify "ENDUNIT" to "UNIT" for syntax checking)
};

class DebuggerModule : Module
{
public:
    std::list<std::string> GetCommands();
    std::string GetCommandDescription();
    // Commands are actually run using the SendCommand API in the Toolchain class.
};

class OptimizerModule : Module
{
public:
    // Not sure what is available here.  Maybe some functions on
    // how much data was saved or what was changed, but that might require
    // interacting with the build API or something.
};

class Toolchain
{
public:
    // Basic informational stuff.
    virtual std::string GetName() = 0;
    virtual std::string GetDescription() = 0;
    virtual std::list<std::string> GetAuthors() = 0;
    virtual std::string GetLicense() = 0;
    virtual void Cycle() = 0;

    // Configuration interface.
    //virtual std::list<ConfigurationOption> GetOptions() = 0;
    //virtual ConfigurationOption GetOption() = 0;
    //virtual void SetOption(std::string Name, ConfigurationType Type, ConfigurationValue Value) = 0;
  
    // Language interface.
    virtual std::list<Language*> GetLanguages() = 0;
    Language* GetLanguageByExtension(std::string ext);
  
    // Debugging / execution interface.
    virtual void Start(std::string path, DebuggingSession& session) = 0;
    //virtual void Pause(DebuggingSession& session) = 0;
    //virtual void Continue(DebuggingSession& session) = 0;
    virtual void Stop(DebuggingSession& session) = 0;
    //virtual void AddBreakpoint(DebuggingSession& session, std::string path, int line) = 0;
    //virtual void AddBreakpoint(DebuggingSession& session, uint16_t memory) = 0;
    //virtual void AttachDevice(DebuggingSession& session, Device& device) = 0;
    //virtual void DetachDevice(DebuggingSession& session, Device& device) = 0;
    //virtual void SendCommand(DebuggingSession& session, std::string cmd) = 0; // send custom command (only to be used on user input).
    // TODO: Some kind of API for live memory update.

    // Modules and extensions.
    //virtual std::list<Module&> GetModules() = 0;
    //virtual std::list<Device&> GetCustomDevices() = 0;
};

#include "toolchains/toolchains.h"

#endif
