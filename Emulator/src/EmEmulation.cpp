#include "../inc/Em.hpp"

bool Emulator::emulationLoop(){
  Emulator::writeLineToHelperOutputTxt("ENTERING EMULATION LOOP");
  //user program location is in ivt[0]
  Emulator::rpc=Emulator::readFromMemory(IVT_ENTRY_PROGRAM_START, WORD);
  Emulator::writeLineToHelperOutputTxt("pc = "+std::to_string(Emulator::rpc));
  //sp points to last occupied location and grows towards lower addresses
  Emulator::rsp=MEMORY_MAPPED_REGISTERS;
  Emulator::writeLineToHelperOutputTxt("sp = "+std::to_string(Emulator::rsp));
  //psw = 0 at the start
  Emulator::resetAllFlags();
  Emulator::writeLineToHelperOutputTxt("psw = "+std::to_string(Emulator::rpsw));

  Emulator::isRunning=true;
  while(Emulator::isRunning){
    Emulator::prevPc=Emulator::rpc;

    if(!Emulator::fetchAndDecodeInstr()){ //can't read/decode instr
      std::cout<<"Can't fetch and decode instruction at pc = "<<Emulator::rpc;
      Emulator::rpc=Emulator::prevPc;
      Emulator::jmpOnInterruptRoutine(IVT_ENTRY_INSTRUCTION_ERROR);
    }
    else{
      Emulator::writeLineToHelperOutputTxt("Instruction fetched and decoded");
    }
    if(!Emulator::execInstr()){ //can't execute the instruction
      std::cout<<"Can't execute instruction at pc = "<<Emulator::rpc;
      Emulator::rpc=Emulator::prevPc;
      Emulator::jmpOnInterruptRoutine(IVT_ENTRY_INSTRUCTION_ERROR);
    }
    else{
      Emulator::writeLineToHelperOutputTxt("Instruction executed");
    }

    //check for interrupts
  }

  Emulator::writeLineToHelperOutputTxt("\n FINISHED WITH EMULATION");
  Emulator::printMemory();
  return true;
}

bool Emulator::fetchAndDecodeInstr(){

}