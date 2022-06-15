#include "../inc/Em.hpp"

short Emulator::readFromMemory(uint offset, uint size, bool isData){
  if(size==1){
    return Emulator::memory[offset].get();
  }

  if(isData){ //little endian (left <- right)
    return (Emulator::memory[offset+1].get()<<8)+Emulator::memory[offset].get();
  }
  else{ //big endian (left -> right)
    return (Emulator::memory[offset].get()<<8)+Emulator::memory[offset+1].get();
  }
}

void Emulator::writeToMemory(short value, uint offset, uint size, bool isData){
  if(size==1){
    Emulator::memory[offset].set(value);
    return;
  }

  if(isData){ //little endian (left <- right)
    Emulator::memory[offset].set(value & 0xFF);
    Emulator::memory[offset+1].set(value>>8);
  }
  else{ //big endian (left -> right)
    Emulator::memory[offset].set(value>>8);
    Emulator::memory[offset+1].set(value & 0xFF);
  }
}