.section ivt
.word isr_reset
.skip 2
.skip 2
.skip 2
.skip 8
.extern myStart
.section isr
isr_reset:
jmp myStart
.end