#include "../inc/Ld.hpp"
#include <iostream>

int main(){
  std::unordered_map<std::string, unsigned int> placeAt;
  //placeAt.insert({"myCode", 65170});
  //placeAt.insert({"myData", 209});
  std::vector<std::string> inputFileNames;
  inputFileNames.push_back("interrupts_output_binary.o");
  inputFileNames.push_back("main_output_binary.o");
  Linker ld(inputFileNames, "linker_output.o", placeAt);
  ld.link();
  return 0;
}