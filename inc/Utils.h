#ifndef UTILS_H
#define UTILS_H

#include <iostream>

#include "TH1.h"
#include "TGraphAsymmErrors.h"
#include "TGraph.h"

class Utils{
public:
   static TH1* Graph2Histo(TGraph const * const graph);
   static TH1* GetShiftedUpHisto(TGraphAsymmErrors const * const graph);
   static TH1* GetShiftedDownHisto(TGraphAsymmErrors const * const graph);
   
   static void ScaleGraph(TGraphAsymmErrors const * const graph,const double scaler);
   
};


#endif

