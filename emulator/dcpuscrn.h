//
// DCPU Emulator by James Rhodes
//
// Virtual screen.
//

#ifndef __DCPUSCRN_H
#define __DCPUSCRN_H

#include <libtcod.h>
#include "dcpu.h"

#define VM_SCREEN_WIDTH 32
#define VM_SCREEN_HEIGHT 24

void vm_scrn_init(vm_t* vm);
void vm_scrn_cycle(vm_t* vm);
void vm_scrn_free(vm_t* vm);

#endif