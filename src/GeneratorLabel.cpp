#include "GeneratorLabel.h"

#include <fstream>
#include <iostream>

GeneratorLabel::GeneratorLabel(const std::string dataFilesPath){
   SetPathName(dataFilesPath);
   m_generator = NUM_GENERATORS;
}

GeneratorLabel::~GeneratorLabel(void){
}

std::string GeneratorLabel::GetLabel(){
   if(m_pathName.find("McAtNlo") != std::string::npos){
      m_label = "McAtNlo";
      m_generator = MCATNLO;
   }
   else if(m_pathName.find("Alpgen") != std::string::npos){
      m_label = "Alpgen";
      m_generator = ALPGEN;
   }
   else if(m_pathName.find("PowHeg") != std::string::npos){
      m_label = "PowHeg";
      m_generator = POWHEG;
   }
   
   return m_label;
}
