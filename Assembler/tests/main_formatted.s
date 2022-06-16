.global myStart
.extern myCounterOutter
.section myCode
myStart:
ldr r0,$0x4
ldr r1,$0x1
ldr r2,$0x8
loop:
add r0,r1
ldr r3,myCounterInner
add r3,r1
str r3,myCounterInner
str r3,myCounterOutter
cmp r0,r2
jne loop
ldr r0,myCounterInner
ldr r1,myCounterOutter
mul r1,r0
cmp r1,r2
jeq loop
halt
.section myData
myCounterInner:
.word 0
.end