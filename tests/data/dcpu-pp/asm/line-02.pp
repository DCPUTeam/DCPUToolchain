1
2
.DEFINE blah 4
blah
5
.MACRO hello(v)
v
v
.ENDMACRO
10
hello(11)
12
hello(13) hello(13)
hello(14)
15
.MACRO a(v)
v
.ENDMACRO
.DEFINE b a
b(20)
b(21)b(21)hello(21)
hello(22)
23
