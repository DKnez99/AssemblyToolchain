#!/bin/bash
./asembler -o Assembler/tests/interrupts_output.o Assembler/tests/interrupts.s
./asembler -o Assembler/tests/main_output.o Assembler/tests/main.s
./asembler -o Assembler/tests/counter_output.o Assembler/tests/counter.s

mv Assembler/tests/main_output_binary.o Linker/tests/
mv Assembler/tests/interrupts_output_binary.o Linker/tests/
mv Assembler/tests/counter_output_binary.o Linker/tests/