
#include "AlpgenMC.h"

#include "globals.h"
#include "KinematicVariable.h"

#include "TFile.h"
#include "TH1.h"

AlpgenMC::AlpgenMC(const std::string channel, const std::string sampleName){
   m_chan = channel;
   m_samp = sampleName;
   m_histo = 0;
   m_histo_inEvtCnts = 0;
   m_histo_norm = 0;
   m_mainGenerator = false;
}


AlpgenMC::AlpgenMC(const std::string sampleName){
   m_samp = sampleName;
   m_histo = 0;
   m_histo_inEvtCnts = 0;
   m_histo_norm = 0;
   m_mainGenerator = false;
}

AlpgenMC::~AlpgenMC(){
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

TH1* AlpgenMC::GetHisto(){
   
   if(!m_histo){
      
      // if Channel is set, only return histogram for that channel
      if(m_chan.size() > 0){
         m_histo =  GetHisto(m_chan);
      }
      // if channel not set, return the average of the two channels
      else{
         TH1* el = GetHisto("el");
         TH1* mu = GetHisto("mu");
         
         std::string histoname = "H_ALPGEN_" + m_samp;
         TH1* average = (TH1*)el->Clone(histoname.c_str());
         average->SetDirectory(false);
         average->Reset();
         
         for(unsigned int bin=1;bin<=average->GetNbinsX();++bin){
            double mean = (el->GetBinContent(bin)+mu->GetBinContent(bin))/2.;
            double error = (el->GetBinError(bin)+mu->GetBinError(bin))/2.;
            average->SetBinContent(bin,mean);
            average->SetBinError(bin,error);
         }
         
//          double weight_ALPGEN = 1./globals::LUMI/globals::BR_ALPGEN;
//          // set lumi correction to false in this function since we are already correcting for LUMI in the weight, but need to add LUMI in somewhere
//          m_histo = globals::LumiEfficiencyCorrectedSpectra(average,m_samp,weight_ALPGEN,false,true);
//          m_histo->SetDirectory(false);
         
         m_histo = average;
         
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

TH1* AlpgenMC::GetHisto(const std::string channel) {
   
   TH1* histo = 0;
   
   std::string filename = globals::dataFilesPath + "/nominal/MC_";
   filename += channel;
   filename += "_AlpgenJimmyttbarlnqq_nominal_forEff.root";
   
   TFile *file = 0;
   for(unsigned int i=0;i<5 && !file;++i){
      file = TFile::Open(filename.c_str());
      if(!file){
         std::cerr << "[AlpgenMC::GetHisto] ERROR file not opened: " << filename << std::endl;
         std::cerr << "[AlpgenMC::GetHisto] Trying again.\n";
      }
   }
   if(!file){
      std::cerr << "[AlpgenMC::GetHisto] ERROR file not opened: " << filename << std::endl;
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
   
   TH1* before_cuts = (TH1*)file->Get(histoName.c_str());
   if(!before_cuts){
      std::cerr << "[AlpgenMC::GetHisto] could not retreive " << histoName << " from file " << filename << "\n";
      return (TH1*)0;
   }
   before_cuts->SetDirectory(false);
   
   double weight_ALPGEN = 1./globals::LUMI/globals::BR_ALPGEN;
   
   //in one event there are two top quarks. Both leptonic and hadronic top Pt were filled.
   if (m_samp.find("TopLepHad") != std::string::npos)
      weight_ALPGEN *= 0.5;
   
   // clear this histogram if it is already set
   if(m_histo_inEvtCnts){
      delete m_histo_inEvtCnts;
      m_histo_inEvtCnts = 0;
   }
   
   std::string histoname = "H_ALPGEN_" + channel + "_" + m_samp;
   std::string histoname_inEvtCnts = histoname + "_EvtCnts";
   std::string histoname_xs = histoname;
   
   m_histo_inEvtCnts = (TH1*)before_cuts->Clone(histoname_inEvtCnts.c_str());
   m_histo_inEvtCnts->SetDirectory(false);
   
   histo = (TH1*)m_histo_inEvtCnts->Clone(histoname_xs.c_str());
   histo->SetDirectory(false);
   
   histo->Scale(weight_ALPGEN);
   
   //dividing by bin width
   for (Int_t i=1;i<=histo->GetNbinsX();++i){
      histo->SetBinContent(i,histo->GetBinContent(i)/histo->GetBinWidth(i));
      histo->SetBinError(i,histo->GetBinError(i)/histo->GetBinWidth(i));
   }
   
   // get right labels
   KinematicVariable* kinVar = new KinematicVariable(m_samp);
   std::string UkinVarLabel = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   
   histo->GetYaxis()->SetTitle(DkinVarLabel.c_str());
   histo->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   histo->SetTitle("");
   
   delete before_cuts;
   before_cuts = 0;
   
   delete file; file = 0;
   
   return histo;
}

TH1* AlpgenMC::GetHistoNorm(){
   if(!m_histo_norm){
      if(GetHisto() == 0)
         return (TH1*)0;
      
      
      std::string name = GetHisto()->GetName();
      name += "Norm";
      m_histo_norm = (TH1*)GetHisto()->Clone(name.c_str());
      m_histo_norm->SetDirectory(false);
      m_histo_norm->Scale(1./GetHisto()->Integral("width"));
   }
   
   return m_histo_norm;
   
}

