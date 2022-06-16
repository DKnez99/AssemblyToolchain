.global myStart
.extern myCounterOutter
.section myCode
myStart:
 ldr r0, $0x4
 ldr r1, $0x1
 ldr r2, $0x8
loop: add r0, r1  # should complete 4 times
 ldr r3, myCounterInner # r3 <= mem[myCounterInner]
 add r3, r1
 str r3, myCounterInner # mem[myCounterInner]<=r3+1
 str r3, myCounterOutter
 cmp r0, r2 # x > 8
 jne loop

 ldr r0, myCounterInner # r0 <= 4
 ldr r1, myCounterOutter # r1 <= 4
 mul r1, r0
 cmp r2, r1 # 16 > 8
 jeq loop

# test psw flags
 ldr r4, $4
 ldr r5, $0b11
 shl r4, r5

# test div by 0
# ldr r4, $0b11
# ldr r5, $0x0
# div r4, r5
 halt
.section myData
myCounterInner:
 .word 0
.end
