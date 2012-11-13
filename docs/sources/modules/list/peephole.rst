.. _module-peephole:

Peephole Optimizer
====================

This optimizer module provides peephole assembly optimizations.  It can
currently optimize:

* simple-assign-back: Removal of the second instruction in SET a, b / SET b, a.
* stack-assign-back: Removal of the second instruction in SET PUSH, a / SET a, PEEK.
* preserve-pop: Removal of both instructions in SET PUSH, a / ADD SP, 1.