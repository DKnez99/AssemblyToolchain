#include "../inc/As.hpp"

void Assembler::writeLineToHelperOutputTxt(const std::string &line){
  Assembler::helperOutputFileStream<<line<<std::endl;
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                      ERRORS                             ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

void Assembler::addError(const std::string &errorMsg){
  Assembler::errorMap[Assembler::lineCnt]=errorMsg;
  Assembler::errorOccured=true;
}

void Assembler::printErrors(){
  if(Assembler::errorOccured){
    for(auto err: Assembler::errorMap){
      std::cout<<"Error occured while assemblying input file "<<Assembler::inputFileName<<". Details below:"<<std::endl;
      if(Assembler::lineMap[err.first]>0){
        std::cout<<"Line "<<Assembler::lineMap[err.first]-1<<": "<<err.second<<std::endl;
      }
      else{
        std::cout<<err.second<<std::endl;
      }
    }
  }
  else{
    std::cout<<"File "<<Assembler::inputFileName<<" assemblied successfuly!"<<std::endl;
  }
}

//||=========================================================||
//||=========================================================||
//||=========================================================||
//||                     CONVERSIONS                         ||
//||=========================================================||
//||=========================================================||
//||=========================================================||

std::vector<std::string> Assembler::splitString(const std::string& str, const char c) //from stack overflow :D
{
    std::vector<std::string> tokens;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(c, prev)) != std::string::npos) {
        tokens.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }
    tokens.push_back(str.substr(prev));
    return tokens;
}

std::string Assembler::stringToHex(const std::string& input){ //from stack overflow :D
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}

std::string Assembler::decToHex(int number, int size){
  std::stringstream stream;
  stream << std::uppercase<<std::setfill('0')<<std::setw(size)<<std::hex << number;
  return stream.str().substr(stream.str().length()-size); 
}

int Assembler::stringToDec(const std::string &literal){
  int result=0;
  if(std::regex_match(literal, rgx_literal_hex)){
    result=std::stoi(literal.substr(2),0,16);
  }
  else if(std::regex_match(literal, rgx_literal_dec)){
    result=std::stoi(literal);
  }
  else if(std::regex_match(literal, rgx_literal_bin)){
    result=std::stoi(literal.substr(2),0,2);
  }
  return result;
}

int Assembler::getRegNumber(const std::string &reg){
  if(reg=="sp"){
    return 6;
  }

  if(reg=="pc"){
    return 7;
  }

  if(reg=="psw"){
    return 8;
  }
  
  return reg.at(1)-'0';
}