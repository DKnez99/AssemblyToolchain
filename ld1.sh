#!/bin/bash
./linker -hex -o Linker/tests/program.hex Linker/tests/interrupts_output_binary.o Linker/tests/main_output_binary.o
mv Linker/tests/program_binary.o Emulator/tests/