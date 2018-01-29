#ifndef MATRIXTABLE_H
#define MATRIXTABLE_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>

#include "TMatrixD.h"

class MatrixTable{
public:
   MatrixTable(TMatrixD const * const matrix,
               const std::string label,
               const std::string sampelName,
               const std::vector<double> binEdges);
   ~MatrixTable();
   
   std::string GetTableText(const std::string caption);
   
   void SetLabel(const std::string label){m_label = label;};
   std::string GetLabel() const {return m_label;};
   
   void SetColumnRules(const std::string rules){m_columnRules = rules;};
   std::string GetColumnRules(void) const {return m_columnRules;};
   
   void SetPlacement(const std::string placement){m_placement = placement;};
   std::string GetPlacement(void){return m_placement;};
   
private:
   MatrixTable();
   
   static const unsigned short int m_precision = 4;
   static const unsigned short int m_width     = 8;
   static const unsigned short int m_ncolumns  = 4;
   
   std::string m_label;
   std::string m_sampleName;
   
   std::string m_columnRules;
   std::string m_placement;
   
   std::vector<double> m_binEdges;
   
   TMatrixD const * const m_matrix;
   
   std::string GenerateHeaderRow(void) const;
   std::string GenerateColumnRules(void) const;
   
};


#endif
