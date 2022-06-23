#include "../inc/Em.hpp"

bool Emulator::emulationLoop(){
  Emulator::writeLineToHelperOutputTxt("ENTERING EMULATION LOOP");
  //user program location is in ivt[0]
  Emulator::rpc=Emulator::readFromMemory(IVT_ENTRY_PROGRAM_START, WORD,true);
  Emulator::helperOutputFileStream<<"pc = 0x"<<std::hex<<Emulator::rpc<<std::endl;
  //sp points to last occupied location and grows towards lower addresses
  Emulator::rsp=MEMORY_MAPPED_REGISTERS;
  Emulator::helperOutputFileStream<<"sp = 0x"<<std::hex<<Emulator::rsp<<std::endl;
  //psw = 0 at the start
  Emulator::resetAllFlags();

  Emulator::isRunning=true;
  while(Emulator::isRunning){

    Emulator::prevPc=Emulator::rpc; //in case of an error

    if(!Emulator::fetchAndDecodeInstr()){ //can't read/decode instr
      std::cout<<"Can't fetch and decode instruction at pc = 0x"<<std::hex<<Emulator::rpc<<std::endl;
      Emulator::rpc=Emulator::prevPc;
      Emulator::jmpOnInterruptRoutine(IVT_ENTRY_INSTRUCTION_ERROR); //should halt the processor
    }
    else{
      Emulator::writeLineToHelperOutputTxt("Instruction fetched and decoded");
    }
    if(!Emulator::execInstr()){ //can't execute the instruction
      std::cout<<"Can't execute instruction at pc = 0x"<<std::hex<<Emulator::rpc<<std::endl;
      Emulator::rpc=Emulator::prevPc;
      Emulator::jmpOnInterruptRoutine(IVT_ENTRY_INSTRUCTION_ERROR); //should halt the processor
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
  Emulator::helperOutputFileStream<<"\n\nReading instruction description (pc=0x"<<std::hex<<rpc<<")"<<std::endl;
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
      if(Emulator::instr_srcReg!=Register::noreg){
        Emulator::addWarning("Instruction INT must have 0xF as source reg.");
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
        Emulator::addWarning("Instruction CALL must have 0xF as destination reg.");
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
        Emulator::addWarning("Instruction JMP must have 0xF as destination reg.");
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
        Emulator::addWarning("Instruction JEQ must have 0xF as destination reg.");
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
        Emulator::addWarning("Instruction JNE must have 0xF as destination reg.");
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
        Emulator::addWarning("Instruction JGT must have 0xF as destination reg.");
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
      Emulator::addWarning("Instruction not recognized!");
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
    Emulator::addWarning("Invalid addressing mode for instruction on pc = "+std::to_string(Emulator::rpc));
    return false;
  }

  if(Emulator::instr_updateType!=UpdateType::noupdate &&
     Emulator::instr_updateType!=UpdateType::deca &&
     Emulator::instr_updateType!=UpdateType::decb &&
     Emulator::instr_updateType!=UpdateType::inca &&
     Emulator::instr_updateType!=UpdateType::incb)
  { 
    Emulator::addWarning("Invalid update type for instruction on pc = "+std::to_string(Emulator::rpc));
    return false;
  }
  return true;
}

bool Emulator::execInstr(){
  switch(Emulator::instr_mnemonic){
    case Instruction::instr_halt:{
      Emulator::writeLineToHelperOutputTxt("Processor stopped.");
      Emulator::isRunning=false;
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_int:{
      Emulator::jmpOnInterruptRoutine(Emulator::reg[Emulator::instr_destReg]);
      return true;
    }
    case Instruction::instr_iret:{
      //just pop pc and psw
      Emulator::rpsw=Emulator::popFromStack();
      Emulator::rpc=Emulator::popFromStack();
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_call:{
      Emulator::updateBeforeInstr();
      Emulator::rpc=Emulator::getOperandByAddrMode();
      Emulator::updateAfterInstr();
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_ret:{
      Emulator::rpc=Emulator::getOperandByAddrMode();
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_jmp:{
      Emulator::updateBeforeInstr();
      Emulator::rpc=Emulator::getOperandByAddrMode();
      Emulator::updateAfterInstr();
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_jeq:{
      Emulator::updateBeforeInstr();
      if(Emulator::conditionMet(Instruction::instr_jeq)){
        Emulator::rpc=Emulator::getOperandByAddrMode();
        Emulator::writeLineToHelperOutputTxt("Condition met!");
      }
      else{
        Emulator::writeLineToHelperOutputTxt("Condition NOT met!");
      }
      Emulator::updateAfterInstr();
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_jne:{
      Emulator::updateBeforeInstr();
      if(Emulator::conditionMet(Instruction::instr_jne)){
        Emulator::rpc=Emulator::getOperandByAddrMode();
        Emulator::writeLineToHelperOutputTxt("Condition met!");
      }
      else{
        Emulator::writeLineToHelperOutputTxt("Condition NOT met!");
      }
      Emulator::updateAfterInstr();
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_jgt:{
      Emulator::updateBeforeInstr();
      if(Emulator::conditionMet(Instruction::instr_jgt)){
        Emulator::rpc=Emulator::getOperandByAddrMode();
        Emulator::writeLineToHelperOutputTxt("Condition met!");
      }
      else{
        Emulator::writeLineToHelperOutputTxt("Condition NOT met!");
      }
      Emulator::updateAfterInstr();
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_xchg:{
      Emulator::printSrcAndDestReg();
      short temp=Emulator::reg[Emulator::instr_srcReg];
      Emulator::reg[Emulator::instr_srcReg]=Emulator::reg[Emulator::instr_destReg];
      Emulator::reg[instr_destReg]=Emulator::reg[Emulator::instr_srcReg];
      Emulator::printSrcAndDestReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_add:{
      Emulator::printSrcAndDestReg();
      Emulator::reg[instr_destReg]+=Emulator::reg[Emulator::instr_srcReg];
      Emulator::printSrcAndDestReg(true);
      return true;
    }
    case Instruction::instr_sub:{
      Emulator::printSrcAndDestReg();
      Emulator::reg[instr_destReg]-=Emulator::reg[Emulator::instr_srcReg];
      Emulator::printSrcAndDestReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_mul:{
      Emulator::printSrcAndDestReg();
      Emulator::reg[instr_destReg]*=Emulator::reg[Emulator::instr_srcReg];
      Emulator::printSrcAndDestReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_div:{
      Emulator::printSrcAndDestReg();
      if(Emulator::reg[Emulator::instr_srcReg]==0){
        Emulator::addWarning("Can't divide by zero.");
        Emulator::writeLineToHelperOutputTxt("Can't divide by zero.");
        return false; //might need to return true
      }
      Emulator::reg[instr_destReg]/=Emulator::reg[Emulator::instr_srcReg];
      Emulator::printSrcAndDestReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_cmp:{
      Emulator::printPswReg();
      short d=Emulator::reg[Emulator::instr_destReg];
      short s=Emulator::reg[Emulator::instr_srcReg];
      if(d==s){
        Emulator::setFlag(Flag::Z);
      }
      else{
        Emulator::resetFlag(Flag::Z);
      }

      if((d-s)<0){
        Emulator::setFlag(Flag::N);
      }
      else{
        Emulator::resetFlag(Flag::N);
      }

      if(d < s){
        Emulator::setFlag(Flag::C);  
      }
      else{
        Emulator::resetFlag(Flag::C);
      }

      if((s>0 && d<0 && (d-s)>0) || (s<0 && d>0 && (d-s)>0)){
        Emulator::setFlag(Flag::O);
      }
      else{
        Emulator::resetFlag(Flag::O);
      }
      Emulator::printPswReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_not:{
      Emulator::printSrcAndDestReg();
      Emulator::reg[Emulator::instr_destReg]=!Emulator::reg[Emulator::instr_destReg];
      Emulator::printSrcAndDestReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_and:{
      Emulator::printSrcAndDestReg();
      Emulator::reg[Emulator::instr_destReg]&=Emulator::reg[Emulator::instr_srcReg];
      Emulator::printSrcAndDestReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_or:{
      Emulator::printSrcAndDestReg();
      Emulator::reg[Emulator::instr_destReg]|=Emulator::reg[Emulator::instr_srcReg];
      Emulator::printSrcAndDestReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_xor:{
      Emulator::printSrcAndDestReg();
      Emulator::reg[Emulator::instr_destReg]^=Emulator::reg[Emulator::instr_srcReg];
      Emulator::printSrcAndDestReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_test:{
      Emulator::printPswReg();
      short d=Emulator::reg[Emulator::instr_destReg];
      short s=Emulator::reg[Emulator::instr_srcReg];
      if((d&s)==0){
        Emulator::setFlag(Flag::Z);
      }
      else{
        Emulator::resetFlag(Flag::Z);
      }

      if((d&s)<0){
        Emulator::setFlag(Flag::N);
      }
      else{
        Emulator::resetFlag(Flag::N);
      }

      Emulator::printPswReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_shl:{
      Emulator::printSrcAndDestReg();
      Emulator::printPswReg();

      short pd=Emulator::reg[Emulator::instr_destReg];
      
      Emulator::reg[Emulator::instr_destReg]<<=Emulator::reg[Emulator::instr_srcReg];
      short cd=Emulator::reg[Emulator::instr_destReg];
      short cs=Emulator::reg[Emulator::instr_srcReg];

      if(cd==0){
        Emulator::setFlag(Flag::Z);
      }
      else{
        Emulator::resetFlag(Flag::Z);
      }

      if(cd<0){
        Emulator::setFlag(Flag::N);
      }
      else{
        Emulator::resetFlag(Flag::N);
      }

      if(cs <= 16 && ((pd>>(16-cs))&0b1)){ //dest[src]==1?  CHECK IF ITS <=16 or <16
        Emulator::setFlag(Flag::C);
      }
      else{
        Emulator::resetFlag(Flag::C);
      }

      Emulator::printSrcAndDestReg(true);
      Emulator::printPswReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_shr:{
      Emulator::printSrcAndDestReg();
      Emulator::printPswReg();

      short pd=Emulator::reg[Emulator::instr_destReg];
      
      Emulator::reg[Emulator::instr_destReg]>>=Emulator::reg[Emulator::instr_srcReg];
      short cd=Emulator::reg[Emulator::instr_destReg];
      short cs=Emulator::reg[Emulator::instr_srcReg];

      if(cd==0){
        Emulator::setFlag(Flag::Z);
      }
      else{
        Emulator::resetFlag(Flag::Z);
      }

      if(cd<0){
        Emulator::setFlag(Flag::N);
      }
      else{
        Emulator::resetFlag(Flag::N);
      }

      if(cs <= 16 && ((pd>>(cs-1))&0b1)){ //dest[src-1]==1?
        Emulator::setFlag(Flag::C);
      }
      else{
        Emulator::resetFlag(Flag::C);
      }

      Emulator::printSrcAndDestReg(true);
      Emulator::printPswReg(true);
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
    case Instruction::instr_ldr:{
      Emulator::printSrcAndDestReg();
      Emulator::updateBeforeInstr();
      Emulator::reg[Emulator::instr_destReg]=Emulator::getOperandByAddrMode();
      Emulator::updateAfterInstr();
      Emulator::printSrcAndDestReg(true);
      return true;
    }
    case Instruction::instr_str:{
      Emulator::updateBeforeInstr();
      if(!Emulator::setOperandByAddrMode(Emulator::reg[Emulator::instr_destReg])){
        return false;
      }
      Emulator::updateAfterInstr();
      Emulator::printPcPswSpStateToTxt();
      return true;
    }
  }
  Emulator::addWarning("Instruction didn't match any of the mnemonics.");
  Emulator::writeLineToHelperOutputTxt("Instruction didn't match any of the mnemonics.");
  return false;
}