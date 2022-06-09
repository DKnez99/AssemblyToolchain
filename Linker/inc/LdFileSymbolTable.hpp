#pragma once

#include "LdSectionTable.hpp"
#include <string>
#include <unordered_map>
#include <list>

typedef enum{
  LOCAL = 0,
  GLOBAL = 1,
  EXTERN = 2,
  SECTION = 3,
  NONE = 4
} SymbolType;

std::ostream& operator<<(std::ostream& out, SymbolType value);

struct FileSymbolData{
  int symbolID;
  std::string section;
  unsigned int value;  //offset in the current section
  SymbolType type;
  bool isDefined;
  FileSymbolData(int symbolID, std::string section, unsigned int value, SymbolType type, bool isDefined)
  :symbolID(symbolID), section(section),value(value),type(type),isDefined(isDefined) {}
  FileSymbolData() : symbolID(symbolID), section(SECTION_UNDEFINED),value(0),type(SymbolType::NONE),isDefined(false) {}
};

class FileSymbolTable{
    std::unordered_map<std::string, std::unordered_map<std::string, FileSymbolData>> table;
  public:
    //symbol's existential dread
    bool symbolExists(const std::string &fileName, const std::string &label);
    bool isEmpty();
    //symbol
    FileSymbolData getSymbol(const std::string &fileName, const std::string &label);
    void addSymbol(const std::string &fileName, const std::string &label, FileSymbolData symbolData);
    void removeSymbol(const std::string &fileName, const std::string &label);
    //symbolID
    int getSymbolID(const std::string &fileName, const std::string &label);
    //section
    std::string getSymbolSecton(const std::string &fileName, const std::string &label);
    void setSymbolSection(const std::string &fileName, const std::string &label, std::string newSectionName);
    //value
    int getSymbolValue(const std::string &fileName, const std::string &label);
    void setSymbolValue(const std::string &fileName, const std::string &label, unsigned int newValue);
    //type
    SymbolType getSymbolType(const std::string &fileName, const std::string &label);
    void setSymbolType(const std::string &fileName, const std::string &label, SymbolType newType);    
    std::vector<std::string> getSymbolsOfType(SymbolType wantedType); //must go through reloc table and check global/local stuff   MAYBE NEEDS TO RETURN SYMBOLDATA ALSO
    //isDefined
    bool getSymbolIsDefined(const std::string &fileName, const std::string &label);
    void setSymbolIsDefined(const std::string &fileName, const std::string &label, bool newIsDefined);
    //invalid symbols (checked in post processing of the file)
    std::vector<std::string> invalidSymbols();
    //print
    void printToHelperTxt(const std::string &fileName);
};