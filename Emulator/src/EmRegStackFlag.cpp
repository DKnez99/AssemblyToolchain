#include "../inc/Em.hpp"

void Emulator::getRegDescr(short value){
  Emulator::instr_destReg=(value>>4)&0xF;
  Emulator::instr_srcReg=value & 0xF;
  Emulator::writeLineToHelperOutputTxt("Dest reg: "+Emulator::instr_destReg);
  Emulator::writeLineToHelperOutputTxt("Src reg: "+Emulator::instr_srcReg);
}

void Emulator::getAddrDescr(short value){
  Emulator::instr_updateType=(value>>4)&0xF;
  Emulator::instr_addrMode=value & 0xF;
  Emulator::writeLineToHelperOutputTxt("Update type: "+Emulator::instr_updateType);
  Emulator::writeLineToHelperOutputTxt("Addr mode: "+Emulator::instr_addrMode);
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
}

void Emulator::resetFlag(short flag){
  Emulator::rpsw&=~flag;
}

bool Emulator::getFlag(short flag){
  return Emulator::rpsw & flag;
}

void Emulator::setFlag(short flag){
  Emulator::rpsw|=flag;
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