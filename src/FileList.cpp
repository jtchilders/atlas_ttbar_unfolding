#include <FileList.h>

#include <fstream>
#include <iostream>

FileList::FileList(std::string filename){
   
   m_fileList = new std::vector<std::string>;
   
   std::ifstream *file = new std::ifstream(filename.c_str());
   
   //loop over each line and save to vector for filename
   // input file should have one filename per line
   char line[500];
   
   // loop over lines in file
   while(file->getline(line,500)){
      
#ifdef DEBUG
      std::cout << "line: " << line << " ";
#endif
      
      // add line to vector
      m_fileList->push_back(line);
      
#ifdef DEBUG
      std::cout << " filename: " << m_fileList->back() << std::endl;
#endif
   }
   
   file->close();
   delete file;
   file = 0;
   
}

FileList::~FileList(void){
   delete m_fileList;
   m_fileList = 0;
}

