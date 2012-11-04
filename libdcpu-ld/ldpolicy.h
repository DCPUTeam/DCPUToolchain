///
/// @addtogroup LibDCPU-LD
/// @{
///
/// @file
/// @brief Header that declares the API for using linker policies on linker bins.
/// @author James Rhodes
///

#ifndef __DCPU_LD_POLICY_API_H
#define __DCPU_LD_POLICY_API_H

#include <bstring.h>
#include "ldbins.h"

void bins_apply_policy(freed_bstring path, freed_bstring target, freed_bstring code, freed_bstring output);

#endif

///
/// @}
///