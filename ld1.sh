#!/bin/bash
./linker -o Linker/tests/linker_output.o Linker/tests/interrupts_output_binary.o Linker/tests/main_output_binary.o -hex -place=myCode@0x0d54 -place=testSection@0x0c04