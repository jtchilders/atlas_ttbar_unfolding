#include "globals.h"

#include "TH1.h"
#include "TH1F.h"
#include "TGraphAsymmErrors.h"
#include "TVectorD.h"
#include "TH1D.h"

#include "KinematicVariable.h"
#include "TopDecayChannel.h"

#include <cmath>
#include <fstream>
#include <iterator>
#include <cassert>
#include <sstream>

const double globals::LUMI                     = 4655.74; //4713.11;
const double globals::LUMI_FRACTIONAL_ERROR    = 0.018;
const double globals::BR                       = 0.438;
const double globals::BR_ALPGEN                = 0.438;
const double globals::BR_MCATNLO               = 0.438;
const double globals::BR_POWHEG                = 0.438;
const double globals::MCATNLO_CORRECTION       = 0.0366595; //0.0371134;

const bool globals::DATA_ON_TOP = true;

const bool globals::useGausSmearing = false; /// SET TO TRUE when using MC only

const std::string globals::ejets         = "e+jets";
const std::string globals::mujets        = "#mu+jets";
const std::string globals::mujetsLatex   = "$\\mu$+jets";
const std::string globals::ljets         = "l+jets";
const std::string globals::btag1         = "#geq 1 b-jet";
const std::string globals::btag2         = "#geq 2 b-jet";
const std::string globals::btag0         = "#geq 0 b-jet";
const std::string globals::btag1Latex    = "$\\geq$ 1 b-jet";
const std::string globals::btag2Latex    = "$\\geq$ 2 b-jet";
const std::string globals::btag0Latex    = "$\\geq$ 0 b-jet";
const std::string globals::leptonicTop   = "leptonic top";
const std::string globals::hadronicTop   = "hadronic top";
const std::string globals::lepHadTop     = "lep+had top";

globals::unf_methods globals::globalUnfoldingMethod = globals::SVD;


const std::string globals::dataFilesPath = "/afs/cern.ch/work/c/carolina/data_14_02_2013";
const std::string globals::outputFilesPath = "/afs/cern.ch/work/c/childers/unfoldedData/March_4/Alpgen";
// const std::string globals::outputFilesPath = "/afs/cern.ch/work/c/carolina/data_Unfolded_14_02_2013/Alpgen";

const std::string globals::outputFilesPathTex = globals::outputFilesPath + "/tex";
const std::string globals::outputFilesPathEps = globals::outputFilesPathTex + "/eps";
const std::string globals::outputFilesPathEpsSplit = globals::outputFilesPathEps + "/split";
const std::string globals::outputFilesPathPdf = globals::outputFilesPathTex + "/pdf";
const std::string globals::outputFilesPathPdfSplit = globals::outputFilesPathPdf + "/split";


std::string globals::GetOutputPath(){
   std::string path = globals::outputFilesPath;
   if     (globals::globalUnfoldingMethod == globals::SVD)
      path += "/SVD";
   else if(globals::globalUnfoldingMethod == globals::BAYESIAN)
      path += "/Bayesian";
   else if(globals::globalUnfoldingMethod == globals::BINBYBIN)
      path += "/BinByBin";
   else if(globals::globalUnfoldingMethod == globals::INVERSION)
      path += "/Inversion";
   return path;
}

std::string globals::GetLatexPath(){
   return GetOutputPath() + "/tex";
}

std::string globals::GetLatexEpsPath(){
   return GetLatexPath() + "/eps";
}

std::string globals::GetLatexEpsSplitPath(){
   return GetLatexEpsPath() + "/Split";
}

std::string globals::GetLatexPdfPath(){
   return GetLatexPath() + "/pdf";
}

std::string globals::GetLatexPdfSplitPath(){
   return GetLatexPdfPath() + "/Split";
}


std::string globals::GetCombinationTextFilePath(){
   return GetOutputPath() + "/TextFiles";
}



const std::string globals::KidonakisNNLO_filenameA = 
      globals::dataFilesPath + "/pttopnnloapprox7lhc173m.root";
const std::string globals::KidonakisNNLO_histonameA = "h_theory";
const std::string globals::KidonakisNNLO_filenameB = 
      globals::dataFilesPath + "/pttopnnloapprox7lhc173mT.root";
const std::string globals::KidonakisNNLO_histonameB = "h_theory";

TH1* globals::LumiEfficiencyCorrectedSpectra(TH1 const * const unfolded,
                                             const std::string sampleName, 
                                             const std::string efficiencyFilename, 
                                             const std::string efficiencyHistoname){
   
   std::cout << "[globals::LumiEfficiencyCorrectedSpectra] histo file: " << efficiencyFilename << std::endl;
   std::cout << "[globals::LumiEfficiencyCorrectedSpectra] histo name: " << efficiencyHistoname << std::endl;
   
   // for some plots no efficiency correction is needed
   TH1* efficiencyHisto = 0;
   if(efficiencyFilename.size() > 0 && efficiencyHistoname.size() > 0){
      efficiencyHisto = GetEfficiencyHistogram(efficiencyFilename,efficiencyHistoname);
   }
   
   TH1* hist = LumiEfficiencyCorrectedSpectra(unfolded,sampleName,efficiencyHisto);
   
   delete efficiencyHisto;
   efficiencyHisto = 0;
   
   return hist;
   
}



TH1* globals::LumiEfficiencyCorrectedSpectra(TH1 const * const unfolded,
                                             const std::string sampleName, 
                                             TH1 const * const efficiencyHisto){
   
   float scalefactor=1.;
   
   // get right labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string UkinVarLabel = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   
   if (sampleName.find("TopLepHad") != std::string::npos)
      scalefactor=0.5;    //in one event there are two top quarks. Both leptonic and hadronic top Pt were filled.
   
   
   
   TH1* corrected = (TH1*)unfolded->Clone(std::string(std::string(unfolded->GetName()) + "Clone").c_str());
   
   // for some plots no efficiency correction is needed
   if(efficiencyHisto){
   
      //dividing by efficiency
      corrected->Scale(1.);
      corrected->Divide(efficiencyHisto);
      // scale by 100 since efficiency is in percent
      corrected->Scale(100.);
      
   }
   
   
   //scale by luminosity, the luminosity error is also added!!!
   corrected->Scale(1./LUMI);
   
//    for (unsigned int i=1;i<corrected->GetNbinsX()+1;++i){
//       corrected->SetBinError(i,corrected->GetBinError(i)/LUMI);
//       //corrected->SetBinError(i,sqrt(corrected->GetBinError(i)*corrected->GetBinError(i)+corrected->GetBinContent(i)*corrected->GetBinContent(i)*LUMI_FRACTIONAL_ERROR*LUMI_FRACTIONAL_ERROR)/LUMI);
//       corrected->SetBinContent(i,corrected->GetBinContent(i)/LUMI);
//    }
   
   corrected->Scale(scalefactor);
   
  //dividing by bin width 
   for (Int_t i=1;i<corrected->GetNbinsX()+1;++i){
      corrected->SetBinContent(i,corrected->GetBinContent(i)/corrected->GetBinWidth(i));
      corrected->SetBinError(i,corrected->GetBinError(i)/corrected->GetBinWidth(i));
   }
  
   corrected->Scale(1.);
   corrected->GetYaxis()->SetTitle(DkinVarLabel.c_str());
   corrected->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   corrected->SetTitle("");
   
   delete kinVar;
   kinVar = 0;
   
   return corrected;
}

TH1* globals::LumiEfficiencyCorrectedSpectra(TH1 const * const unfolded,
                                             const std::string sampleName,
                                             const double weight,
                                             const bool doLumiCorrection,
                                             const bool doBinWidthCorrection)
{
   
   float scalefactor=1.;
   
   // get right labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string UkinVarLabel = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   
   if (sampleName.find("TopLepHad") != std::string::npos)
      scalefactor=0.5;    //in one event there are two top quarks. Both leptonic and hadronic top Pt were filled.
   
   
   
   TH1* corrected = (TH1*)unfolded->Clone(std::string(std::string(unfolded->GetName()) + "Clone").c_str());
   corrected->SetDirectory(false);
   
   // scale by lumi weight
   corrected->Scale(weight);
   
   //scale by luminosity, the luminosity error is also added!!! 
   for (unsigned int i=1;i<=corrected->GetNbinsX() && doLumiCorrection;++i){
      corrected->SetBinError(i,corrected->GetBinError(i)/LUMI);
//       corrected->SetBinError(i,sqrt(corrected->GetBinError(i)*corrected->GetBinError(i)+corrected->GetBinContent(i)*corrected->GetBinContent(i)*LUMI_FRACTIONAL_ERROR*LUMI_FRACTIONAL_ERROR)/LUMI);
      corrected->SetBinContent(i,corrected->GetBinContent(i)/LUMI);
   }
   corrected->Scale(scalefactor);
   
  //dividing by bin width
   for (Int_t i=1;i<=corrected->GetNbinsX() && doBinWidthCorrection;++i){
      corrected->SetBinContent(i,corrected->GetBinContent(i)/corrected->GetBinWidth(i));
      corrected->SetBinError(i,corrected->GetBinError(i)/corrected->GetBinWidth(i));
   }
  
   corrected->Scale(1.);
   corrected->GetYaxis()->SetTitle(DkinVarLabel.c_str());
   corrected->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   corrected->SetTitle("");
   
   delete kinVar;
   kinVar = 0;
   
   return corrected;
}


TH1* globals::GetEfficiencyHistogram(const std::string filename,const std::string histoname){
   
   // retrieve efficiency plot
   TFile *efficiencyFile = 0;
   for(unsigned int i=0;i<5 && !efficiencyFile;++i){
      efficiencyFile = TFile::Open(filename.c_str());
      if(!efficiencyFile){
         std::cerr << "[globals::GetEfficiencyHistogram] ERROR file not opened: " << filename << std::endl;
         std::cerr << "[globals::GetEfficiencyHistogram] Trying again.\n";
      }
   }
   if(!efficiencyFile){
      std::cerr << "[globals::GetEfficiencyHistogram] ERROR file not opened: " << filename << std::endl;
      return (TH1*)0;
   }
   
   TH1* efficiencyHisto = (TH1*)efficiencyFile->Get(histoname.c_str());
   if(!efficiencyHisto){
      std::cerr << "[globals::GetEfficiencyHistogram] ERROR could not retreive " << histoname << " from file " << filename << "\n";
      return (TH1*)0;
   }
   efficiencyHisto->SetDirectory(false);
   
   efficiencyFile->Close();
   delete efficiencyFile;
   efficiencyFile = 0;
   
   
   return efficiencyHisto;
   
}

TGraphAsymmErrors* globals::DivideAsymmErrors(TGraphAsymmErrors const * const numerator,
                                              TGraphAsymmErrors const * const denominator)
{
   
   if(numerator->GetN() != denominator->GetN()){
      std::cerr << "[globals::DivideAsymmErrors] plots do not have the same number of data points, " << numerator->GetName() << " has " << numerator->GetN() << " != " << numerator->GetName() << " has " << denominator->GetN() << ".\n";
      return (TGraphAsymmErrors*)0;
   }
   
   TGraphAsymmErrors* output = new TGraphAsymmErrors;
   std::string gname = "ratio_";
   gname += numerator->GetName();
   gname += "_";
   gname += denominator->GetName();
   output->SetName(gname.c_str());
   
   for(unsigned int i=0;i<numerator->GetN();++i){
      double num     = numerator->GetY()[i];
      double numUp   = numerator->GetEYhigh()[i];
      double numDown = numerator->GetEYlow()[i];
      
      double den     = denominator->GetY()[i];
      double denUp   = denominator->GetEYhigh()[i];
      double denDown = denominator->GetEYlow()[i];
      
      double ratio   = (den == 0) ? 0 : ( num / den );
      double up      = (den == 0) ? 0 : sqrt( pow(numUp/den,2) + pow(num*denUp*pow(den,-2),2) );
      double down    = (den == 0) ? 0 : sqrt( pow(numDown/den,2) + pow(num*denDown*pow(den,-2),2) );
      
      output->SetPoint(i,numerator->GetX()[i],ratio);
      output->SetPointError(i,numerator->GetEXlow()[i],numerator->GetEXhigh()[i],down,up);
      
   }
   
   return output;
}

// uses relative errors only
TGraphAsymmErrors* globals::DivideAsymmErrors2(TGraphAsymmErrors const * const numerator,
                                              TGraphAsymmErrors const * const denominator)
{
   
   if(numerator->GetN() != denominator->GetN()){
      std::cerr << "[globals::DivideAsymmErrors] plots do not have the same number of data points, " << numerator->GetName() << " has " << numerator->GetN() << " != " << numerator->GetName() << " has " << denominator->GetN() << ".\n";
      return (TGraphAsymmErrors*)0;
   }
   
   TGraphAsymmErrors* output = new TGraphAsymmErrors;
   std::string gname = "ratio_";
   gname += numerator->GetName();
   gname += "_";
   gname += denominator->GetName();
   output->SetName(gname.c_str());
   
   for(unsigned int i=0;i<numerator->GetN();++i){
      double num     = numerator->GetY()[i];
      double numUp   = numerator->GetEYhigh()[i];
      double numDown = numerator->GetEYlow()[i];
      
      double den     = denominator->GetY()[i];
      double denUp   = denominator->GetEYhigh()[i];
      double denDown = denominator->GetEYlow()[i];
      
      double ratio   = (den == 0) ? 0 : ( num     / den );
      double up      = (den == 0) ? 0 : ( numDown / den );
      double down    = (den == 0) ? 0 : ( numUp   / den );
      
      output->SetPoint(i,numerator->GetX()[i],ratio);
      output->SetPointError(i,numerator->GetEXlow()[i],numerator->GetEXhigh()[i],down,up);
      
   }
   
   return output;
}


TGraphAsymmErrors* globals::DivideAsymmErrors(TGraphAsymmErrors* numerator,TH1* denominator){
   
   if(numerator->GetN() != denominator->GetNbinsX()){
      std::cerr << "[globals::DivideAsymmErrors] plots do not have the same number of data points, " << numerator->GetName() << " has " << numerator->GetN() << " != " << numerator->GetName() << " has " << denominator->GetNbinsX() << ".\n";
      return (TGraphAsymmErrors*)0;
   }
   
   TGraphAsymmErrors* output = new TGraphAsymmErrors;
   
   
   for(unsigned int i=0;i<numerator->GetN();++i){
      
      double num     = numerator->GetY()[i];
      double numUp   = numerator->GetEYhigh()[i];
      double numDown = numerator->GetEYlow()[i];
      
      double den     = denominator->GetBinContent(i+1);
      double denUp   = denominator->GetBinError(i+1);
      double denDown = denominator->GetBinError(i+1);
      
      double ratio   = (den == 0) ? 0 : ( num / den );
      double up      = (den == 0) ? 0 : sqrt( pow(numUp/den,2) + pow(num*denUp*pow(den,-2),2) );
      double down    = (den == 0) ? 0 : sqrt( pow(numDown/den,2) + pow(num*denDown*pow(den,-2),2) );
      
      output->SetPoint(i,numerator->GetX()[i],ratio);
      output->SetPointError(i,numerator->GetEXlow()[i],numerator->GetEXhigh()[i],down,up);
      
   }
   
   return output;
}

bool globals::IsFile(const std::string path){
   
   struct stat status;
   if(stat(path.c_str(),&status) == -1){
      std::cerr << "[globals::IsFile] could not stat path: " << path << "\n";
      if(errno == ENOENT || errno == ENOTDIR)
         std::cerr << "[globals::IsFile] path does not exist.\n";
      return false;
   }
   
   if(S_ISREG(status.st_mode)){
      return true;
   }
   
   return false;
   
}

bool globals::IsFolder(const std::string path){
   
   struct stat status;
   if(stat(path.c_str(),&status) == -1){
      std::cerr << "[globals::IsFolder] could not stat path: " << path << "\n";
      if(errno == ENOENT || errno == ENOTDIR)
         std::cerr << "[globals::IsFolder] path does not exist.\n";
      return false;
   }
   
   if(S_ISDIR(status.st_mode)){
      return true;
   }
   
   return false;
   
}

bool globals::CreatePath(const std::string path){
   
   if(mkdir(path.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0){
      if(!set_chmod(path)){
         std::cout << "[globals::CreatePath] cannot chmod path: " << path << "\n";
         return false;
      }
      return true;
   }
   else{
      switch(errno){
      case EACCES:
         std::cerr << "[globals::CreatePath] mkdir reports permissions problem in path, cannot write to path\n";
         break;
      case EEXIST:
         std::cout << "[globals::CreatePath] mkdir reports path already exists. path: " << path << "\n";
         return true;
         break;
      case ENOENT:
         std::cout << "[globals::CreatePath] mkdir reports component of the path prefix does not name an existing directory or path is empty string.\n";
         if(path.size() > 0){
            std::string subpath = path.substr(0,path.rfind("/"));
            std::cout << "[globals::CreatePath] try to create subpath: " << subpath << "\n";
            if(CreatePath(subpath)){
               std::cout << "[globals::CreatePath] try to create path: " << path << "\n";
               if(CreatePath(path)){
                  return true;
               }
            }
         }
         break;
      case ENOSPC:
         std::cerr << "[globals::CreatePath] mkdir reports no space to create path on disk.\n";
         break;
      case ENOTDIR:
         std::cerr << "[globals::CreatePath] mkdir reports a component of the path prefix is not a directory.\n";
         break;
      case EROFS:
         std::cerr << "[globals::CreatePath] mkdir reports the  parent directory resides on a read-only file system.\n";
         break;
      default:
         std::cerr << "[globals::CreatePath] mkdir error not recognized.\n";
      }
      std::cerr << "[globals::CreatePath] path: " << path << "\n";
      return false;
   }
   
   return false;
}

std::string globals::GetPath(const std::string pathWithFilename){
   return pathWithFilename.substr(0,pathWithFilename.rfind("/"));
}

bool globals::set_chmod(const std::string path){
   if(IsFolder(path)){
      if(chmod(path.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0)
         return true;
   }
   
   if(IsFile(path)){
      if(chmod(path.c_str(),S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH ) == 0)
         return true;
   }
   
   return false;
   
}

std::vector<double> globals::ReadPdfSyst(std::string sampleName, TopDecayChannel channel, std::string filename){
   std::vector<double> output;
   
   std::string outputPath = globals::outputFilesPath;
   std::string fullPath = outputPath.append("/");
   fullPath.append(filename);
   ifstream infile(fullPath.c_str());
   if(!infile.is_open()){
      std::cout << "[globals::ReadPdfSyst] ERROR, could not open file: " << fullPath << std::endl;
      return output;
   }
   
   std::string previousName;
   std::string line;
   std::string variables[5] = {"ma", "y_", "pt", "ha", "le"};
   
   
   std::string hadOrLep;
   if(sampleName.find("Top1") != std::string::npos)
      hadOrLep = "leptonic";
   else if(sampleName.find("Top2") != std::string::npos)
      hadOrLep = "hadronic";
   
   while(!infile.eof()){
      getline(infile,line);
      for(int i=0; i<5; i++){
         if (line.substr(0,2)==variables[i])
            previousName=variables[i];
      }
      std::string listOfErrors;
      if(previousName==hadOrLep.substr(0,2) && line.substr(0,2)==channel.GetChannelString()){
         listOfErrors=line.substr(8,line.size()-8);
         std::istringstream iss(listOfErrors);
         std::copy(std::istream_iterator<float>(iss),
                   std::istream_iterator<float>(),
                   back_inserter(output));
      }
   } // end reading file
   
   infile.close();
   return output;
}


void globals::PrettyLaTeXPrintMatrix(TMatrixD const * const Cov)
{
   if (not Cov)
      return;
  
   int nr =Cov->GetNrows();
   int nc =Cov->GetNcols();
   std::cout << "\\begin{tabular}{";
   for (int i = 0; i < nc; ++i) 
      std::cout << "l";
   std::cout << "}" << std::endl;
   for (int i = 0; i < nr; ++i) {
      for (int j = 0; j < nc; ++j) {
         std::cout << " " << (*Cov)[i][j];
         if (j < nr-1)
            std::cout << " & ";
      }
      std::cout << " \\\\" << std::endl;
   }
   std::cout << "\\end{tabular}" << std::endl;
   return;

}

// ________________________________________________________________

TMatrixD* globals::MakeVector(TH1 const * const hist,const bool MakeRow)
{

   if (not hist) {
      std::cerr << "MakeVector: Error getting pointer!" << std::endl;
      return 0;
   }

   int n = hist -> GetXaxis() -> GetNbins();
  //  TVectorD *vec = new TVectorD(n);
   TMatrixD *vec = 0;
   if (MakeRow)
      vec = new TMatrixD(n, 1);
   else
      vec = new TMatrixD(1, n);
   for (int i = 0; i < n; ++i) {
      if (MakeRow)
         (*vec)[i][0] = hist -> GetBinContent(i+1);
      else
         (*vec)[0][i] = hist -> GetBinContent(i+1);
   }

   return vec;

}

// ________________________________________________________________

TMatrixD* globals::MakeVector(TGraphAsymmErrors const * const graph,const bool MakeRow)
{

   if (not graph) {
      std::cerr << "MakeVector: Error getting pointer!" << std::endl;
      return 0;
   }

   int n = graph -> GetN();
  //  TVectorD *vec = new TVectorD(n);
   TMatrixD *vec = 0;
   if (MakeRow)
      vec = new TMatrixD(n, 1);
   else
      vec = new TMatrixD(1, n);
   for (int i = 0; i < n; ++i) {
      if (MakeRow)
         (*vec)[i][0] = graph -> GetY()[i];
      else
         (*vec)[0][i] = graph -> GetY()[i];
   }

   return vec;

}


// ________________________________________________________________

double globals::GetChisquared(TGraphAsymmErrors const * const data,
                     TH1 const * const theory,
                     TMatrixD const * const Cov,
                     const bool UseTheoryErrors)
{

   if (not data or not theory or not Cov) {
      std::cerr << "[globals::GetChisquared] Error getting pointers!" << data << " " << theory << " " << Cov << std::endl;
      return -999.;
   }

   // computes generalized chi2 as
   // (data-theory) Cov^{-1} (data-theory)^2,
   // where the covariance is the covariance matrix from data.
   // Option is to add to the Cov the errors from the theory.

   TMatrixD TheoryCov(Cov->GetNrows(), Cov->GetNcols());
   if (false){//UseTheoryErrors) {
      for (int ii = 0; ii < Cov->GetNrows(); ++ii)
         TheoryCov[ii][ii] = pow(theory -> GetBinError(ii+1), 2);
   }
   
   TMatrixD Inv = *Cov + TheoryCov;
   Inv.Invert();

   // row and column vector representations of the histogram
   TMatrixD *datar = MakeVector(data, true);
   TMatrixD *theoryr = MakeVector(theory, true);
   TMatrixD *datac = MakeVector(data, false);
   TMatrixD *theoryc = MakeVector(theory, false);
    
   // the real meat;-) 
   TMatrixD chi2M = ((*datac)-(*theoryc)) * Inv * ((*datar)-(*theoryr));
   
   delete datar;datar = 0;
   delete datac;datac = 0;
   delete theoryr;theoryr = 0;
   delete theoryc;theoryc = 0;
   
   
   //   std::cout << "Returning the chi2..." << std::endl;
   return chi2M[0][0];

}

// ________________________________________________________________

double globals::GetChisquared(TGraphAsymmErrors const * const data,
                     TGraphAsymmErrors const * const theory,
                     TMatrixD const * const Cov,
                     const bool UseTheoryErrors)
{

   if (not data or not theory or not Cov) {
      std::cerr << "[globals::GetChisquared] Error getting pointers!" << data << " " << theory << " " << Cov << std::endl;
      return -999.;
   }

  // computes generalized chi2 as
  // (data-theory) Cov^{-1} (data-theory)^2,
  // where the covariance is the covariance matrix from data.
  // Option is to add to the Cov the errors from the theory.

   TMatrixD TheoryCov(Cov->GetNrows(), Cov->GetNcols());
   if (UseTheoryErrors) {
      for (int ii = 0; ii < Cov->GetNrows(); ++ii){
         double combined_error = (theory->GetEYhigh()[ii] + theory->GetEYlow()[ii])/2.;
         TheoryCov[ii][ii] = pow(combined_error, 2);
      }
   }
  
   TMatrixD Inv = *Cov + TheoryCov;
   Inv.Invert();

  // row and column vector representations of the histogram
   TMatrixD *datar = MakeVector(data, true);
   TMatrixD *theoryr = MakeVector(theory, true);
   TMatrixD *datac = MakeVector(data, false);
   TMatrixD *theoryc = MakeVector(theory, false);
   
   
  // the real meat;-) 
   TMatrixD chi2M = ((*datac)-(*theoryc)) * Inv * ((*datar)-(*theoryr));
   
  //   std::cout << "Returning the chi2..." << std::endl;
   return chi2M[0][0];

}


