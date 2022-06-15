#include "../inc/Em.hpp"

void Emulator::writeLineToHelperOutputTxt(const std::string &line){
  Emulator::helperOutputFileStream<<line<<std::endl;
}

bool Emulator::readDataFromInputFile(){
  Emulator::writeLineToHelperOutputTxt("READING DATA FROM INPUT FILE");
  std::ifstream inputFileStream;
  inputFileStream.open(Emulator::inputFileName, std::ios::binary);
  if(inputFileStream.fail()){
    Emulator::addError("Can't open file "+Emulator::inputFileName);
    return false;
  }

  unsigned int numberOfSegments;
  inputFileStream.read((char *)&numberOfSegments, sizeof(numberOfSegments));
  std::cout<<"Read "<<std::to_string(numberOfSegments)<<" segment."<<std::endl;

}

bool Emulator::loadDataToMemory(){

}