1
2
#define blah 4
blah
5
#define hello(v) \
v \
v \

10
hello(11)
12
hello(13) hello(13)
hello(14)
15
#define a(v) \
v \

#define b a
b(20)
b(21) b(21) hello(21)
hello(22)
23
