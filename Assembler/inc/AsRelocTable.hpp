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

struct AbsSymbolInfo{
  int value;
  std::string section;
  int offset;
  int size;
};

class RelocationTable{
    std::unordered_map<std::string, std::vector<RelocEntry>> table;
  public:
    //reloc's existential dread
    bool relocationsExistsForSection(const std::string &sectionName);
    bool isEmpty();
    //relocation entries
    std::vector<RelocEntry> getRelocEntriesForSection(const std::string &sectionName);
    void addRelocEntry(const std::string &sectionName, RelocEntry entry);
    //for local symbols which were previously undefined (will get sectName and offset by going through flinks)
    void changeRelocEntriesForLocal(const std::string &sectionName, int offset, std::string newSymbol, int newAddend);
    //for global symbols which were previously local
    //  old symbol = section name in which local symbol was defined
    //  old addend = local symbol's value
    void changeRelocEntriesForGlobal(std::string oldSymbol, int oldAddend, std::string newSymbol, int newAddend);
    //delete absolute symbol relocs and return places in which to insert them in mem
    std::vector<AbsSymbolInfo> getAndDeleteRelocEntriesForAbsolute(); 
    //print
    void printToOutput(const std::string &fileName);
    void printToHelperTxt(const std::string &fileName);
};