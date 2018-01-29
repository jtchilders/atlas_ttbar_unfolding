#include "ttbarUnfold.h"

#include "TROOT.h"
#include "TRandom.h"
#include "TLegend.h"
#include "TString.h"
#include "TStyle.h"
#include "TText.h"
#include "TPave.h"
#include "TPaveText.h"
#include "TRandom.h"
#include "TPostScript.h"
#include "TFile.h"
#include "TVectorD.h"
#include "TLine.h"
#include "TNtuple.h"
#include "TProfile.h"
#include <cmath>

#include "RooUnfoldResponse.h"
#include "SampleDef.h"
#include "globals.h"
#include "KinematicVariable.h"
#include "GeneratorLabel.h"
#include "TopDecayChannel.h"
#include "AlpgenMC.h"
#include "McAtNloBeforeCuts.h"

#include <iostream>
#include <sstream>

ttbarUnfold::ttbarUnfold(const std::string dataFilename,
               const std::string mcFilename, std::string sampleName,
               TH1 * const efficiencyHisto){
   
   // root that we want to manage memory
   TH1::AddDirectory(false);
   
   m_dataFilename             = dataFilename;
   m_mcFilename               = mcFilename;
   m_sampleName               = sampleName;
   
   m_efficiencyHisto          = efficiencyHisto;
   
   // zero my pointers
   m_unfolded                 = 0;
   m_response                 = 0;
   m_errors                   = 0;
   
   m_mc_truth                 = 0;
   m_mc_truthNorm             = 0;
   m_mc_truthScaled           = 0;
   m_mc_truth_diffxs          = 0;
   m_mc_truth_diffxsNorm      = 0;
   m_mc_reco                  = 0;
   m_mc_recoNorm              = 0;
   m_mc_recoScaled            = 0;
   m_mc_reco_diffxs           = 0;
   m_mc_reco_diffxsNorm       = 0;
   
   m_mc_migration             = 0;
   m_mc_migrationNorm         = 0;
   m_mc_migrationScaled       = 0;
   
   m_data_measuredMinusBkgd   = 0;
   m_data_measuredMinusBkgdNorm = 0;
   m_data_measured            = 0;
   m_data_measuredNorm        = 0;
   m_data_bkgd                = 0;
   m_data_bkgdNorm            = 0;
   
   m_data_unfolded            = 0;
   m_data_unfoldedNorm        = 0;
   m_data_unfolded_diffxs     = 0;
   m_data_unfolded_diffxsNorm = 0;
   
   m_deviation                = 0;
   m_deviationReco            = 0;
   m_deviation_diffxs         = 0;
   
   m_rmsResiduals             = 0;
   m_meanResiduals            = 0;
   m_rmsResidualsDiffxs       = 0;
   m_meanResidualsDiffxs      = 0;
   m_rmsResidualsDiffxsNorm   = 0;
   m_meanResidualsDiffxsNorm  = 0;
   m_unfoldingError           = 0;
   m_meanDeviation            = 0;
   m_rmsDeviation             = 0;
   m_meanDeviationScaled      = 0;
   m_rmsDeviationScaled       = 0;

   m_regularizationScan       = 0;
   
   // Set some default values
   m_unfoldErrorTreatment     = RooUnfold::kNoError;
   
   m_method                   = SVD;
   
   m_mc_truth_hname           = "";
   m_mc_reco_hname            = "";
   m_mc_migration_hname       = "";
   
   m_histo_baseName           = "";
   
   m_reg                      = 4;
   m_iters                    = 4;
   m_ntoys_unfold             = 5000;
   m_ntoys_errors             = 10;
}

ttbarUnfold::~ttbarUnfold(void){
   if(m_response){
      delete m_response;
      m_response = 0;
   }
   
   if(m_unfolded){
      switch(m_method){
      case SVD:
         delete (RooUnfoldSvd*)m_unfolded;
         m_unfolded = 0;
         break;
      case Bayes:
         delete (RooUnfoldBayes*)m_unfolded;
         m_unfolded = 0;
         break;
      case BinByBin:
         delete (RooUnfoldBinByBin*)m_unfolded;
         m_unfolded = 0;
         break;
      default:
         delete m_unfolded;
         m_unfolded = 0;
         break;
      }
   }
   
   if(m_mc_truth){
      delete m_mc_truth;
      m_mc_truth = 0;
   }
   
   if(m_mc_truth_diffxs){
      delete m_mc_truth_diffxs;
      m_mc_truth_diffxs = 0;
   }
   
   if(m_mc_truth_diffxsNorm){
      delete m_mc_truth_diffxsNorm;
      m_mc_truth_diffxsNorm = 0;
   }
   
   if(m_mc_truthNorm){
      delete m_mc_truthNorm;
      m_mc_truthNorm = 0;
   }
   
   if(m_mc_truthScaled){
      delete m_mc_truthScaled;
      m_mc_truthScaled = 0;
   }
   
   if(m_mc_reco){
      delete m_mc_reco;
      m_mc_reco = 0;
   }
   
   if(m_mc_recoNorm){
      delete m_mc_recoNorm;
      m_mc_recoNorm = 0;
   }
   
   if(m_mc_recoScaled){
      delete m_mc_recoScaled;
      m_mc_recoScaled = 0;
   }
   
   if(m_mc_reco_diffxs){
      delete m_mc_reco_diffxs;
      m_mc_reco_diffxs = 0;
   }
   
   if(m_mc_reco_diffxsNorm){
      delete m_mc_reco_diffxsNorm;
      m_mc_reco_diffxsNorm = 0;
   }
   
   if(m_mc_migration){
      delete m_mc_migration;
      m_mc_migration = 0;
   }
   
   if(m_mc_migrationNorm){
      delete m_mc_migrationNorm;
      m_mc_migrationNorm = 0;
   }
   
   if(m_mc_migrationScaled){
      delete m_mc_migrationScaled;
      m_mc_migrationScaled = 0;
   }
   
   if(m_data_measuredMinusBkgd){
      delete m_data_measuredMinusBkgd;
      m_data_measuredMinusBkgd = 0;
   }
   
   if(m_data_measuredMinusBkgdNorm){
      delete m_data_measuredMinusBkgdNorm;
      m_data_measuredMinusBkgdNorm = 0;
   }
   
   if(m_data_measured){
      delete m_data_measured;
      m_data_measured = 0;
   }
   
   if(m_data_measuredNorm){
      delete m_data_measuredNorm;
      m_data_measuredNorm = 0;
   }
   
   if(m_data_bkgd){
      delete m_data_bkgd;
      m_data_bkgd = 0;
   }
   
   if(m_data_bkgdNorm){
      delete m_data_bkgdNorm;
      m_data_bkgdNorm = 0;
   }
   
   if(m_deviation){
      delete m_deviation;
      m_deviation = 0;
   }
   
   if(m_deviation_diffxs){
      delete m_deviation_diffxs;
      m_deviation_diffxs = 0;
   }
   
   if(m_deviationReco){
      delete m_deviationReco;
      m_deviationReco = 0;
   }
   
   if(m_data_unfolded){
      delete m_data_unfolded;
      m_data_unfolded = 0;
   }
   
   if(m_data_unfoldedNorm){
      delete m_data_unfoldedNorm;
      m_data_unfoldedNorm = 0;
   }
   
   if(m_data_unfolded_diffxs){
      delete m_data_unfolded_diffxs;
      m_data_unfolded_diffxs = 0;
   }
   
   if(m_data_unfolded_diffxsNorm){
      delete m_data_unfolded_diffxsNorm;
      m_data_unfolded_diffxsNorm = 0;
   }
   
   if(m_errors){
      delete m_errors;
      m_errors = 0;
   }
   
   if(m_regularizationScan){
      delete m_regularizationScan;
      m_regularizationScan = 0;
   }
   
   if(m_rmsResiduals){
      delete m_rmsResiduals;
      m_rmsResiduals = 0;
   }
   
   if(m_meanResiduals){
      delete m_meanResiduals;
      m_meanResiduals = 0;
   }
   
   if(m_rmsResidualsDiffxsNorm){
      delete m_rmsResidualsDiffxsNorm;
      m_rmsResidualsDiffxsNorm = 0;
   }
   
   if(m_meanResidualsDiffxsNorm){
      delete m_meanResidualsDiffxsNorm;
      m_meanResidualsDiffxsNorm = 0;
   }
   
   if(m_rmsResidualsDiffxs){
      delete m_rmsResidualsDiffxs;
      m_rmsResidualsDiffxs = 0;
   }
   
   if(m_meanResidualsDiffxs){
      delete m_meanResidualsDiffxs;
      m_meanResidualsDiffxs = 0;
   }
   
   if(m_unfoldingError){
      delete m_unfoldingError;
      m_unfoldingError = 0;
   }
   
   if(m_meanDeviation){
      delete m_meanDeviation;
      m_meanDeviation = 0;
   }
   
   if(m_rmsDeviation){
      delete m_rmsDeviation;
      m_rmsDeviation = 0;
   }
   
   if(m_meanDeviationScaled){
      delete m_meanDeviationScaled;
      m_meanDeviationScaled = 0;
   }
   
   if(m_rmsDeviationScaled){
      delete m_rmsDeviationScaled;
      m_rmsDeviationScaled = 0;
   }
   
   if(m_efficiencyHisto){
      delete m_efficiencyHisto;
      m_efficiencyHisto = 0;
   }
}


bool ttbarUnfold::doUnfolding(void){
   
   // get histograms from file
   RetrieveHistos();
   
   // get right labels
   KinematicVariable* kinVar = new KinematicVariable(m_sampleName);
   std::string kinVarLabel  = kinVar->GetKinVarLabel();
   std::string UkinVarLabel = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   std::string NkinVarLabel = kinVar->GetNKinVarLabel(); 
        
   // Get Response matrix from RooUnfold
   m_response = new RooUnfoldResponse(m_mc_reco,m_mc_truth,m_mc_migration);
   // Get Unfolding
   switch(m_method){
      case SVD:
         std::cout << "[ttbarUnfold::doUnfolding] Doing SVD unfolding.\n";
         m_unfolded = new RooUnfoldSvd(m_response,m_data_measuredMinusBkgd,m_reg,m_ntoys_errors);
         break;
      case Bayes:
         std::cout << "[ttbarUnfold::doUnfolding] Doing Bayes unfolding.\n";
         m_unfolded = new RooUnfoldBayes(m_response,m_data_measuredMinusBkgd,m_iters);
         break;
      case BinByBin:
         std::cout << "[ttbarUnfold::doUnfolding] Doing BinByBin unfolding.\n";
         m_unfolded = new RooUnfoldBinByBin(m_response,m_data_measuredMinusBkgd);
         break;
      case Inversion:
         std::cout << "[ttbarUnfold::doUnfolding] Doing Inversion unfolding.\n";
         m_unfolded = new RooUnfoldInvert(m_response,m_data_measuredMinusBkgd);
         break;
      default:
         std::cerr << "[ttbarUnfold::doUnfolding] something went wrong. m_method is not defined.\n";
         return false;
   }
   
   // retrieve the copy of the unfolded histogram
   std::stringstream name;
   name <<  m_histo_baseName << "data_unfolded";
   m_data_unfolded = (TH1D*) m_unfolded->Hreco(m_unfoldErrorTreatment);
   m_data_unfolded->SetName(name.str().c_str());
   m_data_unfolded->SetTitle("");
   m_data_unfolded->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_data_unfolded->GetYaxis()->SetTitle("N^{t}");
   m_data_unfolded->SetDirectory(0);
   
   // Clone unfolded result and normalize using truth
   name << "Norm";
   m_data_unfoldedNorm = (TH1D*)m_data_unfolded->Clone(name.str().c_str());
   m_data_unfoldedNorm->SetDirectory(false);
   m_data_unfoldedNorm->Scale(1.0/m_data_unfolded->Integral("width"));
   m_data_unfoldedNorm->SetTitle("");
   m_data_unfoldedNorm->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_data_unfoldedNorm->GetYaxis()->SetTitle("#frac{1}{#SigmaN^{t}} N^{t}");
   
   // Clone unfolded result and get deviation
   name.str("");
   name << m_histo_baseName << "deviation";
   m_deviation = (TH1D*) m_mc_truth->Clone(name.str().c_str());
   m_deviation->SetDirectory(false);
   m_deviation->SetTitle("");
   m_deviation->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_data_unfoldedNorm->GetYaxis()->SetTitle("#left(#||{N^{t}}_{unf}-#||{N^{t}}_{truth}#right)/#||{N^{t}}_{truth}");
   GetDeviationHisto(m_data_unfolded,m_mc_truth,m_deviation);
   
   if(m_efficiencyHisto){
      
      // set efficiencyHisto name
      name.str("");
      name << m_histo_baseName << "efficiency";
      m_efficiencyHisto->SetName(name.str().c_str());
      
      // create the dsigma/dpt plot of unfolded results
      name.str("");
      name <<  m_histo_baseName << "data_unfolded_diffxs";
      m_data_unfolded_diffxs =  globals::LumiEfficiencyCorrectedSpectra(m_data_unfolded,m_sampleName,m_efficiencyHisto);
      m_data_unfolded_diffxs->SetName(name.str().c_str());
      m_data_unfolded_diffxs->SetDirectory(0);
      m_data_unfolded_diffxs->GetYaxis()->SetTitle(DkinVarLabel.c_str());
      
      // Clone dsigma/dpt and normalize 
      name << "Norm";
      m_data_unfolded_diffxsNorm = (TH1D*)m_data_unfolded_diffxs->Clone(name.str().c_str());
      m_data_unfolded_diffxsNorm->SetDirectory(false);
      m_data_unfolded_diffxsNorm->Scale(1.0/m_data_unfolded_diffxsNorm->Integral("width"));
      m_data_unfolded_diffxsNorm->SetTitle("");
      m_data_unfolded_diffxsNorm->GetYaxis()->SetTitle(NkinVarLabel.c_str());
      
      // create the dsigma/dpt plot of mc truth
      name.str("");
      name <<  m_histo_baseName << "mc_truth_diffxs";
      m_mc_truth_diffxs =  globals::LumiEfficiencyCorrectedSpectra(m_mc_truth,m_sampleName,m_efficiencyHisto);
      m_mc_truth_diffxs->SetName(name.str().c_str());
      m_mc_truth_diffxs->SetDirectory(0);
      m_mc_truth_diffxs->GetYaxis()->SetTitle(DkinVarLabel.c_str());
      
      // Clone dsigma/dpt and normalize 
      name << "Norm";
      m_mc_truth_diffxsNorm = (TH1D*)m_mc_truth_diffxs->Clone(name.str().c_str());
      m_mc_truth_diffxsNorm->SetDirectory(false);
      m_mc_truth_diffxsNorm->Scale(1.0/m_mc_truth_diffxsNorm->Integral("width"));
      m_mc_truth_diffxsNorm->SetTitle("");
      m_mc_truth_diffxsNorm->GetYaxis()->SetTitle(NkinVarLabel.c_str());
      
      
      // create the dsigma/dpt plot of mc reco
      name.str("");
      name <<  m_histo_baseName << "mc_reco_diffxs";
      m_mc_reco_diffxs =  globals::LumiEfficiencyCorrectedSpectra(m_mc_reco,m_sampleName,m_efficiencyHisto);
      m_mc_reco_diffxs->SetName(name.str().c_str());
      m_mc_reco_diffxs->SetDirectory(0);
      m_mc_reco_diffxs->GetYaxis()->SetTitle(DkinVarLabel.c_str());
      
      // Clone dsigma/dpt and normalize 
      name << "Norm";
      m_mc_reco_diffxsNorm = (TH1D*)m_mc_reco_diffxs->Clone(name.str().c_str());
      m_mc_reco_diffxsNorm->SetDirectory(false);
      m_mc_reco_diffxsNorm->Scale(1.0/m_mc_reco_diffxsNorm->Integral("width"));
      m_mc_reco_diffxsNorm->SetTitle("");
      m_mc_reco_diffxsNorm->GetYaxis()->SetTitle(NkinVarLabel.c_str());
      
      
      // Clone unfolded result and get deviation
      name.str("");
      name << m_histo_baseName << "deviation_diffxs";
      std::stringstream ytitle;
      ytitle << "#left(#||{#frac{d#sigma}{d" << kinVarLabel << "}}_{unf} - #||{#frac{d#sigma}{" << kinVarLabel << "}}_{truth}#right)/#||{#frac{d#sigma}{d" << kinVarLabel << "}}_{truth}"; 
      m_deviation_diffxs = (TH1D*) m_data_unfolded_diffxs->Clone(name.str().c_str());
      m_deviation_diffxs->SetDirectory(false);
      m_deviation_diffxs->SetTitle("");
      m_deviation->GetXaxis()->SetTitle(UkinVarLabel.c_str());
      m_deviation->GetYaxis()->SetTitle(ytitle.str().c_str());
      GetDeviationHisto(m_data_unfolded_diffxs,m_mc_truth_diffxs,m_deviation_diffxs);  
   }
   
   // Get the regularization histogram from ROOT if using SVD method
   if(m_method == SVD){
      std::stringstream nameD;
      nameD << m_histo_baseName << "Regularization";
      TSVDUnfold* m_svdUnfold = (TSVDUnfold*) ((RooUnfoldSvd*)m_unfolded)->Impl();
      
      m_regularizationScan = (TH1D *) m_svdUnfold->GetD()->Clone(nameD.str().c_str());
      m_regularizationScan->SetDirectory(0);
   }
   
   delete kinVar;
   kinVar = 0;
   
   return true;
}

bool ttbarUnfold::runToys(void){
   
   RooUnfoldErrors* errors = new RooUnfoldErrors(m_ntoys_unfold,m_unfolded,
         m_mc_truth,m_data_measured,m_data_bkgd,m_efficiencyHisto,
         m_sampleName,globals::useGausSmearing);
   
   std::stringstream nToysSS;
   nToysSS << m_ntoys_errors;
   
   // get right labels
   KinematicVariable* kinVar = new KinematicVariable(m_sampleName);
   std::string kinVarLabel  = kinVar->GetKinVarLabel();
   std::string UkinVarLabel  = kinVar->GetUKinVarLabel();
      
   std::stringstream name;
   name << m_histo_baseName << "rmsResiduals";
   std::stringstream ytitle;
   ytitle << "RMS on #frac{dN}{d" << kinVarLabel << "} of Toy Models";
   m_rmsResiduals = (TH1D*)errors->RMSResiduals();
   m_rmsResiduals->SetName(name.str().c_str());
   m_rmsResiduals->SetTitle("");
   m_rmsResiduals->SetXTitle(UkinVarLabel.c_str());
   m_rmsResiduals->SetYTitle(ytitle.str().c_str());
   m_rmsResiduals->SetDirectory(false);
   
   name.str("");
   name << m_histo_baseName << "meanResiduals";
   ytitle.str("");
   ytitle << "Mean #frac{dN}{d" << kinVarLabel << "} of Toy Models";
   m_meanResiduals = (TH1D*)errors->MeanResiduals();
   m_meanResiduals->SetName(name.str().c_str());
   m_meanResiduals->SetXTitle(UkinVarLabel.c_str());
   m_meanResiduals->SetYTitle(ytitle.str().c_str());
   m_meanResiduals->SetDirectory(false);
   
   name.str("");
   name << m_histo_baseName << "rmsResidualsDiffxsNorm";
   ytitle.str("");
   ytitle << "RMS on #frac{1}{#sigma} #frac{d#sigma}{d" << kinVarLabel << "} of Toy Models"; 
   m_rmsResidualsDiffxsNorm = (TH1D*)errors->RMSResidualsDiffxsNorm();
   m_rmsResidualsDiffxsNorm->SetName(name.str().c_str());
   m_rmsResidualsDiffxsNorm->SetTitle("");
   m_rmsResidualsDiffxsNorm->SetXTitle(UkinVarLabel.c_str());
   m_rmsResidualsDiffxsNorm->SetYTitle(ytitle.str().c_str());
   m_rmsResidualsDiffxsNorm->SetDirectory(false);
   
   name.str("");
   name << m_histo_baseName << "meanResidualsDiffxsNorm";
   ytitle.str("");
   ytitle << "Mean of #frac{1}{#sigma} #frac{d#sigma}{d" << kinVarLabel << "} of Toy Models";
   m_meanResidualsDiffxsNorm = (TH1D*)errors->MeanResidualsDiffxsNorm();
   m_meanResidualsDiffxsNorm->SetName(name.str().c_str());
   m_meanResidualsDiffxsNorm->SetTitle("");   
   m_meanResidualsDiffxsNorm->SetXTitle(UkinVarLabel.c_str());
   m_meanResidualsDiffxsNorm->SetYTitle(ytitle.str().c_str());
   m_meanResidualsDiffxsNorm->SetDirectory(false);
   
   name.str("");
   name << m_histo_baseName << "rmsResidualsDiffxs";
   ytitle.str("");
   ytitle <<"RMS on #frac{d#sigma}{d" << kinVarLabel << "} of Toy Models";
   m_rmsResidualsDiffxs = (TH1D*)errors->RMSResidualsDiffxs();
   m_rmsResidualsDiffxs->SetName(name.str().c_str());
   m_rmsResidualsDiffxs->SetTitle("");
   m_rmsResidualsDiffxs->SetXTitle(UkinVarLabel.c_str());
   m_rmsResidualsDiffxs->SetYTitle(ytitle.str().c_str());
   m_rmsResidualsDiffxs->SetDirectory(false);
   
   name.str("");
   name << m_histo_baseName << "meanResidualsDiffxs";
   ytitle.str("");
   ytitle << "Mean of #frac{d#sigma}{d"<< kinVarLabel << "} of Toy Models";
   m_meanResidualsDiffxs = (TH1D*)errors->MeanResidualsDiffxs();
   m_meanResidualsDiffxs->SetName(name.str().c_str());
   m_meanResidualsDiffxs->SetTitle("");
   m_meanResidualsDiffxs->SetXTitle(UkinVarLabel.c_str());
   m_meanResidualsDiffxs->SetYTitle(ytitle.str().c_str());
   m_meanResidualsDiffxs->SetDirectory(false);
   
   name.str("");
   name << m_histo_baseName << "unfoldingError";
   m_unfoldingError = (TH1D*)errors->UnfoldingError();
   m_unfoldingError->SetName(name.str().c_str());
   m_unfoldingError->SetTitle("");   
   m_unfoldingError->SetXTitle(UkinVarLabel.c_str());
   m_unfoldingError->SetYTitle("Errors on unfolded result from each Toy Model");
   m_unfoldingError->SetDirectory(false);
   
   name.str("");
   name << m_histo_baseName << "meanDeviation";
   m_meanDeviation = (TH1D*)errors->MeanDeviation();
   m_meanDeviation->SetName(name.str().c_str());
   m_meanDeviation->SetTitle("");     
   m_meanDeviation->SetXTitle(UkinVarLabel.c_str());
   m_meanDeviation->SetYTitle("Mean Deviation of all Toy Model");
   m_meanDeviation->SetDirectory(false);
   
   
   name.str("");
   name << m_histo_baseName << "rmsDeviation";
   m_rmsDeviation = (TH1D*)errors->RMSDeviation();
   m_rmsDeviation->SetName(name.str().c_str());
   m_rmsDeviation->SetTitle("");    
   m_rmsDeviation->SetXTitle(UkinVarLabel.c_str());
   m_rmsDeviation->SetYTitle("RMS of Deviation of all Toy Model");
   m_rmsDeviation->SetDirectory(false);
   
   name.str("");
   name << m_histo_baseName << "meanDeviationScaled";
   m_meanDeviationScaled = (TH1D*)errors->MeanDeviationScaled();
   m_meanDeviationScaled->SetName(name.str().c_str());
   m_meanDeviationScaled->SetTitle("");   
   m_meanDeviationScaled->SetXTitle(UkinVarLabel.c_str());
   m_meanDeviationScaled->SetYTitle("Mean Scaled Deviation of all Toy Model");
   m_meanDeviationScaled->SetDirectory(false);
   
   
   name.str("");
   name << m_histo_baseName << "rmsDeviationScaled";
   m_rmsDeviationScaled = (TH1D*)errors->RMSDeviationScaled();
   m_rmsDeviationScaled->SetName(name.str().c_str());
   m_rmsDeviationScaled->SetTitle("");      
   m_rmsDeviationScaled->SetXTitle(UkinVarLabel.c_str());
   m_rmsDeviationScaled->SetYTitle("RMS of Scaled Deviation of all Toy Model");
   m_rmsDeviationScaled->SetDirectory(false);
   
   delete errors;
   errors = 0;
   
   delete kinVar;
   kinVar = 0;
   
   return true;
}


void ttbarUnfold::RetrieveHistos(void){
   
   // open MC file
   TFile* mcFile = TFile::Open(m_mcFilename.c_str(),"READ");
   if(!mcFile){
      std::cerr << "[ttbarUnfold::RetrieveHistos] ERROR could not open " << m_mcFilename << "\n";
      return;
   }

   // retrieve histogram name
   //std::string m_sampleName ="Tag1_Top1";
   SampleDef sample(m_sampleName);
   std::string name_truth              = sample.GetTruthHistoPath();
   std::string name_reco               = sample.GetRecoHistoPath();
   std::string name_migration          = sample.GetMigrationHistoPath();
   std::string name_measuredMinusBkgd  = sample.GetRecoHistoPath();
   std::string name_measured;
   std::string name_bkgd;
   if(m_measuredMinusBkgdHistoName.size() > 0){
      name_measuredMinusBkgd  = sample.GetMeasuredMinusBkgd();
      name_measured           = sample.GetMeasured();
      name_bkgd               = sample.GetBkgd();
   }
   
   std::cout << "[ttbarUnfold::RetrieveHistos]: \n";
   std::cout << " retrieving for sample name " << m_sampleName << " \n";
   std::cout << "   " << name_truth << "\n";
   std::cout << "   " << name_reco << "\n";
   std::cout << "   " << name_migration << "\n";
   std::cout << "   " << name_truth << "\n";

   // retrieve right labels
   KinematicVariable* kinVar = new KinematicVariable(m_sampleName);
   std::string kinVarLabel   = kinVar->GetKinVarLabel();
   std::string UkinVarLabel  = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel  = kinVar->GetDKinVarLabel();
   std::string NkinVarLabel  = kinVar->GetNKinVarLabel(); 
   
   // retrieve the true pt spectrum
   TH1D* truth = (TH1D*)mcFile->Get(name_truth.c_str());
   if (!truth){
      std::cerr << "[ttbarUnfold::RetrieveHistos] ERROR retrieving histogram, " << name_truth << ", from file, " << m_mcFilename << std::endl;
      return;
   }
   std::stringstream name;
   name << m_histo_baseName << "mc_truth";
   m_mc_truth = (TH1D*)truth->Clone(name.str().c_str());
   m_mc_truth->SetDirectory(false);
   m_mc_truth->SetTitle("");
   m_mc_truth->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_mc_truth->GetYaxis()->SetTitle("N^{t}");
   
   // delete temp histogram
   delete truth;
   truth = 0;
   
   // Clone truth and normalize using truth
   name << "Norm";
   m_mc_truthNorm = (TH1D*)m_mc_truth->Clone(name.str().c_str());
   m_mc_truthNorm->SetDirectory(false);
   m_mc_truthNorm->Scale(1.0/m_mc_truth->Integral("width"));
   m_mc_truthNorm->SetTitle("");
   m_mc_truthNorm->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_mc_truthNorm->GetYaxis()->SetTitle("#frac{1}{#SigmaN^{t}} N^{t}");
   
   // retrieve the pt spectrum reconstructed from the true spectrum
   TH1D* mcReco = (TH1D*)mcFile->Get(name_reco.c_str());
   if (!mcReco){
      std::cerr << "[ttbarUnfold::RetrieveHistos] ERROR retrieving histogram, " << name_reco << ", from file, " << m_mcFilename << std::endl;
      return;
   }
   name.str("");
   name << m_histo_baseName << "mc_reco";
   m_mc_reco = (TH1D*)mcReco->Clone(name.str().c_str());
   m_mc_reco->SetDirectory(false);
   m_mc_reco->SetTitle("");
   m_mc_reco->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_mc_reco->GetYaxis()->SetTitle("N^{t}");
   // delete temp histogram
   delete mcReco;
   mcReco = 0;
   
   // Clone reco and normalize using truth
   name << "Norm";
   m_mc_recoNorm = (TH1D*)m_mc_reco->Clone(name.str().c_str());
   m_mc_recoNorm->SetDirectory(false);
   m_mc_recoNorm->Scale(1.0/m_mc_reco->Integral("width"));
   m_mc_recoNorm->SetTitle("");
   m_mc_recoNorm->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_mc_recoNorm->GetYaxis()->SetTitle("#frac{1}{#SigmaN^{t}} N^{t}");   
   
   // retrieve the plot of true pt versus reconstructed-from-truth for pt
   // if a migration matrix has already been set, us it instead of loading
   // from file
   std::stringstream xtitle;
   xtitle << "reco " << UkinVarLabel; 
   std::stringstream ytitle;
   ytitle << "truth " << UkinVarLabel; 
   
   if(!m_mc_migration){
      TH2D* mcMigration = (TH2D*)mcFile->Get(name_migration.c_str());
      if (!mcMigration){
         std::cerr << "[ttbarUnfold::RetrieveHistos] ERROR retrieving histogram, " << name_migration << ", from file, " << m_mcFilename << std::endl;
         return;
      }
      name.str("");
      name << m_histo_baseName << "mc_migration";
      m_mc_migration = (TH2D*)mcMigration->Clone(name.str().c_str());
      m_mc_migration->SetTitle("");
      m_mc_migration->SetDirectory(false);
      // transpose because we get reco(y) vs. truth(x), RooUnfold needs reco(x) vs. truth(y)
      Transpose(m_mc_migration);
      m_mc_migration->GetXaxis()->SetTitle(xtitle.str().c_str());
      m_mc_migration->GetYaxis()->SetTitle(ytitle.str().c_str());
      m_mc_migration->GetZaxis()->SetTitle("N^{t}");
      
      
      // to scale the rapidity migration matrix when unfolding mc halves
      if(sample == SampleDef::Tag1_SystemRapidity)
         m_mc_migration->Scale(0.5);
      
      // delete temp histogram
      delete mcMigration;
      mcMigration = 0;
   }
   
   // Clone migration matrix and normalize using truth (do we need this?)
   name << "Norm";
   m_mc_migrationNorm = (TH2D*)m_mc_migration->Clone(name.str().c_str());
   m_mc_migrationNorm->SetDirectory(false);
   m_mc_migrationNorm->Scale(1.0/m_mc_migration->Integral("width"));
   m_mc_migrationNorm->SetTitle("");
   m_mc_migration->GetXaxis()->SetTitle(xtitle.str().c_str());
   m_mc_migration->GetYaxis()->SetTitle(ytitle.str().c_str());
   m_mc_migration->GetZaxis()->SetTitle("#frac{1}{#SigmaN^{t}} N^{t}");
   
   // close mc file
   mcFile->Close();
   
   //////////////////////////////
   // open data file
   TFile* dataFile = TFile::Open(m_dataFilename.c_str(),"READ");
   if(!dataFile){
      std::cerr << "[ttbarUnfold::RetrieveHistos] ERROR could not open " << m_dataFilename << "\n";
      return;
   }
   
   // retrieve the data Minus Background spectrum
   TH1D* measuredMinusBkgd = (TH1D*)dataFile->Get(name_measuredMinusBkgd.c_str());
   if (!measuredMinusBkgd){
      std::cerr << "[ttbarUnfold::RetrieveHistos] ERROR retrieving histogram, " << name_measuredMinusBkgd << ", from file, " << m_dataFilename << std::endl;
      return;
   }
   name.str("");
   name << m_histo_baseName << "data_measured";
   m_data_measuredMinusBkgd = (TH1D*)measuredMinusBkgd->Clone(name.str().c_str());
   m_data_measuredMinusBkgd->SetDirectory(false);
   m_data_measuredMinusBkgd->SetTitle("");
   m_data_measuredMinusBkgd->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_data_measuredMinusBkgd->GetYaxis()->SetTitle("N^{t}");
   // delete temp histogram
   delete measuredMinusBkgd;
   measuredMinusBkgd = 0;
   
   // Clone data spectrum and normalize by truth
   name << "Norm";
   m_data_measuredMinusBkgdNorm = (TH1D*)m_data_measuredMinusBkgd->Clone(name.str().c_str());
   m_data_measuredMinusBkgdNorm->SetDirectory(false);
   m_data_measuredMinusBkgdNorm->Scale(1.0/m_data_measuredMinusBkgd->Integral("width"));
   m_data_measuredMinusBkgdNorm->SetTitle("");
   m_data_measuredMinusBkgdNorm->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_data_measuredMinusBkgdNorm->GetYaxis()->SetTitle("#frac{1}{#SigmaN^{t}} N^{t}");
   
   // retrieve the data spectrum if available
   if(name_measured.size()){
      TH1D* measured = (TH1D*)dataFile->Get(name_measured.c_str());
      if (!measured){
         std::cerr << "[ttbarUnfold::RetrieveHistos] ERROR retrieving histogram, " << name_measured << ", from file, " << m_dataFilename << std::endl;
         return;
      }
      name.str("");
      name << m_histo_baseName << "data_measuredWithBkgd";
      m_data_measured = (TH1D*)measured->Clone(name.str().c_str());
      m_data_measured->SetDirectory(false);
      m_data_measured->SetTitle("");
      m_data_measured->GetXaxis()->SetTitle(UkinVarLabel.c_str());
      m_data_measured->GetYaxis()->SetTitle("N^{t}");
      // delete temp histogram
      delete measured;
      measured = 0;
      
      // Clone data spectrum and normalize by truth
      name << "Norm";
      m_data_measuredNorm = (TH1D*)m_data_measured->Clone(name.str().c_str());
      m_data_measuredNorm->SetDirectory(false);
      m_data_measuredNorm->Scale(1.0/m_data_measuredMinusBkgd->Integral("width"));
      m_data_measuredNorm->SetTitle("");
      m_data_measuredNorm->GetXaxis()->SetTitle(UkinVarLabel.c_str());
      m_data_measuredNorm->GetYaxis()->SetTitle("#frac{1}{#SigmaN^{t}} N^{t}");
   }
   
   // retrieve the background spectrum if available
   if(name_bkgd.size()){
      TH1D* bkgd = (TH1D*)dataFile->Get(name_bkgd.c_str());
      if (!bkgd){
         std::cerr << "[ttbarUnfold::RetrieveHistos] ERROR retrieving histogram, " << name_bkgd << ", from file, " << m_dataFilename << std::endl;
         return;
      }
      name.str("");
      name << m_histo_baseName << "data_bkgdOnly";
      m_data_bkgd = (TH1D*)bkgd->Clone(name.str().c_str());
      m_data_bkgd->SetDirectory(false);
      m_data_bkgd->SetTitle("");
      m_data_bkgd->GetXaxis()->SetTitle(UkinVarLabel.c_str());
      m_data_bkgd->GetYaxis()->SetTitle("N^{t}");
   // delete temp histogram
      delete bkgd;
      bkgd = 0;
      
      // Clone data spectrum and normalize by truth
      name << "Norm";
      m_data_bkgdNorm = (TH1D*)m_data_bkgd->Clone(name.str().c_str());
      m_data_bkgdNorm->SetDirectory(false);
      m_data_bkgdNorm->Scale(1.0/m_data_measuredMinusBkgd->Integral("width"));
      m_data_bkgdNorm->SetTitle("");
      m_data_bkgdNorm->GetXaxis()->SetTitle(UkinVarLabel.c_str());
      m_data_bkgdNorm->GetYaxis()->SetTitle("#frac{1}{#SigmaN^{t}} N^{t}");
   }
   
   // also want to scale the MC to the level of the data
   double scale = m_data_measuredMinusBkgd->Integral("width")/m_mc_reco->Integral("width");
   
   // Clone Truth to create scaled plot
   name.str("");
   name << m_mc_truth->GetName() << "Scaled";
   m_mc_truthScaled = (TH1D*)m_mc_truth->Clone(name.str().c_str());
   m_mc_truthScaled->SetDirectory(false);
   m_mc_truthScaled->Scale(scale);
   m_mc_truthScaled->SetTitle("");
   m_mc_truthScaled->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_mc_truthScaled->GetYaxis()->SetTitle("scaled by measured/reco");   
   
   // Clone MC Reco to create scaled plot
   name.str("");
   name << m_mc_reco->GetName() << "Scaled";
   m_mc_recoScaled = (TH1D*)m_mc_reco->Clone(name.str().c_str());
   m_mc_recoScaled->SetTitle("");
   m_mc_recoScaled->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_mc_recoScaled->GetYaxis()->SetTitle("scaled by measured/reco");
   m_mc_recoScaled->SetDirectory(false);
   m_mc_recoScaled->Scale(scale);
   
   // Clone MC migration matrix to create scaled plot
   name.str("");
   name << m_mc_migration->GetName() << "Scaled";
   m_mc_migrationScaled = (TH2D*)m_mc_migration->Clone(name.str().c_str());
   m_mc_migrationScaled->SetTitle("");
   m_mc_migrationScaled->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_mc_migrationScaled->GetYaxis()->SetTitle("scaled by measured/reco"); 
   m_mc_migrationScaled->SetDirectory(false);
   m_mc_migrationScaled->Scale(scale);
   
   // close data file
   dataFile->Close();
   
   delete mcFile;
   mcFile = 0;
   
   delete dataFile;
   dataFile = 0;
   
   delete kinVar;
   kinVar = 0;
   
}

void ttbarUnfold::DrawMigrationMatrix(const std::string sampleName){
   
   gStyle->SetPalette(1);
   gStyle->SetPaintTextFormat("5.2f");
   gStyle->SetTitleBorderSize(0);
   gStyle->SetTitleFillColor(0);
   
   // retrieve right labels
   KinematicVariable* kinVar = new KinematicVariable(m_sampleName);
   std::string UkinVarLabel  = kinVar->GetUKinVarLabel();
   
   std::stringstream xtitle;
   xtitle << "reco " << UkinVarLabel; 
   std::stringstream ytitle;
   ytitle << "truth " << UkinVarLabel; 

   m_mc_migration->SetLineColor(kBlack);
   m_mc_migration->SetLineWidth(2);
   m_mc_migration->SetLineStyle(1);
   m_mc_migration->SetMarkerColor(kBlack);
   m_mc_migration->SetMarkerStyle(20);
   m_mc_migration->SetNdivisions(508,"X");
   m_mc_migration->SetNdivisions(508,"Y");
   m_mc_migration->SetNdivisions(508,"Z");
   m_mc_migration->SetStats(0);
   m_mc_migration->SetTitle(sampleName.c_str());
   m_mc_migration->GetXaxis()->SetTitle(xtitle.str().c_str());
   m_mc_migration->GetYaxis()->SetTitle(ytitle.str().c_str());
   m_mc_migration->GetZaxis()->SetTitle("N^{t}");
   m_mc_migration->GetYaxis()->SetTitleOffset(1.2);
   m_mc_migration->GetXaxis()->SetTitleOffset(1.2);
   m_mc_migration->Draw("COLZ");
   m_mc_migration->Draw("TEXTsame");
   
   delete kinVar;
   kinVar = 0;
}

void ttbarUnfold::DrawPtSpectraScale(const std::string sampleName){
   gStyle->SetTitleBorderSize(0);
   gStyle->SetTitleFillColor(0);
   
   // retrieve right labels
   KinematicVariable* kinVar = new KinematicVariable(m_sampleName);
   std::string UkinVarLabel  = kinVar->GetUKinVarLabel();

   m_data_unfolded->SetLineColor(kBlue);
   m_data_unfolded->SetLineWidth(2);
   m_data_unfolded->SetLineStyle(1);
   m_data_unfolded->SetMarkerColor(kBlue);
   m_data_unfolded->SetMarkerSize(1.2);
   m_data_unfolded->SetMarkerStyle(21);
   m_data_unfolded->SetStats(0);
   m_data_unfolded->SetTitle(sampleName.c_str());
   m_data_unfolded->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_data_unfolded->GetYaxis()->SetTitle("");
   m_data_unfolded->GetYaxis()->SetTitleOffset(1.2);
   m_data_unfolded->GetXaxis()->SetTitleOffset(1.2);
   //double umax = m_data_unfolded->GetMaximum();
  
   m_mc_truthScaled->SetLineColor(kBlack);
   m_mc_truthScaled->SetLineWidth(2);
   m_mc_truthScaled->SetLineStyle(2);
   m_mc_truthScaled->SetMarkerSize(1.2);
   m_mc_truthScaled->SetMarkerColor(kBlack);
   m_mc_truthScaled->SetMarkerStyle(20);
   //double tmax=m_mc_truthScaled->GetMaximum();
   
   //if(umax<tmax) umax = tmax;

   m_mc_recoScaled->SetLineColor(kRed);
   m_mc_recoScaled->SetLineWidth(2);
   m_mc_recoScaled->SetLineStyle(3); 
   m_mc_recoScaled->SetMarkerSize(1.2);  
   m_mc_recoScaled->SetMarkerColor(kRed);
   m_mc_recoScaled->SetMarkerStyle(22);
   //double mrmax=m_mc_recoScaled->GetMaximum();
   
   //if(umax<mrmax) umax = mrmax;
  
   m_data_measuredMinusBkgd->SetLineColor(kGreen);
   m_data_measuredMinusBkgd->SetLineWidth(2);
   m_data_measuredMinusBkgd->SetLineStyle(4);
   m_data_measuredMinusBkgd->SetMarkerSize(1.2);  
   m_data_measuredMinusBkgd->SetMarkerColor(kGreen);
   m_data_measuredMinusBkgd->SetMarkerStyle(23);
   //double rmax=m_data_measuredMinusBkgd->GetMaximum();

   //if(umax<rmax) umax = rmax;
   //double max = umax + 0.1 * fabs(umax);

   //m_data_unfolded->SetMaximum(max);
   m_data_unfolded->Draw("e");
   m_mc_truthScaled->Draw("esame");
   m_mc_recoScaled->Draw("esame");
   m_data_measuredMinusBkgd->Draw("esame");

   TLegend *leg = new TLegend(0.55,0.58,0.88,0.85);
   leg->SetFillColor(0);
   leg->SetShadowColor(0);
   leg->SetLineColor(0);
   leg->AddEntry(m_mc_truthScaled, "MC Truth", "lp");
   leg->AddEntry(m_mc_recoScaled,  "MC Reco", "lp");
   leg->AddEntry(m_data_measuredMinusBkgd, "Data Reco", "lp");
   leg->AddEntry(m_data_unfolded, "Data Unfolded", "lp");
   leg->Draw();
   
   delete kinVar;
   kinVar = 0;
}

void ttbarUnfold::DrawPtSpectra(const std::string sampleName){
   gStyle->SetTitleBorderSize(0);
   gStyle->SetTitleFillColor(0);
   
   // retrieve right labels
   KinematicVariable* kinVar = new KinematicVariable(m_sampleName);
   std::string UkinVarLabel  = kinVar->GetUKinVarLabel();

   m_data_unfolded->SetLineColor(kBlue);
   m_data_unfolded->SetLineWidth(2);
   m_data_unfolded->SetLineStyle(1);
   m_data_unfolded->SetMarkerColor(kBlue);
   m_data_unfolded->SetMarkerSize(1.);
   m_data_unfolded->SetMarkerStyle(21);
   m_data_unfolded->SetStats(0);
   m_data_unfolded->SetTitle(sampleName.c_str());
   m_data_unfolded->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_data_unfolded->GetYaxis()->SetTitle("");
   m_data_unfolded->GetYaxis()->SetTitleOffset(1.2);
   m_data_unfolded->GetXaxis()->SetTitleOffset(1.2);
   //double umax = m_data_unfolded->GetMaximum();
  
   m_mc_truth->SetLineColor(kBlack);
   m_mc_truth->SetLineWidth(2);
   m_mc_truth->SetLineStyle(2);
   m_mc_truth->SetMarkerSize(1.2);
   m_mc_truth->SetMarkerColor(kBlack);
   m_mc_truth->SetMarkerStyle(20);
   //double tmax=m_mc_truth->GetMaximum();
   
   //if(umax<tmax) umax = tmax;

   m_mc_reco->SetLineColor(kRed);
   m_mc_reco->SetLineWidth(2);
   m_mc_reco->SetLineStyle(3); 
   m_mc_reco->SetMarkerSize(1.2);  
   m_mc_reco->SetMarkerColor(kRed);
   m_mc_reco->SetMarkerStyle(22);
   //double mrmax=m_mc_reco->GetMaximum();
   
   //if(umax<mrmax) umax = mrmax;
  
   m_data_measuredMinusBkgd->SetLineColor(kGreen);
   m_data_measuredMinusBkgd->SetLineWidth(2);
   m_data_measuredMinusBkgd->SetLineStyle(4);
   m_data_measuredMinusBkgd->SetMarkerSize(1.2);  
   m_data_measuredMinusBkgd->SetMarkerColor(kGreen);
   m_data_measuredMinusBkgd->SetMarkerStyle(23);
   //double rmax=m_data_measuredMinusBkgd->GetMaximum();

   //if(umax<rmax) umax = rmax;
   //double max = umax + 0.1 * fabs(umax);

   //m_data_unfolded->SetMaximum(max);
   m_data_unfolded->Draw("e");
   m_mc_truth->Draw("esame");
   m_mc_reco->Draw("esame");
   m_data_measuredMinusBkgd->Draw("esame");
   
   TLegend *leg = new TLegend(0.55,0.58,0.88,0.85);
   leg->SetFillColor(0);
   leg->SetShadowColor(0);
   leg->SetLineColor(0);
   leg->AddEntry(m_mc_truth, "MC Truth", "lp");
   leg->AddEntry(m_mc_reco,  "MC Reco", "lp");
   leg->AddEntry(m_data_measuredMinusBkgd, "Data Reco", "lp");
   leg->AddEntry(m_data_unfolded, "Data Unfolded", "lp");
   leg->Draw();
   
   delete kinVar;
   kinVar = 0;
}

void ttbarUnfold::DrawDeviation(){
   
   std::stringstream name;
   name << m_histo_baseName << "_Deviation";
   
   m_deviation->SetLineColor(kBlue);
   m_deviation->SetLineWidth(2);
   m_deviation->SetLineStyle(1);
   m_deviation->SetMarkerColor(kBlue);
   m_deviation->SetMarkerStyle(22);
   m_deviation->SetStats(0);
   m_deviation->GetYaxis()->SetTitleOffset(1.2);
   m_deviation->GetXaxis()->SetTitleOffset(1.2);
   m_deviation->Draw("e");
}


void ttbarUnfold::DrawDeviationReco(){
   
   std::stringstream name;
   name << m_histo_baseName << "_Deviation_Reco";
   
      
   // retrieve right labels
   KinematicVariable* kinVar = new KinematicVariable(m_sampleName);
   std::string UkinVarLabel  = kinVar->GetUKinVarLabel();
   
   
   m_deviationReco = (TH1D *) m_data_measuredMinusBkgd->Clone(name.str().c_str());
   m_deviationReco->SetDirectory(0);
   m_deviationReco->Reset();
   
   bool doRatios = true;
   double min=999., max=-999.;

   for (Int_t i= 1; i<=m_mc_truth->GetNbinsX(); i++) {
      if ((m_data_measuredMinusBkgd->GetBinContent(i) > 0.0) && (m_mc_truth->GetBinContent(i))) {
         double unf_i = m_data_measuredMinusBkgd->GetBinContent(i);
         double truth_i = m_mc_truth->GetBinContent(i);
         double dev = (unf_i - truth_i);
         if (doRatios)
            dev /= truth_i;
         double unf_err = m_data_measuredMinusBkgd-> GetBinError(i);
         double truth_err = m_mc_truth-> GetBinError(i);
         double err = unf_err;
         if (doRatios)
            err = sqrt( pow(unf_err/truth_i, 2) + pow(unf_i*truth_err/(truth_i*truth_i), 2));
         m_deviationReco->SetBinContent(i,dev);
         m_deviationReco->SetBinError(i, err);

         if(max<dev) max=dev+0.2*fabs(dev);
         if(min>dev) min=dev-0.2*fabs(dev);
      }
   }   
   
   m_deviationReco->SetLineColor(kBlue);
   m_deviationReco->SetLineWidth(2);
   m_deviationReco->SetLineStyle(1);
   m_deviationReco->SetMarkerColor(kBlue);
   m_deviationReco->SetMarkerStyle(22);
   m_deviationReco->SetStats(0);
   m_deviationReco->SetTitle("");
   m_deviationReco->SetMinimum(min);
   m_deviationReco->SetMaximum(max);
   m_deviationReco->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   m_deviationReco->GetYaxis()->SetTitle("(x^{Reco}-x^{Truth})/x^{Truth}");
   m_deviationReco->GetYaxis()->SetTitleOffset(1.2);
   m_deviationReco->GetXaxis()->SetTitleOffset(1.2);
   m_deviationReco->Draw("e");
}

void ttbarUnfold::DrawDistributionD(){
   m_regularizationScan->SetLineColor(kBlue);
   m_regularizationScan->SetLineWidth(2);
   m_regularizationScan->SetLineStyle(1);     
   m_regularizationScan->SetMarkerColor(kBlue);
   m_regularizationScan->SetMarkerStyle(20);
   m_regularizationScan->SetStats(0);
   m_regularizationScan->SetTitle("");
   m_regularizationScan->GetXaxis()->SetTitle("i");
   m_regularizationScan->GetYaxis()->SetTitle("d_{i}");
   m_regularizationScan->GetYaxis()->SetTitleOffset(1.2);
   m_regularizationScan->GetXaxis()->SetTitleOffset(1.2);
   m_regularizationScan->Draw("e");
}

void ttbarUnfold::Transpose(TH2 *histo) {
 for (int i = 0; i <= histo -> GetNbinsX() + 1; ++i) {
   for (int j = 0; j <= i; ++j) {

     float swap = histo -> GetBinContent(i, j);
     float swaperr = histo -> GetBinError(i, j);
     histo -> SetBinContent(i, j, histo -> GetBinContent(j, i));
     histo -> SetBinError(i, j, histo -> GetBinError(j, i));

     histo -> SetBinContent(j, i, swap);
     histo -> SetBinError(j, i, swaperr);
   }
 }
}

std::string ttbarUnfold::GetMethodString(const UnfoldMethod& method){
   switch(method){
   case SVD:
      return "SVD";
   case BinByBin:
      return "BinByBin";
   case Bayes:
      return "Bayes";
   case Inversion:
      return "Inversion";
   }
   return "";
}


TH2D* ttbarUnfold::GetSmearedMigrationMatrix(bool usePoisson) const{
   
   if(!m_mc_migration){
      std::cout << "[ttbarUnfold::GetSmearedMigrationMatrix] no migration matrix available to smear.\n";
      return (TH2D*)0;
   }
   unsigned long long int seed = (unsigned long long int)m_mc_migration;
   TRandom rand((unsigned int)(seed >> 32));
   
   unsigned int Nx = m_mc_migration->GetNbinsX();
   unsigned int Ny = m_mc_migration->GetNbinsY();
   
   std::stringstream name;
   name << m_mc_migration->GetName() << "Smeared";
   TH2D* output = (TH2D*)m_mc_migration->Clone(name.str().c_str());
   output->Clear();
   
   for(unsigned int x=1;x<=Nx;++x){
      for(unsigned int y=1;y<=Ny;++y){
         float binContent = 0;
         if(usePoisson)
            binContent = rand.Poisson(m_mc_migration->GetBinContent(x,y));
         else
            binContent = rand.Gaus(m_mc_migration->GetBinContent(x,y),m_mc_migration->GetBinError(x,y));
         
         output->SetBinContent(x,y,binContent);
         output->SetBinError(x,y,m_mc_migration->GetBinError(x,y));
      }
   }
   
   return output;
}

TH1D* ttbarUnfold::GetSmearedEfficiency(TH2D const * const migration, const TopDecayChannel& channel) const {
   
   /// determine which generator we are using ALPGEN or MCATNLO
   GeneratorLabel genLabel(globals::GetOutputPath());
   genLabel.GetLabel();
   
   //std::cerr << "[ttbarUnfold::GetSmearedEfficiency] gen = " << genLabel.GetLabel() << "\n";
   
   TH1D* newEff = 0;
   if(genLabel.GetGenerator() == GeneratorLabel::ALPGEN){
      
      // use truth before cuts from ALPGEN
      AlpgenMC truthBeforeCuts(channel.GetChannelString(),m_sampleName);
      truthBeforeCuts.GetHisto();
      
      newEff = migration->ProjectionY("newEfficieny");
      newEff->SetDirectory(0);
      newEff->Divide(truthBeforeCuts.GetHistoInEventCounts());
      newEff->Scale(100.);
      
   }
   else if(genLabel.GetGenerator() == GeneratorLabel::MCATNLO){
      
      // use truth before cuts from MC@NLO
      McAtNloBeforeCuts truthBeforeCuts(channel.GetChannelString(),m_sampleName);
      
      newEff = migration->ProjectionY("newEfficieny");
      newEff->SetDirectory(0);
      newEff->Divide(truthBeforeCuts.GetHistoInEventCounts());
      newEff->Scale(100.);
      
   }
   else{
      std::cerr << "[ttbarUnfold::GetSmearedEfficiency] ERROR No Generator found." << genLabel.GetLabel() << "\n";
   }
   
   return newEff;
}

ttbarUnfold* ttbarUnfold::GetSmearedMigrationToy(const TopDecayChannel& channel,bool usePoisson) const {
   
   if(!m_unfolded || !m_mc_reco || !m_mc_truth || !m_mc_migration){
      std::cerr << "[ttbarUnfold::GetSmearedMigrationMatrix] must first perform unfolding before getting migration toy.\n";
      return (ttbarUnfold*)0;
   }
   
   TH2D* newMigration = GetSmearedMigrationMatrix(usePoisson);
   TH1D* newEfficiency = GetSmearedEfficiency(newMigration,channel);
   
   ttbarUnfold* toy = new ttbarUnfold(m_dataFilename,m_mcFilename,m_sampleName,newEfficiency);
   toy->SetMcMigration(newMigration);
   toy->SetToysForUnfolding(m_ntoys_unfold);
   toy->SetToysForErrors(m_ntoys_errors);
   std::string histoBase = "toy";
   toy->SetHistoBaseName(histoBase.c_str());
   toy->SetUnfoldingMethod(m_method);
   toy->SetMeasuredDataMinusBkgdHistogramName(m_measuredMinusBkgdHistoName);
   
   // set method specific values
   if(m_method == ttbarUnfold::SVD)
      toy->SetRegularization(m_reg);
   else if(m_method == ttbarUnfold::Bayes)
      toy->SetBayesianIterations(m_iters);
   
   if(!toy->doUnfolding()){
      std::cerr << "[ttbarUnfold::GetSmearedMigrationMatrix] ERROR running doUnfolding.\n";
      return (ttbarUnfold*)0;
   }
   
   return toy;
   
   
}

void ttbarUnfold::GetDeviationHisto(TH1* const x,TH1* const y,TH1* const z){
   
   if(x->GetNbinsX() != y->GetNbinsX()){
      std::cerr << "[ttbarUnfold::GetDeviation] ERROR: Plots have different number of bins.\n";
      return;
   }
   
   for(unsigned int bin=1;bin<=y->GetNbinsX();++bin){
      double a = x->GetBinContent(bin);
      double da = x->GetBinError(bin);
      double b = y->GetBinContent(bin);
      double db = y->GetBinError(bin);
            
      double dev = (b != 0) ? (a-b)/b : 0;
      double dev_error = (b != 0) ? (1.0/b)*sqrt(da*da + db*db*a*a/(b*b)) : 0;
            
      z->SetBinContent(bin,dev);
      z->SetBinError(bin,dev_error);
   }
}

