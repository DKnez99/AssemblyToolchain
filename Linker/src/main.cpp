#include "../inc/Ld.hpp"
#include "../inc/LdRegexes.hpp"
#include <iostream>

int main(int argc, const char *argv[]){
  std::vector<std::string> inputFileNames;
  std::string outputFileName;
  bool readOutputFileFlag=false;
  bool hasOutputFile=false;
  bool placeAtFlag=false;
  std::smatch regexMatcher;
  bool hexFlag=false;
  bool relocatableFlag=false;
  std::unordered_map<std::string, unsigned int> placeAt;
  std::string currArg;
  for(int i=1;i<argc;i++){
    currArg=argv[i];
    if(currArg=="-o"){
      readOutputFileFlag=true;
    }
    else if(std::regex_search(currArg,regexMatcher,rgx_placeAt)){
      placeAtFlag=true;
      std::string section=regexMatcher.str(1);
      std::string strAddr=regexMatcher.str(2);
      unsigned int intAddr;
      if(std::regex_match(strAddr, rgx_decAddr)){
        intAddr=std::stoi(strAddr,0,16);
      }
      else if(std::regex_match(strAddr, rgx_hexAddr)){
        intAddr=std::stoi(strAddr);
      }
      placeAt.insert({section,intAddr});
    }
    else if(currArg=="-hex"){
      if(hexFlag==true){
        std::cout<<"Error. Multiple -hex options detected.";
        return 0;
      }
      if(relocatableFlag==true){
        std::cout<<"Error. Can't use -hex and and -relocateable options together.";
        return 0;
      }
      hexFlag=true;
    }
    else if(currArg=="-relocateable"){  //misspell
      if(relocatableFlag==true){
        std::cout<<"Error. Multiple -relocateable options detected.";
        return 0;
      }
      if(hexFlag==true){
        std::cout<<"Error. Can't use -hex and and -relocateable options together.";
        return 0;
      }
      relocatableFlag=true;
    }
    else if(readOutputFileFlag==true){
      if(hasOutputFile==true){
        std::cout<<"Error. Multiple -o options detected."<<std::endl;
        return 0;
      }
      outputFileName=currArg;
      readOutputFileFlag=false;
      hasOutputFile=true;
    }
    else{ //input files
      inputFileNames.push_back(currArg);
    }
  }

  if(hexFlag==false && relocatableFlag==false){
    std::cout<<"Error. Must use either -hex or -relocateable option.";
    return 0;
  }

  Linker ld(inputFileNames, outputFileName, placeAt, relocatableFlag);
  ld.link();
  return 0;
}