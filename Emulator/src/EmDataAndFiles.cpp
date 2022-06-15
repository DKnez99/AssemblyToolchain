#include "../inc/Em.hpp"
#include <sstream>
#include <iomanip>

void Emulator::writeLineToHelperOutputTxt(const std::string &line){
    Emulator::helperOutputFileStream<<line<<std::endl;
}

void Emulator::printMemory(){
  Emulator::writeLineToHelperOutputTxt("\nCURRENT MEM. STATE:");
  int cnt=0;
  for(int i=0; i<Emulator::memory.size(); i++){
    if(cnt%8==0){ //print addresses
      Emulator::helperOutputFileStream<<std::hex<<std::setw(4)<<std::setfill('0')<<(cnt & 0xFFFF)<<": ";
    }
    Data data=memory[i];
    Emulator::helperOutputFileStream<<std::hex<<std::setw(2)<<std::setfill('0')<<(((data.hex1<<4)+data.hex2) & 0xFF)<<" ";
    cnt++;
    if(cnt%8==0){
      Emulator::helperOutputFileStream<<std::endl;
    }
  }
  Emulator::helperOutputFileStream<<std::endl;
 } 

bool Emulator::readDataFromInputFile(){
  Emulator::writeLineToHelperOutputTxt("READING DATA FROM INPUT FILE");
  std::ifstream inputFileStream(Emulator::inputFileName, std::ios::in | std::ios::binary);
  if(inputFileStream.fail()){
    Emulator::addError("Can't find file '"+Emulator::inputFileName+"'.");
    return false;
  }

  int numberOfSegments=0;
  inputFileStream.read((char *)&numberOfSegments, sizeof(numberOfSegments));
  for(int i=0; i<numberOfSegments; i++){
    Segment seg;
    inputFileStream.read((char *)&seg.startAddress, sizeof(seg.startAddress));
    inputFileStream.read((char *)&seg.size, sizeof(seg.size));
    //std::cout<<"\nSegment "<<std::to_string(i)<<"("<<seg.size<<"B) @"<<std::hex<<seg.startAddress<<std::dec<<std::endl;
    for(int j=0; j<seg.size; j++){
      int hex1=0;
      int hex2=0;
      inputFileStream.read((char *)&hex1, sizeof(hex1));
      inputFileStream.read((char *)&hex2, sizeof(hex2));
      //std::cout<<std::hex<<hex1<<hex2<<" ";
      // if(j%8==0 & j!=0){
      //   std::cout<<std::endl;
      // }
      seg.data.push_back({hex1,hex2});
    }
    Emulator::segments.push_back(seg);
  }
  return true;
}

bool Emulator::loadDataToMemory(){
  Emulator::writeLineToHelperOutputTxt("\nLOADING DATA TO MEMORY");
  for(auto &segment:Emulator::segments){
    if((segment.size+segment.startAddress) > MEMORY_MAPPED_REGISTERS){  //checked this in ld, but check again just to be sure
      Emulator::addError("Segment overlaps with memory mapped registers at "+std::to_string(MEMORY_MAPPED_REGISTERS));
      return false;
    }
    for(int i=0;i<segment.data.size(); i++){
      Emulator::memory[i+segment.startAddress]=segment.data[i];
    }
  }
  Emulator::printMemory();
  return true;
}