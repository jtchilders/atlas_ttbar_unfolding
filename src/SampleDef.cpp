#include "SampleDef.h"

void SampleDef::Setup(void){
   m_sampleString = str(m_sample);
   
   switch (m_sample) {
      case Tag0_Top1:
         m_truth     = "Kinem/TopFit_NTag0/PartonTop1/PartonTop1PtSpectrum";
         m_reco      = "Kinem/TopFit_NTag0/Perm0Top1/Perm0Top1PtSpectrum";
         m_migration = "Kinem/TopFit_NTag0/Perm0Top1RecoVsPerm0Top1Truth/Perm0Top1RecoVsPerm0Top1TruthPt2VsPt1";
         m_measured           = "Top1Pt_Data";
         m_measuredMinusBkgd  = "Top1Pt_DataMinusBG";
         m_bkgd               = "Top1Pt_BG";
         m_variable           = "leptonic top";
         return;
      case Tag0_Top2:
         m_truth     = "Kinem/TopFit_NTag0/PartonTop2/PartonTop2PtSpectrum";
         m_reco      = "Kinem/TopFit_NTag0/Perm0Top2/Perm0Top2PtSpectrum";
         m_migration = "Kinem/TopFit_NTag0/Perm0Top2RecoVsPerm0Top2Truth/Perm0Top2RecoVsPerm0Top2TruthPt2VsPt1";
         m_measured           = "Top2Pt_Data";
         m_measuredMinusBkgd  = "Top2Pt_DataMinusBG";
         m_bkgd               = "Top2Pt_BG";
         m_variable           = "hadronic top";
         return;
      case Tag0_Top1_lhood:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag0/PartonTop1/PartonTop1PtSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag0/Perm0Top1/Perm0Top1PtSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag0/Perm0Top1RecoVsPerm0Top1Truth/Perm0Top1RecoVsPerm0Top1TruthPt2VsPt1";
         m_measured           = "Top1Pt_Data";
         m_measuredMinusBkgd  = "Top1Pt_DataMinusBG";
         m_bkgd               = "Top1Pt_BG";
         m_variable           = "leptonic top";
         return;
      case Tag0_Top2_lhood:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag0/PartonTop2/PartonTop2PtSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag0/Perm0Top2/Perm0Top2PtSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag0/Perm0Top2RecoVsPerm0Top2Truth/Perm0Top2RecoVsPerm0Top2TruthPt2VsPt1";
         m_measured           = "Top2Pt_Data";
         m_measuredMinusBkgd  = "Top2Pt_DataMinusBG";
         m_bkgd               = "Top2Pt_BG";
         m_variable           = "hadronic top";
         return;
      case Tag0_SystemMass:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag0/PartonSystem/PartonSystemMassSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag0/Perm0System/Perm0SystemMassSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag0/Perm0SystemRecoVsPerm0SystemTruth/Perm0SystemRecoVsPerm0SystemTruthMass2VsMass1";
         m_measured           = "SystemMass_Data";
         m_measuredMinusBkgd  = "SystemMass_DataMinusBG";
         m_bkgd               = "SystemMass_BG";
         m_variable           = "top pair invariant mass";
         return;
      case Tag0_SystemPt:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag0/PartonSystem/PartonSystemPtSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag0/Perm0System/Perm0SystemPtSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag0/Perm0SystemRecoVsPerm0SystemTruth/Perm0SystemRecoVsPerm0SystemTruthPt2VsPt1";
         m_measured           = "SystemPt_Data";
         m_measuredMinusBkgd  = "SystemPt_DataMinusBG";
         m_bkgd               = "SystemPt_BG";
         m_variable           = "top pair transverse momentum";
         return;
      case Tag0_SystemRapidity:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag0/PartonSystem/PartonSystemRapiditySpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag0/Perm0System/Perm0SystemRapiditySpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag0/Perm0SystemRecoVsPerm0SystemTruth/Perm0SystemRecoVsPerm0SystemTruthRapidity2VsRapidity1";
         m_measured           = "SystemRapidity_Data";
         m_measuredMinusBkgd  = "SystemRapidity_DataMinusBG";
         m_bkgd               = "SystemRapidity_BG";
         m_variable           = "top pair rapidity";
         return;
      case Tag1_Top1:
         m_truth     = "Kinem/TopFit_NTag1/PartonTop1/PartonTop1PtSpectrum";
         m_reco      = "Kinem/TopFit_NTag1/Perm0Top1/Perm0Top1PtSpectrum";
         m_migration = "Kinem/TopFit_NTag1/Perm0Top1RecoVsPerm0Top1Truth/Perm0Top1RecoVsPerm0Top1TruthPt2VsPt1";
         m_measured           = "Top1Pt_Data";
         m_measuredMinusBkgd  = "Top1Pt_DataMinusBG";
         m_bkgd               = "Top1Pt_BG";
         m_variable           = "leptonic top";
         return;
      case Tag1_Top2:
         m_truth     = "Kinem/TopFit_NTag1/PartonTop2/PartonTop2PtSpectrum";
         m_reco      = "Kinem/TopFit_NTag1/Perm0Top2/Perm0Top2PtSpectrum";
         m_migration = "Kinem/TopFit_NTag1/Perm0Top2RecoVsPerm0Top2Truth/Perm0Top2RecoVsPerm0Top2TruthPt2VsPt1";
         m_measured           = "Top2Pt_Data";
         m_measuredMinusBkgd  = "Top2Pt_DataMinusBG";
         m_bkgd               = "Top2Pt_BG";
         m_variable           = "hadronic top";
         return;
      case Tag1_Top1_lhood:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag1/PartonTop1/PartonTop1PtSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag1/Perm0Top1/Perm0Top1PtSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag1/Perm0Top1RecoVsPerm0Top1Truth/Perm0Top1RecoVsPerm0Top1TruthPt2VsPt1";
         m_measured           = "Top1Pt_Data";
         m_measuredMinusBkgd  = "Top1Pt_DataMinusBG";
         m_bkgd               = "Top1Pt_BG";
         m_variable           = "leptonic top";
         return;
      case Tag1_Top2_lhood:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag1/PartonTop2/PartonTop2PtSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag1/Perm0Top2/Perm0Top2PtSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag1/Perm0Top2RecoVsPerm0Top2Truth/Perm0Top2RecoVsPerm0Top2TruthPt2VsPt1";
         m_measured           = "Top2Pt_Data";
         m_measuredMinusBkgd  = "Top2Pt_DataMinusBG";
         m_bkgd               = "Top2Pt_BG";
         m_variable           = "hadronic top";
         return; 
      case Tag1_SystemMass:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag1/PartonSystem/PartonSystemMassSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag1/Perm0System/Perm0SystemMassSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag1/Perm0SystemRecoVsPerm0SystemTruth/Perm0SystemRecoVsPerm0SystemTruthMass2VsMass1";
         m_measured           = "SystemMass_Data";
         m_measuredMinusBkgd  = "SystemMass_DataMinusBG";
         m_bkgd               = "SystemMass_BG";
         m_variable           = "top pair invariant mass";
         return;
      case Tag1_SystemPt:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag1/PartonSystem/PartonSystemPtSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag1/Perm0System/Perm0SystemPtSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag1/Perm0SystemRecoVsPerm0SystemTruth/Perm0SystemRecoVsPerm0SystemTruthPt2VsPt1";
         m_measured           = "SystemPt_Data";
         m_measuredMinusBkgd  = "SystemPt_DataMinusBG";
         m_bkgd               = "SystemPt_BG";
         m_variable           = "top pair transverse momentum";
         return;
      case Tag1_SystemRapidity:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag1/PartonSystem/PartonSystemRapiditySpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag1/Perm0System/Perm0SystemRapiditySpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag1/Perm0SystemRecoVsPerm0SystemTruth/Perm0SystemRecoVsPerm0SystemTruthRapidity2VsRapidity1";
         m_measured           = "SystemRapidity_Data";
         m_measuredMinusBkgd  = "SystemRapidity_DataMinusBG";
         m_bkgd               = "SystemRapidity_BG";
         m_variable           = "top pair rapidity";
         return;
      case Tag2_Top1:
         m_truth     = "Kinem/TopFit_NTag2/PartonTop1/PartonTop1PtSpectrum";
         m_reco      = "Kinem/TopFit_NTag2/Perm0Top1/Perm0Top1PtSpectrum";
         m_migration = "Kinem/TopFit_NTag2/Perm0Top1RecoVsPerm0Top1Truth/Perm0Top1RecoVsPerm0Top1TruthPt2VsPt1";
         m_measured           = "Top1Pt_Data";
         m_measuredMinusBkgd  = "Top1Pt_DataMinusBG";
         m_bkgd               = "Top1Pt_BG";
         m_variable           = "leptonic top";
         return;
      case Tag2_Top2:
         m_truth     = "Kinem/TopFit_NTag2/PartonTop2/PartonTop2PtSpectrum";
         m_reco      = "Kinem/TopFit_NTag2/Perm0Top2/Perm0Top2PtSpectrum";
         m_migration = "Kinem/TopFit_NTag2/Perm0Top2RecoVsPerm0Top2Truth/Perm0Top2RecoVsPerm0Top2TruthPt2VsPt1";
         m_measured           = "Top2Pt_Data";
         m_measuredMinusBkgd  = "Top2Pt_DataMinusBG";
         m_bkgd               = "Top2Pt_BG";
         m_variable           = "hadronic top";
         return;
      case Tag2_Top1_lhood:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag2/PartonTop1/PartonTop1PtSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag2/Perm0Top1/Perm0Top1PtSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag2/Perm0Top1RecoVsPerm0Top1Truth/Perm0Top1RecoVsPerm0Top1TruthPt2VsPt1";
         m_measured           = "Top1Pt_Data";
         m_measuredMinusBkgd  = "Top1Pt_DataMinusBG";
         m_bkgd               = "Top1Pt_BG";
         m_variable           = "leptonic top";
         return;
      case Tag2_Top2_lhood:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag2/PartonTop2/PartonTop2PtSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag2/Perm0Top2/Perm0Top2PtSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag2/Perm0Top2RecoVsPerm0Top2Truth/Perm0Top2RecoVsPerm0Top2TruthPt2VsPt1";
         m_measured           = "Top2Pt_Data";
         m_measuredMinusBkgd  = "Top2Pt_DataMinusBG";
         m_bkgd               = "Top2Pt_BG";
         m_variable           = "hadronic top";
         return;
      case Tag2_SystemMass:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag2/PartonSystem/PartonSystemMassSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag2/Perm0System/Perm0SystemMassSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag2/Perm0SystemRecoVsPerm0SystemTruth/Perm0SystemRecoVsPerm0SystemTruthMass2VsMass1";
         m_variable           = "top pair invariant mass";
         return;
      case Tag2_SystemPt:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag2/PartonSystem/PartonSystemPtSpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag2/Perm0System/Perm0SystemPtSpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag2/Perm0SystemRecoVsPerm0SystemTruth/Perm0SystemRecoVsPerm0SystemTruthPt2VsPt1";
         m_variable           = "top pair transverse momentum";
         return;
      case Tag2_SystemRapidity:
         m_truth     = "Kinem_likelihoodCut/TopFit_NTag2/PartonSystem/PartonSystemRapiditySpectrum";
         m_reco      = "Kinem_likelihoodCut/TopFit_NTag2/Perm0System/Perm0SystemRapiditySpectrum";
         m_migration = "Kinem_likelihoodCut/TopFit_NTag2/Perm0SystemRecoVsPerm0SystemTruth/Perm0SystemRecoVsPerm0SystemTruthRapidity2VsRapidity1";
         m_variable           = "top pair rapidity";
         return;
      case FakeData:
         m_truth     = "truth";
         m_reco      = "reco";
         m_migration = "migration";
   }
};

std::string SampleDef::str(SampleNameEnum sample){
   switch(sample){
   case Tag0_Top1: //without b-tagging /best output of KLFitter / leptonic
      return "Tag0_Top1";
   case Tag0_Top2: //without b-tagging /best output of KLFitter / hadronic
      return "Tag0_Top2";
   case Tag0_Top1_lhood: //without b-tagging /best output of KLFitter / leptonic
      return "Tag0_Top1_lhood";
   case Tag0_Top2_lhood: //without b-tagging /best output of KLFitter / hadronic
      return "Tag0_Top2_lhood";
   case Tag0_SystemMass: //without b-tagging /top system
      return "Tag0_SystemMass";
   case Tag0_SystemPt: //without b-tagging /top system
      return "Tag0_SystemPt";
   case Tag0_SystemRapidity: //without b-tagging /top system
      return "Tag0_SystemRapidity";
   case Tag1_Top1: //one or more b-tagging /best output of KLFitter / leptonic
      return "Tag1_Top1";
   case Tag1_Top2: //one or more b-tagging /best output of KLFitter / hadronic
      return "Tag1_Top2";
   case Tag1_Top1_lhood: //one or more b-tagging /best output of KLFitter / leptonic
      return "Tag1_Top1_lhood";
   case Tag1_Top2_lhood: //one or more b-tagging /best output of KLFitter / hadronic
      return "Tag1_Top2_lhood";
   case Tag1_SystemMass: //without b-tagging /top system
      return "Tag1_SystemMass";
   case Tag1_SystemPt: //without b-tagging /top system
      return "Tag1_SystemPt";
   case Tag1_SystemRapidity: //without b-tagging /top system
      return "Tag1_SystemRapidity";
   case Tag2_Top1: //two or more b-tagging /best output of KLFitter / leptonic
      return "Tag2_Top1";
   case Tag2_Top2: //two or more b-tagging /best output of KLFitter / hadronic
      return "Tag2_Top2";
   case Tag2_Top1_lhood: //two or more b-tagging /best output of KLFitter / leptonic
      return "Tag2_Top1_lhood";
   case Tag2_Top2_lhood: //two or more b-tagging /best output of KLFitter / hadronic
      return "Tag2_Top2_lhood";
   case Tag2_SystemMass: //two or more b-tagging /top system
      return "Tag2_SystemMass";
   case Tag2_SystemPt: //without b-tagging /top system
      return "Tag2_SystemPt";
   case Tag2_SystemRapidity: //without b-tagging /top system
      return "Tag2_SystemRapidity";
   case FakeData:
      return "FakeData";
   }
   return "";
}

SampleDef::SampleNameEnum SampleDef::enumValue(std::string sample){
   
   if(sample == "Tag0_Top1") //without b-tagging /best output of KLFitter / leptonic
      return Tag0_Top1;
   if(sample == "Tag0_Top2") //without b-tagging /best output of KLFitter / hadronic
      return Tag0_Top2;
   if(sample.find("Tag0_Top1_lhood") != std::string::npos) //without b-tagging /best output of KLFitter / leptonic
      return Tag0_Top1_lhood;
   if(sample.find("Tag0_Top2_lhood") != std::string::npos) //without b-tagging /best output of KLFitter / hadronic
      return Tag0_Top2_lhood;
   if(sample.find("Tag0_SystemMass") != std::string::npos) //without b-tagging /top system
      return Tag0_SystemMass;
   if(sample.find("Tag0_SystemPt") != std::string::npos) //without b-tagging /top system
      return Tag0_SystemPt;
   if(sample.find("Tag0_SystemRapidity") != std::string::npos) //without b-tagging /top system
      return Tag0_SystemRapidity;
   if(sample == "Tag1_Top1") //one or more b-tagging /best output of KLFitter / leptonic
      return Tag1_Top1;
   if(sample == "Tag1_Top2") //one or more b-tagging /best output of KLFitter / hadronic
      return Tag1_Top2;
   if(sample.find("Tag1_Top1_lhood") != std::string::npos) //one or more b-tagging /best output of KLFitter / leptonic
      return Tag1_Top1_lhood;
   if(sample.find("Tag1_Top2_lhood") != std::string::npos) //one or more b-tagging /best output of KLFitter / hadronic
      return Tag1_Top2_lhood;
   if(sample.find("Tag1_SystemMass") != std::string::npos) //without b-tagging /top system
      return Tag1_SystemMass;
   if(sample.find("Tag1_SystemPt") != std::string::npos) //without b-tagging /top system
      return Tag1_SystemPt;
   if(sample.find("Tag1_SystemRapidity") != std::string::npos) //without b-tagging /top system
      return Tag1_SystemRapidity;
   if(sample == "Tag2_Top1") //two or more b-tagging /best output of KLFitter / leptonic
      return Tag2_Top1;
   if(sample == "Tag2_Top2") //two or more b-tagging /best output of KLFitter / hadronic
      return Tag2_Top2;
   if(sample.find("Tag2_Top1_lhood") != std::string::npos) //two or more b-tagging /best output of KLFitter / leptonic
      return Tag2_Top1_lhood;
   if(sample.find("Tag2_Top2_lhood") != std::string::npos) //two or more b-tagging /best output of KLFitter / hadronic
      return Tag2_Top2_lhood;
   if(sample.find("Tag2_SystemMass") != std::string::npos) //two or more b-tagging /top system
      return Tag2_SystemMass;   
   if(sample.find("Tag2_SystemPt") != std::string::npos) //two or more b-tagging /top system
      return Tag2_SystemPt; 
   if(sample.find("Tag2_SystemRapidity") != std::string::npos) //two or more b-tagging /top system
      return Tag2_SystemRapidity; 
   if(sample.find("FakeData") != std::string::npos)
      return FakeData;
   return NUM_OF_SAMPLES;
}

std::vector<SampleDef>* SampleDef::getVector(std::string filter){
   std::vector<SampleDef>* list = new std::vector<SampleDef>;
   for(unsigned int i=0;i<NUM_OF_SAMPLES;++i){
      SampleDef temp((SampleNameEnum)i);
      
      if(filter.size() > 0)
         if(temp.str().find(filter) == std::string::npos) continue;
      
      list->push_back(temp);
   }
   
   return list;
}
