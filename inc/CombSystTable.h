#ifndef COMBSYSTTABLE_H
#define COMBSYSTTABLE_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>

#include "TH1.h"

class SystematicList;

class CombSystTable{
public:
   CombSystTable(SystematicList& systematics,
             const std::string label,
             TH1* nominal,
             std::string sampleName,
             const bool normalized = false,
             const bool relativeErrors = false);
   ~CombSystTable();
   
   std::string GetTableText(const std::string caption,const std::string filename);
   
   void SetLabel(const std::string label){m_label = label;};
   std::string GetLabel() const {return m_label;};
   
   void SetColumnRules(const std::string rules){m_columnRules = rules;};
   std::string GetColumnRules(void) const {return m_columnRules;};
   
   void SetPlacement(const std::string placement){m_placement = placement;};
   std::string GetPlacement(void){return m_placement;};
   
   void isNormalized(bool set)   {m_normalized = set;SetScaler();};
   bool isNormalized(void) const {return m_normalized;};
   
   void isRelative(bool set)     {m_relative = set;};
   bool isRelative(void) const   {return m_relative;};
   
   static double RoundValue(const double value,const unsigned int precision = 2);
private:
   CombSystTable();
   
   static const unsigned short int m_precision = 4;
   static const unsigned short int m_width     = 8;
   static const unsigned short int m_ncolumns  = 4;
   
   std::string m_label;
   std::string m_sampleName;
   
   std::string m_columnRules;
   std::string m_placement;
   
   bool m_normalized;
   bool m_relative;
   double m_scaler;
   
   SystematicList& m_syst;
   
   TH1* m_nominal;
   
   std::string GenerateColumnRules(void) const;
   std::string GenerateHeaderRow(void) const;
   void SetScaler(void);
};


#endif
