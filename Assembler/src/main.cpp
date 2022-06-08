#include "../inc/As.hpp"

void symbolTableStuff(std::string fileName1, std::string fileName2, std::string fileName3){
  SymbolTable table;
  table.addSymbol("sectA", SymbolData("sectA", 0, SymbolType::SECTION, true));
  table.addSymbol("sectB", SymbolData("sectB", 0, SymbolType::SECTION, true));
  table.addSymbol("symA", SymbolData("sectA", 4, SymbolType::LOCAL, true));
  //table.addSymbol("symZ", SymbolData());
  table.addSymbol("symB", SymbolData("sectB", 15, SymbolType::GLOBAL, true));
  std::cout<<"\nValid symbol table: "<<std::to_string(table.invalidSymbols().capacity()==0)<<std::endl;
  table.printToOutput(fileName1);
  table.printToHelperTxt(fileName2);
  table.printToBinaryOutput(fileName3);
}

void sectionTableStuff(std::string fileName1, std::string fileName2, std::string fileName3){
  SectionTable table;

  table.addSectionEntry("sectA", SectionEntry(0,5,0xF1002F6612));
  table.addSectionEntry("sectA", SectionEntry(0,4,0x4502FE));
  table.addSectionEntry("sectB", SectionEntry(0,3,0x123456));
  table.addSectionEntry("sectB", SectionEntry(0,3,0x789ABC, false));
  table.addSectionEntry("sectA", SectionEntry(0,4,0));

  table.printToOutput(fileName1);
  table.printToHelperTxt(fileName2);
  table.printToBinaryOutput(fileName3);
}

void relocTableStuff(std::string fileName1, std::string fileName2, std::string fileName3){
  RelocationTable table;
  table.addRelocEntry("sectA", RelocEntry(14, RelocType::R_X86_64_16, "symB", 0, false));
  table.addRelocEntry("sectA", RelocEntry(125, RelocType::R_X86_64_16, "symZ", 0, true));
  table.addRelocEntry("sectC", RelocEntry(0, RelocType::R_X86_64_PC16, "sectA", 4, true));

  table.printToOutput(fileName1);
  table.printToHelperTxt(fileName2);
  table.printToBinaryOutput(fileName3);
}

int main(){
  // symbolTableStuff("output.o", "output_helper.txt", "binary_output.o");
  // sectionTableStuff("output.o", "output_helper.txt", "binary_output.o");
  // relocTableStuff("output.o", "output_helper.txt", "binary_output.o");

  Assembler as("input.s", "output.o");
  bool assemblySuccessful=!as.assemble();

  return assemblySuccessful;

  return 0;
}

