1
2
.DEFINE blah 12
.MACRO hello(v)
v
.ENDMACRO
7
.INCLUDE "include-01.pp"
9
10
.INCLUDE "component.pp"
blah
13
14
hello(15)
16
.INCLUDE "incmacro.pp"
18
19
20
