#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"
#include "TColor.h"
#include "TH2F.h"
#include "TH1F.h"
#include <iostream>
#include "TString.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TStyle.h"
#include "TMatrixD.h"
#include "TRandom.h"
#include "TLatex.h"
//#include "TopPlot.C"
#include <stdio.h>


void SaveCanvas(TCanvas *can,TString name, bool createLogY=false){
  can->SaveAs(("png/"+name+".png").Data());
  can->SaveAs(("pdf/"+name+".pdf").Data());
  can->SaveAs(("eps/"+name+".eps").Data());
 if (createLogY){
    can->SetLogy();
    can->SaveAs(("png/"+name+"_log.png").Data());
    can->SaveAs(("pdf/"+name+"_log.pdf").Data());
    can->SaveAs(("eps/"+name+"_log.eps").Data());
    can->SetLogy(0);
  }
  
}