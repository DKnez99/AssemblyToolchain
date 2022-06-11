#include "../inc/Ld.hpp"

Linker::Linker(std::vector<std::string> inputFileNames, const std::string &outputFileName, std::unordered_map<std::string, unsigned int> placeAt, bool isRelocatable)
:inputFileNames(inputFileNames), outputFileName(outputFileName), placeAt(placeAt), isRelocatable(isRelocatable) {
  Linker::helperOutputFileName=outputFileName.substr(0,outputFileName.find_last_of('.'))+"_helper.txt";
  Linker::globalSymbolTable=SymbolTable(true);
}

//1. symbol, 2. section, 3. relocs
bool Linker::readFromInputFiles(){
  Linker::writeLineToHelperOutputTxt("READING DATA FROM INPUT FILES");
  for(const auto fileName: Linker::inputFileNames){
    Linker::writeLineToHelperOutputTxt("FILE '"+fileName+"':");
    std::ifstream inputFile(fileName, std::ios::in | std::ios::binary);
    if(inputFile.fail()){
      Linker::addError("Can't find file '"+fileName+"'.");
      return false;
    }

    //symtab
    int numberOfSymbols=0; //no abs and und sections
    inputFile.read((char *)&numberOfSymbols, sizeof(numberOfSymbols));
    Linker::writeLineToHelperOutputTxt("Found "+std::to_string(numberOfSymbols)+" symbol(s) (without undefined and absolute sections).");
    SymbolTable symbolTable;
    for(int i=0; i<numberOfSymbols; i++){
      Linker::writeLineToHelperOutputTxt("\tSymbol :");
      unsigned int stringLength;
      //label
      inputFile.read((char *)&stringLength, sizeof(stringLength));
      std::string label;
      label.resize(stringLength);
      inputFile.read((char *)label.c_str(), stringLength);
      Linker::writeLineToHelperOutputTxt("\t\tLabel: "+label);
      //symbolID
      int symbolID;
      inputFile.read((char *)&symbolID, sizeof(symbolID));
      Linker::writeLineToHelperOutputTxt("\t\tID: "+std::to_string(symbolID));
      //symbolSection
      inputFile.read((char *)&stringLength, sizeof(stringLength));
      std::string section;
      section.resize(stringLength);
      inputFile.read((char *)section.c_str(), stringLength);
      Linker::writeLineToHelperOutputTxt("\t\tSection: "+section);
      //symbolValue
      unsigned int value;
      inputFile.read((char *)(&value), sizeof(value));
      Linker::writeLineToHelperOutputTxt("\t\tValue: "+std::to_string(value));
      //symbolType
      SymbolType type;
      inputFile.read((char *)(&type), sizeof(type));
      Linker::writeLineToHelperOutputTxt("\t\tType: "+std::to_string(type));
      //symbolIsDefined
      bool isDefined;
      inputFile.read((char *)(&isDefined), sizeof(isDefined));
      Linker::writeLineToHelperOutputTxt("\t\tIsDefined: "+std::to_string(isDefined));
      symbolTable.addSymbol(label, SymbolData(symbolID,section,value,type,isDefined, fileName));
    }  
    Linker::fileSymbolTables[fileName]=symbolTable;

    //section table
    int numberOfSections=0;
    inputFile.read((char *)&numberOfSections, sizeof(numberOfSections));
    Linker::writeLineToHelperOutputTxt("Found "+std::to_string(numberOfSections)+" section(s).");
    for(int i=0; i<numberOfSections; i++){
      FileSectionData fileSectionData;
      fileSectionData.originFile=fileName;
      Linker::writeLineToHelperOutputTxt("\tSection: ");
      unsigned int stringLength;
      //sectionName
      inputFile.read((char *)&stringLength, sizeof(stringLength));
      std::string sectionName;
      sectionName.resize(stringLength);
      inputFile.read((char *)sectionName.c_str(), stringLength);
      Linker::writeLineToHelperOutputTxt("\t\tName: "+sectionName);

      //sectionSize
      unsigned int sectionSize;
      inputFile.read((char *)&sectionSize, sizeof(sectionSize));
      Linker::writeLineToHelperOutputTxt("\t\tSize: "+std::to_string(sectionSize));
      fileSectionData.size=sectionSize;
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
        fileSectionData.entries.push_back(entry);
        fileSectionData.size+=entry.size;
      }
      Linker::globalSectionTable.addFileSectionData(sectionName, fileSectionData);
    }
    
    //relocations
    inputFile.close();
  }
  return true;
}

bool Linker::calculateOffsets(){
  return true;
}
bool Linker::calculateRelocs(){
  return true;
}

void Linker::writeToOutputFile(){
  
}

void Linker::link(){
  Linker::helperOutputFileStream.open(Linker::helperOutputFileName);
  Linker::readFromInputFiles();
  Linker::calculateOffsets();
  Linker::calculateRelocs();
  Linker::printResults();
  Linker::writeToOutputFile();
  Linker::helperOutputFileStream.close();
}