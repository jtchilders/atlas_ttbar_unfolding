
#include "SystTable.h"

#include "TexTable.h"
#include "SystematicSample.h"
#include "SystematicList.h"
#include "SystematicBinList.h"
#include "KinematicVariable.h"
#include "MasterSystList.h"

SystTable::SystTable(SystematicList& systematics,
                     const std::string label,
                     TH1* nominal,
                     std::string sampleName,
                     const bool normalized,
                     const bool relative):
      m_syst(systematics)
{
   m_label = label;
   m_sampleName = sampleName;
   m_placement = "ht!";
   m_normalized = normalized;
   m_relative = relative;
   m_nominal = nominal;
   
   SetScaler();
}

SystTable::~SystTable(){
}

std::string SystTable::GetTableText(const std::string caption){
   
   // warn if caption is empty
   if(caption.size() == 0){
      std::cout << "[SystTable::GetTableText] WARNING no caption set for table, " << m_label << "\n";
   }
   
   SetScaler();
   
   // if there are no column rules, generate some
   if(m_columnRules.size() == 0){
      //std::cout << "[SystTable::GetTableText] Generating default column rules for table, " << m_label << "\n";
      m_columnRules = GenerateColumnRules();
   }
   
   ////////////
   /// First create the JES component table if needed
   /// Later create the systematic table
   ////////////////////////////////////////////////////////
   
   // create table
   std::string jes_output;
   if(MasterSystList::UseJesComponents()){
      TexTable jes_table(m_placement,m_columnRules);
      
      // generate header
      jes_table.hline();
      jes_table.AddRow(GenerateHeaderRow());
      jes_table.hline();
      jes_table.hline();
      
      // now add the total JES
      SystematicBinList* jes_bins = m_syst.GetCombinedJesComponentBins();
      std::string line = ListSystematicOnLine("Combined JES",jes_bins);
      jes_table.AddRow(line);
      jes_table.hline();
      jes_table.hline();
      
      // now loop over the JES components and print
      SystematicList::iterator sysItr = m_syst.begin();
      
      // iterate over the systematics, each one gets a row
      for(;sysItr != m_syst.end();++sysItr){
         std::string systematic = sysItr->first;
         // only want JES components
         if(MasterSystList::IsJesComponentSyst(systematic) ||
            MasterSystList::IsJesComponentBackgroundSyst(systematic)){
            // get pretty name
            std::string name=SystematicSample::GetSamplePrettyBaseString(systematic);
            
            SystematicBinList& binErrors = sysItr->second;
            std::string line = ListSystematicOnLine(name,&binErrors);
            
            jes_table.AddRow(line);
            jes_table.hline();
         }
      }// end for(systematics)
      std::string jes_label = m_label;
      jes_label += "_jesi";
      std::string jes_caption = caption;
      jes_caption += " This table shows only the JES Components and the total combined uncertainty resulting from these components.";
      jes_output = "\\clearpage\n";
      jes_output += jes_table.GetTableText(jes_caption,jes_label);
      
   }
   
   ////////////////
   /// Create the Systematic table
   ///////////////////////////////////////////
   
   // create table
   TexTable table(m_placement,m_columnRules);
   
   // add header row with labels
   table.hline();
   table.AddRow(GenerateHeaderRow());
   table.hline();
   table.hline();
   
   // fill Total Errors -----------------------
   SystematicBinList* totalSystPerBin = m_syst.GetCombinedBins();
   std::stringstream totalError;
   totalError.precision(m_precision);
   totalError.width(m_width);
   totalError.setf(std::ios::fixed);
   
   totalError << "Total";
   for(unsigned int bin=1;bin<=m_nominal->GetNbinsX();++bin){
      double stat = m_nominal->GetBinError(bin);
      double sys_up   = (*totalSystPerBin)[bin-1].up();
      double sys_down = (*totalSystPerBin)[bin-1].down();
      
      double comb_up = sqrt(stat*stat + sys_up*sys_up);
      double comb_down = sqrt(stat*stat + sys_down*sys_down);
      
      // add column separators
      totalError << " & ";
      
      double factor = m_relative ? (m_scaler/m_nominal->GetBinContent(bin)) : m_scaler;
      
      totalError << "$\\begin{array}{c}+" << std::fixed << comb_up*factor << "\\\\-" << comb_down*factor << "\\end{array}$";
      
   }
   
   // add totals to table
   table.AddRow(totalError.str());
   table.hline();
   
   // fill Total Statistical --------------------
   std::stringstream totalStat;
   totalStat.precision(m_precision);
   totalStat.width(m_width);
   totalStat.setf(std::ios::fixed);
   
   totalStat << "Statistical";
   for(unsigned int bin=1;bin<=m_nominal->GetNbinsX();++bin){
      double error = m_nominal->GetBinError(bin);
      
      // add column separators
      totalStat << " & ";
      
      double factor = m_relative ? (m_scaler/m_nominal->GetBinContent(bin)) : m_scaler;
      
      totalStat << "$\\begin{array}{c}\\pm" << std::fixed << error*factor << "\\end{array}$";
      
   }
   
   // add totals to table
   table.AddRow(totalStat.str());
   table.hline();
   
   // fill Total Systematics --------------------------
   std::string line = ListSystematicOnLine("Systematics",totalSystPerBin);
   // add totals to table
   table.AddRow(line);
   table.hline();
   table.hline();
   
   
   
   // fill each systematic ------------------------------
   
   // first add total component JES to the systematic list
   if(MasterSystList::UseJesComponents()){
      m_syst["jes"] = *(m_syst.GetCombinedJesComponentBins());
   }
   
   SystematicList::iterator sysItr = m_syst.begin();
   
   
   // iterate over the systematics, each one gets a row
   for(;sysItr != m_syst.end();++sysItr){
      std::string systematic = sysItr->first;
      
      // skip JES Components
      if(MasterSystList::UseJesComponents()){
         if(MasterSystList::IsJesComponentSyst(systematic) ||
            MasterSystList::IsJesComponentBackgroundSyst(systematic))
            continue;
      }
      // if normalized, skip luminosity
      if(m_normalized && systematic.find(SystematicSample::GetSampleBaseString(SystematicSample::LUMI)) != std::string::npos){
         continue;
      }
      
      // get pretty name
      std::string name=SystematicSample::GetSamplePrettyBaseString(systematic);
      
      SystematicBinList& binErrors = sysItr->second;
      std::string line = ListSystematicOnLine(name,&binErrors);
      
      
      table.AddRow(line);
      table.hline();
      
   }// end for(systematics)
   
   
   std::string output = "\\clearpage\n";
   output += table.GetTableText(caption,m_label);
   if(MasterSystList::UseJesComponents()){
      output += "\n\n";
      output += jes_output;
   }
   return output;
}


std::string SystTable::GenerateColumnRules(void) const{
   const unsigned int ncols = (m_syst.begin()->second).size();
   
   std::stringstream s;
   // first bin is the syst name
   s << "l ";
   // next bins are the contents
   for(unsigned int i=0;i<ncols;++i)
      s << "r ";
   
   return s.str();
   
}

std::string SystTable::GenerateHeaderRow(void) const{
   // get iterator to first systematic
   SystematicList::iterator sysItr = m_syst.begin();
   if(sysItr == m_syst.end()){
      std::cerr << "[SystTable::GenerateHeaderRow] ERROR no systematics found.\n";
      return "";
   }
   const SystematicBinList& binList = sysItr->second;
   
   KinematicVariable* kinVar = new KinematicVariable(m_sampleName);
   std::string kinVarLabel  = kinVar->GetLatexKinVarLabel();
   std::string dkinVarNoUnitsLabel  = kinVar->GetLatexDKinVarLabelNoUnits();
   std::string tnkinVarNoUnitsLabel  = kinVar->GetLatexTNKinVarNoUnitsLabel();
   std::string UnitNkinVarLabel = kinVar->GetUnitNKinVarLabel();
   std::string UnitDkinVarLabel = kinVar->GetUnitDKinVarLabel(); 
   
   std::stringstream s;
   if(isNormalized())
      s << tnkinVarNoUnitsLabel;
   else
      s << dkinVarNoUnitsLabel;
   
   if(isRelative())
      s << " [$\\%$]";
   else{
      if(isNormalized())
         s << " [ " << UnitNkinVarLabel <<"]";
      else
         s << UnitDkinVarLabel;
   }
   
   // next bins are the contents
   s.precision(0);
   s.width(3);
   s << std::fixed;
   for(unsigned int i=0;i<binList.size();++i)
      s << " & " << binList[i].low() << " to " << binList[i].high();
   
   return s.str();
   
}

void SystTable::SetScaler(void){
   if(m_relative){
      m_scaler = 100.; // want in %
      
   }
   else{
      if(m_normalized)
         m_scaler = 1000.; // want in TeV^{-1} instead of GeV^{-1}
      else
         m_scaler = 1.; // want in pb/GeV instead of fb/GeV
      
   }
}

// rounds irrational number to the decimal place speficied by precision
double SystTable::RoundValue(const double value,const unsigned int precision){
   double factor = pow(10.,precision);
   return floor(value * factor + 0.5) / factor;
}

std::string SystTable::ListSystematicOnLine(const std::string label,
                                            const SystematicBinList* systematicBins)
{
   std::stringstream totalSyst;
   totalSyst.precision(m_precision);
   totalSyst.width(m_width);
   totalSyst.setf(std::ios::fixed);
   
   totalSyst << label;
   for(unsigned int i=0;i<systematicBins->size();++i){
      double up         = (*systematicBins)[i].up();
      double shape_up   = (*systematicBins)[i].shape_up();
      double down       = (*systematicBins)[i].down();
      double shape_down = (*systematicBins)[i].shape_down();
      double content    = (*systematicBins)[i].content();
      
      // add column separators
      totalSyst << " & ";
      
      double factor = m_relative ? (m_scaler/m_nominal->GetBinContent(i+1)) : m_scaler;
      
      // if both up and down are equal to zero, just fill a dash '-'
      if(fabs(up) < pow(10.,-10) && fabs(down) < pow(10.,-10))
         totalSyst << "\\multicolumn{1}{c}{$-$}";
      // if the shape_up and shape_down are nonzero then use them
      else if(shape_up != 0 and shape_down != 0){
         std::string str_up;
         if( (shape_up - content) >= 0)
            str_up = "+";
         else
            str_up = "-";
         
         std::string str_down;
         if( (shape_down - content) >= 0)
            str_down = "+";
         else
            str_down = "-";
         
         totalSyst << "$\\begin{array}{c}+" << std::fixed << up*factor << "(" << str_up << ")" << "\\\\-" << down*factor << "(" << str_down << ")" << "\\end{array}$";
      }
      // test if up and down are symmetric to the second decimal place
      // using precision to determine where to make the cut of calling
      // the up = down. 0.99999 is to avoid killing differences to close
      // to the cut.
      else if(fabs(RoundValue(up*factor,m_precision)-RoundValue(down*factor,m_precision)) < (0.9999999999*pow(10.,-1.*(m_precision))))
         totalSyst << "$\\begin{array}{c}\\pm" << std::fixed << up*factor << "\\end{array}$";
      else
         totalSyst << "$\\begin{array}{c}+" << std::fixed << up*factor << "\\\\-" << down*factor << "\\end{array}$";
      
   }
   
   return totalSyst.str();
   
}
