.. _module-inttrace:

Interrupt Tracing
======================

This debugger module provides runtime checks for ensuring that
interrupts are called and handled correctly.  It deals with
ensuring the interrupt queue is not turned off in the middle of an
interrupt handler.

This module is useful for kernel developers who want to ensure
that their interrupt handling infrastructure is correct.