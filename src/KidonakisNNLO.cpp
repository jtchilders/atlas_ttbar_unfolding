#include <iostream>
#include <fstream>
#include <cmath>
#include "KidonakisNNLO.h"

#include "globals.h"

#include "TH1.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"


KidonakisNNLO::KidonakisNNLO(){
   m_histo = 0;
   m_histo_norm = 0;
   m_h_m = 0;
   m_h_mT = 0;
   m_h_2m = 0;
   m_h_halfm = 0;
   m_graph = 0;
   m_graph_norm = 0;
   
   m_fname_m = globals::dataFilesPath + "/pttopnnloapprox7lhc173m.root";
   m_fname_mT = globals::dataFilesPath + "/pttopnnloapprox7lhc173mT.root";
   m_fname_2m = globals::dataFilesPath + "/pttopnnloapprox7lhc1732m.root";
   m_fname_halfm = globals::dataFilesPath + "/pttopnnloapprox7lhc173halfm.root";
   m_histoname = "h_theory";

}

KidonakisNNLO::~KidonakisNNLO(){
   if(!m_histo){
      delete m_histo;m_histo = 0;
   }
   if(!m_histo_norm){
      delete m_histo_norm;m_histo_norm = 0;
   }
   if(!m_h_m){
      delete m_h_m;m_h_m = 0;
   }
   if(!m_h_mT){
      delete m_h_mT;m_h_mT = 0;
   }
   if(!m_h_2m){
      delete m_h_2m;m_h_2m = 0;
   }
   if(!m_h_halfm){
      delete m_h_halfm;m_h_halfm = 0;
   }
   
   if(!m_graph){
      delete m_graph;m_graph = 0;
   }
   if(!m_graph_norm){
      delete m_graph_norm;m_graph_norm = 0;
   }
}

TGraphAsymmErrors* KidonakisNNLO::GetGraph(){
   
   if(!m_graph){
      // get histograms
      RetrieveHistosFromFile();
      if(!m_h_m)
         return (TGraphAsymmErrors*)0;
      
      m_graph = new TGraphAsymmErrors(m_h_m);
      m_graph->SetName("G_KIDONAKIS_NLO_NNLL_TOPPT");
      m_h_m->SetName("H_KIDONAKIS_NLO_NNLL_TOPPT");
      
      // loop over histograms and get errors
      for(unsigned int bin=1;bin<=m_h_m->GetNbinsX();++bin){
         
         // use m as the data point
         m_graph->SetPoint(bin-1,m_h_m->GetBinCenter(bin),m_h_m->GetBinContent(bin));
         
         double up=0,down=0;
         GetUpDown(m_h_m,m_h_halfm,m_h_2m,m_h_mT,bin,up,down);
         
         m_graph->SetPointEYlow(bin-1,down);
         m_graph->SetPointEYhigh(bin-1,up);
         
      }
      
      m_graph->SetLineColor(  (unsigned int)m_lineColor);
      m_graph->SetLineStyle(  (unsigned int)m_lineStyle);
      m_graph->SetLineWidth(  (unsigned int)m_lineWidth);
      m_graph->SetFillColor(  (unsigned int)m_fillColor);
      m_graph->SetFillStyle(  (unsigned int)m_fillStyle);
      m_graph->SetMarkerColor((unsigned int)m_markerColor);
      m_graph->SetMarkerStyle((unsigned int)m_markerStyle);
      
      m_histo = m_h_m;
      
      m_histo->SetLineColor(  (unsigned int)m_lineColor);
      m_histo->SetLineStyle(  (unsigned int)m_lineStyle);
      m_histo->SetLineWidth(  (unsigned int)m_lineWidth);
      m_histo->SetFillColor(  (unsigned int)m_fillColor);
      m_histo->SetFillStyle(  (unsigned int)m_fillStyle);
      m_histo->SetMarkerColor((unsigned int)m_markerColor);
      m_histo->SetMarkerStyle((unsigned int)m_markerStyle);
      
      TH1* hup = (TH1*)m_histo->Clone("hup");
      hup->SetDirectory(false);
      hup->Reset();
      TH1* hdown = (TH1*)m_histo->Clone("hdown");
      hdown->SetDirectory(false);
      hdown->Reset();
      for(unsigned int bin=1;bin<=m_histo->GetNbinsX();++bin){
         hup->SetBinContent(bin,m_graph->GetX()[bin-1] + m_graph->GetEXhigh()[bin-1]);
         hdown->SetBinContent(bin,m_graph->GetX()[bin-1] - m_graph->GetEXlow()[bin-1]);
      }
      
      // need to write an output file for combineChannels
      double xs = m_histo->Integral("width");
      double up = hup->Integral("width") - xs;
      double down = xs - hdown->Integral("width");
      
      std::string texFilename = globals::GetCombinationTextFilePath() + "/InclusiveXS_KidonakisNNLO.tex";
      //std::cout << "[KidonakisNNLO::GetGraph] Writing tex file for theory: " << texFilename << "\n";
      std::ofstream outputFile(texFilename.c_str());
      if(outputFile.fail()){
         std::cerr << "[KidonakisNNLO::GetGraph] ERROR opening file " << texFilename << "\n";
      }
      else{
         outputFile<<m_histo->Integral("width") <<"   "<<up<<"   "<<down<<"\n";
         outputFile.close();
      }
      
   }
   
   return m_graph;
}

TGraphAsymmErrors* KidonakisNNLO::GetGraphNorm(){
   if(!m_graph_norm){
      if(GetGraph() == 0)
         return (TGraphAsymmErrors*)0;
      
      
      // m
      std::string name = m_h_m->GetName(); name+="_norm";
      TH1* norm_h_m = (TH1*)m_h_m->Clone(name.c_str());
      norm_h_m->SetDirectory(false);
      norm_h_m->Scale(1./norm_h_m->Integral("width"));
      // mT
      name = m_h_mT->GetName(); name+="_norm";
      TH1* norm_h_mT = (TH1*)m_h_mT->Clone(name.c_str());
      norm_h_mT->SetDirectory(false);
      norm_h_mT->Scale(1./norm_h_mT->Integral("width"));
      // 2m
      name = m_h_2m->GetName(); name+="_norm";
      TH1* norm_h_2m = (TH1*)m_h_2m->Clone(name.c_str());
      norm_h_2m->SetDirectory(false);
      norm_h_2m->Scale(1./norm_h_2m->Integral("width"));
      // halfm
      name = m_h_halfm->GetName(); name+="_norm";
      TH1* norm_h_halfm = (TH1*)m_h_halfm->Clone(name.c_str());
      norm_h_halfm->SetDirectory(false);
      norm_h_halfm->Scale(1./norm_h_halfm->Integral("width"));
      
      m_graph_norm = new TGraphAsymmErrors(norm_h_m);
      m_graph_norm->SetName("H_KIDONAKIS_NLO_NNLL_TOPPT_NORM");
      
      for(unsigned int bin=1;bin<=norm_h_m->GetNbinsX();++bin){
         
         double up=0,down=0;
         GetUpDown(norm_h_m,norm_h_halfm,norm_h_2m,norm_h_mT,bin,up,down);
         
         m_graph_norm->SetPointEYlow(bin-1,down);
         m_graph_norm->SetPointEYhigh(bin-1,up);
         
      }
      
      delete norm_h_m;     norm_h_m     = 0;
      delete norm_h_mT;    norm_h_mT    = 0;
      delete norm_h_2m;    norm_h_2m    = 0;
      delete norm_h_halfm; norm_h_halfm = 0;
   }
   
   return m_graph_norm;
}


TH1* KidonakisNNLO::GetHisto(){
   if(!m_histo){
      if(GetGraph() == 0)
         return (TH1*)0;
   }
   
   return m_histo;
}


TH1* KidonakisNNLO::GetHistoNorm(){
   
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

void KidonakisNNLO::RetrieveHistosFromFile(){
   
   // m
   if(!m_h_m)
   {
      TFile file(m_fname_m.c_str());
      if(!file.IsOpen()){
         std::cerr << "[KidonakisNNLO::RetrieveHistosFromFile] ERROR opening file: " << m_fname_m << "\n";
         return;
      }
      m_h_m = (TH1*)file.Get(m_histoname.c_str());
      if(!m_h_m){
         std::cerr << "[KidonakisNNLO::RetrieveHistosFromFile] ERROR retrieving histogram named: " << m_histoname << " from file: " << m_fname_m << "\n";
         return;
      }
      m_h_m->SetDirectory(false);
      m_h_m->SetName("kidonakisNNLO_m");
      
      file.Close();
   }
   
   
   // mT
   if(!m_h_mT)
   {
      TFile file(m_fname_mT.c_str());
      if(!file.IsOpen()){
         std::cerr << "[KidonakisNNLO::RetrieveHistosFromFile] ERROR opening file: " << m_fname_mT << "\n";
         return;
      }
      m_h_mT = (TH1*)file.Get(m_histoname.c_str());
      if(!m_h_mT){
         std::cerr << "[KidonakisNNLO::RetrieveHistosFromFile] ERROR retrieving histogram named: \"" << m_histoname << "\" from file: " << m_fname_mT << "\n";
         return;
      }
      m_h_mT->SetDirectory(false);
      m_h_mT->SetName("kidonakisNNLO_mT");
      
      file.Close();
   }
   
   // 2m
   if(!m_h_2m)
   {
      TFile file(m_fname_2m.c_str());
      if(!file.IsOpen()){
         std::cerr << "[KidonakisNNLO::RetrieveHistosFromFile] ERROR opening file: " << m_fname_2m << "\n";
         return;
      }
      m_h_2m = (TH1*)file.Get(m_histoname.c_str());
      if(!m_h_2m){
         std::cerr << "[KidonakisNNLO::RetrieveHistosFromFile] ERROR retrieving histogram named: " << m_histoname << " from file: " << m_fname_2m << "\n";
         return;
      }
      m_h_2m->SetDirectory(false);
      m_h_2m->SetName("kidonakisNNLO_2m");
      
      file.Close();
   }
   
   // halfm
   if(!m_h_halfm)
   {
      TFile file(m_fname_halfm.c_str());
      if(!file.IsOpen()){
         std::cerr << "[KidonakisNNLO::RetrieveHistosFromFile] ERROR opening file: " << m_fname_halfm << "\n";
         return;
      }
      m_h_halfm = (TH1*)file.Get(m_histoname.c_str());
      if(!m_h_halfm){
         std::cerr << "[KidonakisNNLO::RetrieveHistosFromFile] ERROR retrieving histogram named: " << m_histoname << " from file: " << m_fname_halfm << "\n";
         return;
      }
      m_h_halfm->SetDirectory(false);
      m_h_halfm->SetName("kidonakisNNLO_halfm");
      
      file.Close();
   }
}

void KidonakisNNLO::GetUpDown(TH1 const * const h_m,
                              TH1 const * const h_halfm,
                              TH1 const * const h_2m,
                              TH1 const * const h_mT,
                              const unsigned int& bin,
                              double& up,
                              double& down)
{
   // get difference between 2m/halfm/mT
   double d_2m    = h_2m   ->GetBinContent(bin) - h_m->GetBinContent(bin);
   double d_halfm = h_halfm->GetBinContent(bin) - h_m->GetBinContent(bin);
   double d_mT    = h_mT   ->GetBinContent(bin) - h_m->GetBinContent(bin);
    
   // set up/down uncertainty based on signs of 2m/halfm
   up = 0., down = 0.;
   if(d_2m > 0.) up   = d_2m;
   else         down = fabs(d_2m);
   // if both halfm/2m shift in the same direction, use the larger of the two
   if(d_halfm > 0.)
      if(d_halfm > up)
         up = d_halfm;
   else
      if(fabs(d_halfm) > down)
         down = fabs(d_halfm);
   
   // now if mT is larger than use it
   d_mT = fabs(d_mT);
   if(d_mT > up)   up   = d_mT;
   if(d_mT > down) down = d_mT;
}

