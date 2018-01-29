#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <errno.h> 
#include <vector>
#include <iostream>

#include "TFile.h"
#include "TMatrixD.h"

class TH1;
class TGraphAsymmErrors;

class TopDecayChannel;

class globals{
private:
   
   
public:
   
   enum unf_methods {SVD,BAYESIAN,BINBYBIN,INVERSION};
   
   static double GetLumi(void){return LUMI;};
   static double GetLumiFractionalError(void){return LUMI_FRACTIONAL_ERROR;};
   
   static const double LUMI;
   static const double LUMI_FRACTIONAL_ERROR;
   static const double BR;
   static const double BR_ALPGEN;
   static const double BR_MCATNLO;
   static const double BR_POWHEG;
   static const double MCATNLO_CORRECTION;
   
   static const bool DATA_ON_TOP;
   
   static void PrettyLaTeXPrintMatrix(TMatrixD const * const Cov);
   static TMatrixD* MakeVector(TH1 const * const hist,const bool MakeRow);
   static TMatrixD* MakeVector(TGraphAsymmErrors const * const graph,const bool MakeRow);
   static double GetChisquared(TGraphAsymmErrors const * const data,
                               TH1 const * const theory,
                               TMatrixD const * const Cov,
                               const bool UseTheoryErrors);
   static double GetChisquared(TGraphAsymmErrors const * const data,
                               TGraphAsymmErrors const * const theory,
                               TMatrixD const * const Cov,
                               const bool UseTheoryErrors);
   
   static TH1* LumiEfficiencyCorrectedSpectra(TH1 const * const unfolded,
                                              const std::string sampleName, 
                                              TH1 const * const efficiencyHisto);
   
   static TH1* LumiEfficiencyCorrectedSpectra(TH1 const * const unfolded,
                                              const std::string sampleName,
                                              const double weight,
                                              const bool doLumiCorrection = true,
                                              const bool doBinWidthCorrection = true
                                             );
   
   static TH1* LumiEfficiencyCorrectedSpectra(TH1 const * const unfolded,
                                              const std::string sampleName, 
                                              const std::string efficiencyFilename = "", 
                                              const std::string efficiencyHistoname = "");
   
   static TH1* GetEfficiencyHistogram(const std::string filename,
                                      const std::string histoname);
   
   static TGraphAsymmErrors* DivideAsymmErrors(TGraphAsymmErrors const * const numerator,
                                               TGraphAsymmErrors const * const denominator);
   static TGraphAsymmErrors* DivideAsymmErrors2(TGraphAsymmErrors const * const numerator,
                                                TGraphAsymmErrors const * const denominator);
   static TGraphAsymmErrors* DivideAsymmErrors(TGraphAsymmErrors* numerator,TH1* denominator);
   
   template<class T>
   static T* GetFromFile(const std::string error_include,
                           TFile* const file,
                           const std::string histo_name)
   {
      T* t = (T*)file->Get(histo_name.c_str());
      if(!t){
         std::cerr << error_include << " ERROR retrieving histogram, " << histo_name << ", from file, " << file->GetName() << "\n";
         return (T*)0;
      }
      return t;
   }
   
   static TFile* OpenFile(const std::string error_include,
                         const std::string filename)
   {
      TFile* f = TFile::Open(filename.c_str());
      if(!f || !f->IsOpen()){
         std::cerr << error_include << " ERROR opening file, " << filename << "\n";
         return (TFile*)0;
      }
      return f;
   }
   
   static bool IsFile(const std::string path);
   static bool IsFolder(const std::string path);
   static bool CreatePath(const std::string path);
   static std::string GetPath(const std::string pathWithFilename);
   static bool set_chmod(const std::string path);
   
   static std::vector<double> ReadPdfSyst(std::string sample, TopDecayChannel channel,
   std::string filename = "pdf_syst.txt");
   
   static const std::string ejets;
   static const std::string mujets;
   static const std::string mujetsLatex;
   static const std::string ljets;
   static const std::string btag1;
   static const std::string btag2;
   static const std::string btag0;
   static const std::string btag1Latex;
   static const std::string btag2Latex;
   static const std::string btag0Latex;
   static const std::string leptonicTop;
   static const std::string hadronicTop;
   static const std::string lepHadTop;
   
   static const std::string ptt;
   static const std::string mttbar;
   static const std::string yttbar;
   
   static const std::string dataFilesPath;
   static const std::string outputFilesPath;
   
   static unf_methods globalUnfoldingMethod;
   
   static std::string GetOutputPath();
   
   static std::string GetLatexPath();
   static std::string GetLatexEpsPath();
   static std::string GetLatexEpsSplitPath();
   static std::string GetLatexPdfPath();
   static std::string GetLatexPdfSplitPath();
   
   static std::string GetCombinationTextFilePath();
   
   static const std::string outputFilesPathTex;
   static const std::string outputFilesPathEps;
   static const std::string outputFilesPathEpsSplit;
   static const std::string outputFilesPathPdf;
   static const std::string outputFilesPathPdfSplit;
   
   // Kidonakis NNLO files
   static const std::string KidonakisNNLO_filenameA;
   static const std::string KidonakisNNLO_histonameA;
   static const std::string KidonakisNNLO_filenameB;
   static const std::string KidonakisNNLO_histonameB;
   
   
   static const bool useGausSmearing;
};


#endif

