#ifndef POWHEGMC_H
#define POWHEGMC_H

#include <string>
#include <iostream>
#include <cmath>


class TH1;

class PowHegMC{
public:
   PowHegMC(const std::string sampleName);
   ~PowHegMC();
   
   TH1* GetHisto();
   TH1* GetHistoNorm();
   
   TH1* GetHistoInEventCounts() const {return m_histo_inEvtCnts;};
   
   void SetMainGenerator(const bool value){m_mainGenerator = value;};
   
private:
   PowHegMC(){};
   PowHegMC(PowHegMC& temp){};
   
   std::string m_samp;
   
   TH1* m_histo;
   TH1* m_histo_inEvtCnts;
   TH1* m_histo_norm;
   
   TH1* GetHistoFromFile() const;
   TH1* GetHistoFromFile(const std::string filename) const;
   bool HistogramsAreEqual(TH1 const * const histA, TH1 const * const histB) const;
   
   bool m_mainGenerator;
   
   static const std::string m_filename_base;
   
   static const unsigned short int m_lineColor   = 6; // 2 = red 4 = blue
   static const unsigned short int m_lineStyle   = 2;
   static const unsigned short int m_lineWidth   = 2;
   static const unsigned short int m_fillColor   = 6;
   static const unsigned short int m_fillStyle   = 3006;
   static const unsigned short int m_markerColor = 6;
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
