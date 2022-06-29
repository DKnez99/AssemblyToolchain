.extern isr_reset,isr_timer,isr_terminal
.section ivt
.word isr_reset
.skip 2
.word isr_timer
.word isr_terminal
.skip 8
.end