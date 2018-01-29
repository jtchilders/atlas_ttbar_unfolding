#ifndef PVALUETABLE_H
#define PVALUETABLE_H

#include <string>
#include <map>

#include "TopDecayChannel.h"
#include "PValueList.h"

class TexTable;

class PValueTable{
public:
   PValueTable(const PValueList& pValues,
               const std::string label,
               const std::string caption
              );
   ~PValueTable();
   
   std::string GetTableText();
   void WriteTableToFile(const std::string filename);
   
   void SetLabel(const std::string label){m_label = label;};
   std::string GetLabel() const {return m_label;};
   
   void SetColumnRules(const std::string rules){m_columnRules = rules;};
   std::string GetColumnRules(void) const {return m_columnRules;};
   
   void SetPlacement(const std::string placement){m_placement = placement;};
   std::string GetPlacement(void){return m_placement;};
   
private:
   PValueTable();
   
   const PValueList& m_pValues;
   
   static const unsigned short int m_precision = 3;
   static const unsigned short int m_width     = 8;
   static const unsigned short int m_ncolumns  = 10;
   
   std::string m_label;
   std::string m_caption;
   
   std::string m_columnRules;
   std::string m_placement;
   
   std::string GenerateColumnRules(void) const;
   void GenerateHeaderRow(TexTable& table) const;
};


#endif
