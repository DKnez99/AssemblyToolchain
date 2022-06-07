#include "../inc/AsRelocTable.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

std::ostream& operator<<(std::ostream& out, RelocType value){
    const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(R_X86_64_16);
        PROCESS_VAL(R_X86_64_PC16);
    }
#undef PROCESS_VAL
    return out << std::string(s);
}
    
//||=========================================================||
//||=========================================================||
//||=========================================================||
//||               RELOC'S EXISTENTIAL DREAD                 ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

bool RelocationTable::relocationsExistsForSection(const std::string &sectionName){
  return RelocationTable::table.find(sectionName)!=RelocationTable::table.end();
}

bool RelocationTable::isEmpty(){

}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                   RELOCATION ENTRIES                    ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

std::vector<RelocEntry> RelocationTable::getRelocEntriesForSection(const std::string &sectionName){
  return RelocationTable::table.at(sectionName);
}

void RelocationTable::addRelocEntry(const std::string &sectionName, RelocEntry entry){
  RelocationTable::table.at(sectionName).push_back(entry);
}

//for local symbols which were previously undefined (will get sectName and offset by going through flinks)
void RelocationTable::changeRelocEntriesForLocal(const std::string &sectionName, int offset, std::string newSymbol, int newAddend){
  for(auto &entry: RelocationTable::table.at(sectionName)){
    if(entry.offset==offset){
      entry.symbol=newSymbol;
      entry.addend=newAddend;
    }
  }
}

//for global symbols which were previously local
//  old symbol = section name in which local symbol was defined
//  old addend = local symbol's value
void RelocationTable::changeRelocEntriesForGlobal(std::string oldSymbol, int oldAddend, std::string newSymbol, int newAddend){
  for(auto &relocEntries: RelocationTable::table){
    for(auto &entry: relocEntries.second){
      if(entry.symbol==oldSymbol && entry.addend==oldAddend){
        entry.symbol=newSymbol;
        entry.addend=newAddend;
      }
    }
  }
}

std::vector<AbsSymbolInfo> RelocationTable::getAndDeleteRelocEntriesForAbsolute(){

}

//print
void RelocationTable::printToTxt(const std::string &fileName){

}

void RelocationTable::printToHelperTxt(const std::string &fileName){

}