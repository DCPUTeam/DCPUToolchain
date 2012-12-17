///
/// @addtogroup LibDCPU-VM
/// @brief VM library.
///
/// This library holds and defines all of the functionality needed to run a
/// DCPU vm.
///
/// @{
///
/// @file
/// @brief Main VM functions and declarations.
/// @author James Rhodes
///
/// It includes dcpu.h and defines virtual machine definitions.
///

#ifndef __VM_H
#define __VM_H

#include "dcpu.h"

///
/// @brief Creates a new OpenGL host context for usage with hardware.
///
/// @param title The title of the hardware being created.
/// @param width The width of the window required for the hardware.
/// @param height The height of the window required for the hardware.
/// @param resizable Whether the window should be resizable.
/// @param ud The userdata to associate with this context.
/// @return The new OpenGL host context.
///
typedef void* (*create_context_t)(const char* title, int width, int height, bool resizable, void* ud);

///
/// @brief Destroys an OpenGL host context.
///
/// @param context The OpenGL host context to destroy.
///
typedef void (*destroy_context_t)(void* context);

///
/// @brief Activates an OpenGL host context.
///
/// @param context The OpenGL host context to activate.
///
typedef void (*activate_context_t)(void* context);

///
/// @brief Swap the buffers of an OpenGL host context.
///
/// @param context The OpenGL host context to buffer swap.
///
typedef void (*swap_buffers_t)(void* context);

///
/// @brief Retrieve the userdata associated with an OpenGL host context.
///
/// @param context The OpenGL host context to retrieve from.
/// @return The associated userdata.
///
typedef void* (*get_ud_t)(void* ud);

///
/// @brief Represents the hosting context of a virtual machine.
///
/// This is used to provide the graphical hardware of the DCPU-16
/// a rendering API that is independent of the program hosting
/// the DCPU-16 execution.  By providing a host context (which is
/// required for all virtual machines), the application can render
/// the graphical hardware onto any OpenGL surface it wants.
///
typedef struct
{
    ///
    /// @brief Requests a new OpenGL context for usage with hardware.
    /// 
    create_context_t create_context;
    
    ///
    /// @brief Requests an OpenGL context be destroyed or freed.
    ///
    /// The application does not have to actually close or hide any
    /// rendering surface that may be reused for a future context
    /// creation.
    ///
    destroy_context_t destroy_context;

    ///
    /// @brief Requests that an OpenGL context become the active context.
    ///
    activate_context_t activate_context;

    ///
    /// @brief Requests that the specified OpenGL surface has it's buffers
    ///        swapped in the case of surfaces that are using double buffering.
    ///
    swap_buffers_t swap_buffers;

    ///
    /// @brief Requests the userdata associated with a surface during the
    ///        initial context creation call.
    ///
    get_ud_t get_ud;
} host_context_t;

// Pre declared.
typedef struct vm vm_t;

/// Defines the maximum number of interrupts permitted on the DCPU.
#define INTERRUPT_MAX    255

/// The frequency the DCPU-16 should run at.
#define DCPU_TICKS_KHZ          100
/// The number of milliseconds in a second.
#define DCPU_NUM_TIMING_TICKS   1000
/// The number of milliseconds to consume per cycle.
#define DCPU_MICS_PER_CYCLE     (1000/DCPU_TICKS_KHZ)

/// The number of maximum amout of hardwear units that can be attached to a VM.
#define VM_HW_MAX 32 /*0x1000*/

///
/// @brief The function signature of an internal hardware interrupt.
///
typedef void (*hw_interrupt)(vm_t* vm, void* ud);

///
/// @brief The function signature for freeing resources used by hardware.
///
typedef void (*hw_free_handler)(void* ud);

///
/// @brief Represents a DCPU-16 hardware component.
///
typedef struct
{
    uint32_t id;                ///< The hardware identifier.
    uint16_t version;           ///< The hardware version.
    uint32_t manufacturer;      ///< The hardware manufacturer.
    hw_interrupt handler;       ///< The function which handles interrupts sent to this hardware component.
    hw_free_handler free_handler; ///< The function which handles free'ing any data this hardware component uses.
    void* userdata;             ///< Userdata associated with the hardware.
} hw_t;

/// The number of maximum hooks that the vm can have installed.
#define VM_HOOK_MAX 50

///
/// @brief Called when the registerd hook has been fired.
///
/// @param pos On write hook the memory location written.
/// @param ud The given userdata when the hook was registered.
/// 
typedef void (*vm_hook)(vm_t*, uint16_t pos, void* ud);

///
/// @brief Represents a DCPU-16 virtual machine.
///
/// One of the core structures of the toolchain.  Represents a DCPU-16
/// virtual machine upon which you can perform operations using the library.
///
struct vm
{
    uint16_t registers[8];      ///< The 8 virtual machine registers: A, B, C, X, Y, Z, I, J.
    uint16_t pc;                ///< The virtual machine's current instruction position.
    uint16_t sp;                ///< The virtual machine's current stack position.
    uint16_t ia;                ///< The current interrupt handler location.
    uint16_t ex;                ///< The overflow or excess data register.
    uint16_t ram[0x10000];      ///< The virtual machine's main RAM segment.
    uint16_t dummy;             ///< A dummy position that is used internally to silently redirect assignments.
    uint16_t irq[256];          ///< The interrupt queue.
    uint16_t irq_count;         ///< The number of interrupts currently in the queue.
    uint16_t sleep_cycles;      ///< An internal counter used to measure how many additional cycles the VM should sleep for.
    uint8_t queue_interrupts;   ///< Whether the interrupt queue is enabled.
    uint8_t halted;             ///< Whether the virtual machine is currently halted.
    uint8_t exit;               ///< Whether the virtual machine is quitting.
    uint8_t skip;               ///< Whether the virtual machine will skip the next instruction.
    uint8_t debug;              ///< Whether the virtual machine will output each instruction to standard output.

    bool can_fire;              ///< Will the DCPU catch fire on an invalid operation?
    bool on_fire;               ///< Is the DCPU on fire?
    int fire_cycles;
    int fire_cycles_target;
    int radiation_factor;

    bool radiation;
    int radiation_cycles;
    int radiation_cycles_target;

    vm_hook hook_list[VM_HOOK_MAX];
    uint16_t hook_mode[VM_HOOK_MAX];
    void* hook_userdata[VM_HOOK_MAX];

    hw_t* hw_list[VM_HW_MAX];

    FILE* dump;                 ///< An open file descriptor where instruction execution information should be sent, or NULL.

    host_context_t* host;       ///< The hardware emulation host.
};

vm_t* vm_create();
void vm_init(vm_t* vm);
void vm_reset(vm_t* vm, bool reset_memory);
void vm_flash(vm_t* vm, uint16_t memory[0x10000]);
void vm_free(vm_t* vm);
void vm_halt(vm_t* vm, const char* message, ...);
void vm_interrupt(vm_t* vm, uint16_t msgid);
uint16_t vm_consume_word(vm_t* vm);
uint16_t vm_resolve_value(vm_t* vm, uint16_t val, uint8_t pos);
void vm_cycle(vm_t* vm);

#endif
