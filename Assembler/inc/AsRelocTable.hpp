#pragma once

#include <string>
#include <unordered_map>
#include <vector>

enum  RelocType{           //page 75 in psABI amd64
  R_X86_64_16,      //S + A
  R_X86_64_PC16,    //S + A - P
};

std::ostream& operator<<(std::ostream& out, RelocType value);

struct RelocEntry{
  int offset;
  RelocType type; //type of relocation from System V ABI
  std::string symbol;
  int addend;
  bool isData;  //if it's data use little endian, if it's an instruction use big endian
  RelocEntry(int offset, RelocType type, std::string symbol, int addend, bool isData=true)
  :offset(offset), type(type), symbol(symbol), addend(addend), isData(isData) {}
};

struct Relocation{
  std::vector<RelocEntry> entries;
};

class RelocationTable{
  public:
    std::unordered_map<std::string, Relocation> table;
    bool sectionExists(std::string sectionName);
    Relocation getRelocation(std::string sectionName);
    void addEntry(std::string sectionName, RelocEntry entry);
    void changeSymbolAndAddend(std::string sectionName, int offset, std::string newSymbol, int newAddend);
    void removeSection(std::string sectionName);
    void removeAllSections();
    bool isEmpty();
    void printToTxt(std::string fileName);
    void printToHelperTxt(std::string fileName);
};