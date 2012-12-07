/**
 * 
 *    File:       main.c
 * 
 *    Project:        DCPU-16 Tools
 *    Component:      Emulator
 * 
 *    Authors:        James Rhodes
 *            Tyrel Haveman
 *            Jose Manuel Diez
 *            Patrick Flick
 * 
 *    Description:    Main entry point.
 * 
 **/


#include <libgen.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C"{
#include <argtable2.h>
#include <bstring.h>
#include <dcpu.h>
#include <osutil.h>
#include <version.h>
#include <ldata.h>
#include <debug.h>
#include <derr.h>
#include <iio.h>
#include <hwtimer.h>
#include <hw.h>
#include <timing.h>
#include <dcpubase.h>
}


#include <iostream>
#include <cstdio>
#include <vector>



int main(int argc, char* argv[])
{
    // Define our variables.
    FILE* load;
    uint16_t flash[0x10000];
    char leading[0x100];
    unsigned int i;
    bool uread = true;
    
    int nerrors;
    
    
    // Show version information.
    version_print(bautofree(bfromcstr("Emulator")));
    
    // Set global path variable.
    osutil_setarg0(bautofree(bfromcstr(argv[0])));
    
    // Set endianness.
    isetmode(IMODE_BIG);
    
    
    // Zero out the flash space.
    for (i = 0; i < 0x10000; i++)
        flash[i] = 0x0;
    
    // Zero out the leading space.
    for (i = 0; i < 0x100; i++)
        leading[i] = 0x0;
        
        // Load from either file or stdin.
    if (strcmp(argv[1], "-") != 0)
    {
        // Open file.
        load = fopen(argv[1], "rb");
        
        if (load == NULL)
            std::cout << "error loading file " << argv[1] << std::endl;
    }
    else
    {
        // Windows needs stdin in binary mode.
        #ifdef _WIN32
        _setmode(_fileno(stdin), _O_BINARY);
        #endif
        
        // Set load to stdin.
        load = stdin;
    }
    
    // Read leading component.
    for (i = 0; i < strlen(ldata_objfmt); i++)
        leading[i] = fgetc(load);
    fseek(load, 0, SEEK_SET);
    
    // Read up to 0x10000 words.
    for (i = 0; i < 0x10000 && !feof(load); i++)
        iread(&flash[i], load);
    fclose(load);
    
            
    // And then use the VM.
    

    for (int size = 16; size < 5000; size <<= 1){
        std::vector<vm_t*> vms(size);
        
        // create all vms
        for (std::vector<vm_t*>::iterator it = vms.begin(); it != vms.end(); ++it)
        {
            *it = vm_create();
            (*it)->debug = 0;
            (*it)->radiation_factor = 0;
            (*it)->can_fire = true;
            vm_flash(*it, flash);
            //vm_hw_timer_init(*it);
        }
        
        struct vm_tod_timer freq_timer;
        unsigned long freq_cycle_count = 0;
        const unsigned int ticks_per_cycle = 10000;
        vm_timing_start_timer(&freq_timer);
        for (int j = 0; j < 1;)
        {
            if (vm_timing_has_reached_mics(&freq_timer, 1000000))
            {
                std::cout << "Emulating " << size << " at " << (double) freq_cycle_count / (double)vm_timing_get_cur_elapsed_mics(&freq_timer)*1000.0 << " kHz each." << std::endl;
                vm_timing_reset_timer(&freq_timer);
                freq_cycle_count = 0;
                j++;
            }
            for (std::vector<vm_t*>::iterator it = vms.begin(); it != vms.end(); ++it)
            {
                for (int i = 0; i < ticks_per_cycle; i++)
                    vm_cycle(*it);
                
            }
            freq_cycle_count += ticks_per_cycle;
        }
        
        // clean up
        for (std::vector<vm_t*>::iterator it = vms.begin(); it != vms.end(); ++it)
        {
            vm_free(*it);
        }
    }
    
    

    return 0;
}


