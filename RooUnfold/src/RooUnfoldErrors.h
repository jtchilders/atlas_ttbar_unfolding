//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id: RooUnfoldErrors.h 273 2011-01-31 00:00:27Z T.J.Adye $
//
// Description:
//      Graph Drawing Class for use with RooUnfold.
//
// Authors: Tim Adye <T.J.Adye@rl.ac.uk> and Richard Claridge <richard.claridge@stfc.ac.uk>
//
//==============================================================================

#ifndef ROOUNFOLDERRORS_H_
#define ROOUNFOLDERRORS_H_

#include "TNamed.h"
#include <string>

class TH1;
class RooUnfold;
class TNtuple;

class RooUnfoldErrors : public TNamed {

public:
  
  int toys; // Number of toys 
  RooUnfold* unfold; // Input unfolding object
  const TH1* hTrue;
  const TH1* hData;
  const TH1* hBkgd;
  RooUnfoldErrors (int NToys,
                   RooUnfold* unfold,
                   const TH1* Truth=0,
                   TH1 const *Data =0,
                   TH1 const *Bkgd =0,
                   TH1 * const efficiencyHisto = 0,
                   const std::string sampleName = "",
                   const bool useGaus = false);
  virtual ~RooUnfoldErrors();
  TNtuple* Chi2(); 

  TH1* RMSResiduals();
  TH1* MeanResiduals();
  TH1* RMSResidualsDiffxs();
  TH1* MeanResidualsDiffxs();
  TH1* RMSResidualsDiffxsNorm();
  TH1* MeanResidualsDiffxsNorm();
  TH1* UnfoldingError();
  TH1* MeanDeviation();
  TH1* MeanDeviationScaled();
  TH1* RMSDeviation();
  TH1* RMSDeviationScaled();
  

private:
  void CreatePlots();
  void CreatePlotsWithChi2();
  TH1* h_err; // Output plot
  TH1* h_rms_res; // Output plot
  TH1* h_mean_res;
  TH1* h_rms_res_diffxs; 
  TH1* h_mean_res_diffxs;
  TH1* h_rms_res_diffxsNorm; 
  TH1* h_mean_res_diffxsNorm;
  TH1* h_mean_deviation;
  TH1* h_mean_deviation_scaled;
  TH1* h_rms_deviation;
  TH1* h_rms_deviation_scaled;
  TNtuple* hchi2;  // Output plot 
  void GraphParameters(); //
  double xlo; // Minimum x-axis value 
  double xhi; // Maximum x-axis value
  int ntx; // Number of bins in true distribution
  
  TH1* h_efficiency;
  std::string str_sampleName;
  bool m_useGaus;
  
public:

  ClassDef (RooUnfoldErrors, 0)  // Show unfolding errors
};

#endif
