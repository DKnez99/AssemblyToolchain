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