#ifndef YANGPECJAKNNLO_H
#define YANGPECJAKNNLO_H

#include <iostream>
#include <string>
#include <vector>

class TH1;
class TGraphAsymmErrors;

//function to get the YangPecjak NNLO prediction for System Mtt
// http://arxiv.org/abs/1006.4682

class YangPecjakNNLO{
public:
   YangPecjakNNLO();
   ~YangPecjakNNLO();
   
   TH1* GetHisto();
   TH1* GetHistoNorm();
   
   TGraphAsymmErrors* GetGraph();
   TGraphAsymmErrors* GetGraphNorm();
   
private:
   TH1* m_histo;
   TH1* m_histo_norm;
   
   TGraphAsymmErrors* m_graph;
   TGraphAsymmErrors* m_graph_norm;
   
   TH1* m_h_up;
   TH1* m_h_down;
   
   std::vector<TH1*> m_h_theory;
   
   std::string m_filename;
   std::string m_histoname;
   
   static const unsigned short int m_numberOfHistos = 7;
   
   static const unsigned short int m_lineColor   = 4; // 2 = red 4 = blue
   static const unsigned short int m_lineStyle   = 3;
   static const unsigned short int m_lineWidth   = 2;
   static const unsigned short int m_fillColor   = 4;
   static const unsigned short int m_fillStyle   = 3335;
   static const unsigned short int m_markerColor = 4;
   static const unsigned short int m_markerStyle = 20;
   
   void RetrieveHistosFromFile();
};


#endif
