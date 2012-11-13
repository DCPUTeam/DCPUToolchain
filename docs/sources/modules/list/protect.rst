.. _module-protect:

Runtime Memory Protection
============================

This preprocessor and debugger module pair provides runtime memory
protection checks to ensure memory is not corrupted.

.. warning::
    
    These checks are only performed in the debugger and provide no
    safety under a normal environment.  These modules do not modify
    code in any way to make it "safer".
    
.PROTECT
----------------
Begins a protected memory section.  When the DCPU-16 attempts to write
anywhere inside this section, the debugger will halt and display the
instruction that attempted to write to the protected location.

.ENDPROTECT
----------------
Ends a protected memory section.