#include "../inc/Ld.hpp"

void Linker::writeLineToHelperOutputTxt(const std::string &line){
  Linker::helperOutputFileStream<<line<<std::endl;
}

void Linker::printResults(){

}

void Linker::addError(const std::string &errorMsg){
  errorMessages.push_back(errorMsg);
}