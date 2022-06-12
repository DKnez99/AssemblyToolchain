#include "../inc/Ld.hpp"
#include <iostream>

Linker::Linker(std::vector<std::string> inputFileNames, const std::string &outputFileName, std::unordered_map<std::string, unsigned int> placeAt, bool isRelocatable)
:inputFileNames(inputFileNames), outputFileName(outputFileName), placeSectionAt(placeAt), isRelocatable(isRelocatable), errorOccured(false), warningOccured(false) {
  Linker::helperOutputFileName=outputFileName.substr(0,outputFileName.find_last_of('.'))+"_helper.txt";
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
      symbolTablesForAllFiles.addSymbol(Linker::currentFileName, label, SymbolData(symbolID,section,value,type,isDefined));
    }

    //section table
    int numberOfSections=0;
    inputFile.read((char *)&numberOfSections, sizeof(numberOfSections));
    Linker::writeLineToHelperOutputTxt("Found "+std::to_string(numberOfSections)+" section(s).");
    for(int i=0; i<numberOfSections; i++){
      SectionData sectionData;
      Linker::writeLineToHelperOutputTxt("\tSection: ");
      unsigned int stringLength;
      //sectionName
      inputFile.read((char *)&stringLength, sizeof(stringLength));
      std::string sectionName;
      sectionName.resize(stringLength);
      inputFile.read((char *)sectionName.c_str(), stringLength);
      Linker::writeLineToHelperOutputTxt("\t\tName: "+sectionName);

      //sectionSize
      inputFile.read((char *)&sectionData.size, sizeof(sectionData.size));
      Linker::writeLineToHelperOutputTxt("\t\tSize: "+std::to_string(sectionData.size));
      //entries
      unsigned int numberOfEntries;
      inputFile.read((char *)&numberOfEntries, sizeof(numberOfEntries));  //number of entries
      Linker::writeLineToHelperOutputTxt("\t\tFound "+std::to_string(numberOfEntries)+" entries.");
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
    
    // //relocations
    // int numberOfRelocations=0;
    // inputFile.read((char *)&numberOfRelocations, sizeof(numberOfRelocations));
    // Linker::writeLineToHelperOutputTxt("Found "+std::to_string(numberOfRelocations)+" relocation sections.");
    // for(int i=0; i<numberOfRelocations; i++){
    //   Linker::writeLineToHelperOutputTxt("\tRelocation section:");
    //   unsigned int stringLength;
    //   //sectionName
    //   inputFile.read((char *)&stringLength, sizeof(stringLength));
    //   std::string sectionName;
    //   sectionName.resize(stringLength);
    //   inputFile.read((char *)sectionName.c_str(), stringLength);
    //   Linker::writeLineToHelperOutputTxt("\t\tReloc for section: "+sectionName);

    //   int numberOfRelocEntries=0;
    //   inputFile.read((char *)&numberOfRelocEntries, sizeof(numberOfRelocEntries));
    //   Linker::writeLineToHelperOutputTxt("\t\tFound "+std::to_string(numberOfRelocEntries)+" reloc entries.");
    //   for(int j=0; j<numberOfRelocEntries; j++){
    //   Linker::writeLineToHelperOutputTxt("\t\tReloc entry "+std::to_string(j)+":");
    //     int addend;
    //     RelocType type;
    //     unsigned int offset;
    //     std::string symbol;
    //     bool isData;
    //     inputFile.read((char *)&offset, sizeof(offset));
    //     inputFile.read((char *)&type, sizeof(type));
    //     inputFile.read((char *)&stringLength, sizeof(stringLength));
    //     symbol.resize(stringLength);
    //     inputFile.read((char *)symbol.c_str(), stringLength);
    //     inputFile.read((char *)&addend, sizeof(addend)); 
    //     inputFile.read((char *)&isData, sizeof(isData));
    //     Linker::writeLineToHelperOutputTxt("\t\t\tSymbol: "+symbol);
    //     Linker::writeLineToHelperOutputTxt("\t\t\tType: "+std::to_string(type));
    //     Linker::writeLineToHelperOutputTxt("\t\t\tOffset: "+std::to_string(offset));
    //     Linker::writeLineToHelperOutputTxt("\t\t\tAddend: "+std::to_string(addend));
    //     Linker::writeLineToHelperOutputTxt("\t\t\tIs data?: "+std::to_string(isData));
    //     Linker::fileRelocTables[fileName].addRelocEntry(sectionName, RelocEntry(offset, type, symbol, addend, isData));
    //   }
    // }
    inputFile.close();
  }
  return true;
}

//go through global section table and set addresses
void Linker::calculateSectionAddresses(){
  
}

void Linker::calculateOffsets(){
  // for(auto &sectionName: Linker::globalSectionTable.getSectionNames()){
  //   for(auto &fileSection: Linker::globalSectionTable.getSectionData(sectionName).fileSections){
  //     std::cout<<"\nFile: "<<fileSection.originFile;
  //     std::cout<<"\nUpdating RelocTables...";
  //     if(Linker::fileRelocTables.find(fileSection.originFile)!=Linker::fileRelocTables.end())
  //       Linker::fileRelocTables.at(fileSection.originFile).updateOffsets(sectionName, fileSection.memoryAddress);
  //     std::cout<<"\nRelocTables updated!";
  //     std::cout<<"\nUpdating SymbolTables...";
  //     if(Linker::fileSymbolTables.find(fileSection.originFile)!=Linker::fileSymbolTables.end())
  //       Linker::fileSymbolTables.at(fileSection.originFile).updateSymbolValues(sectionName, fileSection.memoryAddress);
  //     std::cout<<"\nSymbolTables updated!";
  //   }
  // }
}

void Linker::calculateRelocsHex(){
  
}

void Linker::calculateRelocsRelocatable(){

}

void Linker::writeToOutputFiles(){
  Linker::symbolTablesForAllFiles.printToHelperTxt(Linker::helperOutputFileName);
  Linker::sectionTablesForAllFiles.printToHelperTxt(Linker::helperOutputFileName);
}

void Linker::link(){
  Linker::helperOutputFileStream.open(Linker::helperOutputFileName);
  Linker::readFromInputFiles();
  Linker::calculateSectionAddresses();
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