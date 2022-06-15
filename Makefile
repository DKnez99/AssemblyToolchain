ASSEMBLER = asembler
LINKER = linker
EMULATOR = emulator

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
Linker/src/LdSymbolTablesForAllAsFiles.cpp \
Linker/src/LdSectionTable.cpp \
Linker/src/LdSectionTablesForAllAsFiles.cpp \
Linker/src/LdRelocTable.cpp \
Linker/src/LdRelocTablesForAllAsFiles.cpp \
Linker/src/LdConversionsAndErrors.cpp

EMULATOR_SOURCE_CPP_LIST = \
Emulator/src/main.cpp \
Emulator/src/EmMainLogic.cpp \
Emulator/src/EmDataAndFiles.cpp \
Emulator/src/EmMemoryOperations.cpp \
Emulator/src/EmErrors.cpp \

vpath Assembler/src/%.cpp $(sort $(dir $(ASSEMBLER_SOURCE_CPP_LIST)))
vpath Linker/src/%.cpp $(sort $(dir $(LINKER_SOURCE_CPP_LIST)))
vpath Emulator/src/%.cpp $(sort $(dir $(EMULATOR_SOURCE_CPP_LIST)))

CC = g++

all : clean $(ASSEMBLER) $(LINKER) $(EMULATOR)

$(ASSEMBLER) : $(ASSEMBLER_SOURCE_CPP_LIST) 
	$(CC) $(^) -o $(@)

$(LINKER) : $(LINKER_SOURCE_CPP_LIST) 
	$(CC) $(^) -o $(@)

$(EMULATOR) : $(EMULATOR_SOURCE_CPP_LIST)
	$(CC) $(^) -o $(@)

clean:
	-rm -rf *.txt *.o