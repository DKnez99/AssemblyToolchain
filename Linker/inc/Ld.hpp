#pragma once

#include "LdSymbolTable.hpp"
#include "LdRelocTable.hpp"
#include "LdSectionTable.hpp"
#include <fstream>

class Linker{
  private:
    //file stuff
    std::vector<std::string> inputFileNames;
    std::string helperOutputFileName;
    std::string outputFileName;
    void writeLineToHelperOutputTxt(const std::string &line);
    std::ofstream helperOutputFileStream;
    
    //important internal assembler variables
    //int lineCnt;
    //int locationCnt;
    std::string currentSection;
    std::string currentFileName;
    std::unordered_map<std::string, SymbolTable> fileSymbolTables;
    std::unordered_map<std::string, RelocationTable> fileRelocTables;
    SymbolTable globalSymbolTable;
    SectionTable globalSectionTable;
    RelocationTable globalRelocTable;
    std::unordered_map<std::string, unsigned int> placeAt;  //map for saving user -placeAt input
    bool isRelocatable;

    //error handling
    bool errorOccured;
    bool warningOccured;
    void addError(const std::string &errorMsg); //errors which will be displayed in terminal
    void addWarning(const std::string &errorMsg); //warnings which will be displayed in terminal
    std::vector<std::string> errorMessages;
    std::vector<std::string> warningMessages;
    void printErrors();
    void printWarnings();
    void printResults();

    //main logic parts
    bool readFromInputFiles();  //goes through formatted input file and categorizes data
    void calculateSectionAddresses();
    void calculateOffsets();
    void calculateRelocs();  //fixes stuff in tables
    void writeToOutputFile();
  public:
    Linker(std::vector<std::string> inputFileNames, const std::string &outputFileName, std::unordered_map<std::string, unsigned int> placeAt, bool isRelocatable=false);
    void link();
};