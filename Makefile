ASSEMBLER = asembler

ASSEMBLER_SOURCE_CPP_LIST = \
Assembler/src/main.cpp \
Assembler/src/AsMainLogic.cpp \
Assembler/src/AsConversionsAndErrors.cpp \
Assembler/src/AsProcess.cpp \
Assembler/src/AsRegexes.cpp \
Assembler/src/AsRelocTable.cpp \
Assembler/src/AsSectionTable.cpp \
Assembler/src/AsSymbolTable.cpp

vpath %.cpp $(sort $(dir $(ASSEMBLER_SOURCE_CPP_LIST)))

CC = g++

all : clean $(ASSEMBLER)
	./$(ASSEMBLER)

$(ASSEMBLER) : $(ASSEMBLER_SOURCE_CPP_LIST) 
	$(CC) $(^) -o $(@)

clean:
	-rm -rf *.txt *.o