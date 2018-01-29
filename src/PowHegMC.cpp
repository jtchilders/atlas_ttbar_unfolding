
#include "PowHegMC.h"

#include "globals.h"

#include "TFile.h"
#include "TH1.h"

const std::string PowHegMC::m_filename_base = "TTbar_PowHeg_Jimmy_nominal_forEff.root";

PowHegMC::PowHegMC(const std::string sampleName){
   m_samp = sampleName;
   m_histo = 0;
   m_histo_inEvtCnts = 0;
   m_histo_norm = 0;
   m_mainGenerator = false;
}

PowHegMC::~PowHegMC(){
   if(!m_histo){
      delete m_histo;m_histo = 0;
   }
   if(!m_histo_norm){
      delete m_histo_norm;m_histo_norm = 0;
   }
   if(!m_histo_inEvtCnts){
      delete m_histo_inEvtCnts;m_histo_inEvtCnts = 0;
   }
}

TH1* PowHegMC::GetHisto(){
   
   if(!m_histo){
      
      // reset this histogram if it already exists
      if(!m_histo_inEvtCnts){
         delete m_histo_inEvtCnts;m_histo_inEvtCnts = 0;
      }
      
      m_histo_inEvtCnts = GetHistoFromFile();
      if(!m_histo_inEvtCnts)
         return (TH1*)0;
      
      
      double weight = 1./globals::LUMI/globals::BR_POWHEG;
      m_histo = globals::LumiEfficiencyCorrectedSpectra(m_histo_inEvtCnts,m_samp,weight,false,true);
      //m_histo = globals::LumiEfficiencyCorrectedSpectra(tmp,m_samp,globals::LUMI_WEIGHT_MCATNLO);
      m_histo->SetDirectory(false);
      
      std::string hname = "H_POWHEG_" + m_samp;
      m_histo->SetName(hname.c_str());
      hname += "_EvtCnts";
      m_histo_inEvtCnts->SetName(hname.c_str());
      
      
      
      if(m_mainGenerator){
         m_histo->SetLineColor(  (unsigned int)m_mg_lineColor);
         m_histo->SetLineStyle(  (unsigned int)m_mg_lineStyle);
         m_histo->SetLineWidth(  (unsigned int)m_mg_lineWidth);
         m_histo->SetFillColor(  (unsigned int)m_mg_fillColor);
         m_histo->SetFillStyle(  (unsigned int)m_mg_fillStyle);
         m_histo->SetMarkerColor((unsigned int)m_mg_markerColor);
         m_histo->SetMarkerStyle((unsigned int)m_mg_markerStyle);
      }
      else{
         m_histo->SetLineColor(  (unsigned int)m_lineColor);
         m_histo->SetLineStyle(  (unsigned int)m_lineStyle);
         m_histo->SetLineWidth(  (unsigned int)m_lineWidth);
         m_histo->SetFillColor(  (unsigned int)m_fillColor);
         m_histo->SetFillStyle(  (unsigned int)m_fillStyle);
         m_histo->SetMarkerColor((unsigned int)m_markerColor);
         m_histo->SetMarkerStyle((unsigned int)m_markerStyle);
      }
   }
   
   return m_histo;
   
}

// used by combineSystematics
TH1* PowHegMC::GetHistoFromFile() const{
   
   // first there are two files with this plot one from the e+jets
   // and one from the mu+jets, but they should be identical. I want
   // to make a check and ensure they are identical before just choosing
   // one. There is an ERROR thrown if they do not match.
   
   std::string filename_el = globals::dataFilesPath + "/nominal/MC_el_" + m_filename_base;
   std::string filename_mu = globals::dataFilesPath + "/nominal/MC_mu_"  + m_filename_base;
   
   TH1* histo_el = GetHistoFromFile(filename_el);
   TH1* histo_mu = GetHistoFromFile(filename_mu);
   
   if(!HistogramsAreEqual(histo_el,histo_mu)){
      std::cerr << "[PowHegMC::GetHistoFromFile] ERROR Histograms from e+jets and mu+jets are not equal.\n";
      return (TH1*)0;
   }
   
   TH1* output = histo_el;
   
   return output;
}

TH1* PowHegMC::GetHistoFromFile(const std::string filename) const{
   TFile *file = 0;
   for(unsigned int i=0;i<5 && !file;++i){
      file = TFile::Open(filename.c_str());
      if(!file){
         std::cerr << "[PowHegMC::GetHistoFromFile] ERROR file not opened: " << filename << std::endl;
         std::cerr << "[PowHegMC::GetHistoFromFile] Trying again.\n";
      }
   }
   if(!file){
      std::cerr << "[PowHegMC::GetHistoFromFile] ERROR file not opened: " << filename << std::endl;
      return (TH1*)0;
   }
   
   
   std::string histoName = "Kinem/TopFit_NTag0/";
   
   if(m_samp.find("Top1") != std::string::npos){
      histoName += "GeneratedWeightTop1/GeneratedWeightTop1PtSpectrum";
   }
   else if(m_samp.find("Top2") != std::string::npos){
      histoName += "GeneratedWeightTop2/GeneratedWeightTop2PtSpectrum";
   } 
   else if(m_samp.find("TopLepHad") != std::string::npos){
      histoName += "GeneratedWeightTopLepHad/GeneratedWeightTopLepHadPtSpectrum";
   }
   else if(m_samp.find("Pt") != std::string::npos){
      histoName += "GeneratedWeightSystem/GeneratedWeightSystemPtSpectrum";
   }
   else if(m_samp.find("Mass") != std::string::npos){
      histoName += "GeneratedWeightSystem/GeneratedWeightSystemMassSpectrum";
   }
   else if(m_samp.find("Rapidity") != std::string::npos){
      histoName += "GeneratedWeightSystem/GeneratedWeightSystemRapiditySpectrum";
   }   
   
   TH1* output = (TH1*)file->Get(histoName.c_str());
   if(!output){
      std::cerr << "[PowHegMC::GetHisto(channel)] could not retrieve " << histoName << " from file " << filename << "\n";
      return (TH1*)0;
   }
   output->SetDirectory(false);
   
   return output;
}


bool PowHegMC::HistogramsAreEqual(TH1 const * const histA, TH1 const * const histB) const{
   
   if(histA->GetEntries() != histB->GetEntries())
      return false;
   
   if(histA->GetNbinsX() != histB->GetNbinsX())
      return false;
   
   if(histA->GetMean() != histB->GetMean())
      return false;
   
   for(unsigned int bin=1;bin<=histA->GetNbinsX();++bin){
      
      if(histA->GetBinContent(bin) != histB->GetBinContent(bin))
         return false;
      
      if(histA->GetBinError(bin) != histB->GetBinError(bin))
         return false;
      
      if(histA->GetBinLowEdge(bin) != histB->GetBinLowEdge(bin))
         return false;
      
      if(histA->GetBinWidth(bin) != histB->GetBinWidth(bin))
         return false;
   }
   
   return true;
}


TH1* PowHegMC::GetHistoNorm(){
   if(!m_histo_norm){
      if(GetHisto() == 0){
         return (TH1*)0;
      }
      std::string name = GetHisto()->GetName();
      name += "_NORM";
      m_histo_norm = (TH1*)GetHisto()->Clone(name.c_str());
      m_histo_norm->SetDirectory(false);
      m_histo_norm->Scale(1./GetHisto()->Integral("width"));
   }
   
   return m_histo_norm;
   
}

