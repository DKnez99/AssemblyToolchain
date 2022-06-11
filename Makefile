ASSEMBLER = asembler
LINKER = linker

ASSEMBLER_SOURCE_CPP_LIST = \
Assembler/src/main.cpp \
Assembler/src/AsMainLogic.cpp \
Assembler/src/AsConversionsAndErrors.cpp \
Assembler/src/AsProcess.cpp \
Assembler/src/AsRegexes.cpp \
Assembler/src/AsRelocTable.cpp \
Assembler/src/AsSectionTable.cpp \
Assembler/src/AsSymbolTable.cpp

LINKER_SOURCE_CPP_LIST = \
Linker/src/main.cpp \
Linker/src/LdMainLogic.cpp \
Linker/src/LdSymbolTable.cpp \
Linker/src/LdSectionTable.cpp \
Linker/src/LdRelocTable.cpp \
Linker/src/LdConversionsAndErrors.cpp

vpath Assembler/src/%.cpp $(sort $(dir $(ASSEMBLER_SOURCE_CPP_LIST)))
vpath Linker/src/%.cpp $(sort $(dir $(LINKER_SOURCE_CPP_LIST)))

CC = g++

all : clean $(ASSEMBLER) $(LINKER)

$(ASSEMBLER) : $(ASSEMBLER_SOURCE_CPP_LIST) 
	$(CC) $(^) -o $(@)

$(LINKER) : $(LINKER_SOURCE_CPP_LIST) 
	$(CC) $(^) -o $(@)

clean:
	-rm -rf *.txt *.o