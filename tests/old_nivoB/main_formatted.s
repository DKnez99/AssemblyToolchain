.global my_start,my_counter
.section my_code
my_start:
ldr r6,$0xFEFE
ldr r0,$0x1
str r0,0xFF10
wait:
ldr r0,my_counter
ldr r1,$5
cmp r0,r1
jne wait
halt
.section my_data
my_counter:
.word 0
.end