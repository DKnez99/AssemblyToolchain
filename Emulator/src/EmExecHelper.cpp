#include "../inc/Em.hpp"

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