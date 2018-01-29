#ifndef MCATNLOBEFORECUTS_H
#define MCATNLOBEFORECUTS_H

#include <string>
#include <iostream>
#include <cmath>

#include "TopDecayChannel.h"

class TH1;

class McAtNloBeforeCuts{
public:
   McAtNloBeforeCuts(const TopDecayChannel channel, const std::string sampleName);
   // if no channel is specified, both are read in, and 
   // the m_histo(_norm) hold the average of each
   McAtNloBeforeCuts(const std::string sampleName);
   ~McAtNloBeforeCuts();
   
   TH1* GetHisto();
   TH1* GetHistoNorm();
   
   TH1* GetHistoInEventCounts() const {return m_histo_inEvtCnts;};
   
   void SetMainGenerator(const bool value){m_mainGenerator = value;};
   
private:
   McAtNloBeforeCuts(){};
   McAtNloBeforeCuts(McAtNloBeforeCuts& temp){};
   
   TopDecayChannel m_chan;
   std::string m_samp;
   
   TH1* m_histo;
   TH1* m_histo_inEvtCnts;
   TH1* m_histo_norm;
   
   TH1* GetHisto(const TopDecayChannel channel) const;
   TH1* GetAverageHisto() const;
   
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
