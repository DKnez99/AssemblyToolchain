#include "../inc/Em.hpp"

Emulator::Emulator(const std::string &inputFileName):inputFileName(inputFileName),isRunning(false),
memory(MEMORY_SIZE,{0,0}),reg(NUMBER_OF_REGISTERS,0),interruptRequests(NUMBER_OF_PERIFERIES,0){
  Emulator::helperOutputFileName=Emulator::inputFileName.substr(0,Emulator::inputFileName.find_last_of('.'))+"_emulator_helper.txt";
}

void Emulator::emulate(){
  Emulator::helperOutputFileStream.open(Emulator::helperOutputFileName, std::ios::out);
  if(!Emulator::readDataFromInputFile()){
    Emulator::printResults();
    return;
  }
  if(!Emulator::loadDataToMemory()){
    Emulator::printResults();
    return;
  }

  Emulator::printResults();
  Emulator::helperOutputFileStream.close();
  return;
}

bool Emulator::emulationLoop(){
  Emulator::writeLineToHelperOutputTxt("ENTERING EMULATION LOOP");
  //user program location is in ivt[0]
  rpc=Emulator::readFromMemory(IVT_ENTRY_PROGRAM_START, WORD);
  Emulator::writeLineToHelperOutputTxt("pc = "+std::to_string(rpc));
  //sp points to last occupied location and grows towards lower addresses
  rsp=MEMORY_MAPPED_REGISTERS;
  Emulator::writeLineToHelperOutputTxt("sp = "+std::to_string(rsp));
  //psw = 0 at the start
  Emulator::resetAllFlags();
  Emulator::writeLineToHelperOutputTxt("psw = "+std::to_string(rpsw));

  Emulator::isRunning=true;
  while(Emulator::isRunning){
    Emulator::prevPc=rpc;

    if(!Emulator::fetchAndDecodeInstr()){ //can't read/decode instr

    }
    else{
      Emulator::writeLineToHelperOutputTxt("")
    }
    if(!Emulator::execInstr()){ //can't execute the instruction

    }
  }
}