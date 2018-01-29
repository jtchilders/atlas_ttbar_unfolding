#ifndef SAMPLENAME_H
#define SAMPLENAME_H

#include <string>
#include <vector>
#include <iostream>

class SampleDef{
public:
   enum SampleNameEnum{
      Tag0_Top1, //without b-tagging /best output of KLFitter / leptonic
      Tag0_Top2, //without b-tagging /best output of KLFitter / hadronic      
      Tag0_Top1_lhood, //without b-tagging /best output of KLFitter / leptonic
      Tag0_Top2_lhood, //without b-tagging /best output of KLFitter / hadronic      
      Tag0_SystemMass, //without b-tagging /top system
      Tag0_SystemPt, //without b-tagging /top system
      Tag0_SystemRapidity, //without b-tagging /top system
      Tag1_Top1, //one or more b-tagging /best output of KLFitter / leptonic
      Tag1_Top2, //one or more b-tagging /best output of KLFitter / hadronic
      Tag1_Top1_lhood, //one or more b-tagging /best output of KLFitter / leptonic
      Tag1_Top2_lhood, //one or more b-tagging /best output of KLFitter / hadronic
      Tag1_SystemMass, //without b-tagging /top system
      Tag1_SystemPt, //without b-tagging /top system
      Tag1_SystemRapidity, //without b-tagging /top system
      Tag2_Top1, //two or more b-tagging /best output of KLFitter / leptonic
      Tag2_Top2, //two or more b-tagging /best output of KLFitter / hadronic
      Tag2_Top1_lhood, //two or more b-tagging /best output of KLFitter / leptonic
      Tag2_Top2_lhood, //two or more b-tagging /best output of KLFitter / hadronic
      Tag2_SystemMass, //without b-tagging /top system
      Tag2_SystemPt, //without b-tagging /top system
      Tag2_SystemRapidity, //without b-tagging /top system
      FakeData,
      NUM_OF_SAMPLES
   };
   
   SampleDef(void){SetSampleName(Tag0_Top1);};
   SampleDef(SampleNameEnum sample){SetSampleName(sample);};
   SampleDef(std::string sample){SetSampleName(sample);};
   
   void SetSampleName(SampleNameEnum sample){m_sample = sample;Setup();};
   void SetSampleName(std::string sample){m_sample = enumValue(sample);SetSampleName(m_sample);};
   
   SampleNameEnum GetSampleName(void) const {return m_sample;};
   std::string GetSampleNameString(void) const {return m_sampleString;};
   std::string GetTruthHistoPath(void) const {return m_truth;};
   std::string GetRecoHistoPath(void) const {return m_reco;};
   std::string GetMigrationHistoPath(void) const {return m_migration;};
   std::string GetMeasured(void) const {return m_measured;};
   std::string GetMeasuredMinusBkgd() const {return m_measuredMinusBkgd;};
   std::string GetBkgd() const {return m_bkgd;};
   std::string GetVariable() const {return m_variable;};
   
   std::string str(void){return str(m_sample);};
   
   // convert enum to string
   static std::string str(SampleNameEnum sample);
   // convert string to enum
   static SampleNameEnum enumValue(std::string sample);
   // get a vector containing all samples
   static std::vector<SampleDef>* getVector(std::string filter);
   
   bool operator==(const SampleDef& rhs){return (m_sample == rhs.GetSampleName());};
   
   
   
private:
   SampleNameEnum m_sample;
   std::string m_sampleString;
   std::string m_truth;
   std::string m_reco;
   std::string m_migration;
   
   std::string m_measured;
   std::string m_measuredMinusBkgd;
   std::string m_bkgd;
   
   std::string m_variable;
   
   void Setup(void);
   
};


#endif
