#include "../inc/Em.hpp"

bool Emulator::emulationLoop(){
  Emulator::writeLineToHelperOutputTxt("ENTERING EMULATION LOOP");
  //user program location is in ivt[0]
  Emulator::rpc=Emulator::readFromMemory(IVT_ENTRY_PROGRAM_START, WORD,true);
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

  Emulator::writeLineToHelperOutputTxt("\nFINISHED WITH EMULATION");
  Emulator::printMemory();
  return true;
}

bool Emulator::fetchAndDecodeInstr(){
  Emulator::writeLineToHelperOutputTxt("\nReading instruction description (pc="+std::to_string(rpc)+")");
  Emulator::instr_descr=Emulator::readFromMemory(Emulator::rpc,BYTE);
  Emulator::rpc+=1;
  switch(Emulator::instr_descr){
    case Instruction::instr_halt:{
      Emulator::writeLineToHelperOutputTxt("Recognized HALT");
      Emulator::instr_mnemonic=Instruction::instr_halt;
      Emulator::instr_size=1;
      return true;
    }
    case Instruction::instr_int:{
      Emulator::writeLineToHelperOutputTxt("Recognized INT");
      Emulator::instr_mnemonic=Instruction::instr_int;
      Emulator::instr_size=2;
      Emulator::getRegDescr(Emulator::readFromMemory(Emulator::rpc,BYTE));
      Emulator::rpc+=1;
      Emulator::writeLineToHelperOutputTxt("Dest reg: "+Emulator::instr_destReg);
      Emulator::writeLineToHelperOutputTxt("Src reg: "+Emulator::instr_srcReg);
      if(Emulator::instr_srcReg!=Register::noreg){
        Emulator::addError("Instruction INT must have 0xF as source reg.");
        return false;
      }
      return true;
    }
    case Instruction::instr_iret:{
      Emulator::writeLineToHelperOutputTxt("Recognized IRET");
      Emulator::instr_mnemonic=Instruction::instr_iret;
      Emulator::instr_size=1;
      return true;
    }
    case Instruction::instr_call:{
      Emulator::writeLineToHelperOutputTxt("Recognized CALL");
      Emulator::instr_mnemonic=Instruction::instr_call;
      Emulator::getRegDescr(Emulator::readFromMemory(Emulator::rpc, BYTE));
      Emulator::rpc+=1;
      if(Emulator::instr_destReg!=Register::noreg){
        Emulator::addError("Instruction CALL must have 0xF as destination reg.");
        return false;
      }
      return Emulator::threeOrFiveByteInstr();
    }
    case Instruction::instr_ret:{
      Emulator::writeLineToHelperOutputTxt("Recognized IRET");
      Emulator::instr_mnemonic=Instruction::instr_ret;
      Emulator::instr_size=1;
      return true;
    }
    case Instruction::instr_jmp:{
      Emulator::writeLineToHelperOutputTxt("Recognized JMP");
      Emulator::instr_mnemonic=Instruction::instr_jmp;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      if(Emulator::instr_destReg!=Register::noreg){
        Emulator::addError("Instruction JMP must have 0xF as destination reg.");
        return false;
      }
      return Emulator::threeOrFiveByteInstr();
    }
    case Instruction::instr_jeq:{
      Emulator::writeLineToHelperOutputTxt("Recognized JEQ");
      Emulator::instr_mnemonic=Instruction::instr_jeq;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      if(Emulator::instr_destReg!=Register::noreg){
        Emulator::addError("Instruction JEQ must have 0xF as destination reg.");
        return false;
      }
      return Emulator::threeOrFiveByteInstr();
    }
    case Instruction::instr_jne:{
      Emulator::writeLineToHelperOutputTxt("Recognized JNE");
      Emulator::instr_mnemonic=Instruction::instr_jne;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      if(Emulator::instr_destReg!=Register::noreg){
        Emulator::addError("Instruction JNE must have 0xF as destination reg.");
        return false;
      }
      return Emulator::threeOrFiveByteInstr();
    }
    case Instruction::instr_jgt:{
      Emulator::writeLineToHelperOutputTxt("Recognized JGT");
      Emulator::instr_mnemonic=Instruction::instr_jgt;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      if(Emulator::instr_destReg!=Register::noreg){
        Emulator::addError("Instruction JGT must have 0xF as destination reg.");
        return false;
      }
      return Emulator::threeOrFiveByteInstr();
    }
    case Instruction::instr_xchg:{
      Emulator::writeLineToHelperOutputTxt("Recognized XCHG");
      Emulator::instr_mnemonic=Instruction::instr_xchg;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_add:{
      Emulator::writeLineToHelperOutputTxt("Recognized ADD");
      Emulator::instr_mnemonic=Instruction::instr_add;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_sub:{
      Emulator::writeLineToHelperOutputTxt("Recognized SUB");
      Emulator::instr_mnemonic=Instruction::instr_sub;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_mul:{
      Emulator::writeLineToHelperOutputTxt("Recognized MUL");
      Emulator::instr_mnemonic=Instruction::instr_mul;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_div:{
      Emulator::writeLineToHelperOutputTxt("Recognized DIV");
      Emulator::instr_mnemonic=Instruction::instr_div;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_cmp:{
      Emulator::writeLineToHelperOutputTxt("Recognized CMP");
      Emulator::instr_mnemonic=Instruction::instr_cmp;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_not:{
      Emulator::writeLineToHelperOutputTxt("Recognized NOT");
      Emulator::instr_mnemonic=Instruction::instr_not;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_and:{
      Emulator::writeLineToHelperOutputTxt("Recognized AND");
      Emulator::instr_mnemonic=Instruction::instr_and;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_or:{
      Emulator::writeLineToHelperOutputTxt("Recognized OR");
      Emulator::instr_mnemonic=Instruction::instr_or;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_xor:{
      Emulator::writeLineToHelperOutputTxt("Recognized XOR");
      Emulator::instr_mnemonic=Instruction::instr_xor;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_test:{
      Emulator::writeLineToHelperOutputTxt("Recognized TEST");
      Emulator::instr_mnemonic=Instruction::instr_test;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_shl:{
      Emulator::writeLineToHelperOutputTxt("Recognized SHL");
      Emulator::instr_mnemonic=Instruction::instr_shl;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_shr:{
      Emulator::writeLineToHelperOutputTxt("Recognized SHR");
      Emulator::instr_mnemonic=Instruction::instr_shr;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      Emulator::instr_size=2;
      return true;
    }
    case Instruction::instr_ldr:{
      Emulator::writeLineToHelperOutputTxt("Recognized LDR");
      Emulator::instr_mnemonic=Instruction::instr_ldr;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      return Emulator::threeOrFiveByteInstr();
    }
    case Instruction::instr_str:{
      Emulator::writeLineToHelperOutputTxt("Recognized STR");
      Emulator::instr_mnemonic=Instruction::instr_str;
      Emulator::getRegDescr(Emulator::readFromMemory(rpc, BYTE));
      Emulator::rpc+=1;
      return Emulator::threeOrFiveByteInstr();
    }
    default:{
      Emulator::addError("Instruction not recognized!");
      return false; //might need to return true
    }
  }
}

bool Emulator::threeOrFiveByteInstr(){
  Emulator::getAddrDescr(Emulator::readFromMemory(Emulator::rpc, BYTE));
  Emulator::rpc+=1;
  if(Emulator::instr_addrMode==AddressingMode::regdir || Emulator::instr_addrMode==AddressingMode::regind){
    Emulator::instr_size=3;
  }
  else if(Emulator::instr_addrMode == AddressingMode::immed ||
      Emulator::instr_addrMode == AddressingMode::regindmv ||
      Emulator::instr_addrMode == AddressingMode::memdir ||
      Emulator::instr_addrMode == AddressingMode::regdirmv)
  {
    Emulator::instr_payload=Emulator::readFromMemory(Emulator::rpc, WORD);
    Emulator::rpc+=2;
    Emulator::instr_size+=2;
  }
  else{
    Emulator::addError("Invalid addressing mode for instruction on pc="+std::to_string(Emulator::rpc));
    return false;
  }

  if(Emulator::instr_updateType!=UpdateType::noupdate &&
     Emulator::instr_updateType!=UpdateType::deca &&
     Emulator::instr_updateType!=UpdateType::decb &&
     Emulator::instr_updateType!=UpdateType::inca &&
     Emulator::instr_updateType!=UpdateType::incb)
  { 
    Emulator::addError("Invalid update type for instruction on pc="+std::to_string(Emulator::rpc));
    return false;
  }
  return true;
}

bool Emulator::execInstr(){
  switch(Emulator::instr_mnemonic){
    case Instruction::instr_halt:{
      Emulator::isRunning=false;
      return true;
    }
  }
  return true;
}