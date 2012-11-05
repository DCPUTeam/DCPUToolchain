///
/// @addtogroup LibDCPU-LD
/// @{
///
/// @file
/// @brief The implementation for the "stack-call" calling convention.
/// @author James Rhodes
///

#include <stdlib.h>
#include <bstring.h>
#include <simclist.h>
#include <derr.h>
#include <policy.h>
#include "ldbin.h"
#include "ldconv.h"
#include "ccstackcall.h"

void generate_ccstackcall(policies_t* policies, list_t* all, struct ldbin* bin, struct lconv_entry* call)
{
    struct ldbin* temp = NULL;
    bstring policy_interrupt_call_name = NULL;
    bstring policy_interrupt_size_name = NULL;
    bstring policy_interrupt_call_text = NULL;
    bstring policy_interrupt_size_text = NULL;
    uint16_t policy_interrupt_call_result = 0x0000;
    uint16_t policy_interrupt_size_result = 0x0000;
    uint16_t i = 0;
    
    // Determine the name of the policy settings for this .CALL
    policy_interrupt_call_name = bfromcstr("interrupt-call(");
    policy_interrupt_size_name = bfromcstr("interrupt-size(");
    bconcat(policy_interrupt_call_name, call->label);
    bconcat(policy_interrupt_size_name, call->label);
    bconchar(policy_interrupt_call_name, ')');
    bconchar(policy_interrupt_size_name, ')');
    
    // Find the interrupt call index and size in the policy.
    policy_interrupt_call_text = policies_get_setting(policies, bautofree(policy_interrupt_call_name));
    policy_interrupt_size_text = policies_get_setting(policies, bautofree(policy_interrupt_size_name));
    
    // If either of these settings don't exist, we can't generate the call.
    if (policy_interrupt_call_text == NULL || policy_interrupt_size_text == NULL)
        dhalt(ERR_CALL_MISSING_INTERRUPT_PARAMETERS, call->label->data);
    
    // Determine numeric values for the settings.
    if (policy_interrupt_call_text->data[1] == 'x')
        policy_interrupt_call_result = strtoul(policy_interrupt_call_text->data, NULL, 16);
    else
        policy_interrupt_call_result = strtoul(policy_interrupt_call_text->data, NULL, 10);
    if (policy_interrupt_size_text->data[1] == 'x')
        policy_interrupt_size_result = strtoul(policy_interrupt_size_text->data, NULL, 16);
    else
        policy_interrupt_size_result = strtoul(policy_interrupt_size_text->data, NULL, 10);
    
    // Create a new bin for the instructions.
    temp = bin_create(bautofree(bfromcstr("<temp>")), true);
    
    // Push the arguments for the call.
    if (policy_interrupt_size_result == 0)
        bin_write(temp, INSTRUCTION_CREATE(OP_SET, PUSH_POP, 0x20 /* 0 literal */));
    if (policy_interrupt_size_result >= 1)
        bin_write(temp, INSTRUCTION_CREATE(OP_SET, PUSH_POP, REG_A));
    if (policy_interrupt_size_result >= 2)
        bin_write(temp, INSTRUCTION_CREATE(OP_SET, PUSH_POP, REG_B));
    if (policy_interrupt_size_result >= 3)
        bin_write(temp, INSTRUCTION_CREATE(OP_SET, PUSH_POP, REG_C));
    for (i = 4; i <= policy_interrupt_size_result; i++)
    {
        bin_write(temp, INSTRUCTION_CREATE(OP_SET, PUSH_POP, PICK));
        bin_write(temp, 0x10000 - policy_interrupt_size_result);
    }
    
    // Write out the call.
    bin_write(temp, INSTRUCTION_CREATE(OP_NONBASIC, NBOP_INT, NXT_LIT));
    bin_write(temp, policy_interrupt_call_result);
    
    // Store the result in the A register and pop the arguments that were used.
    bin_write(temp, INSTRUCTION_CREATE(OP_SET, REG_A, PEEK));
    bin_write(temp, INSTRUCTION_CREATE(OP_ADD, SP, NXT_LIT));
    bin_write(temp, policy_interrupt_size_result + (policy_interrupt_size_result - 3));
    
    // Copy the final words to the target bin.
    bin_move(all, bin, temp, call->address, 0, list_size(&temp->words));
    
    // Free data.
    bin_destroy(temp);
}

///
/// @}
///