.global WOWOOWOWOWOW, myStart
.global myCounter, TESTESTESTEST
.section myCode
tim_cfg: .word 0xFF10
WOWOOWOWOWOW:
 ldr r0, $0x1
 str r0, tim_cfg
wait:
 ldr r0, myCounter
 ldr r1, $5
 TESTESTESTEST:
 cmp r0, r1
 jne wait
 myStart:
 halt
.section myData
myCounter:
 .word 0
.end