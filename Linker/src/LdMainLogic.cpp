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
    std::ifstream inputFile(fileName, std::ios::binary);
    if(inputFile.fail()){
      Linker::addError("Can't find file '"+fileName+"'.");
      return false;
    }
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
      Linker::writeLineToHelperOutputTxt("\t\tLabel :"+label+"|");
      //symbolID
      int symbolID;
      inputFile.read((char *)(&symbolID), sizeof(symbolID));
      Linker::writeLineToHelperOutputTxt("\t\tID :"+symbolID);
      //symbolSection
      inputFile.read((char *)&stringLength, sizeof(stringLength));
      std::string section;
      section.resize(stringLength);
      inputFile.read((char *)section.c_str(), stringLength);
      Linker::writeLineToHelperOutputTxt("\t\tSection :"+label);
      //symbolValue
      unsigned int value;
      inputFile.read((char *)(&value), sizeof(value));
      Linker::writeLineToHelperOutputTxt("\t\tValue :"+value);
      //symbolType
      SymbolType type;
      inputFile.read((char *)(&type), sizeof(type));
      Linker::writeLineToHelperOutputTxt("\t\tType :"+type);
      //symbolIsDefined
      bool isDefined;
      inputFile.read((char *)(&isDefined), sizeof(isDefined));
      Linker::writeLineToHelperOutputTxt("\t\tIsDefined :"+isDefined);
      symbolTable.addSymbol(label, SymbolData(symbolID,section,value,type,isDefined));
    }
    inputFile.close();
    Linker::fileSymbolTables[fileName]=symbolTable;
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