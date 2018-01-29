#ifndef GENERATORLABEL_H
#define GENERATORLABEL_H

#include <string>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <errno.h> 

class GeneratorLabel{
public:
   enum Generator{MCATNLO,ALPGEN,POWHEG,NUM_GENERATORS};
   
   GeneratorLabel(const std::string dataFilesPath);
   ~GeneratorLabel(void);
   
   void SetPathName(const std::string pathName){m_pathName = pathName;};
   std::string GetLabel();
   Generator GetGenerator() const {return m_generator;};
private:
   std::string m_pathName;
   std::string m_label;
   Generator   m_generator;
};

#endif

