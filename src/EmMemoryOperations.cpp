#include "../inc/Em.hpp"

short Emulator::readFromMemory(uint offset, uint size, bool isData){
  if(size==1){
    Emulator::helperOutputFileStream<<"Reading data from memory @0x"<<std::hex<<offset<<std::endl;
    return Emulator::memory[offset].get();
  }

  if(isData){ //little endian (left <- right)
    Emulator::helperOutputFileStream<<"Reading data from memory @0x"<<std::hex<<offset+1<<" and @0x"<<offset<<std::endl;
    return ((short)(Emulator::memory[offset+1].get()<<8)+Emulator::memory[offset].get());
  }
  else{ //big endian (left -> right)
    Emulator::helperOutputFileStream<<"Reading data from memory @0x"<<std::hex<<offset<<" and @0x"<<offset+1<<std::endl;
    return ((short)(Emulator::memory[offset].get()<<8)+Emulator::memory[offset+1].get());
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

