#include "../inc/Em.hpp"

void Emulator::getRegDescr(short value){
  Emulator::instr_destReg=(value>>4)&0xF;
  Emulator::instr_srcReg=value & 0xF;
  Emulator::helperOutputFileStream<<"Dest reg: "<<std::hex<<(int)Emulator::instr_destReg<<std::endl;
  Emulator::helperOutputFileStream<<"Src reg: "<<std::hex<<(int)Emulator::instr_srcReg<<std::endl;
}

void Emulator::getAddrDescr(short value){
  Emulator::instr_updateType=(value>>4)&0xF;
  Emulator::instr_addrMode=value & 0xF;
  Emulator::writeLineToHelperOutputTxt("Update type: "+std::to_string(Emulator::instr_updateType));
  Emulator::writeLineToHelperOutputTxt("Addr mode: "+std::to_string(Emulator::instr_addrMode));
}

void Emulator::pushOnStack(short value){
  Emulator::writeLineToHelperOutputTxt("Pushing on stack: "+std::to_string(value));
  Emulator::rsp-=2;
  Emulator::writeToMemory(value, rsp, WORD, true);
}

short Emulator::popFromStack(){
  short value=Emulator::readFromMemory(rsp, WORD, true);
  Emulator::rsp+=2;
  Emulator::writeToMemory(value, rsp, WORD, true);
  Emulator::writeLineToHelperOutputTxt("Popped from stack: "+std::to_string(value));
  return value;
}

void Emulator::resetAllFlags(){
  Emulator::rpsw &=0x0;
  Emulator::writeLineToHelperOutputTxt("Resetting all psw flags. Psw = "+std::to_string(Emulator::rpsw));
}

void Emulator::resetFlag(short flag){
  Emulator::rpsw&=~flag;
  Emulator::writeLineToHelperOutputTxt("Flag reset. Psw = "+std::to_string(Emulator::rpsw));
}

bool Emulator::getFlag(short flag){
  return Emulator::rpsw & flag;
}

void Emulator::setFlag(short flag){
  Emulator::rpsw|=flag;
  Emulator::writeLineToHelperOutputTxt("Flag set. Psw = "+std::to_string(Emulator::rpsw));
}

bool Emulator::conditionMet(Instruction instr){
  switch (instr)
  {
    case Instruction::instr_jeq:{
      return Emulator::getFlag(Flag::Z);
    }
    case Instruction::instr_jne:{
      return !Emulator::getFlag(Flag::Z);
    }
    case Instruction::instr_jgt:{
      return !(Emulator::getFlag(Flag::N) ^ Emulator::getFlag(Flag::O)) & !Emulator::getFlag(Flag::Z);
    }
  }
  return false;
}

void Emulator::printSrcAndDestReg(bool instrExecuted){
  if(instrExecuted){
    Emulator::helperOutputFileStream<<"After: ";
  }
  else{
    Emulator::helperOutputFileStream<<"Before: ";
  }
  Emulator::helperOutputFileStream<<"Reg["<<std::hex<<(int)Emulator::instr_srcReg<<"] = "
  <<Emulator::reg[Emulator::instr_srcReg]<<", Reg["<<std::hex<<(int)Emulator::instr_destReg<<"] = "
  <<Emulator::reg[Emulator::instr_destReg]<<std::endl;
}

void Emulator::printPswReg(bool instrExecuted){
  if(instrExecuted){
    Emulator::helperOutputFileStream<<"After: ";
  }
  else{
    Emulator::helperOutputFileStream<<"Before: ";
  }
  Emulator::helperOutputFileStream<<"pws = "<<std::hex<<Emulator::rpsw<<std::endl;
}