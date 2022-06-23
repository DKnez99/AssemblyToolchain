#include "../inc/Em.hpp"
#include <bitset>

void Emulator::getRegDescr(short value){
  Emulator::instr_destReg=(value>>4)&0xF;
  Emulator::instr_srcReg=value & 0xF;
  Emulator::helperOutputFileStream<<"Dest reg: 0x"<<std::hex<<(int)Emulator::instr_destReg<<std::endl;
  Emulator::helperOutputFileStream<<"Src reg: 0x"<<std::hex<<(int)Emulator::instr_srcReg<<std::endl;
}

void Emulator::getAddrDescr(short value){
  Emulator::instr_updateType=(value>>4)&0xF;
  Emulator::instr_addrMode=value & 0xF;
  Emulator::helperOutputFileStream<<"Update type = 0b"<<std::bitset<8*sizeof(Emulator::instr_updateType)>(Emulator::instr_updateType)<<std::endl;
  Emulator::helperOutputFileStream<<"Addr mode = 0b"<<std::bitset<8*sizeof(Emulator::instr_addrMode)>(Emulator::instr_addrMode)<<std::endl;
}

void Emulator::pushOnStack(short value){
  Emulator::rsp-=2;
  Emulator::helperOutputFileStream<<"SP decreased by 0x2 to 0x"<<std::hex<<Emulator::rsp<<std::endl;
  Emulator::writeToMemory(value, rsp, WORD, true);
  Emulator::helperOutputFileStream<<"Pushed to stack (0x"<<std::hex<<Emulator::rsp<<") value 0x"<<value<<std::endl;
  return;
}

short Emulator::popFromStack(){
  short value=Emulator::readFromMemory(rsp, WORD, true);
  Emulator::helperOutputFileStream<<"Popped from stack (0x"<<std::hex<<Emulator::rsp<<") value 0x"<<value<<std::endl;
  Emulator::rsp+=2;
  Emulator::helperOutputFileStream<<"SP increased by 0x2 to 0x"<<std::hex<<Emulator::rsp<<std::endl;
  return value;
}

void Emulator::resetAllFlags(){
  Emulator::rpsw &=0x0;
  Emulator::helperOutputFileStream<<"ALL flags reset. PSW = 0b"<<std::bitset<8*sizeof(Emulator::rpsw)>(Emulator::rpsw)<<std::endl;
}

void Emulator::resetFlag(short flag){
  Emulator::rpsw&=~flag;
  Emulator::helperOutputFileStream<<"Flag reset. PSW = 0b"<<std::bitset<8*sizeof(Emulator::rpsw)>(Emulator::rpsw)<<std::endl;
}

bool Emulator::getFlag(short flag){
  return Emulator::rpsw & flag;
}

void Emulator::setFlag(short flag){
  Emulator::rpsw|=flag;
  Emulator::helperOutputFileStream<<"Flag set. PSW = 0b"<<std::bitset<8*sizeof(Emulator::rpsw)>(Emulator::rpsw)<<std::endl;
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
  Emulator::helperOutputFileStream<<"Reg["<<std::hex<<(int)Emulator::instr_srcReg<<"] = 0x"
  <<Emulator::reg[Emulator::instr_srcReg]<<", Reg["<<std::hex<<(int)Emulator::instr_destReg<<"] = 0x"
  <<Emulator::reg[Emulator::instr_destReg]<<std::endl;
}

void Emulator::printPswReg(bool instrExecuted){
  if(instrExecuted){
    Emulator::helperOutputFileStream<<"After: ";
  }
  else{
    Emulator::helperOutputFileStream<<"Before: ";
  }
  Emulator::helperOutputFileStream<<"PSW = 0b"<<std::bitset<8*sizeof(Emulator::rpsw)>(Emulator::rpsw)<<std::endl;
}

void Emulator::printPcPswSpStateToTxt(){
  Emulator::helperOutputFileStream<<"PSW = 0b"<<std::bitset<8*sizeof(Emulator::rpsw)>(Emulator::rpsw)<<std::endl;
  Emulator::helperOutputFileStream<<"PC = 0x"<<std::hex<<rpc<<std::endl;
  Emulator::helperOutputFileStream<<"SP = 0x"<<std::hex<<rsp<<std::endl;
}