#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#define SECTION_UNDEFINED "*UND*"
#define SECTION_ABSOLUTE "*ABS*"

struct Data{
  int hex1;
  int hex2;
  bool operator==(Data d) const {
    return d.hex1==hex1 && d.hex2==hex2;
  }
};

struct SectionEntry{
  int offset;
  int size;
  std::vector<Data> data;
  SectionEntry(int offset, int size, long data_, bool isInstruction=false):offset(offset),size(size){
    if(isInstruction){ //big endian (left -> right)
      for(int i=size-1; i>=0; i--){  
        data.push_back({int((data_ >>(i*8+4)) & 0xF), int((data_ >> (i*8)) & 0xF) });
      }
    }
    else{
      for(int i=0; i<size; i++){  //little endian (left <- right)
        data.push_back({int((data_ >>4) & 0xF), int((data_ >> 0) & 0xF) });
        data_>>=8;
      }
    }
  }
  SectionEntry():offset(0),size(0){}
};

struct SectionData{
  int size; //all entry sizes added up
  std::vector<SectionEntry> entries;
};

class SectionTable{
  private:
    std::unordered_map<std::string, SectionData> table;
  public:
    SectionTable();
    bool sectionExists(std::string sectionName);
    SectionData getSection(std::string sectionName);
    void addSection(std::string sectionName, SectionEntry entry);
    std::vector<Data> getSectionData(std::string sectionName, int offset, int size);
    void setSectionData(std::string sectionName, int offset, int size, int data);
    void removeSection(std::string sectionName);
    bool isEmpty();
    void printToTxt(std::string fileName);
    void printToHelperTxt(std::string fileName);
};