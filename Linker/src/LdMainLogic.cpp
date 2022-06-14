#include "../inc/Ld.hpp"
#include <iostream>

Linker::Linker(std::vector<std::string> inputFileNames, const std::string &outputFileName, std::unordered_map<std::string, unsigned int> placeAt, bool isRelocatable)
:inputFileNames(inputFileNames), outputFileName(outputFileName), placeSectionAt(placeAt), isRelocatable(isRelocatable), errorOccured(false), warningOccured(false) {
  Linker::helperOutputFileName=outputFileName.substr(0,outputFileName.find_last_of('.'))+"_helper.txt";
  Linker::memoryMappedRegisters=0xFF00;
}

//1. symbol, 2. section, 3. relocs
bool Linker::readFromInputFiles(){
  Linker::writeLineToHelperOutputTxt("READING DATA FROM INPUT FILES");
  for(const auto fileName: Linker::inputFileNames){
    Linker::currentFileName=fileName;
    Linker::writeLineToHelperOutputTxt("FILE '"+Linker::currentFileName+"':");
    std::ifstream inputFile(Linker::currentFileName, std::ios::in | std::ios::binary);
    if(inputFile.fail()){
      Linker::addError("Can't find file '"+Linker::currentFileName+"'.");
      return false;
    }

    //symtab
    int numberOfSymbols=0;
    inputFile.read((char *)&numberOfSymbols, sizeof(numberOfSymbols));
    Linker::writeLineToHelperOutputTxt("Reading "+std::to_string(numberOfSymbols)+" symbol(s).");
    SymbolTable symbolTable;
    for(int i=0; i<numberOfSymbols; i++){
      unsigned int stringLength;
      //label
      inputFile.read((char *)&stringLength, sizeof(stringLength));
      std::string label;
      label.resize(stringLength);
      inputFile.read((char *)label.c_str(), stringLength);
      //symbolID
      int symbolID;
      inputFile.read((char *)&symbolID, sizeof(symbolID));
      //symbolSection
      inputFile.read((char *)&stringLength, sizeof(stringLength));
      std::string section;
      section.resize(stringLength);
      inputFile.read((char *)section.c_str(), stringLength);
      //symbolValue
      unsigned int value;
      inputFile.read((char *)(&value), sizeof(value));
      //symbolType
      SymbolType type;
      inputFile.read((char *)(&type), sizeof(type));
      //symbolIsDefined
      bool isDefined;
      inputFile.read((char *)(&isDefined), sizeof(isDefined));
      symbolTablesForAllFiles.addSymbol(Linker::currentFileName, label, SymbolData(symbolID,section,value,type,isDefined, Linker::currentFileName));
    }

    //section table
    int numberOfSections=0;
    inputFile.read((char *)&numberOfSections, sizeof(numberOfSections));
    Linker::writeLineToHelperOutputTxt("Reading "+std::to_string(numberOfSections)+" section(s).");
    for(int i=0; i<numberOfSections; i++){
      SectionData sectionData;
      sectionData.originFile=Linker::currentFileName;
      unsigned int stringLength;
      //sectionName
      inputFile.read((char *)&stringLength, sizeof(stringLength));
      std::string sectionName;
      sectionName.resize(stringLength);
      inputFile.read((char *)sectionName.c_str(), stringLength);

      //sectionSize
      inputFile.read((char *)&sectionData.size, sizeof(sectionData.size));
      //entries
      unsigned int numberOfEntries;
      inputFile.read((char *)&numberOfEntries, sizeof(numberOfEntries));  //number of entries
      for(int j=0; j<numberOfEntries; j++){
        SectionEntry entry;
        inputFile.read((char *)&entry.offset, sizeof(entry.offset));                //entry offset
        inputFile.read((char *)&entry.size, sizeof(entry.size));                    //entry size
        inputFile.read((char *)&entry.isData, sizeof(entry.isData));                //entry isData
        //data
        unsigned int numberOfDataEntries;
        inputFile.read((char *)&numberOfDataEntries, sizeof(numberOfDataEntries));  //size of data
        for(int k=0; k<numberOfDataEntries; k++){
          int hex1, hex2;
          inputFile.read((char *)&hex1, sizeof(hex1));                      //hex1
          inputFile.read((char *)&hex2, sizeof(hex2));                      //hex2
          entry.data.push_back({hex1,hex2});
        }
        sectionData.entries.push_back(entry); //we already read size
      }
      Linker::sectionTablesForAllFiles.addSectionData(Linker::currentFileName, sectionName, sectionData);
    }
    
    //relocations
    int numberOfRelocations=0;
    inputFile.read((char *)&numberOfRelocations, sizeof(numberOfRelocations));
    Linker::writeLineToHelperOutputTxt("Reading "+std::to_string(numberOfRelocations)+" relocation section(s).");
    for(int i=0; i<numberOfRelocations; i++){
      unsigned int stringLength;
      //sectionName
      inputFile.read((char *)&stringLength, sizeof(stringLength));
      std::string sectionName;
      sectionName.resize(stringLength);
      inputFile.read((char *)sectionName.c_str(), stringLength);

      int numberOfRelocEntries=0;
      inputFile.read((char *)&numberOfRelocEntries, sizeof(numberOfRelocEntries));
      for(int j=0; j<numberOfRelocEntries; j++){
        int addend;
        RelocType type;
        unsigned int offset;
        std::string symbol;
        bool isData;
        inputFile.read((char *)&offset, sizeof(offset));
        inputFile.read((char *)&type, sizeof(type));
        inputFile.read((char *)&stringLength, sizeof(stringLength));
        symbol.resize(stringLength);
        inputFile.read((char *)symbol.c_str(), stringLength);
        inputFile.read((char *)&addend, sizeof(addend)); 
        inputFile.read((char *)&isData, sizeof(isData));
        Linker::relocationTablesForAllFiles.addRelocEntry(Linker::currentFileName, sectionName, RelocEntry(offset, type, symbol, addend, Linker::currentFileName, isData));
      }
    }
    inputFile.close();
  }
  return true;
}

bool Linker::createGlobalSectionTable(){
  Linker::writeLineToHelperOutputTxt("CREATING GLOBAL SECTIONS TABLE");
  std::unordered_map<std::string, unsigned int> relativeSectionAddressMap;
  for(auto &fileName:Linker::inputFileNames){
    Linker::currentFileName=fileName;
    for(auto &sectionTable: Linker::sectionTablesForAllFiles.getSectionTable(Linker::currentFileName).getTable()){
      Linker::currentSection=sectionTable.first;
      sectionTable.second.originFile=Linker::outputFileName;
      if(!Linker::globalSectionTable.sectionExists(Linker::currentSection)){//if we haven't encountered the section yet, add it
        Linker::globalSectionTable.addSectionData(Linker::currentSection, SectionData(0,0,Linker::outputFileName));
        relativeSectionAddressMap[Linker::currentSection]=0;
      }
      Linker::sectionTablesForAllFiles.setSectionMemAddr(Linker::currentFileName, Linker::currentSection, relativeSectionAddressMap[Linker::currentSection]);
      for(auto &sectionEntry:sectionTable.second.entries){
        Linker::globalSectionTable.addSectionEntry(Linker::currentSection, sectionEntry);
        relativeSectionAddressMap[Linker::currentSection]+=sectionEntry.size;
      }
    }
  }
  return true;
}

bool Linker::createGlobalSymbolTable(){
  int symbolID=-1;
  std::unordered_map<std::string, SymbolType> doneSymbols;  //prob unnecessary
  for(auto &fileName: Linker::inputFileNames){
    Linker::currentFileName=fileName;
    for(auto &symbol: Linker::symbolTablesForAllFiles.getSymbolTable(fileName).getTable()){
      if(symbol.second.type==SymbolType::LOCAL) //skip local symbols
        continue;

      if(Linker::globalSymbolTable.symbolExists(symbol.first)){
        if(symbol.second.type==SymbolType::SECTION){
          continue;
        }

        SymbolData alreadyExistingSymbol = Linker::globalSymbolTable.getSymbol(symbol.first);
        if(alreadyExistingSymbol.type==SymbolType::GLOBAL){ //test if isDefined needs checking
          if(symbol.second.type==SymbolType::EXTERN){
            continue;
          }
          else{
            Linker::addError("Symbol '"+symbol.first+"' is defined both in file '"+alreadyExistingSymbol.originFile+"' and file '"+symbol.second.originFile+"'.");
            return false;
          }
        }

        if(alreadyExistingSymbol.type==SymbolType::EXTERN){//if symbol extern, check if existing is
          symbol.second.symbolID=Linker::globalSymbolTable.getSymbolID(symbol.first);
          //maybe calculate symbol offset immediately?
          Linker::globalSymbolTable.removeSymbol(symbol.first);
          Linker::globalSymbolTable.addSymbol(symbol.first, symbol.second);
        }
      }
      else{ //add it
        //maybe calculate symbol offset immediately?
        symbol.second.symbolID=symbolID++;
        Linker::globalSymbolTable.addSymbol(symbol.first, symbol.second);
      }
    }
  }
  //after going through all symbols, check if there are still unresolved ones
  for(auto &symbol:Linker::globalSymbolTable.getTable()){
    if(symbol.second.isDefined==false || symbol.second.type==SymbolType::EXTERN){
      Linker::addError("Symbol '"+symbol.first+"' can't be resolved.");
      return false;
    }
  }
  return true;
}

bool Linker::createGlobalRelocTable(){
  for(auto &fileName: Linker::inputFileNames){
    Linker::currentFileName=fileName;
    for(auto &relocs: Linker::relocationTablesForAllFiles.getRelocationTable(Linker::currentFileName).getTable()){
      for(auto &reloc: relocs.second){
        Linker::globalRelocTable.addRelocEntry(relocs.first, reloc);
      }
    }
  }
  return true;
}

//must take into account placeAt
//sections not included in placeAt option are placed starting from the next available address after highest placeAt option
bool Linker::calculateAllSectionAddresses(){
  Linker::writeLineToHelperOutputTxt("CALCULATING ALL ADDRESSES FOR SECTIONS");
  std::unordered_map<std::string, bool> doneSections; //map of done sections, so we dont go through them again in diff files
  if(!Linker::isRelocatable){
    if(!Linker::calculatePlaceAtSectionAddresses()){
      return false;
    }
    for(auto &section:Linker::placeSectionAt){
      doneSections.insert({section.first, true});
    }
  }

  
  //calculate addresses for sections not included in placeAt
  for(auto &fileName: Linker::inputFileNames){  //go in order of sections being mentioned in input files
    Linker::currentFileName=fileName;
    for(auto &section:Linker::sectionTablesForAllFiles.getSectionTable(Linker::currentFileName).getTable()){
      Linker::currentSection=section.first;
      if(doneSections.find(Linker::currentSection)==doneSections.end()){ //if it's not already placed
        Linker::globalSectionTable.setSectionMemAddr(Linker::currentSection, Linker::highestAddress);
        Linker::highestAddress+=Linker::globalSectionTable.getSectionSize(Linker::currentSection);
        if(Linker::highestAddress>=Linker::memoryMappedRegisters){
          Linker::addError("Section '"+Linker::currentSection+"' overlaps into memory mapped registers at addr. 65280 (0xFF00).");
          return false;
        }
        doneSections.insert({Linker::currentSection, true});
      }
    }
  }

  return true;
}

bool Linker::calculatePlaceAtSectionAddresses(){
  Linker::writeLineToHelperOutputTxt("CALCULATING PLACE AT ADDRESSES FOR SECTIONS");
  Linker::highestAddress=0;
  for(auto &placeAt:Linker::placeSectionAt){//placing them
    Linker::writeLineToHelperOutputTxt("Going through placeAt option "+placeAt.first+"@"+std::to_string(placeAt.second));
    if(Linker::globalSectionTable.sectionExists(placeAt.first)){
      Linker::globalSectionTable.setSectionMemAddr(placeAt.first, placeAt.second);
      if(placeAt.second+Linker::globalSectionTable.getSectionSize(placeAt.first)>Linker::highestAddress)
        Linker::highestAddress=placeAt.second+Linker::globalSectionTable.getSectionSize(placeAt.first);
    }
    else{
      Linker::addError("Section '"+placeAt.first+"' mentioned in -placeAt option doesn't exist.");
      return false;
    }
  }
  //check for intersections
  for(auto &sectionOne:Linker::placeSectionAt){ //ineffiecient
    if(Linker::globalSectionTable.getSectionData(sectionOne.first).memAddr+Linker::globalSectionTable.getSectionData(sectionOne.first).size >= Linker::memoryMappedRegisters){
      Linker::addError("Section '"+sectionOne.first+"' overlaps with memory mapped registers at addr. 65280 (0xFF00).");
      return false;
    }
    for(auto &sectionTwo:Linker::placeSectionAt){
      if(sectionOne.first!=sectionTwo.first){
        if(Linker::sectionsIntersect(sectionOne.second, Linker::globalSectionTable.getSectionSize(sectionOne.first),
        sectionTwo.second, Linker::globalSectionTable.getSectionSize(sectionTwo.first))){
          Linker::addError("Sections '"+sectionOne.first+"' and '"+sectionTwo.first+"' overlap.");
          return false;
        }
      }
    }
  }
  return true;
}

void Linker::calculateOffsets(){  //for reloc and symbols table
  Linker::writeLineToHelperOutputTxt("CALCULATING NEW, GLOBAL OFFSETS");
  for(auto &fileName: Linker::inputFileNames){
    Linker::currentFileName=fileName;
    //relocs
    for(auto &relocSection: Linker::globalRelocTable.getTable()){
      Linker::currentSection=relocSection.first;
      try{ //index out of bounds can happen
        unsigned int addOffset=Linker::sectionTablesForAllFiles.getSectionMemAddr(Linker::currentFileName, Linker::currentSection);
        Linker::globalRelocTable.increaseOffsetBy(Linker::currentSection, Linker::currentFileName, addOffset);
        Linker::writeLineToHelperOutputTxt("Increased offsets of reloc entries in section '"+Linker::currentSection+"' from file '"+Linker::currentFileName+"' by "+std::to_string(addOffset));
      }
      catch(...){

      }
    }
    //symbols
    for(auto &symbol: Linker::globalSymbolTable.getTable()){
      if(symbol.second.symbolID>0){ //skip undefined and absolute symbol
        Linker::currentSection=symbol.second.section;
        try{ //index out of bounds can happen
          unsigned int addOffset=Linker::sectionTablesForAllFiles.getSectionMemAddr(Linker::currentFileName, Linker::currentSection);
          Linker::globalSymbolTable.increaseValueBy(Linker::currentSection, Linker::currentFileName, addOffset);
          Linker::writeLineToHelperOutputTxt("Increased offsets of symbols defined in section '"+Linker::currentSection+"', of file '"+Linker::currentFileName+"' by "+std::to_string(addOffset));
        }
        catch(...){

        }
      }
    }
  }
}

void Linker::calculateRelocsHex(){
  
}

void Linker::calculateRelocsRelocatable(){

}

void Linker::writeToOutputFiles(){
  Linker::writeLineToHelperOutputTxt("CONJOINED TABLES:");
  Linker::helperOutputFileStream.close();

  Linker::symbolTablesForAllFiles.printToHelperTxt(Linker::helperOutputFileName);
  Linker::sectionTablesForAllFiles.printToHelperTxt(Linker::helperOutputFileName);
  Linker::relocationTablesForAllFiles.printToHelperTxt(Linker::helperOutputFileName);

  Linker::helperOutputFileStream.open(Linker::helperOutputFileName, std::ios::app);
  Linker::writeLineToHelperOutputTxt("GLOBAL TABLES:");
  Linker::helperOutputFileStream.close();
  Linker::globalSymbolTable.printToHelperTxt(Linker::helperOutputFileName);
  Linker::globalSectionTable.printToHelperTxt(Linker::helperOutputFileName);
  Linker::globalRelocTable.printToHelperTxt(Linker::helperOutputFileName);
  Linker::helperOutputFileStream.open(Linker::helperOutputFileName, std::ios::app);
}

void Linker::link(){
  Linker::helperOutputFileStream.open(Linker::helperOutputFileName);
  Linker::readFromInputFiles();
  Linker::createGlobalSectionTable();
  Linker::calculateAllSectionAddresses();
  Linker::createGlobalSymbolTable();
  Linker::createGlobalRelocTable();
  Linker::calculateOffsets();
  if(Linker::isRelocatable){
    Linker::calculateRelocsRelocatable();
  }
  else{
    Linker::calculateRelocsHex();
  }
  Linker::printResults();
  Linker::writeToOutputFiles();
  Linker::helperOutputFileStream.close();
}