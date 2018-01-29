#ifndef TEXTABLE_H
#define TEXTABLE_H

#include <iostream>
#include <string>
#include <sstream>

class TexTable{
public:
   TexTable(const std::string placement, const std::string tabrules);
   
   static std::string GetHeader(const std::string placement,
                                const std::string tabrules);
   static std::string GetTrailer(const std::string caption,
                                 const std::string label);
   
   void AddRow(const std::string line){m_tableText << line << " \\\\ \n";};
   
   void hline(void){m_tableText << "\\hline\n";};
   
   std::string GetTableText(const std::string caption,
                            const std::string label) const;
   
private:
   TexTable(){};
   
   std::stringstream m_tableText;
   
};


#endif
