#include "../inc/AsSectionTable.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

//constr
// SectionTable::SectionTable(){
//   SectionTable::addSectionEntry(SECTION_ABSOLUTE, SectionEntry());
//   SectionTable::addSectionEntry(SECTION_UNDEFINED, SectionEntry());
// }

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||               SEC'S EXISTENTIAL DREAD                   ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

bool SectionTable::sectionExists(const std::string &sectionName){
  return SectionTable::table.find(sectionName)!=SectionTable::table.end();
}

bool SectionTable::isEmpty(){
  return SectionTable::table.empty();
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                    SECTION ENTRIES                      ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

SectionData SectionTable::getSectionData(const std::string &sectionName){
  return SectionTable::table.at(sectionName);
}

void SectionTable::addSectionEntry(const std::string &sectionName, SectionEntry entry){
  SectionTable::table[sectionName].entries.push_back(entry);
  SectionTable::table[sectionName].size+=entry.size;
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                         DATA                            ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

std::vector<Data> SectionTable::getDataAtOffset(const std::string &sectionName, int offset, int size){
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

void SectionTable::setSectionDataAtOffset(const std::string &sectionName, int offset, int size, int newData){
  for (auto &entry : SectionTable::table.at(sectionName).entries) {
    if(offset>=entry.offset && offset<(entry.offset + entry.size)){
      int cnt=0;
      while(cnt<size && cnt<(entry.size-offset+entry.offset)){  //don't go into the next entry to fetch data
        entry.data[offset-entry.offset+cnt++]={(newData >> 4) & 0xF, (newData >> 0) & 0xF};
        newData>>=8;
      }
    }
  }
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                        PRINT                            ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

void SectionTable::printToOutput(const std::string &fileName){
  std::ofstream file;
  file.open(fileName, std::ios_base::app);  //append to file
  for(const auto& section: SectionTable::table){
      if(section.second.size>0){
      std::string sectionName = section.first;
      file<<"#"<<sectionName<<" ("<<std::dec<<section.second.size<<" B)\n";
      int cnt=0;
      for(const auto& entry: section.second.entries){
        if(entry.size!=0){
          for(const auto& data: entry.data){
            file<<std::hex<<std::left<<data.hex1<<data.hex2<<" ";
            if(cnt==3)
              file<<" ";
            if(cnt++==7){
              file<<"\n";
              cnt=0;
            }
          }
        }
      }
      if(cnt!=0)
        file<<"\n";
    }
  }
  file.close();
}

void SectionTable::printToHelperTxt(const std::string &fileName){
  std::ofstream file;
  file.open(fileName, std::ios_base::app);  //append to file
  for(const auto& section: SectionTable::table){
    std::string sectionName = section.first;
    file<<"#"<<sectionName<<" ("<<std::dec<<section.second.size<<" B)";
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