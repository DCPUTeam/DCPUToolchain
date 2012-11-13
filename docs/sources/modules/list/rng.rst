.. _module-rng:

Random Number Generator
=============================================

This hardware module provides a random number generator.

.. code-block:: text
    
    Name: Random Number Generator
    ID: 0x739df773
    Version: 1
    Manufacturer: 0x93765997
    
    Interrupts do different things depending on the contents of the A register:
    
     A | BEHAVIOUR
    ---+-----------------------------------------------------------
     0 | Set the B register to the next random unsigned integer.
     1 | Set the generator seed to the value in the B register.
    ---+-----------------------------------------------------------

