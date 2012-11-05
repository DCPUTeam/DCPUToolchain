///
/// @addtogroup LibDCPU
/// @brief General toolchain library.
///
/// This library holds and defines all of the functionality commonly used
/// through-out the toolchain and it's libraries.  It is essentially the core
/// support library which everything else rests upon.
///
/// This includes functionality such as better string support, buffered file
/// access, DCPU-16 definitions, cross-platform APIs and more.
///
/// @{
///
/// @file
/// @brief General functions and declarations used through-out the toolchain.
/// @author James Rhodes
/// 
/// This unified header provides support for functionality required in almost
/// every part of the toolchain.
///
/// It defines boolean types and virtual machine definitions.
///

#ifndef __DCPU_H
#define __DCPU_H

#include <stdint.h>
#include <stdio.h>

#ifndef __cplusplus
#ifndef __GNUC__
/// Defines a type for representing boolean values.
typedef uint8_t bool;
#ifndef true
/// Represents true.
#define true 1
#endif
#ifndef false
/// Represents false.
#define false 0
#endif
#else
#include <stdbool.h>
#endif
#endif

///
/// @addtogroup ValueDefinitions Value Definitions
/// @ingroup LibDCPU
/// @brief A list of macro definitions that provide the values for accessing DCPU-16 memory and registers.
/// @sa http://dcpu.com/dcpu-16/
/// @{
///

/// The A register of the DCPU.
#define REG_A       0x00
/// The B register of the DCPU.
#define REG_B       0x01
/// The C register of the DCPU.
#define REG_C       0x02
/// The X register of the DCPU.
#define REG_X       0x03
/// The Y register of the DCPU.
#define REG_Y       0x04
/// The Z register of the DCPU.
#define REG_Z       0x05
/// The I register of the DCPU.
#define REG_I       0x06
/// The J register of the DCPU.
#define REG_J       0x07

/// The value of the address in the A register.
#define VAL_A       0x08
/// The value of the address in the B register.
#define VAL_B       0x09
/// The value of the address in the C register.
#define VAL_C       0x0A
/// The value of the address in the X register.
#define VAL_X       0x0B
/// The value of the address in the Y register.
#define VAL_Y       0x0C
/// The value of the address in the Z register.
#define VAL_Z       0x0D
/// The value of the address in the I register.
#define VAL_I       0x0E
/// The value of the address in the J register.
#define VAL_J       0x0F

/// The value of the address in the A register, plus the value of the next word.
#define NXT_VAL_A   0x10
/// The value of the address in the B register, plus the value of the next word.
#define NXT_VAL_B   0x11
/// The value of the address in the C register, plus the value of the next word.
#define NXT_VAL_C   0x12
/// The value of the address in the X register, plus the value of the next word.
#define NXT_VAL_X   0x13
/// The value of the address in the Y register, plus the value of the next word.
#define NXT_VAL_Y   0x14
/// The value of the address in the Z register, plus the value of the next word.
#define NXT_VAL_Z   0x15
/// The value of the address in the I register, plus the value of the next word.
#define NXT_VAL_I   0x16
/// The value of the address in the J register, plus the value of the next word.
#define NXT_VAL_J   0x17

/// Represents either PUSH or POP, depending on which parameter it is used in.
#define PUSH_POP    0x18
/// Represents PEEK, which is the value of the address in the SP register.
#define PEEK        0x19
/// Represents PICK, which is the value of the address pointed to by SP plus the next word.
#define PICK        0x1A
/// The SP register of the DCPU, which points to the top of the stack.
#define SP          0x1B
/// The PC register of the DCPU, which points to the current instruction.
#define PC          0x1C
/// The EX register of the DCPU, which holds overflow and underflow information.
#define EX          0x1D
/// The value stored in the address indicated in the next word.
#define NXT         0x1E
/// The value of the next word.
#define NXT_LIT     0x1F

/// The interrupt address (can not be used directly in assembly).
#define IA          0xFF

/// Indicates no position in the call.
#define POS__      0x0
/// Indicates the value is in the 'a' parameter.
#define POS_A      0x1
/// Indicates the value is in the 'b' parameter.
#define POS_B      0x2

///
/// @}
///
/// @addtogroup OpcodeDefinitions Opcode Definitions
/// @ingroup LibDCPU
/// @brief A list of opcode definitions for the DCPU.
/// @sa http://dcpu.com/dcpu-16/
/// @{
///

/// Indicates a non-basic opcode.
#define OP_NONBASIC   0x00
/// The SET instruction, which assigns the second value to the first.
#define OP_SET        0x01
/// The ADD instruction, which adds the second value onto the first.
#define OP_ADD        0x02
/// The SUB instruction, which subtracts the second value from the first.
#define OP_SUB        0x03
/// The MUL instruction, which multiplies the second value by the first and assigns the result to the first.
#define OP_MUL        0x04
/// The MLI instruction, which is a signed MUL instruction.
#define OP_MLI        0x05
/// The DIV instruction, which divides the first value by the second and assigns the result to the first.
#define OP_DIV        0x06
/// The DVI instruction, which is a signed DIV instruction.
#define OP_DVI        0x07
/// The MOD instruction, which takes the modulo of the first value over the second, and assigns the result to the first.
#define OP_MOD        0x08
/// The MDI instruction, which is a signed MOD instruction.
#define OP_MDI        0x09
/// The AND instruction, which assigns the bitwise AND of the first and second values to the first value.
#define OP_AND        0x0A
/// The BOR instruction, which assigns the bitwise OR of the first and second values to the first value.
#define OP_BOR        0x0B
/// The XOR instruction, which assigns the bitwise XOR of the first and second values to the first value.
#define OP_XOR        0x0C
/// The SHR instruction, which is a shift-right operation of the first and second values.
#define OP_SHR        0x0D
/// The ASR instruction, which is a arithmetic shift-right operation of the first and second values.
#define OP_ASR        0x0E
/// The SHL instruction, which is a shift-left operation of the first and second values.
#define OP_SHL        0x0F
/// The IFB instruction, which performs the next instruction if (b&a)!=0.
#define OP_IFB        0x10
/// The IFC instruction, which performs the next instruction if (b&a)==0.
#define OP_IFC        0x11
/// The IFE instruction, which performs the next instruction if b==a.
#define OP_IFE        0x12
/// The IFN instruction, which performs the next instruction if b!=a.
#define OP_IFN        0x13
/// The IFG instruction, which performs the next instruction if b>a.
#define OP_IFG        0x14
/// The IFA instruction, which performs the next instruction if b>a (signed).
#define OP_IFA        0x15
/// The IFL instruction, which performs the next instruction if b<a.
#define OP_IFL        0x16
/// The IFU instruction, which performs the next instruction if b<a (signed).
#define OP_IFU        0x17
/// The ADX instruction, see the DCPU-16 specification for semantics.
#define OP_ADX        0x1A
/// The SBX instruction, see the DCPU-16 specification for semantics.
#define OP_SBX        0x1B
/// The STI instruction, which sets the second value to the first and increases the I and J registers.
#define OP_STI        0x1E
/// The STD instruction, which sets the second value to the first and decreases the I and J registers.
#define OP_STD        0x1F

/// The reserved non-basic instruction for future expansion.
#define NBOP_RESERVED 0x00
/// The JSR instruction, which pushes the next instruction's address onto the stack and jumps to the address in the first value.
#define NBOP_JSR      0x01
/// The INT instruction, which sends a software interrupt with the message specified by the first value.
#define NBOP_INT      0x08
/// The IAG instruction, which gets the address to the interrupt handler and stores it in the A register.
#define NBOP_IAG      0x09
/// The IAS instruction, which sets the address of the interrupt handler to the first value.
#define NBOP_IAS      0x0A
/// The RFI instruction, which should be used to return from an interrupt handler.  See the DCPU-16 specification for semantics.
#define NBOP_RFI      0x0B
/// The IAQ instruction, which turns interrupt queuing on or off depending on the first value.
#define NBOP_IAQ      0x0C
/// The HWN instruction, which places the total number of hardware into the first value.
#define NBOP_HWN      0x10
/// The HWQ instruction, which queries hardware.  See the DCPU-16 specification for semantics.
#define NBOP_HWQ      0x11
/// The HWI instruction, which sends a hardware interrupt to the hardware specified by the first value.
#define NBOP_HWI      0x12

///
/// @}
///

/// The value for the EX register that indicates no overflow.
#define AR_NOFLOW     0x0000
/// The value for the EX register that indicates an overflow occurred.
#define AR_OVERFLOW   0x0001
/// The value for the EX register that indicates an underflow occurred.
#define AR_UNDERFLOW  0xFFFF
/// The maximu unsigned integer value that can be stored.
#define AR_MAX        0xFFFF

/// Combines the parameters into the 16-bit unsigned integer for the instruction.
#define INSTRUCTION_CREATE(op, b, a) (((a & 0x3f) << 10) + ((b & 0x1f) << 5) + (op & 0x1f))
/// Retrieves the 'a' parameter from an instruction.
#define INSTRUCTION_GET_A(inst) ((inst & 0xfc00) >> 10)
/// Retrieves the 'b' parameter from an instruction.
#define INSTRUCTION_GET_B(inst) ((inst & 0x3e0) >> 5)
/// Retrieves the basic opcode from an instruction.
#define INSTRUCTION_GET_OP(inst) (inst & 0x1f)

/// Defines the maximum number of interrupts permitted on the DCPU.
#define INTERRUPT_MAX    255

/// The frequency the DCPU-16 should run at.
#define DCPU_TICKS_KHZ          100
/// The number of milliseconds in a second.
#define DCPU_NUM_TIMING_TICKS   1000
/// The number of milliseconds to consume per cycle.
#define DCPU_MICS_PER_CYCLE     (1000/DCPU_TICKS_KHZ)

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

///
/// @brief Represents a DCPU-16 virtual machine.
///
/// One of the core structures of the toolchain.  Represents a DCPU-16
/// virtual machine upon which you can perform operations using the library.
///
typedef struct
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
    FILE* dump;                 ///< An open file descriptor where instruction execution information should be sent, or NULL.

    host_context_t* host;       ///< The hardware emulation host.
} vm_t;

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

vm_t* vm_create();
void vm_init(vm_t* vm, bool init_memory);
void vm_flash(vm_t* vm, uint16_t memory[0x10000]);
void vm_execute(vm_t* vm, const char* execution_dump);
void vm_free(vm_t* vm);

#endif

///
/// @}
///
