#include "../inc/Ld.hpp"
#include <iostream>

int main(){
  std::unordered_map<std::string, unsigned int> placeAtmap;
  std::vector<std::string> inputFileNames;
  inputFileNames.push_back("output_binary.o");
  Linker ld(inputFileNames, "linker_output.o", placeAtmap);
  ld.link();
  return 0;
}