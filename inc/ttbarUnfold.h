#ifndef TTBARUNFOLD_H
#define TTBARUNFOLD_H

#include <string>

#include "TH1.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TSVDUnfold.h"

#include "RooUnfoldSvd.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldBinByBin.h"
#include "RooUnfoldInvert.h"
#include "RooUnfoldErrors.h"

class RooUnfoldResponse;
class RooUnfoldErrors;
class TopDecayChannel;

class ttbarUnfold{
public:
   
   enum UnfoldMethod{
      Bayes,
      SVD,
      BinByBin,
      Inversion,
      NUM_OF_METHODS
   };
   
   ttbarUnfold(const std::string dataFilename,
               const std::string mcFilename,
               const std::string sampleName,
               TH1 * const efficiencyHisto = 0);
   ~ttbarUnfold(void);
   
   bool doUnfolding(void);
   bool runToys(void);
   
   void SetUnfoldErrorTreatment(RooUnfold::ErrorTreatment treatment){m_unfoldErrorTreatment = treatment;};
   RooUnfold::ErrorTreatment GetUnfoldErrorTreatment(void){return m_unfoldErrorTreatment;};
   
   void SetRegularization(unsigned int reg){m_reg = reg;};
   unsigned int GetRegularization(void){return m_reg;};
   
   void SetToysForUnfolding(unsigned int ntoys){m_ntoys_unfold = ntoys;};
   unsigned int GetToysForUnfolding(void){return m_ntoys_unfold;};
   void SetToysForErrors(unsigned int ntoys){m_ntoys_errors = ntoys;};
   unsigned int GetToysForErrors(void){return m_ntoys_errors;};

   void DrawMigrationMatrix(const std::string sampleName);
   void DrawPtSpectra(const std::string sampleName);
   void DrawPtSpectraScale(const std::string sampleName);
   void DrawDeviation();
   void DrawDistributionD();
   void DrawDeviationReco();
   void Transpose(TH2 *histo);
   
   TH1* GetMcTruth(void){if(m_mc_truth) return m_mc_truth; else return 0;};
   TH1* GetMcTruthNorm(void){if(m_mc_truthNorm) return m_mc_truthNorm; else return 0;};
   TH1* GetMcTruthScaled(void){if(m_mc_truthScaled) return m_mc_truthScaled; else return 0;};
   TH1* GetMcTruthDiffxs(void){if(m_mc_truth_diffxs) return m_mc_truth_diffxs; else return 0;};
   TH1* GetMcTruthDiffxsNorm(void){if(m_mc_truth_diffxsNorm) return m_mc_truth_diffxsNorm; else return 0;};
   TH1* GetMcReco(void){if(m_mc_reco) return m_mc_reco; else return 0;};
   TH1* GetMcRecoNorm(void){if(m_mc_recoNorm) return m_mc_recoNorm; else return 0;};
   TH1* GetMcRecoScaled(void){if(m_mc_recoScaled) return m_mc_recoScaled; else return 0;};
   TH1* GetMcRecoDiffxs(void){if(m_mc_reco_diffxs) return m_mc_reco_diffxs; else return 0;};
   TH1* GetMcRecoDiffxsNorm(void){if(m_mc_reco_diffxsNorm) return m_mc_reco_diffxsNorm; else return 0;};
   
   TH1* GetDataMeasuredMinusBkgd(void){if(m_data_measuredMinusBkgd) return m_data_measuredMinusBkgd; else return 0;};
   TH1* GetDataMeasuredMinusBkgdNorm(void){if(m_data_measuredMinusBkgdNorm) return m_data_measuredMinusBkgdNorm; else return 0;};
   
   TH1* GetDataMeasured(void){if(m_data_measured) return m_data_measured; else return 0;};
   TH1* GetDataMeasuredNorm(void){if(m_data_measuredNorm) return m_data_measuredNorm; else return 0;};
   
   TH1* GetDataBkgd(void){if(m_data_bkgd) return m_data_bkgd; else return 0;};
   TH1* GetDataBkgdNorm(void){if(m_data_bkgdNorm) return m_data_bkgdNorm; else return 0;};
   
   TH1* GetDataUnfolded(void){if(m_data_unfolded) return m_data_unfolded; else return 0;};
   TH1* GetDataUnfoldedNorm(void){if(m_data_unfoldedNorm) return m_data_unfoldedNorm; else return 0;};
   
   TH1* GetDataUnfoldedDiffxs(void){if(m_data_unfolded_diffxs) return m_data_unfolded_diffxs; else return 0;};
   TH1* GetDataUnfoldedDiffxsNorm(void){if(m_data_unfolded_diffxsNorm) return m_data_unfolded_diffxsNorm; else return 0;};
   
   TH1* GetDeviation(void){if(m_deviation) return m_deviation; else return 0;};
   TH1* GetDeviationReco(void){if(m_deviationReco) return m_deviationReco; else return 0;};
   TH1* GetDeviationDiffxs(void){if(m_deviation_diffxs) return m_deviation_diffxs;else return 0;};
   
   void SetMcMigration(TH2D* hist){m_mc_migration=hist;};
   TH2* GetMcMigration(void){if(m_mc_migration) return m_mc_migration; else return 0;};
   TH2* GetMcMigrationNorm(void){if(m_mc_migrationNorm) return m_mc_migrationNorm; else return 0;};
   TH2* GetMcMigrationScaled(void){if(m_mc_migrationScaled) return m_mc_migrationScaled; else return 0;};
   
   TH1* GetRmsResiduals(void){if(m_rmsResiduals) return m_rmsResiduals; else return 0;};
   TH1* GetMeanResiduals(void){if(m_meanResiduals) return m_meanResiduals; else return 0;};
   TH1* GetRmsResidualsDiffxs(void){if(m_rmsResidualsDiffxs) return m_rmsResidualsDiffxs; else return 0;};
   TH1* GetMeanResidualsDiffxs(void){if(m_meanResidualsDiffxs) return m_meanResidualsDiffxs; else return 0;};
   TH1* GetRmsResidualsDiffxsNorm(void){if(m_rmsResidualsDiffxsNorm) return m_rmsResidualsDiffxsNorm; else return 0;};
   TH1* GetMeanResidualsDiffxsNorm(void){if(m_meanResidualsDiffxsNorm) return m_meanResidualsDiffxsNorm; else return 0;};
   TH1* GetUnfoldingError(void){if(m_unfoldingError) return m_unfoldingError; else return 0;};
   TH1* GetMeanDeviation(void){if(m_meanDeviation) return m_meanDeviation; else return 0;};
   TH1* GetRmsDeviation(void){if(m_rmsDeviation) return m_rmsDeviation; else return 0;};
   TH1* GetMeanDeviationScaled(void){if(m_meanDeviationScaled) return m_meanDeviationScaled; else return 0;};
   TH1* GetRmsDeviationScaled(void){if(m_rmsDeviationScaled) return m_rmsDeviationScaled; else return 0;};
   
   TH1* GetRegularizationScan(void){if(m_regularizationScan) return m_regularizationScan; else return 0;};
   
   
   void SetHistoBaseName(std::string base){m_histo_baseName = base;};
   
   void SetUnfoldingMethod(UnfoldMethod method){m_method = method;};
   UnfoldMethod GetUnfoldingMethod(void){return m_method;};
   std::string GetUnfoldingMethodString(void){return GetMethodString(m_method);};
   static std::string GetMethodString(const UnfoldMethod& method);
   
   TH2D* GetSmearedMigrationMatrix(bool usePoisson = false) const;
   TH1D* GetSmearedEfficiency(TH2D const * const migration, const TopDecayChannel& channel) const;
   ttbarUnfold* GetSmearedMigrationToy(const TopDecayChannel& channel,bool usePoisson = false) const;
   
   void SetMeasuredDataMinusBkgdHistogramName(const std::string name){m_measuredMinusBkgdHistoName = name;};
   std::string GetMeasuredMinusBkgdHistogramName(void){return m_measuredMinusBkgdHistoName;};
   
   void SetMeasuredDataHistogramName(const std::string name){m_measuredHistoName = name;};
   std::string GetMeasuredHistogramName(void){return m_measuredHistoName;};
   
   void SetBkgdHistogramName(const std::string name){m_bkgdHistoName = name;};
   std::string GetBkgdHistogramName(void){return m_bkgdHistoName;};
   
   void SetBayesianIterations(const unsigned int iterations){m_iters = iterations;};
   unsigned int GetBayesianIterations(void){return m_iters;};
   
   void SetEfficiencyHistogram(TH1 * const histo){m_efficiencyHisto = histo;};
   TH1* GetEfficiencyHistogram() const {return m_efficiencyHisto;};
   
   // deviation: z = (x-y)/y
   static void GetDeviationHisto(TH1* const x,TH1* const y,TH1* const z);
   
private:
   
   std::string m_dataFilename;
   std::string m_mcFilename;
   std::string m_sampleName;
   
   
   UnfoldMethod m_method;
   
   RooUnfold*                m_unfolded;
   RooUnfold::ErrorTreatment m_unfoldErrorTreatment;
   unsigned int              m_reg;
   unsigned int              m_iters;
   unsigned int              m_ntoys_unfold;
   unsigned int              m_ntoys_errors;
   
   RooUnfoldResponse*        m_response;
   RooUnfoldErrors*          m_errors;
   
   TH1D* m_rmsResiduals;
   TH1D* m_meanResiduals;
   TH1D* m_rmsResidualsDiffxs;
   TH1D* m_meanResidualsDiffxs;
   TH1D* m_rmsResidualsDiffxsNorm;
   TH1D* m_meanResidualsDiffxsNorm;
   TH1D* m_unfoldingError;
   TH1D* m_meanDeviation;
   TH1D* m_meanDeviationScaled;
   TH1D* m_rmsDeviation;
   TH1D* m_rmsDeviationScaled;
   
   std::string m_histo_baseName;
   
   std::string m_mc_truth_hname;
   TH1D* m_mc_truth;
   TH1D* m_mc_truthNorm;
   TH1D* m_mc_truthScaled;
   
   TH1* m_mc_truth_diffxs;
   TH1* m_mc_truth_diffxsNorm;
   
   std::string m_mc_reco_hname;
   TH1D* m_mc_reco;
   TH1D* m_mc_recoNorm;
   TH1D* m_mc_recoScaled;
   
   TH1* m_mc_reco_diffxs;
   TH1* m_mc_reco_diffxsNorm;
   
   std::string m_mc_migration_hname;
   TH2D* m_mc_migration;
   TH2D* m_mc_migrationNorm;
   TH2D* m_mc_migrationScaled;
   
   std::string m_measuredMinusBkgdHistoName;
   TH1D* m_data_measuredMinusBkgd;
   TH1D* m_data_measuredMinusBkgdNorm;
   std::string m_measuredHistoName;
   TH1D* m_data_measured;
   TH1D* m_data_measuredNorm;
   std::string m_bkgdHistoName;
   TH1D* m_data_bkgd;
   TH1D* m_data_bkgdNorm;
   
   TH1D* m_data_unfolded;
   TH1D* m_data_unfoldedNorm;
   
   TH1* m_data_unfolded_diffxs;
   TH1* m_data_unfolded_diffxsNorm;
   
   TH1D* m_deviation;
   TH1D* m_deviationReco;
   TH1D* m_deviation_diffxs;
   
   TH1D* m_regularizationScan;
   
   void RetrieveHistos(void);
   
   TH1* m_efficiencyHisto;
   
};

#endif

