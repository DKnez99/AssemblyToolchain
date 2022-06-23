#include "../inc/Em.hpp"
#include <iostream>

int main(int argc, const char *argv[]){
  if(argc!=2){
    std::cout<<"Exactly one input file needs to be specified.";
    return 0;
  }

  Emulator emulator(argv[1]);
  emulator.emulate();
  return 0;
}