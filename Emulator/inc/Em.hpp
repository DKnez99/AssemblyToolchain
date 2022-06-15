#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#define MEMORY_SIZE 1<<16
#define MEMORY_MAPPED_REGISTERS = 0xFF00

#define IVT_ENTRY_PROGRAM_START 0
#define IVT_ENTRY_INSTRUCTION_ERROR 1
#define IVT_ENTRY_TIMER 2
#define IVT_ENTRY_TERMINAL 3

#define NUMBER_OF_REGISTERS 9
#define BYTE 1
#define WORD 2

enum Register{
  r0=0x0,
  r1=0x1,
  r2=0x2,
  r3=0x3,
  r4=0x4,
  r5=0x5,
  r6=0x6,
  r7=0x7,
  sp=r6,
  pc=r7,
  psw=0x8,
  none=0xF
};

enum Flag{
  Z=1<<0,
  O=1<<1,
  C=1<<2,
  N=1<<3,
  Tr=1<<13,
  Tl=1<<14,
  I=1<<15
};

enum AddressingMode{
  immed=0x0,
  regdir=0x1,
  regind=0x2,
  regindmv=0x3, //with displacement
  memdir=0x4,
  regdirmv=0x5   //with displacement
};

enum UpdateType{
  none=0x0,
  decb=0x1, //decrement b4
  incb=0x2, //increment b4
  deca=0x3, //decrement after
  inca=0x4  //increment after
};

enum Instruction{
  instr_halt=0x00,
  instr_int=0x10,
  instr_iret=0x20,
  instr_call=0x30,
  instr_ret=0x40,
  instr_jmp=0x50,
  instr_jeq=0x51,
  instr_jne=0x52,
  instr_jgt=0x53,
  instr_xchg=0x60,
  instr_add=0x70,
  instr_sub=0x71,
  instr_mul=0x72,
  instr_div=0x73,
  instr_cmp=0x74,
  instr_not=0x80,
  instr_and=0x81,
  instr_or=0x82,
  instr_xor=0x83,
  instr_test=0x84,
  instr_shl=0x90,
  instr_shr=0x91,
  instr_ldr=0xA0,
  instr_str=0xB0
};

struct Segment{
  unsigned int startAddress;
  unsigned int size;  //in bytes
  std::vector<char> data;
};

class Emulator{
  //vars
  bool isRunning;
  std::vector<Segment> segments;
  unsigned int pc;

  //errors
  bool errorOccured;
  bool warningOccured;
  void addError(const std::string &errorMsg); //errors which will be displayed in terminal
  void addWarning(const std::string &errorMsg); //warnings which will be displayed in terminal
  std::vector<std::string> errorMessages;
  std::vector<std::string> warningMessages;
  void printErrors();
  void printWarnings();
  void printResults();

  //files
  std::string inputFileName;
  std::string helperOutputFileName;
  std::ofstream helperOutputFileStream;
  void writeLineToHelperOutputTxt(const std::string &line);
  
  //data
  bool readDataFromInputFile();
  bool loadDataToMemory();

  //memory
  std::vector<char> memory;
  char readFromMemory(uint offset, uint size, bool isData=true);
  void writeToMemory(char data, uint offset, uint size, bool isData=true);
  void printMemory(const std::string &fileName);

  //reg stuff
  std::vector<char> reg;  //r1-psw
  char &rsp=reg[Register::sp];
  char &rpc=reg[Register::pc];
  char &rpsw=reg[Register::psw];
  void pushOnStack(char reg);
  char popFromStack();
  void setFlag(char flag);
  bool getFlag(char flag);
  void resetFlag(char flag);
  void resetAllFlags();
  bool conditionMet(Instruction instr);

  //instruction stuff
  char instr_size;
  Instruction instr_mnemonic;
  Register instr_destReg;
  Register instr_srcReg;
  UpdateType instr_updateType;
  AddressingMode instr_addrMode;
  char instr_payload; //hex1,hex2
  bool fetchAndDecodeInstr();
  bool execInstr();
  char getOperandByAddrMode();
  bool setOperandByAddrMode(char operand);
  void updateBeforeInstr(); //need to split them, so they can be called both b4 and after ins
  void updateAfterInstr();  //need to split them, so they can be called both b4 and after ins
  
  //interrupts
  std::vector<bool> interruptRequests;
  void requestIntOnLine(char intLineNumber);
  void jmpOnInterruptRoutine(char ivtEntry);  //push(pc); push(psw); pc=mem[(ivtEntry%8)*2];

  //terminal
  //add

  //timer
  //add
public:
  Emulator(const std::string &fileName);
  void emulate();
};