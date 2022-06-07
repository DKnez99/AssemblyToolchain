#include "../inc/AsSymbolTable.hpp"
#include "../inc/AsRelocTable.hpp"
#include <iostream>

void symbolTableStuff(std::string fileName1, std::string fileName2){
  SymbolTable table;
  table.addSymbol("sectA", SymbolData("sectA", 0, SymbolType::SECTION, true));
  table.addSymbol("sectB", SymbolData("sectB", 0, SymbolType::SECTION, true));
  table.addSymbol("symA", SymbolData("sectA", 4, SymbolType::LOCAL, true));
  table.addSymbol("symZ", SymbolData());
  table.addSymbol("symB", SymbolData("sectB", 15, SymbolType::GLOBAL, true));
  std::cout<<"\nValid symbol table: "<<std::to_string(table.invalidSymbols().capacity()==0)<<std::endl;
  table.printToOutput(fileName1);
  table.printToHelperTxt(fileName2);
}

void relocTableStuff(std::string fileName1, std::string fileName2){
  RelocationTable table;
  table.addRelocEntry("sectA", RelocEntry(14, RelocType::R_X86_64_16, "symB", 0, false));
  table.addRelocEntry("sectA", RelocEntry(125, RelocType::R_X86_64_16, "symZ", 0, true));
  table.addRelocEntry("sectC", RelocEntry(0, RelocType::R_X86_64_PC16, "sectA", 4, true));

  table.printToOutput(fileName1);
  table.printToHelperTxt(fileName2);
}

int main(){
  symbolTableStuff("output.o", "output_helper.txt");
  relocTableStuff("output.o", "output_helper.txt");
  return 0;
}

