./asembler -o tests/counter.o tests/counter.s
./asembler -o tests/main.o tests/main.s
./asembler -o tests/interrupts.o tests/interrupts.s

./linker -hex -o tests/program.hex tests/counter.o tests/main.o tests/interrupts.o -place=ivt@0

./emulator tests/program.hex