

#include "TexTable.h"


TexTable::TexTable(const std::string placement, const std::string tabrules){
   m_tableText << GetHeader(placement,tabrules);
}

std::string TexTable::GetTableText(const std::string caption,
                         const std::string label) const
{
   std::string output;
   output = m_tableText.str();
   output += GetTrailer(caption,label);
   
   return output;
}


std::string TexTable::GetHeader(const std::string placement,
                                       const std::string tabrules)
{
   std::stringstream output;
   
   output << "\\begin{table} [" << placement << "]\n";
   output << "\\footnotesize\n";
   output << "\\centering\n";
   output << "\\noindent\\makebox[\\textwidth]{\n";
   output << "\\begin{tabular}{" << tabrules << "}\n";
   
   return output.str();
}

std::string TexTable::GetTrailer(const std::string caption,
                                        const std::string label)
{
   std::stringstream output;
   
   output << "\\end{tabular}}\n";
   output << "\\caption{" << caption << "}\n";
   output << "\\label{" << label << "}\n";
   output << "\\end{table}\n";
   output << "\n";
   
   return output.str();
}

