#!/bin/bash
./linker -hex -o Linker/tests/program.hex Linker/tests/interrupts_output_binary.o Linker/tests/main_output_binary.o Linker/tests/counter_output_binary.o -place=ivt@0x0000 -place=myCode@0x100
mv Linker/tests/program_binary.o Emulator/tests/