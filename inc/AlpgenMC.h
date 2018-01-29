#ifndef ALPGENMC_H
#define ALPGENMC_H

#include <string>
#include <iostream>

class TH1;

class AlpgenMC{
public:
   AlpgenMC(const std::string channel, const std::string sampleName);
   AlpgenMC(const std::string sampleName);
   ~AlpgenMC();
   
   TH1* GetHisto();
   TH1* GetHistoNorm();
   
   TH1* GetHistoInEventCounts(){return m_histo_inEvtCnts;};
   
   void SetMainGenerator(const bool value){m_mainGenerator = value;};
   
private:
   AlpgenMC(){};
   AlpgenMC(AlpgenMC& temp){};
   
   TH1* GetHisto(const std::string channel);
   
   std::string m_chan;
   std::string m_samp;
   
   TH1* m_histo;
   TH1* m_histo_inEvtCnts;
   TH1* m_histo_norm;
   
   bool m_mainGenerator;
   
   static const unsigned short int m_lineColor   = 2; // 2 = red 4 = blue
   static const unsigned short int m_lineStyle   = 2;
   static const unsigned short int m_lineWidth   = 2;
   static const unsigned short int m_fillColor   = 2;
   static const unsigned short int m_fillStyle   = 3005;
   static const unsigned short int m_markerColor = 2;
   static const unsigned short int m_markerStyle = 20;
   
   
   static const unsigned short int m_mg_lineColor   = 1; // 2 = red 4 = blue
   static const unsigned short int m_mg_lineStyle   = 1;
   static const unsigned short int m_mg_lineWidth   = 2;
   static const unsigned short int m_mg_fillColor   = 1;
   static const unsigned short int m_mg_fillStyle   = 3004;
   static const unsigned short int m_mg_markerColor = 1;
   static const unsigned short int m_mg_markerStyle = 20;
   
   
};

#endif
