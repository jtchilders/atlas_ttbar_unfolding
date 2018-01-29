#! /usr/bin/env python26

import os

def main():
   # each systematic needs the following:
   # 0 - systematic name, like nominal or btagsfdown
   # 1 - systematic base name, like nominal or btagsf
   # 2 - systematic pretty base name
   # 3 - is this a systematic up, if so 'true', if not 'false'
   # 4 - is this a systematic down, if so 'true', if not 'false'
   # 5 - systematic partner, if nominal = NUM_OF_SAMPLES, if btagsfdown = btagsfup
   systematics = [ ['nominal','nominal','Nominal','false','false','NUM_OF_SAMPLES'],
                  ['btagsfup','btagsf','b-tag SF','true','false','btagsfdown'],
                  ['btagsfdown','btagsf','b-tag SF','false','true','btagsfup'],
                  ['ctagsfup','ctagsf','c-tag SF','true','false','ctagsfdown'],
                  ['ctagsfdown','ctagsf','c-tag SF','false','true','ctagsfup'],
                  ['mistagsfup','mistagsf','miss-tag SF','true','false','mistagsfdown'],
                  ['mistagsfdown','mistagsf','miss-tag SF','false','true','mistagsfup'],
                  ['eeru','eer','$e$ Energy Res.','true','false','eerd'],
                  ['eerd','eer','$e$ Energy Res.','false','true','eeru'],
                  ['eesu','ees','$e$ Energy Scale','true','false','eesd'],
                  ['eesd','ees','$e$ Energy Scale','false','true','eesu'],
                  ['jeff','jeff','Jet ID Eff.','false','false','NUM_OF_SAMPLES'],
                  ['jer','jer','Jet Energy Res.','false','false','NUM_OF_SAMPLES'],
                  ['loose','loose','Loose','false','false','NUM_OF_SAMPLES'],
                  ['muidu','muid','$\\\\mu$ ID Eff.','true','false','muidd'],
                  ['muidd','muid','$\\\\mu$ ID Eff.','false','true','muidu'],
                  ['musc','musc','$\\\\mu$ Momentum Rescaling','false','false','NUM_OF_SAMPLES'],
                  ['mumsu','mums','$\\\\mu$ Momentum Scale','true','false','mumsd'],
                  ['mumsd','mums','$\\\\mu$ Momentum Scale','false','true','mumsu'],
                  ['lep_id_sfup','lep_id_sf','$l$ ID SF','true','false','lep_id_sfdown'],
                  ['lep_id_sfdown','lep_id_sf','$l$ ID SF','false','true','lep_id_sfup'],
                  ['lep_reco_sfup','lep_reco_sf','$l$ Reco. SF','true','false','lep_reco_sfdown'],
                  ['lep_reco_sfdown','lep_reco_sf','$l$ Reco. SF','false','true','lep_reco_sfup'],
                  ['lep_trig_sfup','lep_trig_sf','$l$ Trigger SF','true','false','lep_trig_sfdown'],
                  ['lep_trig_sfdown','lep_trig_sf','$l$ Trigger SF','false','true','lep_trig_sfup'],
                  ['wjets_iqopt3','wjets_iqopt3','W+jets (iqopt3)','false','false','NUM_OF_SAMPLES'],
                  ['wjets_ptjmin10','wjets_ptjmin10','W+jets (ptjmin10)','false','false','NUM_OF_SAMPLES'],
                  ['wjets_bb4_up','wjets_bb4','W+Jets bb4','true','false','wjets_bb4_down'],
                  ['wjets_bb4_down','wjets_bb4','W+Jets bb4','false','true','wjets_bb4_up'],
                  ['wjets_bb5_up','wjets_bb5','W+Jets bb5','true','false','wjets_bb5_down'],
                  ['wjets_bb5_down','wjets_bb5','W+Jets bb5','false','true','wjets_bb5_up'],
                  ['wjets_bbccc_up','wjets_bbccc','W+Jets bbccc','true','false','wjets_bbccc_down'],
                  ['wjets_bbccc_down','wjets_bbccc','W+Jets bbccc','false','true','wjets_bbccc_up'],
                  ['wjets_bbcc_up','wjets_bbcc','W+Jets bbcc','true','false','wjets_bbcc_down'],
                  ['wjets_bbcc_down','wjets_bbcc','W+Jets bbcc','false','true','wjets_bbcc_up'],
                  ['wjets_c4_up','wjets_c4','W+Jets c4','true','false','wjets_c4_down'],
                  ['wjets_c4_down','wjets_c4','W+Jets c4','false','true','wjets_c4_up'],
                  ['wjets_c5_up','wjets_c5','W+Jets c5','true','false','wjets_c5_down'],
                  ['wjets_c5_down','wjets_c5','W+Jets c5','false','true','wjets_c5_up'],
                  ['wjets_ca_up','wjets_ca','W+Jets ca','true','false','wjets_ca_down'],
                  ['wjets_ca_down','wjets_ca','W+Jets ca','false','true','wjets_ca_up'],
                  ['IFsrPlus','ifsr','IFSR','true','false','IFsrMinus'],
                  ['IFsrMinus','ifsr','IFSR','false','true','IFsrPlus'],
                  ['FsrPlus','fsr','FSR','true','false','FsrMinus'],
                  ['FsrMinus','fsr','FSR','false','true','FsrPlus'],
                  ['IsrPlus','isr','ISR','true','false','IsrMinus'],
                  ['IsrMinus','isr','ISR','false','true','IsrPlus'],
                  ['AlpgenJimmy','alpgenjimmy','Hard Process','false','false','NUM_OF_SAMPLES'],
                  ['PowHeg_Jimmy','powheg','Parton Shower','true','false','PowHeg_Pythia'],
                  ['PowHeg_Pythia','powheg','Parton Shower','false','true','PowHeg_Jimmy'],
                  ['qcdsu','qcds','QCD Shape','true','false','qcdsd'],
                  ['qcdsd','qcds','QCD Shape','false','true','qcdsu'],
                  ['qcdu','qcd','QCD','true','false','qcdd'],
                  ['qcdd','qcd','QCD','false','true','qcdu'],
                  ['unfolding','unfolding','Unfolding Method','false','false','NUM_OF_SAMPLES'],
                  ['binbybin','binbybin','Unfolding Method','false','false','NUM_OF_SAMPLES'],
                  ['migrationToys','migrationtoys','MC Stat.','false','false','NUM_OF_SAMPLES'],
                  #['bgstatu','bgstat','Background Stat.','true','false','bgstatd'],
                  #['bgstatd','bgstat','Background Stat.','false','true','bgstatu'],
                  #['MRSTMCal','pdf','PDF','true','false','NNPDF20'],
                  #['CTEQ66','pdf','PDF','false','true','NUM_OF_SAMPLES'],
                  #['MRST2007','pdf','PDF','false','false','NUM_OF_SAMPLES'],
                  #['NNPDF20','pdf','PDF','false','false','MRSTMCal'],
                  ['PDF','pdf','PDF','false','false','NUM_OF_SAMPLES'],
                  ['hadronization','hadronization','Fragmentation', 'false','false','NUM_OF_SAMPLES'],
                  ['LUMI','lumi','Luminosity','false','false','NUM_OF_SAMPLES'],
                  ['mc_generator','mc_generator','MC Generator','false','false','NUM_OF_SAMPLES'],
                  ['BJesUnc-up','jesc_BJesUnc','BJesUnc','true','false','BJesUnc-down'],
                  ['BJesUnc-down','jesc_BJesUnc','BJesUnc','false','true','BJesUnc-up'],
                  ['closeby-up','jesc_closeby','JesCloseby','true','false','closeby-down'],
                  ['closeby-down','jesc_closeby','JesCloseby','false','true','closeby-up'],
                  ['EffectiveNP-DET1-up','jesc_EffectiveNP-DET1','JesEffNpDet1','true','false','EffectiveNP-DET1-down'], 
                  ['EffectiveNP-DET1-down','jesc_EffectiveNP-DET1','JesEffNpDet1','false','true','EffectiveNP-DET1-up'], 
                  ['EffectiveNP-DET2-up','jesc_EffectiveNP-DET2','JesEffNpDet2','true','false','EffectiveNP-DET2-down'], 
                  ['EffectiveNP-DET2-down','jesc_EffectiveNP-DET2','JesEffNpDet2','false','true','EffectiveNP-DET2-up'], 
                  ['EffectiveNP-MIXED1-up','jesc_EffectiveNP-MIXED1','JesEffNpMixed1','true','false','EffectiveNP-MIXED1-down'], 
                  ['EffectiveNP-MIXED1-down','jesc_EffectiveNP-MIXED1','JesEffNpMixed1','false','true','EffectiveNP-MIXED1-up'], 
                  ['EffectiveNP-MIXED2-up','jesc_EffectiveNP-MIXED2','JesEffNpMixed2','true','false','EffectiveNP-MIXED2-down'], 
                  ['EffectiveNP-MIXED2-down','jesc_EffectiveNP-MIXED2','JesEffNpMixed2','false','true','EffectiveNP-MIXED2-up'], 
                  ['EffectiveNP-MODEL1-up','jesc_EffectiveNP-MODEL1','JesEffNpModel1','true','false','EffectiveNP-MODEL1-down'],  
                  ['EffectiveNP-MODEL1-down','jesc_EffectiveNP-MODEL1','JesEffNpModel1','false','true','EffectiveNP-MODEL1-up'],
                  ['EffectiveNP-MODEL2-up','jesc_EffectiveNP-MODEL2','JesEffNpModel2','true','false','EffectiveNP-MODEL2-down'], 
                  ['EffectiveNP-MODEL2-down','jesc_EffectiveNP-MODEL2','JesEffNpModel2','false','true','EffectiveNP-MODEL2-up'], 
                  ['EffectiveNP-MODEL3-up','jesc_EffectiveNP-MODEL3','JesEffNpModel3','true','false','EffectiveNP-MODEL3-down'], 
                  ['EffectiveNP-MODEL3-down','jesc_EffectiveNP-MODEL3','JesEffNpModel3','false','true','EffectiveNP-MODEL3-up'], 
                  ['EffectiveNP-MODEL4-up','jesc_EffectiveNP-MODEL4','JesEffNpModel4','true','false','EffectiveNP-MODEL4-down'], 
                  ['EffectiveNP-MODEL4-down','jesc_EffectiveNP-MODEL4','JesEffNpModel4','false','true','EffectiveNP-MODEL4-up'], 
                  ['EffectiveNP-STAT1-up','jesc_EffectiveNP-STAT1','JesEffNpStat1','true','false','EffectiveNP-STAT1-down'], 
                  ['EffectiveNP-STAT1-down','jesc_EffectiveNP-STAT1','JesEffNpStat1','false','true','EffectiveNP-STAT1-up'], 
                  ['EffectiveNP-STAT2-up','jesc_EffectiveNP-STAT2','JesEffNpStat2','true','false','EffectiveNP-STAT2-down'], 
                  ['EffectiveNP-STAT2-down','jesc_EffectiveNP-STAT2','JesEffNpStat2','false','true','EffectiveNP-STAT2-up'], 
                  ['EffectiveNP-STAT3-up','jesc_EffectiveNP-STAT3','JesEffNpStat3','true','false','EffectiveNP-STAT3-down'], 
                  ['EffectiveNP-STAT3-down','jesc_EffectiveNP-STAT3','JesEffNpStat2','false','true','EffectiveNP-STAT3-up'],
                  ['EtaIntercalibration-Theory-up','jesc_EtaIntercalibration-Theory','JesEtaIntercalibTheory','true','false','EtaIntercalibration-Theory-down'], 
                  ['EtaIntercalibration-Theory-down','jesc_EtaIntercalibration-Theory','JesEtaIntercalibTheory','false','true','EtaIntercalibration-Theory-up'], 
                  ['EtaIntercalibration-TotalStat-up','jesc_EtaIntercalibration-TotalStat','JesEtaCalibTotalStat','true','false','EtaIntercalibration-TotalStat-down'], 
                  ['EtaIntercalibration-TotalStat-down','jesc_EtaIntercalibration-TotalStat','JesEtaCalibTotalStat','false','true','EtaIntercalibration-TotalStat-up'], 
                  ['flavor-comp-up','jesc_flavor-comp','JesFlavorComp','true','false','flavor-comp-down'], 
                  ['flavor-comp-down','jesc_flavor-comp','JesFlavorComp','false','true','flavor-comp-up'],
                  ['flavor-response-up','jesc_flavor-response','JesFlavorResponse','true','false','flavor-response-down'], 
                  ['flavor-response-down','jesc_flavor-response','JesFlavorResponse','false','true','flavor-response-up'],
                  ['Pileup-OffsetMu-up','jesc_Pileup-OffsetMu','JesPileupOffsetMu','true','false','Pileup-OffsetMu-down'], 
                  ['Pileup-OffsetMu-down','jesc_Pileup-OffsetMu','JesPileupOffsetMu','false','true','Pileup-OffsetMu-up'],
                  ['Pileup-OffsetNPV-up','jesc_Pileup-OffsetNPV','JesPileupOffsetNpv','true','false','Pileup-OffsetNPV-down'], 
                  ['Pileup-OffsetNPV-down','jesc_Pileup-OffsetNPV','JesPileupOffsetNpv','false','true','Pileup-OffsetNPV-up'],
                  ['RelativeNonClosure-MC11b-up','jesc_RelativeNonClosure-MC11b','JesRelNonClosure','true','false','RelativeNonClosure-MC11b-down'], 
                  ['RelativeNonClosure-MC11b-down','jesc_RelativeNonClosure-MC11b','JesRelNonClosure','false','true','RelativeNonClosure-MC11b-up'], 
                  ['SingleParticle-HighPt-up','jesc_SingleParticle-HighPt','JesSinglePart','true','false','SingleParticle-HighPt-down'], 
                  ['SingleParticle-HighPt-down','jesc_SingleParticle-HighPt','JesSinglePart','false','true','SingleParticle-HighPt-up'], 
                  ['celloutu','cellout','$E_{T}^{miss}$ cell-out','true','false','celloutd'],
                  ['celloutd','cellout','$E_{T}^{miss}$ cell-out','false','true','celloutu'],
                  ['pileupu','pileup','$E_{T}^{miss}$ pile-up','true','false','pileupd'],
                  ['pileupd','pileup','$E_{T}^{miss}$ pile-up','false','true','pileupu'],
                  ['bkgd_jes_up','bkgd_jes','Bkgd JES','true','false','bkgd_jes_down'],
                  ['bkgd_jes_down','bkgd_jes','Bkgd JES','false','true','bkgd_jes_up'],
                  ['jesu','jes','Jet Energy Scale','true','false','jesd'],
                  ['jesd','jes','Jet Energy Scale','false','true','jesu'],
                 ]
   
   header = open('./inc/SystematicSample.h','w')
   
   header.write('// This file is automatically generated by the script:\n')
   header.write('// scripts/makeSystematicSample.py\n')
   header.write('// do not edit directly\n')
   header.write('\n')
   header.write('#ifndef SYSTEMATICSAMPLE_H\n')
   header.write('#define SYSTEMATICSAMPLE_H\n')
   header.write('\n')
   header.write('#include <string>\n')
   header.write('#include <iostream>\n')
   header.write('\n')
   header.write('class SystematicSample{\n')
   header.write('public:\n\n')
   header.write('   enum Sample{\n')
   
   for systematic in systematics:
      tmp_syst = systematic[0].replace('-','_')
      header.write('      ' + tmp_syst.upper() + ',\n')
   header.write('      NUM_OF_SAMPLES\n')
   header.write('   };\n')
   header.write('   \n')
   
   header.write('   SystematicSample(void){m_sample = NUM_OF_SAMPLES;};\n')
   header.write('   SystematicSample(const Sample sample){SetSample(sample);};\n')
   header.write('   SystematicSample(const std::string sample){SetSample(sample);};\n')
   header.write('   SystematicSample(const SystematicSample& sample){SetSample(sample.sample());};\n')
   header.write('   SystematicSample& operator=(const Sample sample){SetSample(sample);};\n')
   header.write('   SystematicSample& operator=(const std::string sample){SetSample(sample);};\n')
   header.write('   SystematicSample& operator=(const SystematicSample& sample){SetSample(sample.sample());};\n')
   header.write('   \n')
   header.write('   void SetSample(const Sample sample){\n')
   header.write('      m_sample=sample;\n')
   header.write('      m_sampleString=GetSampleString(m_sample);\n')
   header.write('      m_isUp = GetUp(m_sample);\n')
   header.write('      m_isDown = GetDown(m_sample);\n')
   header.write('   };\n')
   header.write('   \n')
   header.write('   void SetSample(const std::string sample){\n')
   header.write('      SetSample(GetSampleEnum(sample));\n')
   header.write('      if(m_sample == NUM_OF_SAMPLES){\n')
   header.write('         std::cerr << "[SystematicSample::SetSample] Error, incorrect sample string passed " << sample << "\\n";\n')
   header.write('      }\n')
   header.write('   }\n')
   header.write('   \n')
   header.write('   bool isUp(void){return m_isUp;};\n')
   header.write('   bool isDown(void){return m_isDown;};\n')
   header.write('   \n')
   header.write('   std::string str(void) const {return m_sampleString;};\n')
   header.write('   Sample sample(void) const {return m_sample;};\n')
   header.write('   \n')
   header.write('   static Sample GetSampleEnum(const std::string sample){\n')
   
   for systematic in systematics:
      if systematic[0] == systematics[0][0]:
         header.write('      if(sample.find("' + systematic[0] +'") != std::string::npos){\n')
      else:
         header.write('      } else if(sample.find("' + systematic[0] +'") != std::string::npos){\n')
      header.write('         return ' + systematic[0].replace('-','_').upper() + ';\n')
   header.write('      } else{\n')
   header.write('         return NUM_OF_SAMPLES;\n')
   header.write('      }\n')
   header.write('   };\n')
   header.write('   \n')
   header.write('   static std::string GetSampleString(const Sample sample){\n')
   header.write('      switch(sample){\n')
   
   for systematic in systematics:
      header.write('      case ' + systematic[0].replace('-','_').upper() + ':\n')
      header.write('         return "' + systematic[0] + '";\n')
   header.write('      default:\n')
   header.write('         return "";\n')
   header.write('      }\n')
   header.write('   };\n')
   header.write('   \n')
   header.write('   static std::string GetSampleBaseString(const std::string& sample){return GetSampleBaseString(GetSampleEnum(sample));};\n')
   header.write('   static std::string GetSampleBaseString(const SystematicSample& sample){return GetSampleBaseString(sample.sample());};\n')
   header.write('   static std::string GetSampleBaseString(const Sample& sample){\n')
   header.write('      switch(sample){\n')
   for systematic in systematics:
      header.write('      case ' + systematic[0].replace('-','_').upper() + ':\n')
      header.write('         return "' + systematic[1].lower() + '";\n')
   header.write('      default:\n')
   header.write('         return "";\n')
   header.write('      }\n')
   header.write('   };\n')
   header.write('   \n')
   header.write('   // converts output from GetSampleBaseString to a pretty string for tables\n')
   header.write('   static std::string GetSamplePrettyBaseString(const SystematicSample& sample){\n')
   header.write('      return GetSamplePrettyBaseString(sample.sample());\n')
   header.write('   };\n')
   header.write('   static std::string GetSamplePrettyBaseString(const Sample& sample){return GetSamplePrettyBaseString(GetSampleBaseString(sample));};\n')
   header.write('   static std::string GetSamplePrettyBaseString(const std::string& sample){\n')
   for i in range(len(systematics)):
      # skip duplicates
      if i > 0 and systematics[i][1] == systematics[i-1][1]:
         continue
      
      if systematics[i][0] == systematics[0][0]:
         header.write('      if(sample.find("' + systematics[i][1].lower() + '") != std::string::npos){\n')
      else:
         header.write('      } else if(sample.find("' + systematics[i][1].lower() + '") != std::string::npos){\n')
      header.write('         return "' + systematics[i][2] + '";\n')
      
   header.write('      }else{\n')
   header.write('         return sample;\n')
   header.write('      }\n')
   header.write('   };\n')
   header.write('   \n')
   header.write('   static bool GetUp(const std::string systematic){GetUp(GetSampleEnum(systematic));};\n')
   header.write('   static bool GetUp(const Sample sample){\n')
   header.write('      switch(sample){\n')
   
   for systematic in systematics:
      header.write('      case ' + systematic[0].replace('-','_').upper() + ':\n')
      header.write('         return ' + systematic[3].lower() + ';\n')
   header.write('      default:\n')
   header.write('         return false;\n')
   header.write('      }\n')
   header.write('   };\n')
   header.write('   \n')
   header.write('   static bool GetDown(const std::string systematic){GetDown(GetSampleEnum(systematic));};\n')
   header.write('   static bool GetDown(const Sample sample){\n')
   header.write('      switch(sample){\n')
   
   for systematic in systematics:
      header.write('      case ' + systematic[0].replace('-','_').upper() + ':\n')
      header.write('         return ' + systematic[4].lower() + ';\n')
   header.write('      default:\n')
   header.write('         return false;\n')
   header.write('      }\n')
   header.write('   };\n')
   header.write('   \n')
   header.write('   static Sample GetPartner(const Sample sample){\n')
   header.write('      switch(sample){\n')
   
   for systematic in systematics:
      header.write('      case ' + systematic[0].replace('-','_').upper() + ':\n')
      header.write('         return ' + systematic[5].replace('-','_').upper() + ';\n')
   header.write('      default:\n')
   header.write('         return NUM_OF_SAMPLES;\n')
   header.write('      }\n')
   header.write('   };\n')
   header.write('   \n')
   header.write('private:\n')
   header.write('   Sample m_sample;\n')
   header.write('   std::string m_sampleString;\n')
   header.write('   \n')
   header.write('   bool m_isUp;\n')
   header.write('   bool m_isDown;\n')
   header.write('   \n')
   header.write('   };\n')
   header.write('   \n')
   header.write('   \n')
   header.write('   #endif\n')
   header.write('   \n')
   header.write('   \n')
   
   
   header.close()
   
   


if __name__ == "__main__":
   main()
