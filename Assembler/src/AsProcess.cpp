#include "../inc/As.hpp"

bool Assembler::processSection(const std::string &sectionName){
  if(Assembler::sectionTable.sectionExists(sectionName)){
    Assembler::locationCnt=Assembler::sectionTable.getSectionData(sectionName).size;
    Assembler::writeLineToHelperOutputTxt("Section "+sectionName+" already exists, location counter set to "+std::to_string(Assembler::locationCnt));
    return true;
  }
  else{
    
    Assembler::writeLineToHelperOutputTxt("Creating section "+sectionName);
    if(Assembler::symbolTable.symbolExists(sectionName)){
      Assembler::addError("Symbol "+sectionName+" already exists in the symbol table, can't redefine it as a section.");
      Assembler::writeLineToHelperOutputTxt("ERROR! Symbol "+sectionName+" already exists is symbol table, can't redefine it as a section.");
      return false;
    }
    else{
      Assembler::currentSection=sectionName;
      Assembler::locationCnt=0;
      Assembler::symbolTable.addSymbol(sectionName,SymbolData(sectionName,0,SymbolType::SECTION, true));
      Assembler::sectionTable.addSection(sectionName);
      Assembler::writeLineToHelperOutputTxt("Created new symbol and section "+sectionName+", location counter set to 0.");
      return true;
    }
  }
}

bool Assembler::processGlobal(const std::string &globalArgument){
  if(Assembler::symbolTable.symbolExists(globalArgument)){
    switch (Assembler::symbolTable.getSymbolType(globalArgument)){
      case (SymbolType::EXTERN):{
        Assembler::addError("Can't redefine extern symbol "+globalArgument+" as global.");
        Assembler::writeLineToHelperOutputTxt("ERROR! Symbol "+globalArgument+" was already defined as an external symbol.");
        return false;
      }
      case (SymbolType::SECTION):{
        Assembler::addWarning("Tried to redefine section "+globalArgument+" as a global symbol. Redefinition was discarded.");
        Assembler::writeLineToHelperOutputTxt("WARNING! Symbol "+globalArgument+" was already defined as a section.");
        break;
      }
      case (SymbolType::GLOBAL):{
        Assembler::addWarning("Symbol "+globalArgument+" is already defined as a global symbol.");
        Assembler::writeLineToHelperOutputTxt("WARNING! Symbol "+globalArgument+" is already defined as a global symbol.");
        break;
      }
      default:{
        Assembler::symbolTable.setSymbolType(globalArgument, SymbolType::GLOBAL);
        Assembler::writeLineToHelperOutputTxt("Symbol "+globalArgument+" already exists, setting it's type to global");
        //MUST GO THROUGH RELOC TABLE AND FIX ALL ENTRIES WITH IT
        break;
      }
    }

    // if(Assembler::symbolTable.getSymbolType(globalArgument)==SymbolType::EXTERN){
    //   Assembler::addError("Can't redefine extern symbol "+globalArgument+" as global.");
    //   Assembler::writeLineToHelperOutputTxt("ERROR! Symbol "+globalArgument+" was already defined as an external symbol.");
    //   return false;
    // }
    // if(Assembler::symbolTable.getSymbolType(globalArgument)==SymbolType::SECTION){ //gnu allows this (nothing happens tho)
    //   Assembler::addError("Warning! Tried to redefine section "+globalArgument+" as a global symbol. Redefinition was discarded.");
    //   Assembler::writeLineToHelperOutputTxt("WARNING! Symbol "+globalArgument+" was already defined as a section.");
    //   return true;
    // }
    // Assembler::symbolTable.setSymbolType(globalArgument, SymbolType::GLOBAL);
    // Assembler::writeLineToHelperOutputTxt("Symbol "+globalArgument+" already exists, setting it's type to global");
    // //MUST GO THROUGH RELOC TABLE AND FIX ALL ENTRIES WITH IT
  }
  else{
    Assembler::symbolTable.addSymbol(globalArgument, SymbolData(SECTION_UNDEFINED, 0, SymbolType::GLOBAL, false));
    Assembler::writeLineToHelperOutputTxt("Symbol "+globalArgument+" doesn't exists yet, adding it to the SymbolTable");
  }
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