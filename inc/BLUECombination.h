#ifndef BLUECOMBINATION_H
#define BLUECOMBINATION_H

//////////////////////////////////////////////
// A class which uses the BLUE Method:
// 



#include <vector>
#include <string>
#include <map>

#include "TMatrixD.h"

#include "SystematicBinList.h"

class SystematicList;
class TH1D;

class BLUECombination{
public:
   
   /// specifies how the statistical errors should be treated
   /// INDEPENDENT: Statistical uncertainties are added in quadrature
   /// CORRELATED: Statistical uncertainties are treated in the BLUE
   ///             combination as correlated uncertainties
   /// UNCORRELATED: Statistical uncertainties are treated in the BLUE
   ///             combination as uncorrelated uncertainties
   /// Default setting is UNCORRELATED
   enum StatTreatment {INDEPENDENT,CORRELATED,UNCORRELATED};
   
   
   /// constructor that takes two systematic lists and combines them,
   /// treating the correlated systematics given in the vector<string> list
   BLUECombination(SystematicList* listA,SystematicList* listB,
                   std::vector<std::string>& correlatedSystematics,
                   const StatTreatment statTreatment = UNCORRELATED);
   
   /// constructor that takes two text files with the line by line format:
   /// systematicA <bin number> <bin low edge> <bin high edge> <bin contents> <stat uncertainty> <syst uncertainty>
   /// systematicA <next bin number> <bin low edge> <bin high edge> <bin contents> <stat uncertainty> <syst uncertainty>
   /// ... and more bins... all systematics are contained in one text file, just the name changes
   /// systematicB <bin number> <bin low edge> <bin high edge> <bin contents> <stat uncertainty> <syst uncertainty>
   /// systematicB <next bin number> <bin low edge> <bin high edge> <bin contents> <stat uncertainty> <syst uncertainty>
   /// ... and so on...
   /// The uncertainties are then combined in each bin,
   /// treating the correlated systematics given in the vector<string> list
   BLUECombination(const std::string filenameA,const std::string filenameB,
                   std::vector<std::string>& correlatedSystematics,
                   const StatTreatment statTreatment = UNCORRELATED);
   ~BLUECombination();
   
   // retrieve the combined covariance matrix
   // indices: vector[bin number from 0]
   const std::vector<TMatrixD*>& CovMatrix(void) const {return m_combinedCovarianceMatrices;};
   
   // retrieve combined uncertainties
   const SystematicBinList& combined() const {return m_combined;};
   
   static const std::string STATISTICS;
   
   void CalculateCovarianceMatrix();
   // it is possible to pass new data to the combine function
   // in case you are using toy MC and the Uncertainties do not change.
   // This should make things slightly faster
   SystematicBinList& Combine(
         const std::vector<double>& dataA = std::vector<double>::vector<double>(),
         const std::vector<double>& dataB = std::vector<double>::vector<double>());
   
   TH1D* GetHistoWithSyst();
   TH1D* GetHistoWithStat();
private:
   void Init();
   
   SystematicBinList m_combined;
   TH1D* m_histoWithSyst;
   TH1D* m_histoWithStat;
   
   SystematicList* m_listA;
   SystematicList* m_listB;
   bool m_delete_lists;
   std::vector<std::string>& m_correlatedSystematics;
   
   StatTreatment m_statTreatment;
   
   /// combined covariance matrix for each bin
   std::vector<TMatrixD*> m_combinedCovarianceMatrices;
   
   /// covariance matrix for each systematic for each bin
   std::map<std::string,std::vector<TMatrixD*> > m_covarianceMatrices;
   
   void LoopBinsAndFillCovMat(
                              SystematicBinList const * const listA,
                              SystematicBinList const * const listB,
                              const bool correlated);
   TMatrixD CovarianceMatrix(const double systA, double systB,const bool correlated);
   
   void LoopBinsAndCalcCombErrors(const std::vector<double>& dataA,
                                  const std::vector<double>& dataB);
   
   bool SystematicRepeated  (const std::vector<std::string>& list,const std::string& syst);
   bool SystematicCorrelated(const std::vector<std::string>& list,const std::string& syst);
   
   void AddStatsToLists();
   
   void FillSystematicContributions(const TMatrixD& lambda,
                                    const TMatrixD& lambdaT,
                                    const double& combinedUncertainty,
                                    const unsigned int& binNumber,
                                    SystematicBin& systBin);
};

#endif
