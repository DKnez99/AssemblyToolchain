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
cmp r2,r1
jeq loop
ldr r4,$4
ldr r5,$0b11
shl r4,r5
halt
.section myData
myCounterInner:
.word 0
.end