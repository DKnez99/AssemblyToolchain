#include "../inc/LdSectionTable.hpp"
#include <fstream>
#include <iomanip>

//section's existential dread
bool SectionTable::sectionExists(const std::string &sectionName){
  return SectionTable::table.find(sectionName)!=SectionTable::table.end();
}
bool SectionTable::isEmpty(){
  return SectionTable::table.empty();
}

std::unordered_map<std::string, SectionData> SectionTable::getTable(){
  return SectionTable::table;
}
//sectionData
SectionData SectionTable::getSectionData(const std::string &sectionName){
  return SectionTable::table.at(sectionName);
}

void SectionTable::addSectionData(const std::string &sectionName, SectionData sectionData){
  SectionTable::table[sectionName]=sectionData;
}

void SectionTable::addSection(const std::string &sectionName){ //without data
  SectionTable::table[sectionName]=SectionData();
}
//size
unsigned int SectionTable::getSectionSize(const std::string &sectionName){
  return SectionTable::table.at(sectionName).size;
}

void SectionTable::setSectionSize(const std::string &sectionName, unsigned int newSize){
  SectionTable::table.at(sectionName).size=newSize;
}

//memAddr
unsigned int SectionTable::getSectionMemAddr(const std::string &sectionName){
  return SectionTable::table.at(sectionName).memAddr;
}
void SectionTable::setSectionMemAddr(const std::string &sectionName, unsigned int newMemAddr){
  SectionTable::table.at(sectionName).memAddr=newMemAddr;
}
//originFile
std::string SectionTable::getSectionOriginFile(const std::string &sectionName){
  return SectionTable::table.at(sectionName).originFile;
}
void SectionTable::setSectionOriginFile(const std::string &sectionName, std::string newOriginFile){
  SectionTable:;table.at(sectionName).originFile=newOriginFile;
}

//sectionEntries
void SectionTable::addSectionEntry(const std::string &sectionName, SectionEntry entry){
  SectionTable::table[sectionName].entries.push_back(entry);
  SectionTable::table[sectionName].size+=entry.size;
}

std::vector<SectionEntry> SectionTable::getSectionEntries(const std::string &sectioName){
  return SectionTable::table.at(sectioName).entries;
}
//data
std::vector<Data> SectionTable::getDataAtOffset(const std::string &sectionName, unsigned int offset, unsigned int size){
  std::vector<Data> result;
  for (auto &entry : SectionTable::table.at(sectionName).entries) {
    if(offset>=entry.offset && offset<(entry.offset + entry.size)){
      int cnt=0;
      while(cnt<size && cnt<(entry.size-offset+entry.offset)){  //don't go into the next entry to fetch data
        result.push_back(entry.data[offset-entry.offset+cnt++]);
      }
      break;
    }
  }
  return result;
}

void SectionTable::setDataAtOffset(const std::string &sectionName, unsigned int offset, unsigned int size, long newData){
  for (auto &entry : SectionTable::table.at(sectionName).entries) {
    if(offset>=entry.offset && offset<(entry.offset + entry.size)){
      int cnt=0;
      while(cnt<size && cnt<(entry.size-offset+entry.offset)){  //don't go into the next entry to fetch data
        entry.data[offset-entry.offset+cnt++]={(int)(newData >> 4) & 0xF, (int)(newData >> 0) & 0xF};
        newData>>=8;
      }
    }
  }
}
//print
void SectionTable::printToHelperTxt(const std::string &fileName){
  std::ofstream file;
  file.open(fileName, std::ios::app);  //append to file
  for(const auto& section: SectionTable::table){
    std::string sectionName = section.first;
    file<<"#"<<sectionName<<" ("<<std::dec<<section.second.size<<" B) @"<<section.second.memAddr<<", From file '"<<section.second.originFile<<"'";
    file<<"\n"<<std::setw(15)<<std::left<<"Offset"
        <<std::dec<<std::setw(15)<<std::left<<"Size"
        <<std::hex<<std::setw(15)<<std::left<<"Data"
        <<std::endl;
    
    for(const auto& entry: section.second.entries){
      if(entry.size!=0){
        file<<std::dec<<std::setw(15)<<std::left<<entry.offset
            <<std::dec<<std::setw(15)<<std::left<<entry.size;

      for(const auto& data: entry.data){
        file<<std::hex<<std::left<<data.hex1<<data.hex2<<" ";
      }
      file<<"\n";
      }
    }
  }
  file.close();
}