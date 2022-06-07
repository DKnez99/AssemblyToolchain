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
  return RelocationTable::table.empty();
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
  RelocationTable::table[sectionName].push_back(entry);
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
  //implement later
  std::vector<AbsSymbolInfo> result;
  return result;
}

//print
void RelocationTable::printToOutput(const std::string &fileName){
  int offW = 15, typW=20, datW=7, symW=20, addW=15;
  std::ofstream file;
  file.open(fileName, std::ios_base::app);
  for(const auto& relocation: RelocationTable::table){
    std::string sectionName = relocation.first;
    file<<std::left<<"#"<<sectionName<<".reloc\n";
    file<<std::setw(offW)<<"Offset(HX)"
        <<std::setw(20)<<"Type"
        <<std::setw(20)<<"Symbol"
        <<std::setw(15)<<"Addend(HX)";
    for(const auto& entry: relocation.second){
      file<<"\n"
          <<std::setw(offW)<<std::hex<<entry.offset<<std::dec
          <<std::setw(20)<<entry.type
          <<std::setw(20)<<entry.symbol
          <<std::setw(15)<<std::hex<<entry.addend<<std::dec;
    }
    file<<std::endl;
  }
  file.close();
}

void RelocationTable::printToHelperTxt(const std::string &fileName){
  int offW = 15, typW=20, datW=7, symW=20, addW=15;
  std::ofstream file;
  file.open(fileName, std::ios_base::app);
  for(const auto& relocation: RelocationTable::table){
    std::string sectionName = relocation.first;
    file<<std::left<<"#"<<sectionName<<".reloc\n";
    file<<std::setw(offW)<<"Offset(DEC)"
        <<std::setw(typW)<<"Type"
        <<std::setw(datW)<<"Data?"
        <<std::setw(symW)<<"Symbol"
        <<std::setw(addW)<<"Addend(DEC)";
    for(const auto& entry: relocation.second){
      file<<"\n"
          <<std::setw(offW)<<entry.offset
          <<std::setw(typW)<<entry.type
          <<std::setw(datW)<<((entry.isData)?"Yes":"No")
          <<std::setw(symW)<<entry.symbol
          <<std::setw(addW)<<entry.addend;
    }
    file<<std::endl;
  }
  file.close();
}