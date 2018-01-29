
#include "CombSystTable.h"

#include "TexTable.h"
#include "SystematicSample.h"
#include "SystematicList.h"
#include "SystematicBinList.h"
#include "globals.h"
#include "TH1D.h"
#include "KinematicVariable.h"

CombSystTable::CombSystTable(SystematicList& systematics, 
                     const std::string label,
                     TH1* nominal,
                     std::string sampleName,
                     const bool normalized,
                     const bool relativeErrors):
      m_syst(systematics)
{
   m_label = label;
   m_sampleName = sampleName;
   m_placement = "ht!";
   m_normalized = normalized;
   m_nominal = nominal;
   m_relative = relativeErrors;
   SetScaler();
}

CombSystTable::~CombSystTable(){
}

std::string CombSystTable::GetTableText(const std::string caption,const std::string filename){
   
   // warn if caption is empty
   if(caption.size() == 0){
      std::cout << "[CombSystTable::GetTableText] WARNING no caption set for table, " << m_label << "\n";
   }
   
   SetScaler();
   
   // if there are no column rules, generate some
   if(m_columnRules.size() == 0){
//       std::cout << "[CombSystTable::GetTableText] Generating default column rules for table, " << m_label << ": ";
      m_columnRules = GenerateColumnRules();
   }
   
      
   // create table
   TexTable table(m_placement,m_columnRules);
   
   // add header row with labels
   table.hline();
   table.AddRow(GenerateHeaderRow());
   table.hline();
   table.hline();
   
   // fill data
   
   
   // fill Total Errors
   SystematicBinList* totalSystPerBin = m_syst.GetCombinedBins();
   
   double stat2 = 0;
   std::stringstream row;
   row.precision(m_precision);
   row.width(m_width);
   row.setf(std::ios::fixed);
   
   TH1* hup = (TH1*)m_nominal->Clone("hup");
   hup->SetDirectory(false);
   hup->Reset();
   TH1* hdown = (TH1*)m_nominal->Clone("hdown");
   hdown->SetDirectory(false);
   hdown->Reset();
   
   for(unsigned int bin=1;bin<=m_nominal->GetNbinsX();++bin){
      
      double binLowEdge = (*totalSystPerBin)[bin-1].low();
      double binHighEdge = (*totalSystPerBin)[bin-1].high();
      
      double binContent = (*totalSystPerBin)[bin-1].content()*m_scaler;
      double stat       = (*totalSystPerBin)[bin-1].stat()*m_scaler;
      if(m_relative) stat = stat/binContent*100.;
      double down       = (*totalSystPerBin)[bin-1].down()*m_scaler;
      if(m_relative) down = down/binContent*100.;
      double up         = (*totalSystPerBin)[bin-1].up()*m_scaler;
      if(m_relative) up   = up  /binContent*100.;
      
      row.str("");
      row.precision(0);
      row.width(3);
      row << binLowEdge << " & " << binHighEdge << " & ";
      row.precision(m_precision);
      row.width(m_width);
      // bin data
      row << binContent << " & ";
      if(m_relative){
         row.precision(2);
         row.width(4);
         row.setf(std::ios::fixed);
      }
      // statistical error
      row << "$\\pm$" << stat << " & ";
      // systematic error
      row << "$-" << down << "$ & $+" << up << "$";
      if(m_relative){
         row.precision(m_precision);
         row.width(m_width);
         row.setf(std::ios::fixed);
      }
      table.AddRow(row.str());
      table.hline();
      
      // for totals
      stat2 += pow((*totalSystPerBin)[bin-1].stat()*(binHighEdge-binLowEdge),2);
      
      hup->SetBinContent(bin,  (*totalSystPerBin)[bin-1].content()+(*totalSystPerBin)[bin-1].up());
      hdown->SetBinContent(bin,(*totalSystPerBin)[bin-1].content()-(*totalSystPerBin)[bin-1].down());
   }
   
   table.hline();
   
   
   // totals
   row.str("");
   if(!m_normalized){
      double statFactor = sqrt(stat2);
      double stat = sqrt(stat2);
      double up = hup->Integral("width") - m_nominal->Integral("width");
      double down = m_nominal->Integral("width") - hdown->Integral("width");
      
      double total         = m_nominal->Integral("width");
      double total_stat    = stat;
      if(m_relative) total_stat = total_stat/total*100.;
      double total_down    = down;
      if(m_relative) total_down = total_down/total*100.;
      double total_up      = up;
      if(m_relative) total_up   = total_up/total*100.;
      
      // inclusive
      row.str("");
      row.precision(2);
      row << "\\multicolumn{2}{l}{$\\sigma_{tt}^{inc}$ [pb]} & " << total;
      if(m_relative)
         row.precision(2);
      else
         row.precision(4);
      row << " & $\\pm$" << total_stat;
      if(m_relative)
         row.precision(2);
      else
         row.precision(3);
      row << " & $-" << total_down;
      row << "$ & $+" << total_up << "$";
      
      table.AddRow(row.str());
      table.hline();
   
      // need to write an output file for combineChannels
      std::string texFilename = globals::GetCombinationTextFilePath() + "/InclusiveXS_" + filename + ".tex";
//       std::cout << "[CombSystTable::GetTableText] Writing tex file: " << texFilename << "\n";
      std::ofstream outputFile(texFilename.c_str());
      if(outputFile.fail()){
         std::cerr << "[CombSystTable::GetTableText] ERROR opening file " << texFilename << "\n";
      }
      else{
         outputFile<<m_nominal->Integral("width") <<"   "<<sqrt(stat)<<"   "<<down<<"   "<<up<<"\n";
         outputFile.close();
      }
   }
   
   
   return table.GetTableText(caption,m_label);
}


std::string CombSystTable::GenerateColumnRules(void) const{
   
   std::stringstream s;
   // first two bins are the bin edges
   s << "r@{  to }r ";
   // next bins are the contents
   for(unsigned int i=0;i<m_ncolumns-2;++i)
      s << "c ";
   
   s << "r@{/} l";
   
   return s.str();
   
}

std::string CombSystTable::GenerateHeaderRow(void) const{
   
   KinematicVariable* kinVar = new KinematicVariable(m_sampleName);
   std::string kinVarLabel  = kinVar->GetLatexKinVarLabel();
   std::string kinVarUnitLabel = kinVar->GetUnitKinVarLabel();
   std::string NkinVarLabel = kinVar->GetLatexTNKinVarLabel();
   std::string DkinVarLabel = kinVar->GetLatexTDKinVarLabel();
   std::string UnitNkinVarLabel = kinVar->GetUnitNKinVarLabel();
   std::string UnitDkinVarLabel = kinVar->GetUnitDKinVarLabel();
   
   std::stringstream s;
   s << "\\multicolumn{2}{l}{$"<<kinVarLabel<<"$ " << kinVarUnitLabel << "} & ";
   if(isNormalized()){
      s << NkinVarLabel <<" & ";
      if(m_relative){
         s << " stat. [\\%] & ";
         s << " \\multicolumn{2}{c}{syst. [\\%]}";
      }
      else{
         s << " stat. [" << UnitNkinVarLabel <<"] & ";
         s << " \\multicolumn{2}{c}{syst. [" <<UnitNkinVarLabel<<"]}";
      }
   }
   else{
      s << DkinVarLabel <<" & ";
      
      if(m_relative){
         s << " stat. [\\%] & ";
         s << " \\multicolumn{2}{c}{syst. [\\%]}";
      }
      else{
         s << " stat. " << UnitDkinVarLabel <<" & ";
         s << " \\multicolumn{2}{c}{syst. "<< UnitDkinVarLabel <<"}";
      }
   }
   
   return s.str();
   
}

void CombSystTable::SetScaler(void){
   
   if(m_normalized)
      m_scaler = 1000.; // want in TeV^{-1} instead of GeV^{-1}
   else
      m_scaler = 1.; // want in pb/GeV
   
}

// rounds irrational number to the decimal place speficied by precision
double CombSystTable::RoundValue(const double value,const unsigned int precision){
   double factor = pow(10.,precision);
   return floor(value * factor + 0.5) / factor;
}
