//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id: RooUnfoldErrors.cxx 276 2011-02-10 02:00:45Z T.J.Adye $
//
// Description:
//      Unfolding errors class
//
// Author: Richard Claridge <richard.claridge@stfc.ac.uk>
//
//==============================================================================

//____________________________________________________________
/* BEGIN_HTML

<p> A graph drawing class to view the errors associated with an unfolding technique</p>
<p>Before these can be run, the RooUnfoldErrors object must be created and the operation Plotting() run on the object in order to do the 
maths needed to plot these graphs. The object requires the number of toys over which the errors are calculated and a RooUnfold object.</p>
<p>For each iteration each bin in the measured distribution is added to a random number from a gaussian with a width based on the error in that bin. This is then unfolded and the results plotted for each bin. The rms in each bin is then used as the spread of the values in every bin. This gives errors that are slightly larger than those returned by RooUnfold, but are a better representation of the spread in the data.</p> 
<p> If the true distribution is not known, the following can be returned:</p>
<ul>
<li> A graph of the errors from the unfolding (Unf_err())
<li> A graph of the errors due to the spread of the reconstructed points (Spread())
<li> An error matrix based on the spread of the reconstructed points (True_err())
</ul>
<p> If the true distribution is known then a plot of the chi squared values can also be returned (Chi2()).
This requires the inclusion of the truth distribution and the error method on which the chi squared is based 
(0 for a simple calculation, 1 or 2 for a method based on the covariance matrix, depending on the method used for calculation of errors.). </p>
<p>On some occasions the chi squared value can be very large. This is due to the covariance matrices being near singular and thus 
difficult to invert reliably. A warning will be displayed if this is the case. To plot the chi squared distribution use the option Draw("chi2"), to filter out the larger values use Draw("chi2","abs(chi2 < max") where max is the largest value to be included.</p> 
END_HTML */
/////////////////////////////////////////////////////////////////

#include "RooUnfoldErrors.h"

#include <iostream>
#include <cmath>

#include "TString.h"
#include "TStyle.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TAxis.h"

#include "RooUnfoldResponse.h"
#include "RooUnfold.h"
#include "RooUnfoldResponse.h"

#include "globals.h"
#include "MeanValue.h"

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

ClassImp (RooUnfoldErrors);

RooUnfoldErrors::RooUnfoldErrors (int NToys,
                                  RooUnfold* unfold_in,
                                  const TH1* Truth,
                                  TH1 const *Data,
                                  TH1 const *Bkgd,
                                  TH1 * const efficiencyHisto,
                                  const std::string sampleName,
                                  const bool useGaus)
   : toys(NToys),unfold(unfold_in),
      hTrue(Truth),hData(Data),hBkgd(Bkgd),
      h_efficiency(efficiencyHisto),
      str_sampleName(sampleName),
      m_useGaus(useGaus)
{
   h_err=0;
   h_rms_res=0;
   h_mean_res=0;
   h_rms_res_diffxs=0;
   h_mean_res_diffxs=0;
   h_rms_res_diffxsNorm=0;
   h_mean_res_diffxsNorm=0;
   hchi2=0;
   h_mean_deviation = 0;
   h_mean_deviation_scaled = 0;
   h_rms_deviation = 0;
   h_rms_deviation_scaled = 0;
   GraphParameters(); 
   if (hTrue) CreatePlotsWithChi2();
   else       CreatePlots();
}



RooUnfoldErrors::~RooUnfoldErrors()
{
   if(h_err) delete h_err;
   if(h_rms_res) delete h_rms_res;
   if(h_mean_res) delete h_mean_res;
   if(h_rms_res_diffxs) delete h_rms_res_diffxs;
   if(h_mean_res_diffxs) delete h_mean_res_diffxs;
   if(h_rms_res_diffxsNorm) delete h_rms_res_diffxsNorm;
   if(h_mean_res_diffxsNorm) delete h_mean_res_diffxsNorm;
   if(hchi2) delete hchi2;
   if(h_mean_deviation) delete h_mean_deviation;
   if(h_rms_deviation) delete h_rms_deviation;
   if(h_mean_deviation_scaled) delete h_mean_deviation_scaled;
   if(h_rms_deviation_scaled) delete h_rms_deviation_scaled;
}

void 
RooUnfoldErrors::GraphParameters()
{
   //Gets graph size parameters//
   const TH1* HR=unfold->response()->Htruth();
   ntx=unfold->response()->GetNbinsMeasured();
   if (HR->GetDimension()==1) {
      xlo=HR->GetXaxis()->GetXmin();
      xhi=HR->GetXaxis()->GetXmax();
   } else {
      xlo=0.0;
      xhi=ntx;
   }
}


TNtuple*
RooUnfoldErrors::Chi2()
{   
   if (!hchi2) return hchi2;
   //Returns TNtuple of chi squared values. 
   hchi2->SetFillColor(4);
   return hchi2;
}

TH1*
RooUnfoldErrors::RMSResiduals(){
   if (!h_rms_res) return h_rms_res;
   //Returns a TH1D of the spread of the reconstructed points//
   h_rms_res->SetMarkerColor(kRed);
   h_rms_res->SetMarkerStyle(4);
   h_rms_res->SetMinimum(0);
   return dynamic_cast<TH1*>(h_rms_res->Clone());
}

TH1* RooUnfoldErrors::MeanResiduals(){
   if (!h_mean_res) return h_mean_res;
   //Returns a TH1D of the spread of the reconstructed points//
   h_mean_res->SetMarkerColor(kRed);
   h_mean_res->SetMarkerStyle(4);
   h_mean_res->SetMinimum(0);
   return dynamic_cast<TH1*>(h_mean_res->Clone());
}

TH1* RooUnfoldErrors::RMSResidualsDiffxsNorm(){
   if (!h_rms_res_diffxsNorm) return h_rms_res_diffxsNorm;
   //Returns a TH1D of the spread of the reconstructed points//
   h_rms_res_diffxsNorm->SetMarkerColor(kRed);
   h_rms_res_diffxsNorm->SetMarkerStyle(4);
   h_rms_res_diffxsNorm->SetMinimum(0);
   return dynamic_cast<TH1*>(h_rms_res_diffxsNorm->Clone());
}

TH1* RooUnfoldErrors::MeanResidualsDiffxsNorm(){
   if (!h_mean_res_diffxsNorm) return h_mean_res_diffxsNorm;
   //Returns a TH1D of the spread of the reconstructed points//
   h_mean_res_diffxsNorm->SetMarkerColor(kRed);
   h_mean_res_diffxsNorm->SetMarkerStyle(4);
   h_mean_res_diffxsNorm->SetMinimum(0);
   return dynamic_cast<TH1*>(h_mean_res_diffxsNorm->Clone());
}

TH1* RooUnfoldErrors::RMSResidualsDiffxs(){
   if (!h_rms_res_diffxs) return h_rms_res_diffxs;
   //Returns a TH1D of the spread of the reconstructed points//
   h_rms_res_diffxs->SetMarkerColor(kRed);
   h_rms_res_diffxs->SetMarkerStyle(4);
   h_rms_res_diffxs->SetMinimum(0);
   return dynamic_cast<TH1*>(h_rms_res_diffxs->Clone());
}

TH1* RooUnfoldErrors::MeanResidualsDiffxs(){
   if (!h_mean_res_diffxs) return h_mean_res_diffxs;
   //Returns a TH1D of the spread of the reconstructed points//
   h_mean_res_diffxs->SetMarkerColor(kRed);
   h_mean_res_diffxs->SetMarkerStyle(4);
   h_mean_res_diffxs->SetMinimum(0);
   return dynamic_cast<TH1*>(h_mean_res_diffxs->Clone());
}

TH1* 
RooUnfoldErrors::UnfoldingError(){
   if (!h_err) return h_err;
   //Returns a TH1D of the errors from the unfolding// 
   h_err->SetMarkerColor(kBlue);
   h_err->SetLineColor(kBlue);
   h_err->SetMarkerStyle(24);
   h_err->SetMinimum(0);
   return dynamic_cast<TH1*>(h_err->Clone());
}

TH1* RooUnfoldErrors::MeanDeviation(){
   if(!h_mean_deviation) return h_mean_deviation;
   
   return dynamic_cast<TH1*>(h_mean_deviation->Clone());
}

TH1* RooUnfoldErrors::RMSDeviation(){
   if(!h_rms_deviation) return h_rms_deviation;
   
   return dynamic_cast<TH1*>(h_rms_deviation->Clone());
}

TH1* RooUnfoldErrors::MeanDeviationScaled(){
   if(!h_mean_deviation_scaled) return h_mean_deviation_scaled;
   
   return dynamic_cast<TH1*>(h_mean_deviation_scaled->Clone());
}

TH1* RooUnfoldErrors::RMSDeviationScaled(){
   if(!h_rms_deviation_scaled) return h_rms_deviation_scaled;
   
   return dynamic_cast<TH1*>(h_rms_deviation_scaled->Clone());
}


void
RooUnfoldErrors::CreatePlots()
{
   /*Gets the values for plotting. Compares unfolding errors with errors calculated from toy MC.*/

   Bool_t oldstat= TH1::AddDirectoryStatus();
   TH1::AddDirectory (kFALSE);
   h_err     = new TH1D ("unferr", "Unfolding errors", ntx, xlo, xhi); 
   h_rms_res = new TH1D ("toyerr", "Toy MC RMS",       ntx, xlo, xhi); 
   TH1::AddDirectory (oldstat);

   unfold->SetNToys(toys);
   const TVectorD& errunf= unfold->ErecoV(RooUnfold::kErrors);
   const TVectorD& errtoy= unfold->ErecoV(RooUnfold::kCovToy);
   for (int i= 0; i<ntx; i++) {
      h_err    ->SetBinContent(i+1,errunf[i]);
      h_rms_res->SetBinContent(i+1,errtoy[i]);
   }
   return;
}
   

void
RooUnfoldErrors::CreatePlotsWithChi2()
{
   /*Gets the values for plotting. Uses the Runtoy method from RooUnfold to get plots to analyse for
   spread and error on the unfolding. Can also give values for a chi squared plot if a truth distribution is known*/

   const Double_t maxchi2=1e10;

   h_err     = new TProfile ("unferr", "Unfolding errors", ntx, xlo, xhi);
   h_rms_res = (TH1D*)hTrue->Clone("toyerr");
   h_rms_res->Reset();
   h_rms_res->SetDirectory(false);
   h_rms_res->SetTitle(";p^{t}_{T} [GeV];RMS of Toys for each bin");
   h_mean_res = (TH1D*)hTrue->Clone("toymean");
   h_mean_res->Reset();
   h_mean_res->SetDirectory(false);
   h_mean_res->SetTitle("Mean of Toys for each bin;p^{t}_{T} [GeV];Mean Unfolded");
   
   h_rms_res_diffxs = (TH1D*)hTrue->Clone("h_rms_res_diffxs");
   h_rms_res_diffxs->Reset();
   h_rms_res_diffxs->SetDirectory(false);
   h_rms_res_diffxs->SetTitle("RMS of d#sigma/dp_{T} for each bin;p^{t}_{T} [GeV];RMS of mean d#sigma/dp_{T}^{t} for all toys");
   h_rms_res_diffxsNorm = (TH1D*)hTrue->Clone("h_rms_res_diffxsNorm");
   h_rms_res_diffxsNorm->Reset();
   h_rms_res_diffxsNorm->SetDirectory(false);
   h_rms_res_diffxsNorm->SetTitle("RMS of normalized d#sigma/dp_{T} for each bin;p^{t}_{T} [GeV];RMS of mean normalized d#sigma/dp_{T}^{t} for all toys");
   
   h_mean_res_diffxs = (TH1D*)hTrue->Clone("h_mean_res_diffxs");
   h_mean_res_diffxs->Reset();
   h_mean_res_diffxs->SetDirectory(false);
   h_mean_res_diffxs->SetTitle("Mean of d#sigma/dp_{T} for each bin;p^{t}_{T} [GeV];mean d#sigma/dp_{T}^{t} for all toys");
   h_mean_res_diffxsNorm = (TH1D*)hTrue->Clone("h_mean_res_diffxsNorm");
   h_mean_res_diffxsNorm->Reset();
   h_mean_res_diffxsNorm->SetDirectory(false);
   h_mean_res_diffxsNorm->SetTitle("Mean of Normalized d#sigma/dp_{T} for each bin;p^{t}_{T} [GeV];Mean of normalized d#sigma/dp_{T}^{t} for all toys");
   
   
   h_mean_deviation = (TH1D*)hTrue->Clone("mean_deviation");
   h_mean_deviation->Reset();
   h_mean_deviation->SetDirectory(false);
   h_mean_deviation->SetTitle(";p^{t}_{T} [GeV];unfolded - truth / truth");
   
   h_mean_deviation_scaled = (TH1D*)hTrue->Clone("mean_deviation_scaled");
   h_mean_deviation_scaled->Reset();
   h_mean_deviation_scaled->SetDirectory(false);
   h_mean_deviation_scaled->SetTitle(";p^{t}_{T} [GeV];unfolded - scaledTruth / scaledTruth");
   
   h_rms_deviation = (TH1D*)hTrue->Clone("rms_deviation");
   h_rms_deviation->Reset();
   h_rms_deviation->SetDirectory(false);
   h_rms_deviation->SetTitle(";p^{t}_{T} [GeV];#sigma(unfolded - truth / truth)");
   
   h_rms_deviation_scaled = (TH1D*)hTrue->Clone("rms_deviation_scaled");
   h_rms_deviation_scaled->Reset();
   h_rms_deviation_scaled->SetDirectory(false);
   h_rms_deviation_scaled->SetTitle(";p^{t}_{T} [GeV];#sigma(unfolded - scaledTruth / scaledTruth)");
   
//     h_rms_res = new TH1D     ("toyerr", "Toy MC RMS",       ntx, xlo, xhi);
//     h_mean_res = new TH1D     ("toymean", "Toy MC RMS",       ntx, xlo, xhi);
   
   hchi2     = new TNtuple  ("chi2", "chi2", "chi2");
   // vectors to calculate the mean/rms for all toys of the "reco" 
   // (really the unfolded) distribution for each bin
   vector<MeanValue> recoMean(ntx);
   // vectors to calculate the mean/rms for all toys of the diffxs
   // distribution for each bin
   vector<MeanValue> diffxsMean(ntx);
   // vectors to calculate the mean/rms for all toys of the normalized diffxs
   // distribution for each bin
   vector<MeanValue> diffxsNormMean(ntx);
   
   vector<MeanValue> devMean(ntx),devScaledMean(ntx);
   
   int odd_ch=0;
   
   // loop over toys
   for (int k=0; k<toys;k++){
      std::cout << " toy number: " << k << "\n";
      // using existing unfolded result, create a Poisson Smeared Toy
      RooUnfold* toy= unfold->RunToy(hData,hBkgd,m_useGaus);
      // get the Chi2
      Double_t chi2=        toy->Chi2 (hTrue);
      // retreive the the vector of the "reco" (unfolded) bins
      //const TVectorD& reco= toy->Vreco();
      // retreive the vector of the "reco" errors
      const TVectorD& err=  toy->ErecoV();
      // retreive the histogram of the "reco"
      TH1* hreco = toy->Hreco();
      
      // calculate the scaler of measured/truth to scale the deviation later
      double scaler = 0.;
      if(hTrue) scaler = hreco->Integral("width")/hTrue->Integral("width");
      
      // need to get the dsigma/dpT
      TH1* diffxs = 0;
      if(h_efficiency)
         diffxs = globals::LumiEfficiencyCorrectedSpectra(hreco,str_sampleName,h_efficiency);
      
      // need to get the 1/sigma*dsigna/dpT
      TH1* diffxsNorm = 0;
      if(diffxs){
         diffxsNorm = (TH1*)diffxs->Clone();
         diffxsNorm->Scale(1.0/diffxs->Integral("width"));
      }
      
      for (int bin=1; bin<=hreco->GetNbinsX(); ++bin) {
         
         double unfolded = hreco->GetBinContent(bin);
         
         // want to calculate the deviation for each bin
         if(hTrue){
            
            double truth = hTrue->GetBinContent(bin);
            double truthScaled = hTrue->GetBinContent(bin) * scaler;
            double deviation = (truth == 0) ? 0 : (unfolded-truth)/truth;
            double deviationScaled = (truthScaled == 0) ? 0 : (unfolded-truthScaled)/truthScaled;
            
            devMean[bin-1].AddEntry(deviation);
            devScaledMean[bin-1].AddEntry(deviationScaled);
         }
         
         h_err->Fill(h_err->GetBinCenter(bin),err[bin-1]);
         
         recoMean[bin-1].AddEntry(unfolded);
         if(diffxs)
            diffxsMean[bin-1].AddEntry(diffxs->GetBinContent(bin));
         if(diffxsNorm)
            diffxsNormMean[bin-1].AddEntry(diffxsNorm->GetBinContent(bin));
      }
      
      
      if (hTrue){
         hchi2->Fill(chi2);
         if (fabs(chi2)>=maxchi2 && toy->verbose()>=1){
            cerr<<"Large |chi^2| value: "<< chi2 << endl;
            odd_ch++;
         }
      }
      
      if(diffxsNorm){ delete diffxsNorm;diffxsNorm = 0;}
      if(diffxs){ delete diffxs;diffxs = 0;}
      delete hreco; hreco=0;
      delete toy;toy=0;
   }
   
   // fill mean/rms residual plots
   for (int bin=1; bin<=ntx; ++bin){
      
      
      if(bin <= recoMean.size()){
         h_rms_res->SetBinContent (bin, recoMean[bin-1].GetSigma());
         h_rms_res->SetBinError   (bin, recoMean[bin-1].GetSigma()/sqrt(2*recoMean[bin-1].GetN()));
         h_mean_res->SetBinContent(bin, recoMean[bin-1].GetMean());
         h_mean_res->SetBinError  (bin, recoMean[bin-1].GetMean()/sqrt(2*recoMean[bin-1].GetN()));
      }
      
      if(bin <= diffxsMean.size()){
         h_mean_res_diffxs->SetBinContent  (bin,diffxsMean[bin-1].GetMean());
         h_mean_res_diffxs->SetBinError    (bin,diffxsMean[bin-1].GetMean()/sqrt(2*diffxsMean[bin-1].GetN()));
         h_rms_res_diffxs->SetBinContent   (bin,diffxsMean[bin-1].GetSigma());
         h_rms_res_diffxs->SetBinError     (bin,diffxsMean[bin-1].GetSigma()/sqrt(2*diffxsMean[bin-1].GetN()));
      }
      
      if(bin <= diffxsNormMean.size()){
         h_mean_res_diffxsNorm->SetBinContent  (bin,diffxsNormMean[bin-1].GetMean());
         h_mean_res_diffxsNorm->SetBinError    (bin,diffxsNormMean[bin-1].GetMean()/sqrt(2*diffxsNormMean[bin-1].GetN()));
         h_rms_res_diffxsNorm->SetBinContent   (bin,diffxsNormMean[bin-1].GetSigma());
         h_rms_res_diffxsNorm->SetBinError     (bin,diffxsNormMean[bin-1].GetSigma()/sqrt(2*diffxsNormMean[bin-1].GetN()));
      }
      
      if(bin <= devMean.size()){
         h_mean_deviation->SetBinContent  (bin,devMean[bin-1].GetMean());
         h_mean_deviation->SetBinError    (bin,devMean[bin-1].GetMean()/sqrt(2*devMean[bin-1].GetN()));
         h_rms_deviation->SetBinContent   (bin,devMean[bin-1].GetSigma());
         h_rms_deviation->SetBinError     (bin,devMean[bin-1].GetSigma()/sqrt(2*devMean[bin-1].GetN()));
      }
      
      if(bin <= devScaledMean.size()){
         h_mean_deviation_scaled->SetBinContent (bin,devScaledMean[bin-1].GetMean());
         h_mean_deviation_scaled->SetBinError   (bin,devScaledMean[bin-1].GetMean()/sqrt(2*devScaledMean[bin-1].GetN()));
         h_rms_deviation_scaled->SetBinContent  (bin,devScaledMean[bin-1].GetSigma());
         h_rms_deviation_scaled->SetBinError    (bin,devScaledMean[bin-1].GetSigma()/sqrt(2*devScaledMean[bin-1].GetN()));
      }
   }
   
   if (odd_ch){
      cout <<"There are " << odd_ch << " bins over outside the range of 0 to "<<maxchi2 <<endl;
   }

}

