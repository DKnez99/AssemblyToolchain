#include "../inc/LdSectionTable.hpp"
#include <iostream>

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

std::vector<std::string> SectionTable::getSectionNames(){
  std::vector<std::string> result;
  for(auto &section: SectionTable::table){
    result.push_back(section.first);
  }
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                      SECTION DATA                       ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

void SectionTable::addSection(const std::string &sectionName){
  if(SectionTable::table.find(sectionName)==SectionTable::table.end()){
    SectionTable::table[sectionName]=SectionData();
  }
}

SectionData SectionTable::getSectionData(const std::string &sectionName){
  return SectionTable::table.at(sectionName);
}

void SectionTable::addSectionData(const std::string &sectionName, SectionData newSectionData){
  SectionTable::table[sectionName]=newSectionData;
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                  FILE SECTION DATA                      ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

FileSectionData SectionTable::getFileSectionData(const std::string &sectionName, const std::string &fileName){
  for(auto &fileSection: SectionTable::table.at(sectionName).fileSections){
    if(fileSection.originFile==fileName)
      return fileSection;
  }
  return FileSectionData();
}

void SectionTable::addFileSectionData(const std::string &sectionName, FileSectionData fileSectionData){
  SectionTable::table[sectionName].size+=fileSectionData.size;
  SectionTable::table[sectionName].fileSections.push_back(fileSectionData);
}

unsigned int SectionTable::getFileSectionAddress(const std::string &sectionName, const std::string &fileName){
  for(auto &fileSection: SectionTable::table.at(sectionName).fileSections){
    if(fileSection.originFile==fileName)
      return fileSection.memoryAddress;
  }
  return 0;
}

void SectionTable::printDataToTerminal(){
  for(auto &wholeSection: SectionTable::table){
    std::cout<<"\nID: "<<wholeSection.second.sectionID;
    std::cout<<"\nSection: "<<wholeSection.first;
    std::cout<<"\nAddress: "<<wholeSection.second.memoryAddress;
    std::cout<<"\nSize: "<<wholeSection.second.size;
    for(auto &fileSection: wholeSection.second.fileSections){
      std::cout<<"\n\tFrom file: "<<fileSection.originFile;
      std::cout<<"\n\tAddress: "<<fileSection.memoryAddress;
      std::cout<<"\n\tSize: "<<fileSection.size;
      std::cout<<"\n\tNo. entries: "<<fileSection.entries.size();
      int i=1;
      for(auto &entry: fileSection.entries){
        std::cout<<"\n\t\tEntry "<<i++;
        std::cout<<"\n\t\tAddress: "<<entry.offset;
        std::cout<<"\n\t\tSize: "<<entry.size;
      }
    }
  }
}

void SectionTable::calculateSectionAddresses(){
  unsigned int currentAddress=0;
  for(int i=0; i<SectionTable::table.size(); i++){
    for(auto &sectionData: SectionTable::table){
      if(sectionData.second.sectionID==i){
        sectionData.second.memoryAddress=currentAddress;
        for(auto &fileSection: sectionData.second.fileSections){
          fileSection.memoryAddress=currentAddress;
          for(auto &entry: fileSection.entries){
            entry.offset=currentAddress;
            currentAddress+=entry.size;
          }
        }
      }
    }
  }
}

std::vector<Data> SectionTable::getDataAtOffset(unsigned int globalOffset, unsigned int size){

}

void SectionTable::setDataAtOffset(unsigned int globalOffset, unsigned int size, long data){

}
void SectionTable::printToHelperTxt(const std::string &fileName){

}