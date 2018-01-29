
#include "SystematicTextFile.h"

#include "SystematicBin.h"
#include "SystematicBinList.h"
#include "SystematicList.h"

SystematicTextFile::SystematicTextFile(const std::string filename):
   m_filename(filename)
{
   m_list = new SystematicList;
   m_delete_list = true;
   
   ReadDataFromFile();
}

SystematicTextFile::~SystematicTextFile(){
   if(m_list && m_delete_list){
      delete m_list;
      m_list = 0;
   }
}


bool SystematicTextFile::ReadDataFromFile(){
   
   std::ifstream fin(m_filename.c_str());
   if(fin.fail()){
      std::cerr << "[SystematicTextFile::ReadDataFromFile] ERROR file not found: " << m_filename << "\n";
      return false;
   }
   
   while(!fin.eof()){
      
      std::string systematic;
      unsigned int binNumber = 0;
      double binLow=0.,binHigh=0.;
      double data=0.;
      double stat=0.;
      double syst=0.;
      
      fin >> systematic >> binNumber >> binLow >> binHigh >> data >> stat >> syst;
      
      if(fin.eof()){
         //std::cout << "[SystematicTextFile::ReadDataFromFile] end of file reached.\n";
         break;
      }
      
      //std::cerr << " systematic: " << systematic << " " << binNumber << " " << binLow << " " << binHigh << " " << data << " " << stat << " " << syst << "\n";
      
      
      SystematicBin bin;
      bin.low(binLow);
      bin.high(binHigh);
      bin.content(data);
      bin.stat(stat);
      bin.syst(syst);
      
      (*m_list)[systematic].push_back(bin);
      (*m_list)[systematic].SetName(systematic);
   }
   return true;
}
