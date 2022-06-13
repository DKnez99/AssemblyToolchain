#pragma once

#include <string>
#include <unordered_map>
#include <vector>

enum  RelocType{      //page 75 in psABI amd64
  R_X86_64_16=0,      //S + A
  R_X86_64_PC16=1,    //S + A - P
};

std::ostream& operator<<(std::ostream& out, RelocType value);

struct RelocEntry{
  unsigned int offset;
  RelocType type; //type of relocation from System V ABI
  std::string symbol;
  int addend;
  bool isData;  //if it's data use little endian, if it's an instruction use big endian
  std::string originFile;
  RelocEntry(unsigned int offset, RelocType type, std::string symbol, int addend, std::string originFile, bool isData=true)
  :offset(offset), type(type), symbol(symbol), addend(addend), originFile(originFile), isData(isData){}
  RelocEntry():offset(0), type(RelocType::R_X86_64_16), symbol(""), addend(0), originFile(""), isData(true){}
};

struct AbsSymbolInfo{
  int value;
  std::string section;
  unsigned int offset;
  unsigned int size;
};

class RelocationTable{
    std::unordered_map<std::string, std::vector<RelocEntry>> table; //section name, reloc entries
  public:
    //reloc's existential dread
    bool relocationsExistsForSection(const std::string &sectionName);
    bool isEmpty();
    //relocation entries
    std::vector<RelocEntry> getRelocEntriesForSection(const std::string &sectionName);
    void addRelocEntry(const std::string &sectionName, RelocEntry entry);
    std::vector<AbsSymbolInfo> getAndDeleteRelocEntriesForAbsolute(); 
    //print
    void printToHelperTxt(const std::string &fileName);
};