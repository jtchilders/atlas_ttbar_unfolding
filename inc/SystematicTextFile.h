#ifndef SYSTEMATICTEXTFILE_H
#define SYSTEMATICTEXTFILE_H

#include <iostream>
#include <fstream>
#include <string>

class SystematicList;

class SystematicTextFile{
public:
   SystematicTextFile(const std::string filename);
   ~SystematicTextFile();
   
   SystematicList* list(){return m_list;};
   void DeleteList(const bool value = true){m_delete_list = value;};
   
private:
   const std::string m_filename;
   
   SystematicList* m_list;
   bool m_delete_list;
   
   bool ReadDataFromFile();
};

#endif
