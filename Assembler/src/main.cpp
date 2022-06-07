#include "../inc/AsSymbolTable.hpp"

int main(){
  SymbolTable table;

  table.addSymbol("sectA", SymbolData("sectA", 0, SymbolType::SECTION, true));
  table.addSymbol("sectB", SymbolData("sectB", 0, SymbolType::SECTION, true));
  table.addSymbol("symA", SymbolData("sectA", 4, SymbolType::LOCAL, true));
  table.addSymbol("symZ", SymbolData());
  table.addSymbol("symB", SymbolData("sectB", 15, SymbolType::GLOBAL, true));

  table.printToHelperTxt("output_helper.txt");
  table.printToOutput("output.o");

  return 0;
}

