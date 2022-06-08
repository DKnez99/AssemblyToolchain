#include "../inc/As.hpp"

bool Assembler::processSection(const std::string &sectionName){
  return true;
}

bool Assembler::processGlobal(const std::string &globalArgument){
  return true;
}

bool Assembler::processExtern(const std::string &externArgument){
  return true;
}

bool Assembler::processWord(const std::string &wordArgument){
  return true;
}

bool Assembler::processSkip(const std::string & kipArgument){
  return true;
}

bool Assembler::processAscii(const std::string &str){
  return true;
}

bool Assembler::processEqu(const std::string &symbolName, const std::string & expr){
  return true;
}

bool Assembler::processLabel(const std::string &labelName){
  return true;
}

bool Assembler::processZeroOpIns(const std::string &instruction){
  return true;
}

bool Assembler::processOneRegIns(const std::string &instruction, const std::string &reg){
  return true;
}

bool Assembler::processTwoRegIns(const std::string &instruction, const std::string &regOne, const std::string &regTwo){
  return true;
}

bool Assembler::processOneOpIns(const std::string &instruction, const std::string &operand){
  return true;
}

bool Assembler::processOneRegOneOpIns(const std::string &instruction, const std::string &reg, const std::string &operand){
  return true;
}

int Assembler::processAbsoluteAddr(const std::string &operand, int offsetInsideInstr){
  return 1;
}

int Assembler::processPCRelAddr(const std::string &operand, int offsetInsideInstr, int diffBetweenOffsetAndNextInstr){
  return 1;
}