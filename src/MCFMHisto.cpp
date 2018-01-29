#include <iostream>
#include <fstream>

#include "MCFMHisto.h"


#include "TFile.h"
#include "TH1.h"
#include "TH1C.h"
#include "TGraphAsymmErrors.h"
#include "globals.h"

#include "KinematicVariable.h"


MCFMHisto::MCFMHisto(std::string sampleName){
      //m_filename = globals::dataFilesPath + "/Theory_MCFM.root";
      m_filename = globals::dataFilesPath;
   if(sampleName.find("Top1") != std::string::npos || sampleName.find("Top2") != std::string::npos){
      
      m_filename += "/Theory_MCFM_pTt.root";
      
      m_hname_nominal = "pttop_MCFM_scale172";
      m_hname_up      = "pttop_MCFM_scale345";
      m_hname_down    = "pttop_MCFM_scale690";
   }
   else if(sampleName.find("Mass") != std::string::npos){
      
      m_filename += "/Theory_MCFM_Mtt.root";
      
      m_hname_nominal = "Mtt_MCFM_scale172";
      m_hname_up      = "Mtt_MCFM_scale345";
      m_hname_down    = "Mtt_MCFM_scale690";
   }
   else if(sampleName.find("Pt") != std::string::npos){
      
      m_filename += "/Theory_MCFM_pTtt.root";
      
      m_hname_nominal = "pttt_MCFM_scale172";
      m_hname_up      = "pttt_MCFM_scale345";
      m_hname_down    = "pttt_MCFM_scale690";
   }
   else if(sampleName.find("Rapidity") != std::string::npos){
      
      m_filename += "/Theory_MCFM_ytt.root";
      
      m_hname_nominal = "ytt_MCFM_scale172";
      m_hname_up      = "ytt_MCFM_scale345";
      m_hname_down    = "ytt_MCFM_scale690";
   }
    
   m_sampleName    = sampleName;
   m_graph         = 0;
   m_graph_norm    = 0;
   m_histo         = 0;
   m_histo_norm    = 0;
   m_style         = 0;
   m_h_nominal     = 0;
   m_h_up          = 0;
   m_h_down        = 0;
}

MCFMHisto::~MCFMHisto(void){
   if(!m_graph){
      delete m_graph;m_graph = 0;
   }
   if(!m_histo){
      delete m_histo;m_histo = 0;
   }
   if(!m_graph_norm){
      delete m_graph_norm;m_graph_norm = 0;
   }
   if(!m_histo_norm){
      delete m_histo_norm;m_histo_norm = 0;
   }
   if(!m_style){
      delete m_style;m_style = 0;
   }
   if(!m_h_nominal){
      delete m_h_nominal;m_h_nominal = 0;
   }
   if(!m_h_up){
      delete m_h_up;m_h_up = 0;
   }
   if(!m_h_down){
      delete m_h_down;m_h_down = 0;
   }
   
}

TGraphAsymmErrors* MCFMHisto::GetGraph(void){
   
   if(!m_graph){
      
      KinematicVariable* kinVar = new KinematicVariable(m_sampleName);
      std::string UkinVarLabel = kinVar->GetUKinVarLabel();
      std::string DkinVarLabel = kinVar->GetDKinVarLabel();
      
      delete kinVar; kinVar = 0;
      
      TFile *file = 0;
      for(unsigned int i=0;i<5 && !file;++i){
         file = TFile::Open(m_filename.c_str());
         if(!file){
            std::cerr << "[MCFMHisto::GetGraph] ERROR file not opened: " << m_filename << std::endl;
            std::cerr << "[MCFMHisto::GetGraph] Trying again.\n";
         }
      }
      if(!file){
         std::cerr << "[MCFMHisto::GetGraph] ERROR file not opened: " << m_filename << std::endl;
         return (TGraphAsymmErrors*)0;
      }
      
      
      m_h_nominal = (TH1*)file->Get(m_hname_nominal.c_str());
      if(!m_h_nominal){
         std::cerr << "[MCFMHisto::GetGraph] ERROR opening histogram: " << m_hname_nominal << " in file " << m_filename << "\n";
         return (TGraphAsymmErrors*)0;
      }
      m_h_nominal->SetDirectory(false);
      
      m_h_nominal->SetLineColor(  (unsigned int)m_lineColor);
      m_h_nominal->SetLineStyle(  (unsigned int)m_lineStyle);
      m_h_nominal->SetLineWidth(  (unsigned int)m_lineWidth);
      m_h_nominal->SetFillColor(  (unsigned int)m_fillColor);
      m_h_nominal->SetFillStyle(  (unsigned int)m_fillStyle);
      m_h_nominal->SetMarkerColor((unsigned int)m_markerColor);
      m_h_nominal->SetMarkerStyle((unsigned int)m_markerStyle);
      
      
      m_h_up      = (TH1*)file->Get(m_hname_up.c_str());
      if(!m_h_up){
         std::cerr << "[MCFMHisto::GetGraph] ERROR opening histogram: " << m_hname_up << " in file " << m_filename << "\n";
         return (TGraphAsymmErrors*)0;
      }
      m_h_up->SetDirectory(false);
      
      m_h_down    = (TH1*)file->Get(m_hname_down.c_str());
      if(!m_h_down){
         std::cerr << "[MCFMHisto::GetGraph] ERROR opening histogram: " << m_hname_down << " in file " << m_filename << "\n";
         return (TGraphAsymmErrors*)0;
      }
      m_h_down->SetDirectory(false);
      
      file->Close();
      delete file; file = 0;
     
      std::stringstream name;
      name <<"G_MCFM_NLO_"<<m_sampleName;
      std::stringstream ntitle;
      ntitle <<";"<<UkinVarLabel<<";"<<DkinVarLabel;
      
      m_graph = new TGraphAsymmErrors(m_h_nominal);
      m_graph->SetName(name.str().c_str());
      m_graph->SetTitle(ntitle.str().c_str());
      m_graph->SetLineColor(  (unsigned int)m_lineColor);
      m_graph->SetLineStyle(  (unsigned int)m_lineStyle);
      m_graph->SetLineWidth(  (unsigned int)m_lineWidth);
      m_graph->SetFillColor(  (unsigned int)m_fillColor);
      m_graph->SetFillStyle(  (unsigned int)m_fillStyle);
      m_graph->SetMarkerColor((unsigned int)m_markerColor);
      m_graph->SetMarkerStyle((unsigned int)m_markerStyle);
      
      
      for(unsigned int i=0;i<m_h_nominal->GetNbinsX();++i){
         
         double nominal = m_h_nominal->GetBinContent(i+1);
         double up      = m_h_up->GetBinContent(i+1);
         double down    = m_h_down->GetBinContent(i+1);
         
         m_graph->SetPointEYhigh(i,up-nominal);
         m_graph->SetPointEYlow (i,nominal-down);
         
         
      }// end for(bin)
      
      std::string hname = name.str();
      hname[0] = 'H';
      m_h_nominal->SetName(hname.c_str());
      m_histo = m_h_nominal;
      
       // need to write an output file for combineChannels
      double up = m_h_up->Integral("width") - m_histo->Integral("width");
      double down = m_histo->Integral("width") - m_h_down->Integral("width");
      
      std::string texFilename = globals::GetCombinationTextFilePath() + "/InclusiveXS_MCFM" + m_sampleName + ".tex";
//       std::cout << "[MCFMHisto::GetGraph] Writing tex file for theory: " << texFilename << "\n";
      std::ofstream outputFile(texFilename.c_str());
      if(outputFile.fail()){
         std::cerr << "[MCFMHisto::GetGraph] ERROR opening file " << texFilename << "\n";
      }
      else{
         outputFile<<m_histo->Integral("width") <<"   "<<up<<"   "<<down<<"\n";
         outputFile.close();
      }
      
      
   }
   
   //std::cerr << "[MCFMHisto::GetGraph] done.\n";
   
   return m_graph;
}

TGraphAsymmErrors* MCFMHisto::GetGraphNorm(void){
   
   if(!m_graph_norm){
      if(GetGraph() == 0){
         return (TGraphAsymmErrors*)0;
      }
      std::string name = GetGraph()->GetName();
      name += "Norm";
      m_graph_norm = (TGraphAsymmErrors*)GetGraph()->Clone(name.c_str());
      
      
      TH1* local_nominal = (TH1*)m_h_nominal->Clone("nominal");
      local_nominal->SetDirectory(false);
      local_nominal->Scale(1./local_nominal->Integral("width"));
      TH1* local_up = (TH1*)m_h_up->Clone("up");
      local_up->SetDirectory(false);
      local_up->Scale(1./local_up->Integral("width"));
      TH1* local_down = (TH1*)m_h_down->Clone("down");
      local_down->SetDirectory(false);
      local_down->Scale(1./local_down->Integral("width"));
      
      for(unsigned int bin=1;bin<=local_nominal->GetNbinsX();++bin){
         
         double binUp = 0;
         double binDown = 0;
         
         m_graph_norm->GetY()[bin-1] = local_nominal->GetBinContent(bin);
         
         if(local_nominal->GetBinContent(bin) > local_up->GetBinContent(bin))
            binDown = local_nominal->GetBinContent(bin) - local_up->GetBinContent(bin);
         else
            binUp   = local_up->GetBinContent(bin) - local_nominal->GetBinContent(bin);
         
         if(local_nominal->GetBinContent(bin) > local_down->GetBinContent(bin))
            binDown = local_nominal->GetBinContent(bin) - local_down->GetBinContent(bin);
         else
            binUp   = local_down->GetBinContent(bin) - local_nominal->GetBinContent(bin);
         
         m_graph_norm->GetEYhigh()[bin-1] = binUp;
         m_graph_norm->GetEYlow()[bin-1]  = binDown;
         
      }
      
      delete local_nominal; local_nominal = 0;
      delete local_up; local_up = 0;
      delete local_down; local_down = 0;
   }
   
   return m_graph_norm;
}


TH1* MCFMHisto::GetHisto(void){
   
   if(!m_histo) GetGraph();
   
   return m_histo;
   
}

TH1* MCFMHisto::GetHistoNorm(void){
   if(!m_histo_norm){
      if(GetHisto() == 0){
         return (TH1*)0;
      }
      std::string name = GetHisto()->GetName();
      name += "_NORM";
      m_histo_norm = (TH1*)GetHisto()->Clone(name.c_str());
      m_histo_norm->SetDirectory(false);
      m_histo_norm->Scale(1./m_histo_norm->Integral("width"));
      
      
   }
   
   return m_histo_norm;
}

TGraphAsymmErrors* MCFMHisto::Divide(TGraphAsymmErrors* denominator){
   
   if(!m_graph) GetGraph();
   
   std::cout << "[MCFMHisto::Divide]  Use DivideAsymmErrors " << std::endl;
   
   return globals::DivideAsymmErrors(m_graph,denominator);
   
}

TH1* MCFMHisto::GetStyleHisto(void){
   TH1* m_style = new TH1C("mcfmStyle","",10,0,10);
   
   m_style->SetLineColor(  (unsigned int)m_lineColor);
   m_style->SetLineStyle(  (unsigned int)m_lineStyle);
   m_style->SetLineWidth(  (unsigned int)m_lineWidth);
   m_style->SetFillColor(  (unsigned int)m_fillColor);
   m_style->SetFillStyle(  (unsigned int)m_fillStyle);
   m_style->SetMarkerColor((unsigned int)m_markerColor);
   m_style->SetMarkerStyle((unsigned int)m_markerStyle);
      
   
   return m_style;
}

