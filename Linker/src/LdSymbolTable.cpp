#include "../inc/LdSymbolTable.hpp"
#include "../inc/LdSectionTable.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

std::ostream& operator<<(std::ostream& out, SymbolType value){
    const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(NONE);     
        PROCESS_VAL(SECTION);     
        PROCESS_VAL(LOCAL);
        PROCESS_VAL(GLOBAL);
        PROCESS_VAL(EXTERN);
    }
#undef PROCESS_VAL
    return out << std::string(s);
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                   CONSTRUCTOR                           ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

SymbolTable::SymbolTable(){
  
}

SymbolTable::SymbolTable(const bool isGlobal){
  if(isGlobal){
    SymbolTable::addSymbol(SECTION_ABSOLUTE, SymbolData(-1,SECTION_ABSOLUTE,0,SymbolType::SECTION,true));
    SymbolTable::addSymbol(SECTION_UNDEFINED, SymbolData(0,SECTION_UNDEFINED,0,SymbolType::SECTION,true));
  }
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||              SYMBOL'S EXISTENTIAL DREAD                 ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

bool SymbolTable::symbolExists(const std::string &label){
  return SymbolTable::table.find(label)!=SymbolTable::table.end();
}

bool SymbolTable::isEmpty(){
  return SymbolTable::table.empty();
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                       SYMBOL                            ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

SymbolData SymbolTable::getSymbol(const std::string &label){
  return SymbolTable::table.at(label);
}

void SymbolTable::addSymbol(const std::string &label, SymbolData symbolData){
  SymbolTable::table.insert(std::make_pair(label, symbolData));
}

void SymbolTable::removeSymbol(const std::string &label){
  SymbolTable::table.erase(label);
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                    SYMBOL ID                            ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

int SymbolTable::getSymbolID(const std::string &label){
  return SymbolTable::table.at(label).symbolID;
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                      SECTION                            ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

std::string SymbolTable::getSymbolSecton(const std::string &label){
  return SymbolTable::table.at(label).section;
}

void SymbolTable::setSymbolSection(const std::string &label, std::string newSectionName){
  SymbolTable::table.at(label).section=newSectionName;
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                       VALUE                             ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

unsigned int SymbolTable::getSymbolValue(const std::string &label){
  return SymbolTable::table.at(label).value;
}

void SymbolTable::setSymbolValue(const std::string &label, unsigned int newValue){
  SymbolTable::table.at(label).value=newValue;
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                        TYPE                             ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

SymbolType SymbolTable::getSymbolType(const std::string &label){
  return SymbolTable::table.at(label).type;
}

void SymbolTable::setSymbolType(const std::string &label, SymbolType newType){
  SymbolTable::table.at(label).type=newType;
} 

std::vector<std::string> SymbolTable::getSymbolsOfType(const SymbolType &wantedType){
  std::vector<std::string> result;
  for(auto const &symbol: SymbolTable::table){
    if(symbol.second.type==wantedType)
      result.push_back(symbol.first);
  }
  return result;
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                   IS DEFINED                            ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

bool SymbolTable::getSymbolIsDefined(const std::string &label){
  return SymbolTable::table.at(label).isDefined;
}

void SymbolTable::setSymbolIsDefined(const std::string &label, bool newIsDefined){
  SymbolTable::table.at(label).isDefined=newIsDefined;
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                  INVALID SYMBOLS                        ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

std::vector<std::string> SymbolTable::invalidSymbols(){
  std::vector<std::string> invalidSymbols;
  for(const auto &symbol: SymbolTable::table){
    if(symbol.second.isDefined==false && symbol.second.type!=SymbolType::EXTERN){
      invalidSymbols.push_back(symbol.first);
    }
  }
  return invalidSymbols;
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                       PRINT                             ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

void SymbolTable::printToHelperTxt(const std::string &fileName){
  int idW=4, valW=10, typW=9, nameW=20, flnW=30;
  std::ofstream file;
  file.open(fileName, std::ios::app);  //append to file
  file<<std::left<<"#.symtab\n"
      <<std::setw(idW)  <<"ID"
      <<std::setw(valW) <<"Value(HX)"
      <<std::setw(typW) <<"MyType"
      <<std::setw(nameW)<<"Section"
      <<std::setw(nameW)<<"Name";

  for(auto const &symbol: SymbolTable::table){
    std::string label = symbol.first;
    SymbolData symbolData = symbol.second;
    file<<"\n"
        <<std::setw(idW)<<symbolData.symbolID                      //num
        <<std::setw(valW)<<std::hex<<std::right<<(std::to_string(symbolData.value)+" ")<<std::left<<std::dec     //value
        <<std::setw(typW)<<symbolData.type                         //type
        <<std::setw(nameW)<<symbolData.section                      //section
        <<std::setw(nameW)<<std::left<<label.c_str();               //name

  }
  file<<std::endl;
  file.close();
}