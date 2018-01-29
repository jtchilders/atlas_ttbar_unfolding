#ifndef _FILELIST_H_
#define _FILELIST_H_

#include <string>
#include <vector>

class FileList{
public:
   FileList(std::string filename);
   ~FileList(void);
   
   unsigned int size(void){return m_fileList->size();};
   
   std::string front(void){return m_fileList->front();};
   std::string back(void){return m_fileList->back();};
   std::string at(unsigned int i){return m_fileList->at(i);};
   std::string operator[](unsigned int i){return m_fileList->at(i);};
   
   void push_back(std::string str){m_fileList->push_back(str);};
   void pop_back(void){m_fileList->pop_back();};
   
   void clear(void){m_fileList->clear();};
   
   typedef std::vector<std::string>::iterator itr;
   itr begin(void){m_fileList->begin();};
   itr end(void){m_fileList->end();};
   
private:
   
   std::vector<std::string> *m_fileList;
   
};

#endif
