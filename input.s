.global a, c
.extern b
.section text
jeq a
jeq e 
jeq b 
jeq %d
.word 2
d: .word d
ldr r1, d
str r1, c
ldr r1, e
.section data
.skip 8
e: .word a
.word c

a: .word b
.word bss
.section bss
c: .skip 8
.end