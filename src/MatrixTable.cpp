
#include "MatrixTable.h"

#include "TexTable.h"
#include "KinematicVariable.h"

MatrixTable::MatrixTable(TMatrixD const * const matrix,
                        const std::string label,
                        const std::string sampleName,
                        const std::vector<double> binEdges):
      m_matrix(matrix)
{
   m_label = label;
   m_sampleName = sampleName;
   m_placement = "ht!";
   m_binEdges = binEdges;
}

MatrixTable::~MatrixTable(){
}

std::string MatrixTable::GetTableText(const std::string caption){
   
   // warn if caption is empty
   if(caption.size() == 0){
      std::cout << "[MatrixTable::GetTableText] WARNING no caption set for table, " << m_label << "\n";
   }
   
   // if there are no column rules, generate some
   if(m_columnRules.size() == 0){
//       std::cout << "[MatrixTable::GetTableText] Generating default column rules for table, " << m_label << ": ";
      m_columnRules = GenerateColumnRules();
   }
   
      
   // create table
   TexTable table(m_placement,m_columnRules);
   
   // add header row with labels
   table.hline();
   table.AddRow(GenerateHeaderRow());
   table.hline();
   
   // fill data
   
   for(unsigned int rowNum=0;rowNum<m_matrix->GetNrows();++rowNum){
      std::stringstream left,right;
      left << m_binEdges[rowNum];
      right << m_binEdges[rowNum+1];
      std::stringstream row;
      row.setf(std::ios::fixed);
      row.width(10);
      row << left.str() << " & ";
      row.width(10);
      row << right.str();
      
      for(unsigned int colNum=0;colNum<m_matrix->GetNcols();++colNum){
         row << " & ";
         row.width(8);
         row.precision(3);
         row << (*m_matrix)[rowNum][colNum];
      }
      table.AddRow(row.str());
   }
   
   table.hline();
   
   return table.GetTableText(caption,m_label);
}


std::string MatrixTable::GenerateColumnRules(void) const{
   
   std::stringstream s;
   // first two columns are bin edges
   s << "r@{ to }r|";
   // next columns are also bin edges, but below will be content
   for(unsigned int i=0;i<m_matrix->GetNcols();++i)
      s << " c";
   
   return s.str();
   
}

std::string MatrixTable::GenerateHeaderRow(void) const{
   
   KinematicVariable* kinVar = new KinematicVariable(m_sampleName);
   std::string kinVarLabel  = kinVar->GetLatexKinVarLabel();
   std::string kinVarUnitLabel = kinVar->GetUnitKinVarLabel();
   std::string NkinVarLabel = kinVar->GetLatexTNKinVarLabel();
   std::string DkinVarLabel = kinVar->GetLatexTDKinVarLabel();
   std::string UnitNkinVarLabel = kinVar->GetUnitNKinVarLabel();
   std::string UnitDkinVarLabel = kinVar->GetUnitDKinVarLabel();
   
   std::stringstream s;
   s << "\\multicolumn{2}{l}{$"<<kinVarLabel<<"$ " << kinVarUnitLabel << "}";
   for(unsigned int i=0;i<m_binEdges.size()-1;++i){
      if(i == m_binEdges.size()-2)
         s << " & \\multicolumn{1}{|c}{" << m_binEdges[i] << " to " << m_binEdges[i+1] << "}";
      else
         s << " & \\multicolumn{1}{|c|}{" << m_binEdges[i] << " to " << m_binEdges[i+1] << "}";
   }
   return s.str();
   
}
