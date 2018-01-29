#ifndef MCFMHISTO_H
#define MCFMHISTO_H

#include <string>

#include <iostream>
#include <sstream>

#include "TColor.h"

class TGraphAsymmErrors;
class TH1;

class MCFMHisto{
public:
   MCFMHisto(std::string sampleName);
   ~MCFMHisto(void);
   
   TGraphAsymmErrors* GetGraph(void);
   TGraphAsymmErrors* GetGraphNorm(void);
   
   TH1* GetHisto(void);
   TH1* GetHistoNorm(void);
   
   TH1* GetStyleHisto(void);
   
   TGraphAsymmErrors* Divide(TGraphAsymmErrors* denominator);
   
private:
   
   static const unsigned short int m_lineColor   = kGreen + 2; // flat green
   static const unsigned short int m_lineStyle   = 4;
   static const unsigned short int m_lineWidth   = 2;
   static const unsigned short int m_fillColor   = 8;
   static const unsigned short int m_fillStyle   = 3353;
   static const unsigned short int m_markerColor = 8;
   static const unsigned short int m_markerStyle = 20;
   
   std::string m_filename;
   std::string m_hname_nominal;
   std::string m_hname_up;
   std::string m_hname_down;
   std::string m_sampleName;
   
   TGraphAsymmErrors* m_graph;
   TGraphAsymmErrors* m_graph_norm;
   TH1* m_histo;
   TH1* m_histo_norm;
   
   TH1* m_h_nominal;
   TH1* m_h_up;
   TH1* m_h_down;
   
   TH1* m_style;
};


#endif
