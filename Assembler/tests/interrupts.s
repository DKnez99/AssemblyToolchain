# file: interrupts.s
.section ivt
 .word isr_reset
 .skip 2 # isr_error
 .skip 2 # isr_timer
 .skip 2 # isr_terminal
 .skip 8
.extern myStart
.section isr
# prekidna rutina za reset
isr_reset:
 jmp myStart
.end
