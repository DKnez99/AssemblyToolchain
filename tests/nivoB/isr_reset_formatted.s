.extern my_start
.section isr
.global isr_reset
isr_reset:
jmp my_start
.end