#include "../inc/LdRelocTable.hpp"

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||          RELOC TABLE's EXISTENTIAL DREAD                ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

bool RelocationTable::relocationsExistsForSection(const std::string &sectionName){
  return RelocationTable::table.find(sectionName)!=RelocationTable::table.end();
}

bool RelocationTable::isEmpty(){
  return RelocationTable::table.empty();
}



std::vector<RelocEntry> RelocationTable::getRelocEntriesForSection(const std::string &sectionName){
  return RelocationTable::table.at(sectionName);
}

void RelocationTable::addRelocEntry(const std::string &sectionName, RelocEntry entry){
  RelocationTable::table[sectionName].push_back(entry);
}

//should be done after sections are sorted
void RelocationTable::updateOffsets(const std::string &sectionName, unsigned int addOffset){

}


void RelocationTable::printToHelperTxt(const std::string &fileName){
  
}