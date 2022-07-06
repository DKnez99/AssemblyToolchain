.extern my_counter
.section isr
.global isr_terminal
isr_terminal:
push r0
push r1
ldr r0,0xFF02
ldr r1,$1
add r0,r1
str r0,0xFF00
ldr r0,%my_counter
ldr r1,$1
add r0,r1
str r0,my_counter
pop r1
pop r0
iret
.end