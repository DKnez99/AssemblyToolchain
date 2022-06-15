#include "../inc/Em.hpp"

void Emulator::getRegDescr(short value){
  Emulator::instr_destReg=(value>>4)&0xF;
  Emulator::instr_srcReg=value & 0xF;
  Emulator::writeLineToHelperOutputTxt("Dest reg: "+std::to_string(Emulator::instr_destReg));
  Emulator::writeLineToHelperOutputTxt("Src reg: "+std::to_string(Emulator::instr_srcReg));
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

void Emulator::updateAfterInstr(){
  switch(Emulator::instr_updateType){
    case UpdateType::noupdate:{
      Emulator::writeLineToHelperOutputTxt("No post-update");
      return;
    }
    case UpdateType::deca:{
      Emulator::reg[Emulator::instr_srcReg]-=2;
      Emulator::writeLineToHelperOutputTxt("Post decrement of reg"+std::to_string(Emulator::instr_srcReg)+". New value = "+std::to_string(Emulator::reg[Emulator::instr_srcReg]));
      return;
    }
    case UpdateType::inca:{
      Emulator::reg[Emulator::instr_srcReg]+=2;
      Emulator::writeLineToHelperOutputTxt("Post increment of reg"+std::to_string(Emulator::instr_srcReg)+". New value = "+std::to_string(Emulator::reg[Emulator::instr_srcReg]));
      return;
    }
  }
  return;
}

void Emulator::updateBeforeInstr(){
  switch(Emulator::instr_updateType){
    case UpdateType::noupdate:{
      Emulator::writeLineToHelperOutputTxt("No pre-update");
      return;
    }
    case UpdateType::decb:{
      Emulator::reg[Emulator::instr_srcReg]-=2;
      Emulator::writeLineToHelperOutputTxt("Pre decrement of reg"+std::to_string(Emulator::instr_srcReg)+". New value = "+std::to_string(Emulator::reg[Emulator::instr_srcReg]));
      return;
    }
    case UpdateType::incb:{
      Emulator::reg[Emulator::instr_srcReg]+=2;
      Emulator::writeLineToHelperOutputTxt("Pre increment of reg"+std::to_string(Emulator::instr_srcReg)+". New value = "+std::to_string(Emulator::reg[Emulator::instr_srcReg]));
      return;
    }
  }
  return;
}

short Emulator::getOperandByAddrMode(){
  short result;
  switch(Emulator::instr_addrMode){
    case AddressingMode::immed:{
      Emulator::helperOutputFileStream<<"AddrMode: Immed | Operand = "<<std::hex<<Emulator::instr_payload<<std::endl;
      return Emulator::instr_payload;
    }
    case AddressingMode::regdir:{
      Emulator::helperOutputFileStream<<"AddrMode: Regdir | Operand = "<<std::hex<<Emulator::reg[Emulator::instr_srcReg]<<std::endl;
      return Emulator::reg[Emulator::instr_srcReg];
    }
    case AddressingMode::regind:{
      result=Emulator::readFromMemory(Emulator::reg[Emulator::instr_srcReg], WORD, true);
      Emulator::helperOutputFileStream<<"AddrMode: Regind | Operand = "<<std::hex<<result<<std::endl;
      return result;
    }
    case AddressingMode::regindmv:{
      result=Emulator::readFromMemory(Emulator::reg[Emulator::instr_srcReg]+Emulator::instr_payload, WORD, true);
      Emulator::helperOutputFileStream<<"AddrMode: Regind with displ | Operand = "<<std::hex<<result<<std::endl;
      return result;
    }
    case AddressingMode::memdir:{
      result=Emulator::readFromMemory(Emulator::instr_payload, WORD, true);
      Emulator::helperOutputFileStream<<"AddrMode: Memdir| Operand = "<<std::hex<<result<<std::endl;
      return result;
    }
    case AddressingMode::regdirmv:{
      result=Emulator::reg[Emulator::instr_srcReg]+Emulator::instr_payload;
      Emulator::helperOutputFileStream<<"AddrMode: Regdir with displ| Operand = "<<std::hex<<result<<std::endl;
      return result;
    }
  }
  //none matched with given addresing mode
  Emulator::writeLineToHelperOutputTxt("Error while trying to match addressing mode. Returning 0.");
  return 0;
}

bool Emulator::setOperandByAddrMode(short value){
    switch(Emulator::instr_addrMode){
    case AddressingMode::immed:{
      Emulator::addError("Can't store value in immediate operand.");
      Emulator::writeLineToHelperOutputTxt("Can't store value in immediate operand.");
      return false;
    }
    case AddressingMode::regdir:{
      Emulator::helperOutputFileStream<<"AddrMode: Regdir | Operand = "<<std::hex<<value<<std::endl;
      Emulator::reg[Emulator::instr_srcReg]=value;
      Emulator::helperOutputFileStream<<"Reg["<<std::hex<<Emulator::instr_srcReg<<"] = "<<std::hex<<Emulator::reg[Emulator::instr_srcReg]<<std::endl;
      return true;
    }
    case AddressingMode::regind:{
      Emulator::helperOutputFileStream<<"AddrMode: Regind | Operand = "<<std::hex<<value<<std::endl;
      Emulator::writeToMemory(value, Emulator::reg[Emulator::instr_srcReg], WORD, true);
      Emulator::helperOutputFileStream<<"Mem(Reg["<<std::hex<<Emulator::instr_srcReg<<"]) = "<<std::hex<<Emulator::readFromMemory(Emulator::reg[Emulator::instr_srcReg],WORD,true)<<std::endl;
      return true;
    }
    case AddressingMode::regindmv:{
      Emulator::helperOutputFileStream<<"AddrMode: Regind with displ | Operand = "<<std::hex<<value<<std::endl;
      Emulator::writeToMemory(value, Emulator::reg[Emulator::instr_srcReg]+Emulator::instr_payload, WORD, true);
      Emulator::helperOutputFileStream<<"Mem(Reg["<<std::hex<<Emulator::instr_srcReg<<"]+"<<std::hex<<Emulator::instr_payload<<") = "<<Emulator::readFromMemory(Emulator::reg[Emulator::instr_srcReg]+Emulator::instr_payload,WORD,true)<<std::endl;
      return true;
    }
    case AddressingMode::memdir:{
      Emulator::helperOutputFileStream<<"AddrMode: Memdir| Operand = "<<std::hex<<value<<std::endl;
      Emulator::writeToMemory(value, Emulator::instr_payload, WORD, true);
      Emulator::helperOutputFileStream<<"Mem("<<std::hex<<Emulator::instr_payload<<") = "<<Emulator::readFromMemory(Emulator::instr_payload,WORD,true)<<std::endl;
      return true;
    }
    case AddressingMode::regdirmv:{
      Emulator::addError("Can't store value in regdir with displacement operand.");
      Emulator::writeLineToHelperOutputTxt("Can't store value in regdir with displacement operand.");
      return false;
    }
  }
  //none matched with given addresing mode
  Emulator::writeLineToHelperOutputTxt("Error while trying to match addressing mode. Didn't set anything.");
  return false;
}