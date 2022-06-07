#include "../inc/AsSymbolTable.hpp"
#include "../inc/AsSectionTable.hpp"
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
  SymbolTable::addSymbol(SECTION_UNDEFINED, SymbolData(SECTION_UNDEFINED,0,SymbolType::SECTION,true));
  SymbolTable::addSymbol(SECTION_ABSOLUTE, SymbolData(SECTION_ABSOLUTE,0,SymbolType::SECTION,true));
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

int SymbolTable::getSymbolValue(const std::string &label){
  return SymbolTable::table.at(label).value;
}

void SymbolTable::setSymbolValue(const std::string &label, int newValue){
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

std::vector<std::string> SymbolTable::getSymbolsOfType(SymbolType wantedType){
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
//||                       FLINK                             ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

std::list<ForwardRef> SymbolTable::getFlinks(const std::string &label){
  return SymbolTable::table.at(label).flink;
}

void SymbolTable::addFlink(const std::string &label, std::string section, int offset){
  SymbolTable::table.at(label).flink.push_back(ForwardRef(section, offset));
}

bool SymbolTable::hasFlinks(const std::string &label){
  return !SymbolTable::table.at(label).flink.empty();
}

void SymbolTable::removeFlinks(const std::string &label){
  SymbolTable::table.at(label).flink.clear();
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                       PRINT                             ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

void SymbolTable::printToOutput(std::string fileName){
  
}

void SymbolTable::printToHelperTxt(std::string fileName){

}