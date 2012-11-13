.. _module-units:

Unit Testing
===================

This preprocessor and debugger module pair provide unit testing
facilities.

This module should be used in conjunction with the :ref:`module-assert`
as it does not provide any runtime checking facilities itself.

.PREREQ name1, name2, ..., nameN
------------------------------------
Defines that before the next defined unit test is run, the
specified unit tests should be run before it.  This allows
the correct environment for a unit test to be set up.

.UNIT name
--------------
Begins the definition of a unit test with the specified name.

.ENDUNIT
--------------
Ends the definition of the previously defined unit test.

test <name>
--------------
When called in the debugger, runs either the specified test
if name is provided, otherwise runs all tests.