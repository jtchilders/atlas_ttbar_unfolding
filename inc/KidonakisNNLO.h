#ifndef KIDONAKISNNLO_H
#define KIDONAKISNNLO_H

#include <iostream>
#include <string>
#include <vector>

class TH1;
class TGraphAsymmErrors;

//function to get the MCNNLO prediction
//two kind of predictions are considered:
//Approx. NNLO, #mu=m, #mu=102.947, #sigma=68.026, I=177.6
//Approx. NNLO, #mu=m_{T}, #mu=102.747, #sigma=67.464, I=177.4

class KidonakisNNLO{
public:
   KidonakisNNLO();
   ~KidonakisNNLO();
   
   TH1* GetHisto();
   TGraphAsymmErrors* GetGraph();
   TH1* GetHistoNorm();
   TGraphAsymmErrors* GetGraphNorm();
   
private:
   TGraphAsymmErrors* m_graph;
   TGraphAsymmErrors* m_graph_norm;
   TH1* m_histo;
   TH1* m_histo_norm;
   
   TH1* m_h_m;
   TH1* m_h_mT;
   TH1* m_h_2m;
   TH1* m_h_halfm;
   
   std::string m_histoname;
   std::string m_fname_m;
   std::string m_fname_mT;
   std::string m_fname_2m;
   std::string m_fname_halfm;
   
   static const unsigned short int m_lineColor   = 4; // 2 = red 4 = blue
   static const unsigned short int m_lineStyle   = 3;
   static const unsigned short int m_lineWidth   = 2;
   static const unsigned short int m_fillColor   = 4;
   static const unsigned short int m_fillStyle   = 3335;
   static const unsigned short int m_markerColor = 4;
   static const unsigned short int m_markerStyle = 20;
   
   void RetrieveHistosFromFile();
   
   void GetUpDown(TH1 const * const h_m,
                  TH1 const * const h_halfm,
                  TH1 const * const h_2m,
                  TH1 const * const h_mT,
                  const unsigned int& bin,
                  double& up,
                  double& down);
};


#endif
