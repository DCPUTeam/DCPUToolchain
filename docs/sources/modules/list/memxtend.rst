.. _module-memxtend:

Physical Memory Extensions
=============================================

This hardware module implements the MEMXTEND hardware.

.. code-block:: text
    
    TT_MEMXTEND v0.1

                            //=====\\ 
                            ||   //=||==\\
                            \\==||=//    ||
                                 \\=====//
                    
                        TOOLCHAIN  TECHNOLOGIES
                            bringing the best
                            
    DCPU-16 Hardware Info:
        Name: MEMXTEND - Physical Memory Extensions
        ID: 0x38f6ab35, version: 0x0001
        Manufacturer: 0x00000000 (UNLICENSED)
        
    Description:
        The memory extender uses innovative and highly illegal
        technologies to extend your CPU memory with only a slight
        reduction in reliability.  Using our physical insertion
        technology we can bring over 10x the licensed memory
        to your CPU.
        
    Physical installation:
        CAUTION!  Incorrect installation can lead to a
                disfunctioning CPU.
                
        1. Remove the physical protection cover of the CPU, as
        demonstrated in the diagram below.  Be careful not to
        saturate exposed components or touch the CPU internals in
        any way.  Ignore the warranty void warning on the top of
        the CPU.
        
        /-----------------\
        | /----\  /------\|  ->  /---------------\
        | |    |  |      ||      |               |
        | \----/  | MEM  ||  ->  |               |
        |         |      ||      |               |
        | /-----\ |      ||  ->  |               |
        | |     | \------/|      |               |
        | \-----/         |  ->  \---------------/
        \-----------------/
        
        2. Line up the square socket on the bottom of the memory
        extender with the edges of the memory chip.  Be careful 
        to ensure the unit is lined up correctly as shown in the
        diagram below.
        
                        /----\
                    /--/     |
                    |        |
                    \--------/
                    # | | || #
                    
                    |  |
                    v  v
                    
        | /----\  /------\|
        \=================/
        
        3. Use a hammer or other significant force applying mechanism
        to punch the interconnecting pins of the memory extender
        into the memory chip.  This will override the transistors
        in the memory chip allowing for non-transparent extension of
        memory.
        
        4. Use duct tape to secure the memory extender onto the CPU.
        Ensure that the CPU is now kept in a safe, dry place, free
        from solar radiation.
        
        Recommended duct tape configuration:
        
                    /###########\ (duct tape)
                /     /----\ #
                # /--/     | #
                # |        | #
        /######/  \--------/ #
        #| /----\  /------\| #
        #\=================/ #
        
    Reliability information:
        Due to the nature of the CPU, it provides just enough power to
        run the memory extender.  It certain situations where the CPU is
        under high-load, the provided voltage may drop below the
        recommended threshold for the internal memory maintainance
        mechanism which could possibly lead to memory corruption inside
        the memory extender.

    Interrupt behaviour:
        The model for memory sets are as follows (where A is the active
        RAM):
        
        CPU  Memory Extender
        ===  =====================
        |A|  |0|1|2|3|4|5|6|7|8|9|
        
        When a HWI is received by the memory extender, it reads the A
        register and does one of the following actions:
        
        0: MEM_SWAP
            Reads the B register as the index of memory set to swap.  Must
            lie between 0x0 and 0x9.  Values outside of this have
            unintended side-effects.  The active set of memory is swapped
            with the set of memory at the index in B.
        1: MEM_CLONE
            Reads the B register and clones the active set of memory
            into the set of memory at the index in B.  Index in B must
            lie between 0x0 and 0x9.  Values outside of this have
            unintended side-effects. 
