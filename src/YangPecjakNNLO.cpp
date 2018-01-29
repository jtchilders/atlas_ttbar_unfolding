#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include "YangPecjakNNLO.h"

#include "globals.h"

#include "TH1.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"

YangPecjakNNLO::YangPecjakNNLO(){
   m_histo = 0;
   m_histo_norm = 0;
   m_h_up = 0;
   m_h_down = 0;
   m_graph = 0;
   m_graph_norm = 0;
   m_filename = globals::dataFilesPath + "/Mtt_7000_Mtt_fin.root";
   m_histoname = "h_theoryMtt";
}

YangPecjakNNLO::~YangPecjakNNLO(){
   if(!m_histo){
      delete m_histo;m_histo = 0;
   }
   if(!m_histo_norm){
      delete m_histo_norm;m_histo_norm = 0;
   }
   if(!m_graph){
      delete m_graph;m_graph = 0;
   }
   if(!m_graph_norm){
      delete m_graph_norm;m_graph_norm = 0;
   }
   if(!m_h_down){
      delete m_h_down;m_h_down = 0;
   }
   if(!m_h_up){
      delete m_h_up;m_h_up = 0;
   }
   
   for(unsigned int i=0;i<m_h_theory.size();++i){
      delete m_h_theory[i]; m_h_theory[i] = 0;
   }
}

TGraphAsymmErrors* YangPecjakNNLO::GetGraph(){
   
   if(!m_graph){
      
      RetrieveHistosFromFile();
      if(m_h_theory.size() == 0){
         return (TGraphAsymmErrors*)0;
      }
      
      // first plot is nominal all others will be added in quadrature for uncertainty
      m_histo = (TH1*)m_h_theory[0]->Clone("H_YANG_PECJAK_NLO_NNLL_SystemMass");
      m_graph = new TGraphAsymmErrors(m_histo);
      m_graph->SetName("G_YANG_PECJAK_NLO_NNLL_SystemMass");
      
      m_histo->SetLineColor(  (unsigned int)m_lineColor);
      m_histo->SetLineStyle(  (unsigned int)m_lineStyle);
      m_histo->SetLineWidth(  (unsigned int)m_lineWidth);
      m_histo->SetFillColor(  (unsigned int)m_fillColor);
      m_histo->SetFillStyle(  (unsigned int)m_fillStyle);
      m_histo->SetMarkerColor((unsigned int)m_markerColor);
      m_histo->SetMarkerStyle((unsigned int)m_markerStyle);
      
      m_graph->SetLineColor(  (unsigned int)m_lineColor);
      m_graph->SetLineStyle(  (unsigned int)m_lineStyle);
      m_graph->SetLineWidth(  (unsigned int)m_lineWidth);
      m_graph->SetFillColor(  (unsigned int)m_fillColor);
      m_graph->SetFillStyle(  (unsigned int)m_fillStyle);
      m_graph->SetMarkerColor((unsigned int)m_markerColor);
      m_graph->SetMarkerStyle((unsigned int)m_markerStyle);
      
      m_h_up = (TH1*)m_histo->Clone("hup");
      m_h_up->SetDirectory(false);
      m_h_up->Reset();
      m_h_down = (TH1*)m_histo->Clone("hdown");
      m_h_down->SetDirectory(false);
      m_h_down->Reset();
      for(unsigned int bin=1;bin<=m_histo->GetNbinsX();++bin){
         
         double error=0.;
         
         // start loop from one since the first element in vector is the nominal histogram
         // sum over theory bins the square of the difference
         for(unsigned int i=1;i<m_h_theory.size();++i){
            double diff = fabs(m_h_theory[i]->GetBinContent(bin) - m_histo->GetBinContent(bin));
            
            error += diff*diff;
         }
         
         // take square root to get errors added in quadrature
         error = sqrt(error);
         
         m_graph->SetPointEYlow(bin-1,error);
         m_graph->SetPointEYhigh(bin-1,error);
         
         m_h_up->SetBinContent(bin,m_histo->GetBinContent(bin) + error);
         m_h_down->SetBinContent(bin,m_histo->GetBinContent(bin) - error);
      }
      
      // need to write an output file for combineChannels
      double up = m_h_up->Integral("width") - m_histo->Integral("width");
      double down = m_histo->Integral("width") - m_h_down->Integral("width");
      
      std::string texFilename = globals::GetCombinationTextFilePath() + "/InclusiveXS_YangPecjakNNLO.tex";
//       std::cout << "[YangPecjakNNLO::GetGraph] Writing tex file for theory: " << texFilename << "\n";
      std::ofstream outputFile(texFilename.c_str());
      if(outputFile.fail()){
         std::cerr << "[YangPecjakNNLO::GetGraph] ERROR opening file " << texFilename << "\n";
      }
      else{
         outputFile<<m_histo->Integral("width") <<"   "<<up<<"   "<<down<<"\n";
         outputFile.close();
      }
      
      
   }
   
   return m_graph;
}

TGraphAsymmErrors* YangPecjakNNLO::GetGraphNorm(){
   
   if(!m_graph_norm){
      if(GetGraph() == 0)
         return (TGraphAsymmErrors*)0;
      
      TH1* local_norm = (TH1*)m_histo->Clone("norm");
      local_norm->SetDirectory(false);
      local_norm->Scale(1./local_norm->Integral("width"));
      
      TH1* local_norm_up = (TH1*)m_h_up->Clone("norm_up");
      local_norm_up->SetDirectory(false);
      local_norm_up->Scale(1./local_norm_up->Integral("width"));
      
      TH1* local_norm_down = (TH1*)m_h_down->Clone("norm_down");
      local_norm_down->SetDirectory(false);
      local_norm_down->Scale(1./local_norm_down->Integral("width"));
      
      m_graph_norm = new TGraphAsymmErrors(local_norm);
      m_graph_norm->SetName("G_YANG_PECJAK_NLO_NNLL_SystemMass_NORM");
      
      for(unsigned int bin=1;bin<=local_norm->GetNbinsX();++bin){
         double binUp = 0,binDown=0;
         
         if(local_norm->GetBinContent(bin) > local_norm_up->GetBinContent(bin))
            binUp = local_norm->GetBinContent(bin) - local_norm_up->GetBinContent(bin);
         else
            binDown = local_norm_up->GetBinContent(bin) - local_norm->GetBinContent(bin);
         
         if(local_norm->GetBinContent(bin) > local_norm_down->GetBinContent(bin))
            binUp = local_norm->GetBinContent(bin) - local_norm_down->GetBinContent(bin);
         else
            binDown = local_norm_down->GetBinContent(bin) - local_norm->GetBinContent(bin);
         
         m_graph_norm->SetPointEYhigh(bin-1,binUp);
         m_graph_norm->SetPointEYlow (bin-1,binDown);
      }
      
      
   }
   
   
   return m_graph_norm;
}

TH1* YangPecjakNNLO::GetHisto(){
   if(!m_histo){
      if(GetGraph() == 0)
         return (TH1*)0;
   }
   
   return m_histo;
}

TH1* YangPecjakNNLO::GetHistoNorm(){
   
   if(!m_histo_norm){
      if(GetGraph() == 0)
         return (TH1*)0;
      
      std::string name = GetHisto()->GetName();
      name += "_NORM";
      m_histo_norm = (TH1*)GetHisto()->Clone(name.c_str());
      m_histo_norm->SetDirectory(false);
      m_histo_norm->Scale(1./GetHisto()->Integral("width"));
      
      
   }
   
   return m_histo_norm;
}

void YangPecjakNNLO::RetrieveHistosFromFile(){
   TFile *file = 0;
   for(unsigned int i=0;i<5 && !file;++i){
      file = TFile::Open(m_filename.c_str());
      if(!file){
         std::cerr << "[YangPecjakNNLO::RetrieveHistosFromFile] ERROR file not opened: " << m_filename << std::endl;
         std::cerr << "[YangPecjakNNLO::RetrieveHistosFromFile] Trying again.\n";
      }
   }
   if(!file){
      std::cerr << "[YangPecjakNNLO::RetrieveHistosFromFile] ERROR file not opened: " << m_filename << std::endl;
      return;
   }
   
   for(unsigned int i=0;i<m_numberOfHistos;++i){
      std::stringstream hname;
      hname << m_histoname << "_";
      if(i > 0)
         hname << i; 
      TH1* hist = (TH1*)file->Get(hname.str().c_str());
      if(!hist){
         std::cerr << "[YangPecjakNNLO::RetrieveHistosFromFile] failed to retrieve histogram " << hname.str() << " from file " << m_filename << std::endl;
         return;
      }
      hist->SetDirectory(false);
      m_h_theory.push_back(hist);
   }
   
   file->Close();
   delete file; file = 0;
}

