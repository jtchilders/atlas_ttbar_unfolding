#ifndef ASYMBLUECOMBINATION_H
#define ASYMBLUECOMBINATION_H

#include <string>
#include <vector>

#include "SystematicBinList.h"
#include "BLUECombination.h"

//see http://www-cdf.fnal.gov/physics/new/top/2008/singletop/combination/#AIB

class SystematicList;
class SystematicBinList;
class TH1D;
class TGraphAsymmErrors;

class AsymBLUECombination{
public:
   AsymBLUECombination(const std::string filenameA_up,
                       const std::string filenameA_down,
                       const std::string filenameB_up,
                       const std::string filenameB_down,
                       std::vector<std::string>& correlatedSystematics,
                       const BLUECombination::StatTreatment statTreatment = BLUECombination::UNCORRELATED
                      );
   AsymBLUECombination(SystematicList* listA_up,
                       SystematicList* listA_down,
                       SystematicList* listB_up,
                       SystematicList* listB_down,
                       std::vector<std::string>& correlatedSystematics,
                       const BLUECombination::StatTreatment statTreatment = BLUECombination::UNCORRELATED
                      );
   ~AsymBLUECombination();
   
   const SystematicBinList& combined() const {return *m_combined;};
   
   const SystematicBinList& Combine(
         const std::vector<double>& dataA = std::vector<double>::vector<double>(),
         const std::vector<double>& dataB = std::vector<double>::vector<double>());
   
   
   
   TGraphAsymmErrors* GetGraphWithSyst();
   TH1D* GetHistoWithStat();
   
   TH1D* GetUpHistoWithSyst(){return m_histoWithSystUp;};
   TH1D* GetUpHistoWithStat(){return m_histoWithStatUp;};
   
   TH1D* GetDownHistoWithSyst(){return m_histoWithSystDown;};
   TH1D* GetDownHistoWithStat(){return m_histoWithStatDown;};
   
   TH1D* GetCentralHistoWithSyst(){return m_histoWithSystCentral;};
   TH1D* GetCentralHistoWithStat(){return m_histoWithStatCentral;};
   
private:
   
   void Init(); // zero pointers
   void Setup(); // create BLUEs
   
   SystematicBinList* m_combined;
   TGraphAsymmErrors* m_graphWithSyst;
   TH1D* m_histoWithStat;
   
   std::vector<std::string>& m_correlatedSystematics;
   const BLUECombination::StatTreatment m_statTreatment;
   
   SystematicList* m_listA_up;
   SystematicList* m_listA_down;
   SystematicList* m_listA_central;
   SystematicList* m_listB_up;
   SystematicList* m_listB_down;
   SystematicList* m_listB_central;
   bool m_delete_lists;
   
   TH1D* m_histoWithSystUp;
   TH1D* m_histoWithStatUp;
   TH1D* m_histoWithSystDown;
   TH1D* m_histoWithStatDown;
   TH1D* m_histoWithSystCentral;
   TH1D* m_histoWithStatCentral;
   
   BLUECombination* m_comb_up;
   BLUECombination* m_comb_down;
   BLUECombination* m_comb_central;
   
   void GetCentralList(SystematicList* up,
                       SystematicList* down,
                       SystematicList* central);
};

#endif 
