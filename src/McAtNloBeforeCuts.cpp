
#include "McAtNloBeforeCuts.h"

#include "globals.h"

#include "TFile.h"
#include "TH1.h"

McAtNloBeforeCuts::McAtNloBeforeCuts(const TopDecayChannel channel, const std::string sampleName){
   m_chan = channel;
   m_samp = sampleName;
   m_histo = 0;
   m_histo_inEvtCnts = 0;
   m_histo_norm = 0;
   m_mainGenerator = false;
}

McAtNloBeforeCuts::McAtNloBeforeCuts(const std::string sampleName){
   m_samp = sampleName;
   m_histo = 0;
   m_histo_inEvtCnts = 0;
   m_histo_norm = 0;
   m_mainGenerator = false;
}

McAtNloBeforeCuts::~McAtNloBeforeCuts(){
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

TH1* McAtNloBeforeCuts::GetHisto(){
   
   if(!m_histo){
      
      if(!m_chan.IsSet()){
         m_histo = GetAverageHisto();
         if(m_histo == 0)
            return (TH1*)0;
      }
      else{
         
         // reset this histogram if it already exists
         if(!m_histo_inEvtCnts){
            delete m_histo_inEvtCnts;m_histo_inEvtCnts = 0;
         }
         
         m_histo_inEvtCnts = GetHisto(m_chan);
         if(!m_histo_inEvtCnts)
            return (TH1*)0;
         
         
         double weight_MCATNLO = globals::MCATNLO_CORRECTION/globals::LUMI/globals::BR_MCATNLO;
         m_histo = globals::LumiEfficiencyCorrectedSpectra(m_histo_inEvtCnts,m_samp,weight_MCATNLO,false,true);
         //m_histo = globals::LumiEfficiencyCorrectedSpectra(tmp,m_samp,globals::LUMI_WEIGHT_MCATNLO);
         m_histo->SetDirectory(false);
         
         std::string hname = m_histo_inEvtCnts->GetName();
         hname += "_EvtCnts";
         m_histo_inEvtCnts->SetName(hname.c_str());
         
      }
      
      
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
TH1* McAtNloBeforeCuts::GetHisto(const TopDecayChannel channel) const{
   std::string filename = globals::dataFilesPath + "/nominal/MC_";
   filename += channel.GetChannelString();
   filename += "_T1_McAtNlo_Jimmy_nominal_forEff.root";
   
   TFile *file = 0;
   for(unsigned int i=0;i<5 && !file;++i){
      file = TFile::Open(filename.c_str());
      if(!file){
         std::cerr << "[McAtNloBeforeCuts::GetHisto(channel)] ERROR file not opened: " << filename << std::endl;
         std::cerr << "[McAtNloBeforeCuts::GetHisto(channel)] Trying again.\n";
      }
   }
   if(!file){
      std::cerr << "[McAtNloBeforeCuts::GetHisto(channel)] ERROR file not opened: " << filename << std::endl;
      return (TH1*)0;
   }
   
   
   std::string histoName = "Kinem/TopFit_NTag0/";
   
   if(m_samp.find("Top1") != std::string::npos){
      histoName += "GeneratedTop1/GeneratedTop1PtSpectrum";
   }
   else if(m_samp.find("Top2") != std::string::npos){
      histoName += "GeneratedTop2/GeneratedTop2PtSpectrum";
   } 
   else if(m_samp.find("TopLepHad") != std::string::npos){
      histoName += "GeneratedTopLepHad/GeneratedTopLepHadPtSpectrum";
   }
   else if(m_samp.find("Pt") != std::string::npos){
      histoName += "GeneratedSystem/GeneratedSystemPtSpectrum";
   }
   else if(m_samp.find("Mass") != std::string::npos){
      histoName += "GeneratedSystem/GeneratedSystemMassSpectrum";
   }
   else if(m_samp.find("Rapidity") != std::string::npos){
      histoName += "GeneratedSystem/GeneratedSystemRapiditySpectrum";
   }   
   
   TH1* output = (TH1*)file->Get(histoName.c_str());
   if(!output){
      std::cerr << "[McAtNloBeforeCuts::GetHisto(channel)] could not retrieve " << histoName << " from file " << filename << "\n";
      return (TH1*)0;
   }
   output->SetDirectory(false);
   //double weight_MCATNLO = 1./globals::LUMI/globals::BR_MCATNLO;
   //m_histo = globals::LumiEfficiencyCorrectedSpectra(output,m_samp,weight_MCATNLO,true,true);
   output->SetDirectory(false);
   
   std::string hname = "H_MCATNLO_";
   hname += channel.GetChannelString();
   hname += "_";
   hname += m_samp;
   
   output->SetName(hname.c_str());
   
   file->Close();
   delete file;
   file = 0;
   
   return output;
}

// used by combineChannels.
TH1* McAtNloBeforeCuts::GetAverageHisto() const {
   
   TH1* el = GetHisto(TopDecayChannel::EJETS);
   if(el == 0)
      return (TH1*)0;
      
   TH1* mu = GetHisto(TopDecayChannel::MUJETS);
   if(mu == 0)
      return (TH1*)0;
   
   std::string hname = "H_MCATNLO_";
   hname += m_samp;
   
   TH1* average = (TH1*)el->Clone(hname.c_str());
   average->SetDirectory(false);
   average->Reset();
   
   for(unsigned int bin=1;bin<=average->GetNbinsX();++bin){
      double mean = (el->GetBinContent(bin)+mu->GetBinContent(bin))/2.;
      double error = (el->GetBinError(bin)+mu->GetBinError(bin))/2.;
      average->SetBinContent(bin,mean);
      average->SetBinError(bin,error);
   }
   
   double weight_MCATNLO = globals::MCATNLO_CORRECTION/globals::LUMI/globals::BR_MCATNLO;
   TH1* correctedAverage = globals::LumiEfficiencyCorrectedSpectra(average,m_samp,weight_MCATNLO,false,true);
   correctedAverage->SetDirectory(0);
   
   delete average; average = 0;
   delete el; el = 0;
   delete mu; mu = 0;
   
   return correctedAverage;
   
}

TH1* McAtNloBeforeCuts::GetHistoNorm(){
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

