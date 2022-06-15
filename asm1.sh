#!/bin/bash
./asembler -o Assembler/tests/interrupts_output.o Assembler/tests/interrupts.s
./asembler -o Assembler/tests/main_output.o Assembler/tests/main.s
mv Assembler/tests/main_output_binary.o Linker/tests/
mv Assembler/tests/interrupts_output_binary.o Linker/tests/