.section ivt
.word isr_reset
.word isr_error
.skip 2
.skip 2
.skip 8
.extern myStart
.section isr
isr_reset:
jmp myStart
isr_error:
halt
.end