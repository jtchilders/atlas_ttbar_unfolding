#ifndef SYSTTABLE_H
#define SYSTTABLE_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cmath>

#include "TH1.h"

class SystematicList;
class SystematicBinList;

class SystTable{
public:
   SystTable(SystematicList& systematics,
             const std::string label,
             TH1* nominal,
             std::string sampleName,
             const bool normalized = false,
             const bool relative = false);
   ~SystTable();
   
   std::string GetTableText(const std::string caption);
   
   void SetLabel(const std::string label){m_label = label;};
   std::string GetLabel() const {return m_label;};
   
   void SetColumnRules(const std::string rules){m_columnRules = rules;};
   std::string GetColumnRules(void) const {return m_columnRules;};
   
   void SetPlacement(const std::string placement){m_placement = placement;};
   std::string GetPlacement(void){return m_placement;};
   
   void isNormalized(bool set)   {m_normalized = set;SetScaler();};
   bool isNormalized(void) const {return m_normalized;};
   
   void isRelative(bool set)   {m_relative = set;SetScaler();};
   bool isRelative(void) const {return m_relative;};
   
   static double RoundValue(const double value,const unsigned int precision = 2);
private:
   SystTable();
   
   static const unsigned short int m_precision = 2;
   static const unsigned short int m_width     = 6;
   
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
   std::string ListSystematicOnLine(const std::string label,
                                    const SystematicBinList* systematicBins);
};


#endif
