///
/// @addtogroup LibDCPU-LD
/// @{
///
/// @file
/// @brief Implements linker calling convention generation.
/// @author James Rhodes
///

#include <derr.h>
#include <dcpu.h>
#include "ldbin.h"
#include "ldcall.h"
#include "ccstackcall.h"
#include "ccregistercall.h"
#include "ccregisterdirect.h"

///
/// @brief Generates a standard jump call into a jump table.
///
static void generate_jump(list_t* all, struct ldbin* bin, struct lconv_entry* call)
{
    struct lconv_entry* jump = NULL;
    struct ldbin* temp = NULL;
    uint16_t inst = 0x0000;
    uint16_t target = 0x0000;
    
    // Determine the position of the jump entry.
    if (bin->jump == NULL)
        dhalt(ERR_LABEL_NOT_FOUND, call->label->data);    
    list_iterator_start(bin->jump);
    while (list_iterator_hasnext(bin->jump))
    {
        jump = list_iterator_next(bin->jump);
        
        // Compare labels.
        if (biseq(jump->label, call->label))
        {
            // Found the target address.
            target = jump->address;
            break;
        }
    }
    list_iterator_stop(bin->jump);
    
    // Create the instruction.
    inst = INSTRUCTION_CREATE(OP_NONBASIC, NBOP_JSR, NXT);
    
    // Create a new bin for the instructions.
    temp = bin_create(bautofree(bfromcstr("<temp>")), true);
    bin_write(temp, inst);
    bin_write(temp, target);
    bin_move(all, bin, temp, call->address, 0, 2);
    bin_destroy(temp);
}

///
/// @brief Generates code to make the specified kernel call.
///
/// @param policies The current policies.
/// @param all A list of all of the bins (&ldbins.bins).
/// @param bin The bin that contains the .CALL directive.
/// @param call The call entry.
///
void ldcall_generate(policies_t* policies, list_t* all, struct ldbin* bin, struct lconv_entry* call)
{
    bstring policy_jumplist = NULL;
    bstring policy_interrupt_call = NULL;
    
    // If the policies do not have a settings section, throw an error.
    if (policies->settings == NULL)
        dhalt(ERR_CALL_NO_SETTINGS, NULL);
    
    // Determine what convention we have.
    policy_jumplist = policies_get_setting(policies, bautofree(bfromcstr("jumplist")));
    policy_interrupt_call = policies_get_setting(policies, bautofree(bfromcstr("interrupt-call")));
    
    // Check if we have both set.
    if (policy_jumplist != NULL && policy_interrupt_call != NULL)
        dhalt(ERR_CALL_CONVENTION_AMBIGIOUS, NULL);
    if (policy_jumplist == NULL && policy_interrupt_call == NULL)
        dhalt(ERR_CALL_CONVENTION_MISSING, NULL);
    
    // If this is the jumplist calling, we know how to perform that already.
    if (policy_jumplist != NULL)
        generate_jump(all, bin, call);
    else if (biseqcstrcaseless(policy_interrupt_call, "stack-call"))
        generate_ccstackcall(policies, all, bin, call);
    else if (biseqcstrcaseless(policy_interrupt_call, "register-call"))
        generate_ccregistercall(policies, all, bin, call);
    else if (biseqcstrcaseless(policy_interrupt_call, "register-direct"))
        generate_ccregisterdirect(policies, all, bin, call);
    else
        dhalt(ERR_CALL_CONVENTION_UNKNOWN, NULL);
}

///
/// @}
///