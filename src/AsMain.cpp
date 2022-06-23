#include "../inc/As.hpp"

int main(int argc, const char *argv[]){
  std::string outputFile, inputFile;

  if(strcmp(argv[1],"-o")==0){
    outputFile=argv[2];
    inputFile=argv[3];
  }
  else if(strcmp(argv[2],"-o")==0){
    inputFile=argv[1];
    outputFile=argv[3];
  }
  else{
    std::cout<<"\nInvalid asembler call"<<std::endl;
    return -1;
  }
  Assembler as(inputFile, outputFile);
  as.assemble();

  return 0;
}

