#include "../inc/As.hpp"

bool processSection(const std::string &sectionName){
  return true;
}

bool processGlobal(const std::string &globalArgument){
  return true;
}

bool processExtern(const std::string &externArgument){
  return true;
}

bool processWord(const std::string &wordArgument){
  return true;
}

bool processSkip(const std::string & kipArgument){
  return true;
}

bool processAscii(const std::string &str){
  return true;
}

bool processEqu(const std::string &symbolName, const std::string & expr){
  return true;
}

bool processLabel(const std::string &labelName){
  return true;
}

bool processZeroOpIns(const std::string &instruction){
  return true;
}

bool processOneRegIns(const std::string &instruction, const std::string &reg){
  return true;
}

bool processTwoRegIns(const std::string &instruction, const std::string &regOne, const std::string &regTwo){
  return true;
}

bool processOneOpIns(const std::string &instruction, const std::string &operand){
  return true;
}

bool processOneRegOneOpIns(const std::string &instruction, const std::string &reg, const std::string &operand){
  return true;
}

int processAbsoluteAddr(const std::string &operand, int offsetInsideInstr){
  return 1;
}

int processPCRelAddr(const std::string &operand, int offsetInsideInstr, int diffBetweenOffsetAndNextInstr){
  return 1;
}