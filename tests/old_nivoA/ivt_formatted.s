.extern isr_reset,isr_timer,isr_terminal,isr_user0
.section ivt
.word isr_reset
.word isr_error
.word isr_timer
.word isr_terminal
.word isr_user0
.skip 6
isr_error:
halt
.end