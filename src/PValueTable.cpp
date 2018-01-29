#include "PValueTable.h"

#include <iostream>
#include <fstream>

#include "TexTable.h"
#include "KinematicVariable.h"

PValueTable:: PValueTable(const PValueList& pValues,
                          const std::string label,
                          const std::string caption):
      m_pValues(pValues)
{
   m_label = label;
   m_caption = caption;
   
   m_placement = "ht!";
}

PValueTable::~PValueTable(){
}

std::string PValueTable::GetTableText(){
   
   // warn if caption is empty
   if(m_caption.size() == 0){
      std::cout << "[PValueTable::GetTableText] WARNING no caption set for table, " << m_label << "\n";
   }
   
   
   // if there are no column rules, generate some
   if(m_columnRules.size() == 0){
      //std::cout << "[PValueTable::GetTableText] Generating default column rules for table, " << m_label << "\n";
      m_columnRules = GenerateColumnRules();
   }
   
      
   // create table
   TexTable table(m_placement,m_columnRules);
   
   // add header row with labels
   table.hline();
   GenerateHeaderRow(table);
   table.hline();
   table.hline();
   
   // fill data
   
   PValueList::list_const_iterator sampleItr = m_pValues.pvalue_list().begin();
   
   for(;sampleItr != m_pValues.pvalue_list().end();++sampleItr){
      std::string sampleName = sampleItr->first;
      
      KinematicVariable kinVar(sampleName);
      std::string variable = kinVar.GetLatexKinVarLabel();
      variable.insert(0,"$");
      variable.append("$");
      if(sampleName.find("Top1") != std::string::npos)
         variable.insert(0,"Lep-");
      else if(sampleName.find("Top2") != std::string::npos)
         variable.insert(0,"Had-");
      
      PValueList::sublist_const_iterator channelItr = sampleItr->second.begin();
      
      for(;channelItr != sampleItr->second.end();++channelItr){
         TopDecayChannel channel(channelItr->first);
         const PValueList::subsublist& prediction = channelItr->second;
         
         std::stringstream row;
         row.width(20);
         row.fill(' ');
         row.precision(m_precision);
         row.setf(std::ios::fixed);
         row << variable;
         row << " & ";
         row.width(10);
         row << channel.GetChannelStringLatex();
         PValueList::subsublist_const_iterator pvalueItr = prediction.find("Alpgen");
         if(pvalueItr != prediction.end()){
            row << " & ";
            row.width(m_width);
            row << pvalueItr->second.pvalue();
            row << " & ";
            row.width(m_width);
            row << pvalueItr->second.chi2();
            row << " & ";
            row.width(2);
            row.precision(0);
            row << pvalueItr->second.ndf();
            row.precision(m_precision);
         }
         else{
            row << " & ";
            row.width(m_width);
            row << " ";
            row << " & ";
            row.width(m_width);
            row << " \\multicolumn{2}{c|}{} ";
         }
         
         pvalueItr = prediction.find("McAtNlo");
         if(pvalueItr != prediction.end()){
            row << " & ";
            row.width(m_width);
            row << pvalueItr->second.pvalue();
            row << " & ";
            row.width(m_width);
            row << pvalueItr->second.chi2();
            row << " & ";
            row.width(2);
            row.precision(0);
            row << pvalueItr->second.ndf();
            row.precision(m_precision);
         }
         else{
            row << " & ";
            row.width(m_width);
            row << " ";
            row << " & ";
            row.width(m_width);
            row << " \\multicolumn{2}{c|}{} ";
         }
         
         pvalueItr = prediction.find("PowHeg");
         if(pvalueItr != prediction.end()){
            row << " & ";
            row.width(m_width);
            row << pvalueItr->second.pvalue();
            row << " & ";
            row.width(m_width);
            row << pvalueItr->second.chi2();
            row << " & ";
            row.width(2);
            row.precision(0);
            row << pvalueItr->second.ndf();
            row.precision(m_precision);
         }
         else{
            row << " & ";
            row.width(m_width);
            row << " ";
            row << " & ";
            row.width(m_width);
            row << " \\multicolumn{2}{c|}{} ";
         }
         
         pvalueItr = prediction.find("NNLO");
         if(pvalueItr != prediction.end()){
            row << " & ";
            row.width(m_width);
            row << pvalueItr->second.pvalue();
            row << " & ";
            row.width(m_width);
            row << pvalueItr->second.chi2();
            row << " & ";
            row.width(2);
            row.precision(0);
            row << pvalueItr->second.ndf();
            row.precision(m_precision);
         }
         else{
            row << " & ";
            row.width(m_width);
            row << " ";
            row << " & ";
            row.width(m_width);
            row << " \\multicolumn{2}{c|}{} ";
         }
         
         pvalueItr = prediction.find("MCFM");
         if(pvalueItr != prediction.end()){
            row << " & ";
            row.width(m_width);
            row << pvalueItr->second.pvalue();
            row << " & ";
            row.width(m_width);
            row << pvalueItr->second.chi2();
            row << " & ";
            row.width(2);
            row.precision(0);
            row << pvalueItr->second.ndf();
            row.precision(m_precision);
         }
         else{
            row << " & ";
            row.width(m_width);
            row << " ";
            row << " & ";
            row.width(m_width);
            row << " \\multicolumn{2}{c|}{} ";
         }
         
         table.AddRow(row.str());
         
      }// end for(channel)
      
   } // end for(sampleName)
   
   table.hline();
   
   return table.GetTableText(m_caption,m_label);
}

void PValueTable::WriteTableToFile(const std::string filename){
   
   std::string table = GetTableText();
   
   std::ofstream outfile(filename.c_str(),std::ios_base::trunc);
   if(!outfile){
      std::cerr << "[PValueTable::WriteTableToFile] ERROR opening file " << filename << " for writing.\n";
      return;
   }
   
   outfile << table;
   
   outfile.close();
   
}


std::string PValueTable::GenerateColumnRules(void) const{
   std::string s = " c | c | c r@{/}l | c r@{/}l | c r@{/}l | c r@{/}l | c r@{/}l ";
   return s;
}

void PValueTable::GenerateHeaderRow(TexTable& table) const{
   table.AddRow("Variable & Channel & \\multicolumn{3}{|c|}{Alpgen} & \\multicolumn{3}{|c|}{MC@NLO} & \\multicolumn{3}{|c|}{POWHEG} & \\multicolumn{3}{|c|}{NLO+NNLL} & \\multicolumn{3}{|c}{MCFM}");
   table.AddRow("         &         &  $p$-value  & \\multicolumn{2}{|c|}{$\\chi^{2}$/NDF} &  $p$-value  & \\multicolumn{2}{|c|}{$\\chi^{2}$/NDF}   &  $p$-value  & \\multicolumn{2}{|c|}{$\\chi^{2}$/NDF}    &  $p$-value  & \\multicolumn{2}{|c|}{$\\chi^{2}$/NDF}    &  $p$-value  & \\multicolumn{2}{|c}{$\\chi^{2}$/NDF}   ");
}
