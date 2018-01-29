#ifndef MCFMWITHPDF_H
#define MCFMWITHPDF_H

#include "SampleDef.h"
#include "TColor.h"

class TGraphAsymmErrors;
class TH1D;

class MCFMWithPDF{
public:
   enum PDF{CT10NLO,MSTW2008NLO,NNPDF23,HERAPDF15,NUM_PDF};
   MCFMWithPDF(const PDF pdf,const SampleDef::SampleNameEnum sampleName);
   MCFMWithPDF(const PDF pdf,const std::string sampleName);
   ~MCFMWithPDF();
   
   TGraphAsymmErrors* GetGraph(){return m_graph;};
   TGraphAsymmErrors* GetGraphNorm(){return m_graph_norm;};
   
   TH1D* GetStyleHisto(){return m_style_histo;};
private:
   PDF                        m_pdf;
   SampleDef::SampleNameEnum  m_sampleName;
   TGraphAsymmErrors*         m_graph;
   TGraphAsymmErrors*         m_graph_norm;
   TH1D*                      m_style_histo;
   
   static const std::string   m_filename;
   
   static const unsigned int  m_lineColor   = kGreen + 2; // flat green
   static const unsigned int  m_lineStyle   = 4;
   static const unsigned int  m_lineWidth   = 2;
   static const unsigned int  m_fillColor   = 8;
   static const unsigned int  m_fillStyle   = 3353;
   static const unsigned int  m_markerColor = 8;
   static const unsigned int  m_markerStyle = 20;
   
   void RetrievePlots();
};


#endif
