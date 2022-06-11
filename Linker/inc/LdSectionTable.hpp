#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#define SECTION_UNDEFINED ".undefined"
#define SECTION_ABSOLUTE ".absolute"

struct Data{
  int hex1;
  int hex2;
  bool operator==(Data d) const {
    return d.hex1==hex1 && d.hex2==hex2;
  }
};

struct SectionEntry{
  unsigned int offset;  //should add to this rel + parent address at some point
  unsigned int size;
  bool isData;
  std::vector<Data> data;
  SectionEntry(unsigned int offset, unsigned int size, long data_, bool isData=true):offset(offset),size(size), isData(isData){
    if(isData){ //little endian (left <- right)
      for(int i=0; i<size; i++){  
        data.push_back({int((data_ >>4) & 0xF), int((data_ >> 0) & 0xF) });
        data_>>=8;
      }
    }
    else{ //big endian (left -> right)
      for(int i=size-1; i>=0; i--){  
        data.push_back({int((data_ >>(i*8+4)) & 0xF), int((data_ >> (i*8)) & 0xF) });
      }
    }
  }
  SectionEntry():offset(0),size(0),isData(true){}
};

struct FileSectionData{                       //section from a single file
  unsigned int size;                          //all entry sizes added up
  unsigned int memoryAddress;                 //sectionData mem address + previousElementsSizes
  std::string originFile;
  std::vector<SectionEntry> entries;
  FileSectionData(unsigned int size, std::string originFile):size(size), originFile(originFile){}
  FileSectionData():size(0),memoryAddress(0), originFile(""){}
};

static int globalSectionID=0;

struct SectionData{
  unsigned int size;
  unsigned int memoryAddress;
  int sectionID;
  std::vector<FileSectionData> fileSections;
  SectionData(unsigned int size, unsigned int memAddr):size(size),memoryAddress(memAddr),sectionID(globalSectionID++){}
  SectionData():size(0),memoryAddress(0),sectionID(globalSectionID++){}
};

class SectionTable{
  private:
    std::unordered_map<std::string, SectionData> table;  //section name, vector of those sections across different files
  public:
    //section's existential dread
    bool sectionExists(const std::string &sectionName);
    void addSection(const std::string &sectionName); //empty section
    bool isEmpty();
    //sectionData
    SectionData* getSectionDataByID(int id);
    SectionData getSectionData(const std::string &sectionName);
    void addSectionData(const std::string &sectionName, SectionData newSectionData);
    //FileSectionData
    FileSectionData getFileSectionData(const std::string &sectionName, const std::string &fileName);
    void addFileSectionData(const std::string &sectionName, FileSectionData fileSectionData);
    unsigned int getFileSectionAddress(const std::string &sectionName, const std::string &fileName);  //needed to calc new offsets in symb table
    //calcs
    void calculateSectionAddresses();
    //data 
    std::vector<Data> getDataAtOffset(unsigned int globalOffset, unsigned int size);
    void setDataAtOffset(unsigned int globalOffset, unsigned int size, long data);  //after sections have been adjusted
    //print
    void printToHelperTxt(const std::string &fileName);
    void printDataToTerminal();
};