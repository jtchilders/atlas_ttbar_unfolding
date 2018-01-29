#include "MasterSystList.h"
#include "SystematicSample.h"

const bool MasterSystList::m_use_jesi = false;


const unsigned int MasterSystList::m_num_jes_components_bkgd = 2;
const char* MasterSystList::m_jes_component_bkgd_systematics[]   = {
   "bkgd_jes_up",
   "bkgd_jes_down"
};
   
const unsigned int MasterSystList::m_num_jes_components = 42;
const char* MasterSystList::m_jes_component_systematics[]   = {
      "BJesUnc-down",
      "BJesUnc-up",
      "closeby-down",
      "closeby-up",
      "EffectiveNP-DET1-down",
      "EffectiveNP-DET1-up",
      "EffectiveNP-DET2-down",
      "EffectiveNP-DET2-up",
      "EffectiveNP-MIXED1-down",
      "EffectiveNP-MIXED1-up",
      "EffectiveNP-MIXED2-down",
      "EffectiveNP-MIXED2-up",
      "EffectiveNP-MODEL1-down",
      "EffectiveNP-MODEL1-up",
      "EffectiveNP-MODEL2-down",
      "EffectiveNP-MODEL2-up",
      "EffectiveNP-MODEL3-down",
      "EffectiveNP-MODEL3-up",
      "EffectiveNP-MODEL4-down",
      "EffectiveNP-MODEL4-up",
      "EffectiveNP-STAT1-down",
      "EffectiveNP-STAT1-up",
      "EffectiveNP-STAT2-down",
      "EffectiveNP-STAT2-up",
      "EffectiveNP-STAT3-down",
      "EffectiveNP-STAT3-up",
      "EtaIntercalibration-Theory-down",
      "EtaIntercalibration-Theory-up",
      "EtaIntercalibration-TotalStat-down",
      "EtaIntercalibration-TotalStat-up",
      "flavor-comp-down",
      "flavor-comp-up",
      "flavor-response-down",
      "flavor-response-up",
      "Pileup-OffsetMu-down",
      "Pileup-OffsetMu-up",
      "Pileup-OffsetNPV-down",
      "Pileup-OffsetNPV-up",
      "RelativeNonClosure-MC11b-down",
      "RelativeNonClosure-MC11b-up",
      "SingleParticle-HighPt-down",
      "SingleParticle-HighPt-up"
};

MasterSystList::MasterSystList()
{
   
   m_standardSystematics.push_back("nominal");
//    if(!m_use_jesi){
//       m_standardSystematics.push_back("jesu");
//       m_standardSystematics.push_back("jesd");
//    }
//    m_standardSystematics.push_back("btagsfup");
//    m_standardSystematics.push_back("btagsfdown"); 
//    m_standardSystematics.push_back("ctagsfup");
//    m_standardSystematics.push_back("ctagsfdown");
//    m_standardSystematics.push_back("mistagsfup");
//    m_standardSystematics.push_back("mistagsfdown");
//    m_standardSystematics.push_back("celloutu");
//    m_standardSystematics.push_back("celloutd");
//    m_standardSystematics.push_back("pileupu");
//    m_standardSystematics.push_back("pileupd");
//    m_standardSystematics.push_back("jeff");
//    m_standardSystematics.push_back("jer");
//    m_standardSystematics.push_back("eeru");
//    m_standardSystematics.push_back("eerd");
//    m_standardSystematics.push_back("eesu");
//    m_standardSystematics.push_back("eesd");
//    m_standardSystematics.push_back("muidu");
//    m_standardSystematics.push_back("muidd");
//    m_standardSystematics.push_back("mumsu");
//    m_standardSystematics.push_back("mumsd");
//    m_standardSystematics.push_back("musc");
//    m_standardSystematics.push_back("lep_id_sfup");
//    m_standardSystematics.push_back("lep_id_sfdown");
//    m_standardSystematics.push_back("lep_reco_sfup");
//    m_standardSystematics.push_back("lep_reco_sfdown");
//    m_standardSystematics.push_back("lep_trig_sfup");
//    m_standardSystematics.push_back("lep_trig_sfdown");
   m_standardSystematics.push_back("wjets_bb4_up");
   m_standardSystematics.push_back("wjets_bb4_down");
//    m_standardSystematics.push_back("wjets_bb5_up");
//    m_standardSystematics.push_back("wjets_bb5_down");
//    m_standardSystematics.push_back("wjets_bbcc_up");
//    m_standardSystematics.push_back("wjets_bbcc_down");
//    m_standardSystematics.push_back("wjets_bbccc_up");
//    m_standardSystematics.push_back("wjets_bbccc_down");
//    m_standardSystematics.push_back("wjets_c4_up");
//    m_standardSystematics.push_back("wjets_c4_down");
//    m_standardSystematics.push_back("wjets_c5_up");
//    m_standardSystematics.push_back("wjets_c5_down");
//    m_standardSystematics.push_back("wjets_ca_up");
//    m_standardSystematics.push_back("wjets_ca_down");
//    m_standardSystematics.push_back("IFsrMinus");
//    m_standardSystematics.push_back("IFsrPlus");
//    m_standardSystematics.push_back("qcdd");
//    m_standardSystematics.push_back("qcdu");
//    m_standardSystematics.push_back("qcdsd");
//    m_standardSystematics.push_back("qcdsu");
//    m_standardSystematics.push_back("hadronization");
//    m_standardSystematics.push_back("migrationToys");
//    m_standardSystematics.push_back("mc_generator");

   
   m_systematicsToRun.insert(m_systematicsToRun.end(),
                             m_standardSystematics.begin(),
                             m_standardSystematics.end());
   
   if(m_use_jesi){
      for(unsigned int i=0;i<m_num_jes_components;++i)
         m_systematicsToRun.push_back(std::string(m_jes_component_systematics[i]));
      for(unsigned int i=0;i<m_num_jes_components_bkgd;++i)
         m_systematicsToRun.push_back(std::string(m_jes_component_bkgd_systematics[i]));
   }
   
   // create list of base names for the sytematics to run
   for(std::vector<std::string>::const_iterator itr = m_systematicsToRun.begin();
       itr!=m_systematicsToRun.end();++itr)
   {
      std::string systBase = SystematicSample::GetSampleBaseString((*itr));
      // make sure this doesn't already exist in vector
      bool foundRepeat = false;
      for(std::vector<std::string>::const_iterator iter = m_systematicsToRunBase.begin();
          iter!=m_systematicsToRunBase.end() && !foundRepeat;++iter)
      {
         if((*iter) == systBase){
            foundRepeat = true;
            break;
         }
      }
      if(!foundRepeat){
         m_systematicsToRunBase.push_back(systBase);
      }
   }
}

bool MasterSystList::IsJesComponentSyst(const std::string syst){
   
   for(unsigned int i=0;i<m_num_jes_components;++i)
   {
      // look for base names that also have "jesc_" in them
      if(!strcmp(m_jes_component_systematics[i],syst.c_str()) || syst.find("jesc_") != std::string::npos)
         return true;
   }
   
   return false;
}

bool MasterSystList::IsJesComponentBackgroundSyst(const std::string syst){
   
   for(unsigned int i=0;i<m_num_jes_components_bkgd;++i)
   {
      if(!strncmp(m_jes_component_bkgd_systematics[i],syst.c_str(),syst.size()))
         return true;
   }
   
   return false;
}

bool MasterSystList::skipSystematic(const std::string& systematic,const std::string& channel){
   if ((systematic.find("eer") != std::string::npos) && (channel.find("mu") != std::string::npos) )
      return true;
   if ((systematic.find("ees") != std::string::npos) && (channel.find("mu") != std::string::npos) )
      return true;
   if ((systematic.find("muid") != std::string::npos) && (channel.find("el") != std::string::npos) )
      return true;
   if ((systematic.find("musc") != std::string::npos) && (channel.find("el") != std::string::npos) )
      return true;
   if ((systematic.find("mums") != std::string::npos) && (channel.find("el") != std::string::npos) )
      return true;
   
   return false;
}

std::vector<std::string> MasterSystList::GetJesComponentBaseNames(){
   
   std::vector<std::string> output;
   for(unsigned int i=0;i<m_num_jes_components;++i){
      std::string syst = SystematicSample::GetSampleBaseString(m_jes_component_systematics[i]);
      bool repeat = false;
      for(unsigned int j=0;j<output.size();++j){
         if(syst == output[j]){
            repeat = true;
            break;
         }
      }
      if(!repeat)
         output.push_back(syst);
   }
   
   return output;
}

bool MasterSystList::IsJesComponentSystBase(const std::string syst){
   std::vector<std::string> jesi_base = GetJesComponentBaseNames();
   for(unsigned int i=0;i<jesi_base.size();++i)
   {
      // look for base names that also have "jesc_" in them
      if(jesi_base[i] == syst)
         return true;
   }
   
   return false;
}
