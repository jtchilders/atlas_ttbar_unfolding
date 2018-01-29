#ifndef TTBARUNFOLDANALYSIS_H
#define TTBARUNFOLDANALYSIS_H


#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>


#include "ttbarUnfold.h"

class TH1D;
class TH1;

class FileList;
class FileHandler;
class SystematicSample;

class ttbarUnfoldAnalysis{
public:
   ttbarUnfoldAnalysis(void);
   ~ttbarUnfoldAnalysis(void);
   
   bool RunToyUnfolding(
                        const std::string mc_filename,
                        const std::string data_filename,
                        const SystematicSample syst,
                        const std::string sampleName,
                        const std::string measuredHistoName,
                        const ttbarUnfold::UnfoldMethod unfoldMethod,
                        const unsigned int regValue,
                        const unsigned int iterations,
                        const unsigned int nToys,
                        std::vector<TH1D*>* meanOverTruth = 0,
                        std::vector<TH1D*>* rmsOverTruth  = 0,
                        std::vector<TH1D*>* compRms       = 0);
   
   bool RunToyUnfoldingForRegScan(
                           const std::string mc_filename,
                           const std::string data_filename,
                           const SystematicSample syst,
                           const std::string sampleName,
                           const std::string measuredHistoName,
                           const ttbarUnfold::UnfoldMethod unfoldMethod,
                           const unsigned int regValue,
                           const unsigned int nToys);
   
   bool RunOverEnsembleDataSets(
                           FileList* mc, 
                           FileList* data, 
                           const std::string sampleName, 
                           const unsigned int regValue);
   bool OptimizeRegValueWithEnsembles(
                           FileList* mc, 
                           FileList* data, 
                           const std::string sampleName, 
                           const unsigned int mregValue);
   bool ComparisonEnsembleDataSets(const std::string  mc,
                           const std::string  data,
                           FileList* sample,
                           const std::string flag,
                           const unsigned int regValue);
   
   bool RunUnfoldingWithDifferentMethods(
                           const std::string mc,
                           const std::string data,
                           const std::string sampleName,
                           const std::string measuredHistoName,
                           const unsigned int nToys,
                           const unsigned int SvdRegValue,
                           const unsigned int BayesIterations);
   
   bool RunMigrationSmearedToys(const std::string mc,
                           const std::string data,
                           const std::string sampleName,
                           const unsigned int nToys,
                           const unsigned int svdRegValue,
                           const unsigned int bayesIterator,
                           const std::string measuredHistoName,
                           const ttbarUnfold::UnfoldMethod unfoldMethod);
   
   void SetOutputFilename(const std::string name){m_outputFilename = name;};
   std::string GetOutputFilename(void){return m_outputFilename;};
   
   
   static std::string GetHistogramNameToys(
                           const std::string channel,
                           const std::string sampleName,
                           const ttbarUnfold::UnfoldMethod unfoldMethod,
                           const int regValue,
                           const int nToys,
                           const std::string additional);
   
   static std::string GetCanvasNameToys(
                           const std::string channel,
                           const std::string sampleName,
                           const ttbarUnfold::UnfoldMethod unfoldMethod,
                           const int regValue,
                           const int nToys,
                           const std::string additional);
   
   static std::string GetHistogramNameEnsembles(
                           const std::string channel,
                           const std::string sampleName,
                           const ttbarUnfold::UnfoldMethod unfoldMethod,
                           const int regValue,
                           const int ensembleNumber,
                           const std::string additional);
   
   static std::string GetCanvasNameEnsembles(
                           const std::string channel,
                           const std::string sampleName,
                           const ttbarUnfold::UnfoldMethod unfoldMethod,
                           const int regValue,
                           const int ensembleNumber,
                           const std::string additional);
   
   void SetEfficiencyFilename(const std::string name){m_efficiencyFilename = name;};
   std::string GetEfficiencyFilename(void){return m_efficiencyFilename;};
   
   void SetEfficiencyHistoName(const std::string name){m_efficiencyHist = name;};
   std::string GetEfficiencyHistoName(void){return m_efficiencyHist;};
   
private:
     
   TH1D* m_ensembleDeviation;
   TH1D* m_ensembleDeviationRMS;
   TH1D* m_ensembleDeviationPerBin;
   
   TH1D* m_rmsResidualsOverTruth;
   TH1D* m_meanResidualsOverTruth;
   TH1D* m_rmsComparison;
   
   FileHandler* m_rootFile;
   std::string m_outputFilename;
   
   std::string m_efficiencyFilename;
   std::string m_efficiencyHist;
   
   
   void psmerge(const std::string outFilename,const std::string file1, const std::string file2);
   void rmfile(const std::string filename);
   void mvfile(const std::string from,const std::string to);
   
   
};


#endif
