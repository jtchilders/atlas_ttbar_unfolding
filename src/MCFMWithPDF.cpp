#include "MCFMWithPDF.h"
#include "globals.h"
#include "Utils.h"

#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"

const std::string MCFMWithPDF::m_filename = "parton_distribution_functions.root";

MCFMWithPDF::MCFMWithPDF(const PDF pdf,const SampleDef::SampleNameEnum sampleName){
   m_pdf          = pdf;
   m_sampleName   = sampleName;
   m_graph        = 0;
   m_graph_norm   = 0;
   m_style_histo  = 0;
   RetrievePlots();
}

MCFMWithPDF::MCFMWithPDF(const PDF pdf,const std::string sampleName){
   m_pdf          = pdf;
   m_sampleName   = SampleDef::enumValue(sampleName);
   m_graph        = 0;
   m_graph_norm   = 0;
   m_style_histo  = new TH1D("mcfm_style_histo","",10,0,10);
   RetrievePlots();
}

MCFMWithPDF::~MCFMWithPDF(){
   if(m_graph){
      delete m_graph; m_graph = 0;
   }
   if(m_graph_norm){
      delete m_graph_norm; m_graph_norm = 0;
   }
   if(m_style_histo){
      delete m_style_histo; m_style_histo = 0;
   }
}


void MCFMWithPDF::RetrievePlots(){
   
   std::string filename = globals::dataFilesPath + "/" + m_filename;
   TFile* inFile = globals::OpenFile("[MCFMWithPDF::RetrievePlots]",filename);
   
   std::string histoName;
   switch(m_sampleName){
      case SampleDef::Tag1_Top1_lhood:
         histoName = "pt_top_";
         break;
      case SampleDef::Tag1_Top2_lhood:
         histoName = "pt_top_";
         break;
      case SampleDef::Tag1_SystemPt:
         histoName = "pt_tt_";
         break;
      case SampleDef::Tag1_SystemRapidity:
         histoName = "y_tt_";
         break;
      case SampleDef::Tag1_SystemMass:
         histoName = "m_tt_";
         break;
      default:
         std::cerr << "[MCFMWithPDF::RetrievePlots] ERROR, no histogram from sample, " << SampleDef::str(m_sampleName) << ".\n";
         return;
   }
   switch(m_pdf){
      case CT10NLO:
         histoName += "CT10NLO_";
         break;
      case MSTW2008NLO:
         histoName += "MSTW2008NLO_";
         break;
      case NNPDF23:
         histoName += "NNPDF23_";
         break;
      case HERAPDF15:
         histoName += "HERAPDF1.5_";
         break;
      default:
         histoName += "Data_";
         break;
   }
   std::string abs_histoName = histoName + "Absolute";
   m_graph = globals::GetFromFile<TGraphAsymmErrors>("[MCFMWithPDF::RetrievePlots]",inFile,abs_histoName);
   std::string norm_histoName = histoName + "Relative";
   m_graph_norm = globals::GetFromFile<TGraphAsymmErrors>("[MCFMWithPDF::RetrievePlots]",inFile,norm_histoName);
   Utils::ScaleGraph(m_graph_norm,1./1000.); // graph is in y units of 1/TeV, should use 1/GeV
   
   m_graph->SetLineColor(  m_lineColor  );
   m_graph->SetLineStyle(  m_lineStyle  );
   m_graph->SetLineWidth(  m_lineWidth  );
   m_graph->SetFillColor(  m_fillColor  );
   m_graph->SetFillStyle(  m_fillStyle  );
   m_graph->SetMarkerColor(m_markerColor);
   m_graph->SetMarkerStyle(m_markerStyle);
   
   m_graph_norm->SetLineColor(  m_lineColor  );
   m_graph_norm->SetLineStyle(  m_lineStyle  );
   m_graph_norm->SetLineWidth(  m_lineWidth  );
   m_graph_norm->SetFillColor(  m_fillColor  );
   m_graph_norm->SetFillStyle(  m_fillStyle  );
   m_graph_norm->SetMarkerColor(m_markerColor);
   m_graph_norm->SetMarkerStyle(m_markerStyle);
   
   
   m_style_histo->SetLineColor(  m_lineColor  );
   m_style_histo->SetLineStyle(  m_lineStyle  );
   m_style_histo->SetLineWidth(  m_lineWidth  );
   m_style_histo->SetFillColor(  m_fillColor  );
   m_style_histo->SetFillStyle(  m_fillStyle  );
   m_style_histo->SetMarkerColor(m_markerColor);
   m_style_histo->SetMarkerStyle(m_markerStyle);
   
   
   inFile->Close();
   delete inFile; inFile = 0;
   
}

