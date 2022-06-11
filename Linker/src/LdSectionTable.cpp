#include "../inc/LdSectionTable.hpp"

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

void SectionTable::calculateSectionAddresses(){
  
}
std::vector<Data> SectionTable::getDataAtOffset(unsigned int globalOffset, unsigned int size){

}
void SectionTable::setDataAtOffset(unsigned int globalOffset, unsigned int size, long data){

}
void SectionTable::printToHelperTxt(const std::string &fileName){

}