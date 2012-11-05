///
/// @addtogroup LibDCPU-LD-Policy
/// @brief Library for reading and executing linker policies.
///
/// This library parses linker policies (scripts) and provides the ability to
/// register actions when encountering components of scripts.
///
/// @{
///
/// @file
/// @brief General API access for linker policies.
/// @author James Rhodes
/// 
/// This header provides general access to the linker policy parsing API.
///

#ifndef __DCPU_LD_POLICY_H
#define __DCPU_LD_POLICY_H

#include <bstring.h>
#include "policyast.h"

policies_t* policies_load(FILE* input);
void policies_free(policies_t* policies);
policy_t* policies_get_policy(policies_t* policies, freed_bstring name);
policy_t* policies_get_settings(policies_t* policies);
bstring policies_get_setting(policies_t* policies, freed_bstring key);

policy_state_t* state_from_policy(policy_t* policy);
void state_execute(policies_t* policies, policy_state_t* state);
void state_free(policy_state_t* state);

#endif

///
/// @}
///