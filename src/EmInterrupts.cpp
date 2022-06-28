#include "../inc/Em.hpp"

void Emulator::requestIntOnLine(char intLineNumber){
  Emulator::writeLineToHelperOutputTxt("Interrupt requested on line "+std::to_string(intLineNumber)+"; Current psw="+std::to_string(Emulator::rpsw));
  if(intLineNumber>=0 && intLineNumber<NUMBER_OF_PERIFERIES){
    Emulator::interruptRequests[intLineNumber]=true;
    for(int i=0; i<Emulator::interruptRequests.size(); i++){
      Emulator::writeLineToHelperOutputTxt("InterruptRequest["+std::to_string(i)+"]="+std::to_string(Emulator::interruptRequests[i]));
    }
  }
}

void Emulator::processInterrupts(){
  Emulator::writeLineToHelperOutputTxt("Processing interrupt");
  if(Emulator::getFlag(Flag::I)){
    //add code for timer and terminal - check Tr and Tl
  }
}

void Emulator::jmpOnInterruptRoutine(char ivtEntry){
  Emulator::writeLineToHelperOutputTxt("Jumping to interrupt routine in entry "+std::to_string(ivtEntry));
  Emulator::pushOnStack(Emulator::rpc);
  Emulator::pushOnStack(Emulator::rpsw);
  Emulator::rpc=Emulator::readFromMemory((ivtEntry%8)*2, WORD, true); 
  //enable intr
  Emulator::setFlag(Flag::I);
  Emulator::setFlag(Flag::Tr);
  Emulator::setFlag(Flag::Tl);
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                       TERMINAL                          ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

//check https://pubs.opengroup.org/onlinepubs/7908799/xsh/termios.h.html

bool Emulator::startTerminal(){
  
}

void Emulator::readCharFromTerminal(){

}

void Emulator::stopTerminal(){

}