#pragma once

#include "LdSectionTable.hpp"
#include <string>
#include <map>
#include <list>

typedef enum{
  LOCAL = 0,
  GLOBAL = 1,
  EXTERN = 2,
  SECTION = 3,
  NONE = 4
} SymbolType;

std::ostream& operator<<(std::ostream& out, SymbolType value);

struct SymbolData{
  int symbolID;
  std::string section;
  unsigned int value;  //offset in the current section
  SymbolType type;
  bool isDefined;
  SymbolData(int symbolID, std::string section, unsigned int value, SymbolType type, bool isDefined)
  :symbolID(symbolID), section(section),value(value),type(type),isDefined(isDefined) {}
  SymbolData() : symbolID(symbolID), section(SECTION_UNDEFINED),value(0),type(SymbolType::NONE),isDefined(false) {}
};

class SymbolTable{
    std::map<std::string, SymbolData> table;
  public:
    //constructor
    SymbolTable();
    //symbol's existential dread
    bool symbolExists(const std::string &label);
    bool isEmpty();
    //symbol
    SymbolData getSymbol(const std::string &label);
    void addSymbol(const std::string &label, SymbolData symbolData);
    void removeSymbol(const std::string &label);
    //symbolID
    int getSymbolID(const std::string &label);
    //section
    std::string getSymbolSecton(const std::string &label);
    void setSymbolSection(const std::string &label, std::string newSectionName);
    //value
    int getSymbolValue(const std::string &label);
    void setSymbolValue(const std::string &label, unsigned int newValue);
    //type
    SymbolType getSymbolType(const std::string &label);
    void setSymbolType(const std::string &label, SymbolType newType);    
    std::vector<std::string> getSymbolsOfType(SymbolType wantedType); //must go through reloc table and check global/local stuff   MAYBE NEEDS TO RETURN SYMBOLDATA ALSO
    //isDefined
    bool getSymbolIsDefined(const std::string &label);
    void setSymbolIsDefined(const std::string &label, bool newIsDefined);
    //invalid symbols (checked in post processing of the file)
    std::vector<std::string> invalidSymbols();
    //print
    void printToOutput(const std::string &fileName);
    void printToBinaryOutput(const std::string &fileName);
    void printToHelperTxt(const std::string &fileName);
};