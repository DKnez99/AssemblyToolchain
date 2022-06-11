#include "../inc/LdRelocTable.hpp"

bool RelocationTable::relocationsExistsForSection(const std::string &sectionName){

}

bool RelocationTable::isEmpty(){

}


std::vector<RelocEntry> RelocationTable::getRelocEntriesForSection(const std::string &sectionName){

}

void RelocationTable::addRelocEntry(const std::string &sectionName, RelocEntry entry){

}

//should be done after sections are sorted
void RelocationTable::updateOffsets(const std::string &sectionName, unsigned int addOffset){

}


void RelocationTable::printToHelperTxt(const std::string &fileName){
  
}