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
        bool isRealSectionSymb;
        inputFile.read((char *)&offset, sizeof(offset));
        inputFile.read((char *)&type, sizeof(type));
        inputFile.read((char *)&stringLength, sizeof(stringLength));
        symbol.resize(stringLength);
        inputFile.read((char *)symbol.c_str(), stringLength);
        inputFile.read((char *)&addend, sizeof(addend)); 
        inputFile.read((char *)&isData, sizeof(isData));
        inputFile.read((char *)&isRealSectionSymb, sizeof(isRealSectionSymb));
        Linker::relocationTablesForAllFiles.addRelocEntry(Linker::currentFileName, sectionName, RelocEntry(offset, type, symbol, addend, Linker::currentFileName, isData, isRealSectionSymb));
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
      unsigned int offsetAdder=relativeSectionAddressMap[Linker::currentSection];
      for(auto &sectionEntry:sectionTable.second.entries){
        sectionEntry.offset+=offsetAdder; //newly added
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
  Linker::writeLineToHelperOutputTxt("\nCALCULATING ALL ADDRESSES FOR SECTIONS");
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
  Linker::writeLineToHelperOutputTxt("\nCALCULATING PLACE AT ADDRESSES FOR SECTIONS");
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

void Linker::calculateSymbolOffsets(){  //for symbol table
  Linker::writeLineToHelperOutputTxt("\nCALCULATING NEW, GLOBAL SYMBOL OFFSETS");
  //sections
  Linker::writeLineToHelperOutputTxt("\nSetting section values");
  for(auto &section: Linker::globalSectionTable.getTable()){
    Linker::writeLineToHelperOutputTxt("Setting value of symbol "+section.first+" in symbol table to "+std::to_string(section.second.memAddr));
    Linker::globalSymbolTable.setSymbolValue(section.first, section.second.memAddr);
  }

  //other symbols
  Linker::writeLineToHelperOutputTxt("\nSetting other symbol values");
  for(auto &symbol: Linker::globalSymbolTable.getTable()){
    if(symbol.second.type!=SymbolType::SECTION){ //if not already fixed
      unsigned int newSymbolValue = symbol.second.value + Linker::sectionTablesForAllFiles.getSectionMemAddr(symbol.second.originFile, symbol.second.section);
      Linker::writeLineToHelperOutputTxt("Setting value of symbol "+symbol.first+" in symbol table to "+std::to_string(newSymbolValue));
      Linker::globalSymbolTable.setSymbolValue(symbol.first, newSymbolValue);
      Linker::globalSymbolTable.setSymbolOriginFIle(symbol.first, Linker::outputFileName);
    }
    else{
      Linker::globalSymbolTable.setSymbolOriginFIle(symbol.first, Linker::outputFileName);
    }
  }
}

void Linker::calculateRelocOffsetsAndAddends(){
  Linker::writeLineToHelperOutputTxt("\nCALCULATING NEW RELOC OFFSETS AND ADDENDS");
  for(auto &fileName: Linker::inputFileNames){
    Linker::writeLineToHelperOutputTxt("Going through file "+fileName);
    Linker::currentFileName=fileName;
    if(Linker::relocationTablesForAllFiles.sectionTableExists(fileName)){//maybe file doesn't have reloc table
      for(auto &relocTable:Linker::relocationTablesForAllFiles.getRelocationTable(fileName).getTable()){
        Linker::currentSection=relocTable.first;
        Linker::writeLineToHelperOutputTxt("Going through reloc section "+Linker::currentSection);
        unsigned int offsetIncrease=Linker::sectionTablesForAllFiles.getSectionMemAddr(Linker::currentFileName, Linker::currentSection);
        Linker::relocationTablesForAllFiles.increaseOffsetsForFileAndSection(Linker::currentFileName, Linker::currentSection, offsetIncrease);
        Linker::writeLineToHelperOutputTxt("Increasing offset of all reloc entries by "+std::to_string(offsetIncrease));
        for(auto &symbol:Linker::globalSymbolTable.getSymbolsOfType(SymbolType::SECTION)){
          if(Linker::globalSymbolTable.getSymbolID(symbol)>0){  //skip undefined and absolute section
            try{
              unsigned int addendIncrease=Linker::sectionTablesForAllFiles.getSectionMemAddr(Linker::currentFileName, symbol);
              Linker::writeLineToHelperOutputTxt("Increasing addend of reloc entries with symbol "+symbol+" by "+std::to_string(addendIncrease));
              Linker::relocationTablesForAllFiles.increaseAddendsForFileAndSection(Linker::currentFileName, Linker::currentSection, symbol, addendIncrease);
            }
            catch(...){
              //if filename doesn't have that section
            }  
          }
        }
      }
    } 
  }
}

void Linker::calculateRelocsHex(){  //test
  Linker::writeLineToHelperOutputTxt("\nSOLVING HEX RELOCATIONS");
  for(auto &relocSection: Linker::globalRelocTable.getTable()){
    for(auto &entry: relocSection.second){
      unsigned int size=0;
      long data=0;
      if(entry.type==RelocType::R_X86_64_16){
        //std::cout<<"Symbol ("+entry.symbol+") = "<<Linker::globalSymbolTable.getSymbolValue(entry.symbol)<<std::endl;
        //std::cout<<"Addend = "<<entry.addend<<std::endl;
        size=2;
        data=Linker::globalSymbolTable.getSymbolValue(entry.symbol)+entry.addend;
        //std::cout<<"Data (S+A) = "<<data<<std::endl;
      }
      else if(entry.type==RelocType::R_X86_64_PC16){
        size=2;
       // std::cout<<"Symbol ("+entry.symbol+") = "<<Linker::globalSymbolTable.getSymbolValue(entry.symbol)<<std::endl;
        //std::cout<<"Addend = "<<entry.addend<<std::endl;
        //std::cout<<"Offset = "<<entry.offset<<std::endl;
        data=(int)Linker::globalSymbolTable.getSymbolValue(entry.symbol)+entry.addend-(int)entry.offset;
        //std::cout<<"Data (S+A-O) = "<<data<<std::endl;
      }
      if(entry.isData){
        Linker::writeLineToHelperOutputTxt("Inserting data "+std::to_string(data)+"(DEC) to section '"+relocSection.first+"' at offset '"+std::to_string(entry.offset)+"' backwards.");
      }
      else{
        Linker::writeLineToHelperOutputTxt("Inserting data "+std::to_string(data)+"(DEC) to section '"+relocSection.first+"' at offset '"+std::to_string(entry.offset)+"' forwards.");
      }
      Linker::globalSectionTable.setDataAtOffset(relocSection.first, entry.offset, size, data, entry.isData);
    }
  }
}

void Linker::calculateRelocsRelocatable(){

}

void Linker::printInputTables(){
  Linker::writeLineToHelperOutputTxt("\nINPUT TABLES:");
  Linker::helperOutputFileStream.close();
  Linker::symbolTablesForAllFiles.printToHelperTxt(Linker::helperOutputFileName);
  Linker::sectionTablesForAllFiles.printToHelperTxt(Linker::helperOutputFileName);
  Linker::relocationTablesForAllFiles.printToHelperTxt(Linker::helperOutputFileName);
  Linker::helperOutputFileStream.open(Linker::helperOutputFileName, std::ios::app);
}

void Linker::printHelperOutputTables(){
  Linker::writeLineToHelperOutputTxt("\nGLOBAL LD TABLES:");
  Linker::helperOutputFileStream.close();
  Linker::globalSymbolTable.printToHelperTxt(Linker::helperOutputFileName);
  Linker::globalSectionTable.printToHelperTxt(Linker::helperOutputFileName);
  Linker::globalRelocTable.printToHelperTxt(Linker::helperOutputFileName);
  Linker::helperOutputFileStream.open(Linker::helperOutputFileName, std::ios::app);
}

void Linker::writeToOutputFiles(){

}

void Linker::link(){
  Linker::helperOutputFileStream.open(Linker::helperOutputFileName);
  Linker::readFromInputFiles();
  Linker::printInputTables();
  Linker::createGlobalSectionTable();
  Linker::calculateAllSectionAddresses();
  Linker::createGlobalSymbolTable();
  Linker::calculateSymbolOffsets();
  Linker::calculateRelocOffsetsAndAddends();
  Linker::createGlobalRelocTable();
  if(Linker::isRelocatable){
    Linker::calculateRelocsRelocatable();
  }
  else{
    Linker::calculateRelocsHex();
  }
  Linker::printHelperOutputTables();
  Linker::printResults();
  Linker::writeToOutputFiles();
  Linker::helperOutputFileStream.close();
}