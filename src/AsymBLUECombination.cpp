
#include "AsymBLUECombination.h"

#include "SystematicTextFile.h"
#include "SystematicList.h"
#include "SystematicBin.h"

#include "TH1D.h"
#include "TGraphAsymmErrors.h"

AsymBLUECombination::AsymBLUECombination(
         const std::string filenameA_up,
         const std::string filenameA_down,
         const std::string filenameB_up,
         const std::string filenameB_down,
         std::vector<std::string>& correlatedSystematics,
         const BLUECombination::StatTreatment statTreatment):
            m_correlatedSystematics(correlatedSystematics),
            m_statTreatment(statTreatment)
{
   Init();
   
   
   // A & B represent the two channels that will be combined
   // Text files contain the lists of the systematics included
   // for each channel and information about each bin/systematic
   // There is a file containing the upward systematic and a file
   // containing the downward systematic
   SystematicTextFile fileA_up  (filenameA_up);
   fileA_up.DeleteList(false);
   SystematicTextFile fileA_down(filenameA_down);
   fileA_down.DeleteList(false);
   SystematicTextFile fileB_up  (filenameB_up);
   fileB_up.DeleteList(false);
   SystematicTextFile fileB_down(filenameB_down);
   fileB_down.DeleteList(false);
   
   m_delete_lists = true;
   
   m_listA_up   = fileA_up.list();
   m_listA_down = fileA_down.list();
   m_listA_central = new SystematicList();
   m_listB_up   = fileB_up.list();
   m_listB_down = fileB_down.list();
   m_listB_central = new SystematicList();
   
   m_combined = 0;
   
   Setup();
}

AsymBLUECombination::AsymBLUECombination(
      SystematicList* listA_up,
      SystematicList* listA_down,
      SystematicList* listB_up,
      SystematicList* listB_down,
      std::vector<std::string>& correlatedSystematics,
      const BLUECombination::StatTreatment statTreatment):
         m_correlatedSystematics(correlatedSystematics),
         m_statTreatment(statTreatment)
{
   Init();
   
   // A & B represent the two channels that will be combined
   m_listA_up   = listA_up;
   m_listA_down = listA_down;
   m_listA_central = new SystematicList();
   m_listB_up   = listB_up;
   m_listB_down = listB_down;
   m_listB_central = new SystematicList();
   
   Setup();
   
}

void AsymBLUECombination::Init(){
   
   m_graphWithSyst = 0;
   m_histoWithStat = 0;
   
   m_histoWithSystUp = 0;
   m_histoWithStatUp = 0;
   m_histoWithSystDown = 0;
   m_histoWithStatDown = 0;
   m_histoWithSystCentral = 0;
   m_histoWithStatCentral = 0;
   
   m_listA_up      = 0;
   m_listA_down    = 0;
   m_listA_central = 0;
   m_listB_up      = 0;
   m_listB_down    = 0;
   m_listB_central = 0;
   
   m_comb_up       = 0;
   m_comb_down     = 0;
   m_comb_central  = 0;
   
   m_delete_lists  = false;
}

void AsymBLUECombination::Setup(){
   
   // Also need the central values (average of the up/down systematics)
   GetCentralList(m_listA_up,m_listA_down,m_listA_central);
   GetCentralList(m_listB_up,m_listB_down,m_listB_central);
   
   // now run BLUE Method on up, down, cental
   m_comb_up = new BLUECombination(m_listA_up,m_listB_up,m_correlatedSystematics,m_statTreatment);
   m_comb_up->CalculateCovarianceMatrix();
   
   m_comb_down = new BLUECombination(m_listA_down,m_listB_down,m_correlatedSystematics,m_statTreatment);
   m_comb_down->CalculateCovarianceMatrix();
   
   m_comb_central = new BLUECombination(m_listA_central,m_listB_central,m_correlatedSystematics,m_statTreatment);
   m_comb_central->CalculateCovarianceMatrix();
   
}

AsymBLUECombination::~AsymBLUECombination(){
   if(m_graphWithSyst){
      delete m_graphWithSyst; m_graphWithSyst = 0;
   }
   if(m_histoWithStat){
      delete m_histoWithStat; m_histoWithStat = 0;
   }
   if(m_histoWithSystUp){
      delete m_histoWithSystUp; m_histoWithSystUp = 0;
   }
   if(m_histoWithStatUp){
      delete m_histoWithStatUp; m_histoWithStatUp = 0;
   }
   if(m_histoWithSystDown){
      delete m_histoWithSystDown; m_histoWithSystDown = 0;
   }
   if(m_histoWithStatDown){
      delete m_histoWithStatDown; m_histoWithStatDown = 0;
   }
   if(m_histoWithSystCentral){
      delete m_histoWithSystCentral; m_histoWithSystCentral = 0;
   }
   if(m_histoWithStatCentral){
      delete m_histoWithStatCentral; m_histoWithStatCentral = 0;
   }
   if(m_comb_up){
      delete m_comb_up;m_comb_up = 0;
   }
   if(m_comb_down){
      delete m_comb_down;m_comb_down = 0;
   }
   if(m_comb_central){
      delete m_comb_central;m_comb_central = 0;
   }
   if(m_listA_central){
      delete m_listA_central;m_listA_central = 0;
   }
   if(m_listB_central){
      delete m_listB_central;m_listB_central = 0;
   }
   if(m_delete_lists){
      if(m_listA_up){
         delete m_listA_up;m_listA_up = 0;
      }
      if(m_listA_down){
         delete m_listA_down;m_listA_down = 0;
      }
      if(m_listB_up){
         delete m_listB_up;m_listB_up = 0;
      }
      if(m_listB_down){
         delete m_listB_down;m_listB_down = 0;
      }
   }
   if(m_combined){
      delete m_combined;m_combined = 0;
   }
}

const SystematicBinList& AsymBLUECombination::Combine(
                                 const std::vector<double>& dataA,
                                 const std::vector<double>& dataB)
{
   
   // make combination, use new data if wanted
   const SystematicBinList& binListUp      = m_comb_up->Combine(dataA,dataB);
   const SystematicBinList& binListDown    = m_comb_down->Combine(dataA,dataB);
   const SystematicBinList& binListCentral = m_comb_central->Combine(dataA,dataB);
   
   m_combined = new SystematicBinList;
   
   // use central BLUE to fill the bin list
   for(unsigned int i=0;i<binListCentral.size();++i){
      m_combined->push_back(binListCentral[i]);
      
      // This is the asymmetric BLUE method
      double Rupper = 0;
      if(binListUp[i].syst()+binListDown[i].syst())
         Rupper = (binListUp[i].syst())/(binListUp[i].syst()+binListDown[i].syst());
      else{
         std::cerr << "ERROR Rupper divide by zero, should not be possible.\n";
         return *m_combined;
      }
      double testUp = 2. * Rupper * binListCentral[i].syst();
      double testDown = 2. * (1.-Rupper) * binListCentral[i].syst();
      (*m_combined)[i].up(testUp);
      (*m_combined)[i].percentContributionUp(binListUp[i].percentContribution());
      
      (*m_combined)[i].down(testDown);
      (*m_combined)[i].percentContributionDown(binListDown[i].percentContribution());
      
      (*m_combined)[i].percentContribution(binListCentral[i].percentContribution());
      (*m_combined)[i].syst(binListCentral[i].syst());
      
   }
   
   // keep copies of the histograms
   m_histoWithSystUp = (TH1D*)m_comb_up->GetHistoWithSyst()->Clone();
   m_histoWithSystUp->SetDirectory(false);
   m_histoWithStatUp = (TH1D*)m_comb_up->GetHistoWithStat()->Clone();
   m_histoWithStatUp->SetDirectory(false);
   m_histoWithSystDown = (TH1D*)m_comb_down->GetHistoWithSyst()->Clone();
   m_histoWithSystDown->SetDirectory(false);
   m_histoWithStatDown = (TH1D*)m_comb_down->GetHistoWithStat()->Clone();
   m_histoWithStatDown->SetDirectory(false);
   m_histoWithSystCentral = (TH1D*)m_comb_central->GetHistoWithSyst()->Clone();
   m_histoWithSystCentral->SetDirectory(false);
   m_histoWithStatCentral = (TH1D*)m_comb_central->GetHistoWithStat()->Clone();
   m_histoWithStatCentral->SetDirectory(false);
   
   return *m_combined;
}

void AsymBLUECombination::GetCentralList(SystematicList* up,
                                         SystematicList* down,
                                         SystematicList* central)
{
   SystematicList::const_iterator itrUp = up->begin();
   SystematicList::const_iterator itrDown = down->begin();
   
   
   for(;itrUp != up->end() && itrDown != down->end();++itrUp,++itrDown){
      const std::string systematic = itrUp->first;
      if(systematic != itrDown->first){
         std::cerr << "[AsymBLUECombination::GetCentralList] ERROR systematic names do not match.\n";
         return;
      }
      
      const SystematicBinList& binListUp = itrUp->second;
      const SystematicBinList& binListDown = itrDown->second;
      
      if(binListUp.size() != binListDown.size()){
         std::cerr << "[AsymBLUECombination::GetCentralList] ERROR number of bins do not match.\n";
         return;
      }
      
      SystematicBinList binListCentral = binListUp;
      for(unsigned int i=0;i<binListUp.size();++i){
         
         // this is the main quantity we need, the average of the up/down
         binListCentral[i].syst( (binListUp[i].syst()+binListDown[i].syst())/2. );
         
      }
      
      (*central)[systematic] = binListCentral;
      
   }
   
}

TGraphAsymmErrors* AsymBLUECombination::GetGraphWithSyst(){
   
   if(!m_graphWithSyst){
      
      // use histogram to set data points
      m_graphWithSyst = new TGraphAsymmErrors(GetHistoWithStat());
      m_graphWithSyst->SetName("BlueCombinationWithSyst");
      
      // then only need to change up/down errors
      for(unsigned int point=0;point<m_graphWithSyst->GetN();++point){
         m_graphWithSyst->SetPointEYhigh(point,(*m_combined)[point].up());
         m_graphWithSyst->SetPointEYlow(point,(*m_combined)[point].down());
      }
      
      
   }
   
   return m_graphWithSyst;
}


TH1D* AsymBLUECombination::GetHistoWithStat(){
   
   if(!m_histoWithStat){
      // build custom bin vector
      std::vector<double> binBoundaries;
      for(unsigned int i=0;i<m_combined->size();++i){
         binBoundaries.push_back((*m_combined)[i].low()); // push_back the low bin edge
      }
      // last include the high bin edge
      binBoundaries.push_back((*m_combined)[m_combined->size()-1].high());
      
      m_histoWithStat = new TH1D("BlueCombinationWithStat","",binBoundaries.size()-1,&binBoundaries.front());
      m_histoWithStat->SetDirectory(false);
      
      for(unsigned int bin=1;bin<=m_histoWithStat->GetNbinsX();++bin){
         m_histoWithStat->SetBinContent(bin,(*m_combined)[bin-1].content());
         m_histoWithStat->SetBinError(bin,(*m_combined)[bin-1].stat());
      }
      
      
   }
   
   return m_histoWithStat;
}

