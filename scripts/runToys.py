#! /usr/bin/env python26

import os,sys,subprocess,shlex
from multiprocessing import Pool

MCmodels     = [
               'McAtNlo',
               'Alpgen'
               ]
systematics = [
               'nominal',
               'jesu','jesd',
               'btagsfup','btagsfdown',
               'ctagsfup','ctagsfdown',
               'mistagsfup','mistagsfdown',
               'eeru','eerd',
               'eesu','eesd',
               'jeff',
               'jer',
               'muidu','muidd',
               'musc',
               'mumsu','mumsd',
               'celloutu','celloutd',
               'pileupu','pileupd',
               'lep_id_sfup','lep_id_sfdown',
               'lep_reco_sfup','lep_reco_sfdown',
               'lep_trig_sfup','lep_trig_sfdown',
               ###'wjets_iqopt3', # deprecated
               ###'wjets_ptjmin10', # deprecated
               'wjets_bb4_down','wjets_bb4_up',
               'wjets_bb5_down','wjets_bb5_up',
               'wjets_bbccc_down','wjets_bbccc_up',
               'wjets_bbcc_down','wjets_bbcc_up',
               'wjets_c4_down','wjets_c4_up',
               'wjets_c5_down','wjets_c5_up',
               'wjets_ca_down','wjets_ca_up',
               'qcdd','qcdu',
               'qcdsd','qcdsu',
               ###'bgstatu','bgstatd', # deprecated
               ###'PowHeg_Jimmy','PowHeg_Pythia', #deprecated
               'IFsrMinus','IFsrPlus',
               'hadronization',
               ##'AlpgenJimmy',  #deprecated
               ##'McAtNloJimmy', #deprecated
               ##'unfolding', # no longer used
               'migrationToys',
               ]
use_jesi = True
jesi_systematics = [
               'BJesUnc-down', 'BJesUnc-up',
               'closeby-down', 'closeby-up',
               'EffectiveNP-DET1-down', 'EffectiveNP-DET1-up', 
               'EffectiveNP-DET2-down','EffectiveNP-DET2-up', 
               'EffectiveNP-MIXED1-down','EffectiveNP-MIXED1-up', 
               'EffectiveNP-MIXED2-down','EffectiveNP-MIXED2-up', 
               'EffectiveNP-MODEL1-down','EffectiveNP-MODEL1-up', 
               'EffectiveNP-MODEL2-down','EffectiveNP-MODEL2-up', 
               'EffectiveNP-MODEL3-down','EffectiveNP-MODEL3-up', 
               'EffectiveNP-MODEL4-down','EffectiveNP-MODEL4-up', 
               'EffectiveNP-STAT1-down','EffectiveNP-STAT1-up', 
               'EffectiveNP-STAT2-down', 'EffectiveNP-STAT2-up', 
               'EffectiveNP-STAT3-down','EffectiveNP-STAT3-up', 
               'EtaIntercalibration-Theory-down','EtaIntercalibration-Theory-up', 
               'EtaIntercalibration-TotalStat-down','EtaIntercalibration-TotalStat-up', 
               'flavor-comp-down','flavor-comp-up', 
               'flavor-response-down', 'flavor-response-up',
               'Pileup-OffsetMu-down', 'Pileup-OffsetMu-up', 
               'Pileup-OffsetNPV-down','Pileup-OffsetNPV-up', 
               'RelativeNonClosure-MC11b-down','RelativeNonClosure-MC11b-up', 
               'SingleParticle-HighPt-down','SingleParticle-HighPt-up',
               'bkgd_jes_up','bkgd_jes_down',
               'oppositeGenerator', # just unfolds the nominal data from the opposite generator
               ]
sampleNames = [
               #'Tag1_Top1',
               #'Tag1_Top2',
               'Tag1_Top1_lhood',
               'Tag1_Top2_lhood',
               #'Tag1_TopLepHad_lhood',
               'Tag1_SystemPt',
               'Tag1_SystemMass',
               'Tag1_SystemRapidity',
               ]
channels    = [
               #'el',
               #'mu',
               'co'
               ]

regValues   = {'Tag1_Top1_lhood':4,
               'Tag1_Top2_lhood':4,
               'Tag1_TopLepHad_lhood':4,
               'Tag1_SystemPt':3,
               'Tag1_SystemMass':3,
               'Tag1_SystemRapidity':4
               }

# need current path for batch submission
PWD = os.getenv("PWD")

def main():
   
   # check if command line option is set
   runOnLxbatch = False
   num_of_procs = 2
   if len(sys.argv) == 2:
      if sys.argv[1] == '-b':
         runOnLxbatch = True
         num_of_procs = 1
      else:
         print 'usage: script [-b]'
         print '    -b    optional flag to make jobs run on lxplus batch queue'
         return
   elif len(sys.argv) > 2:
      print 'usage: script [-b]'
      print '    -b    optional flag to make jobs run on lxplus batch queue'
      return
   
   # create processing pool with up to 3 concurrent running threads
   pool = Pool(processes=num_of_procs)
   
   # get the list of command line commands needed to run all the tests
   cmd_list = get_cmd_list(runOnLxbatch)
   
   # run all commands through the subprocess package
   pool.map(run_cmd,cmd_list)
   
   #sys = Systematic(systematic = "nominal")
   #sys.get_command()

def run_cmd(systematic):
   subprocess.call(systematic.get_command(),shell=True)

def get_cmd_list(runOnLxbatch): 
   global systematics # use systematics global
   
   cmd_list = list()
   
   for MCmodel in MCmodels:
      local_systematics = None
      if MCmodel is 'McAtNlo':
         use_jesi = False
         local_systematics = systematics
      else:
         use_jesi = True
         local_systematics = systematics + jesi_systematics
         if 'jesu' in local_systematics:
            local_systematics.remove('jesu')
         if 'jesd' in local_systematics:
            local_systematics.remove('jesd')
      
      for syst in local_systematics:
         
         for sampleName in sampleNames:
            
            regValue = regValues[sampleName]
            
            for channel in channels:
               if ((syst.find('eer') >= 0) and (channel == 'mu')):
                  print 'skipping',syst,sampleName,channel
                  continue
               if ((syst.find('ees') >= 0) and (channel == 'mu')):
                  print 'skipping',syst,sampleName,channel
                  continue
               if ((syst.find('muid') >= 0) and (channel == 'el')):
                  print 'skipping',syst,sampleName,channel
                  continue
               if ((syst.find('musc') >= 0) and (channel == 'el')):
                  print 'skipping',syst,sampleName,channel
                  continue
               if ((syst.find('mums') >= 0) and (channel == 'el')):
                  print 'skipping',syst,sampleName,channel
                  continue
            
               entry = Systematic(MCmodel = MCmodel, systematic = syst, sampleName = sampleName, channel = channel, runOnLxbatch = runOnLxbatch,regValue = regValue)
               cmd_list.append(entry)
      
   return cmd_list



class Systematic:
   """ class to define a single command to runToys for a systematic """
   def __init__(self,MCmodel           = "McAtNlo",
                     systematic        = "nominal", 
                     exe               = "../bin/compToys",
                     data_path         = "/afs/cern.ch/work/c/carolina/data_14_02_2013",
                     #output_file_path  = "/afs/cern.ch/work/c/childers/unfoldedData/March_4",
                     #data_path         = "/afs/cern.ch/work/c/carolina/test_08_03_2013",
                     output_file_path  = "/afs/cern.ch/work/c/carolina/data_Unfolded_14_02_2013",
                     log_file_path     = "../logFiles",
                     method            = '-s',
                     nToys             = 5000,
                     nIter             = 4,
                     regValue          = 4,
                     sampleName        = 'Tag1_Top1',
                     channel           = 'el',
                     dataHistoName     = '',
                     runOnLxbatch      = False
      ):
      self.MCmodel            = MCmodel
      self.systematic         = systematic
      self.exe                = exe
      self.queue              = "1nh"
      if runOnLxbatch:
         self.exe = "bsub -q " + self.queue + " " + PWD + "/" + exe
      
      
      if self.systematic.find("migrationToys") >= 0:
         if runOnLxbatch:
            self.exe = "bsub -q " + self.queue + " " + PWD + "/../bin/migrationToys"
         else:
            self.exe = "../bin/migrationToys"
      
      # set the unfolding method
      # -s   >   SVD
      # -b   >   Bayesian
      # -c   >   BinByBin
      # -f   >   Inversion
      self.method             = method
      if(self.systematic.find('unfolding') >= 0):
         if(self.method.find('-s') >= 0):
            self.method = ' -b '
         elif(self.method.find('-b') >= 0):
            self.method = ' -s '
      
      self.sampleName         = sampleName
      self.channel            = channel
      self.nToys              = nToys
      self.nIter              = nIter
      self.regValue           = regValue
      self.runOnLxbatch       = runOnLxbatch
      
      self.data_path          = data_path
      
      #mc file name
      self.get_mc_filename()
      
      #data file name
      self.get_data_filename()
      
      # get output filename
      self.output_file_path   = output_file_path + "/" + self.MCmodel
      self.output_file        = self.output_file_path + "/" + self.get_output_filename() + ".root"
      
      # get log filename
      self.log_file_path      = log_file_path
      self.log_full_name      = self.log_file_path + "/" + self.get_output_filename() + ".txt"
      
      
      # build efficiency filename
      self.get_efficiency_filename()
      
      # build efficiency histogram name
      self.eff_hist_name      = ""
      if(self.sampleName.find("Tag0") >= 0):
         self.eff_hist_name = "pretag_" + channel
      elif(self.sampleName.find("Tag1") >= 0):
         self.eff_hist_name = "tagged_" + channel
         
      # Likelihood or not
      if (self.sampleName.find('SystemMass')      >= 0 or
          self.sampleName.find('SystemPt')        >= 0 or
          self.sampleName.find('SystemRapidity')  >= 0 or
          self.sampleName.find('lhood')           >= 0):
         self.eff_hist_name += "_lhood"
      
      if (self.systematic.find('qcd')           >= 0 or
          self.systematic.find('bgstat')        >= 0 or 
          self.systematic.find('PowHeg_')       >= 0 or
          self.systematic.find('IFsr')          >= 0 or
          self.systematic.find('hadronization') >= 0 or
          self.systematic.find('AlpgenJimmy')   >= 0 or
          self.systematic.find('McAtNloJimmy')  >= 0 or
          self.systematic.find("unfolding")     >= 0 or
          self.systematic.find("migrationToys") >= 0 or
          self.systematic.find("wjets_")        >= 0 or
          self.systematic.find("bkgd_jes")      >= 0 or
          self.systematic.find("oppositeGenerator") >= 0):
         self.eff_hist_name += "_nominal"
      else:
         self.eff_hist_name += "_" + self.systematic
      
      self.dataHistoName      = dataHistoName
      if(self.sampleName.find("Top1") >= 0):
         self.dataHistoName = "Top1Pt_DataMinusBG"
         self.eff_hist_name += "_Top1Pt"
      elif(self.sampleName.find("Top2") >= 0):
         self.dataHistoName = "Top2Pt_DataMinusBG"
         self.eff_hist_name += "_Top2Pt"
      elif(self.sampleName.find("TopLepHad") >= 0):
         self.dataHistoName = "TopLepHadPt_DataMinusBG"
         self.eff_hist_name += "_TopLepHadPt"
      elif(self.sampleName.find("SystemPt") >= 0):
         self.dataHistoName = "SystemPt_DataMinusBG"
         self.eff_hist_name += "_SystemPt"
      elif(self.sampleName.find("SystemMass") >= 0):
         self.dataHistoName = "SystemMass_DataMinusBG"
         self.eff_hist_name += "_SystemMass"
      elif(self.sampleName.find("SystemRapidity") >= 0):
         self.dataHistoName = "SystemRapidity_DataMinusBG"
         self.eff_hist_name += "_SystemRapidity"
               
      # use MC as data for these systematics
      if (self.systematic.find('PowHeg_')       >= 0 or
          self.systematic.find('IFsr')          >= 0 or
          self.systematic.find('parton_shower') >= 0):
         self.dataHistoName = ''
   
   def get_command(self):
      
      cmd = self.exe
      cmd += " -m " + self.mc_file
      cmd += " -d " + self.data_file
      cmd += " " + self.method + " "   
      cmd += " -n " + self.sampleName
      
      cmd += " -o " + self.output_file
      
      if(self.method.find("-s") >= 0):
         cmd += " -r " + str(self.regValue)
      elif(self.method.find("-b") >= 0):
         cmd += " -i " + str(self.nIter)
      
      if( len(self.dataHistoName) > 0):
         cmd += " -u " + self.dataHistoName
      
      cmd += " -e " + self.systematic
      
      cmd += " -g " + self.eff_file
      cmd += " -j " + self.eff_hist_name
      
      if(self.exe.find("compToys") >= 0 or self.exe.find("migrationToys") >= 0):
         cmd += " -t " + str(self.nToys)
      
      if not self.runOnLxbatch:
         cmd += " > " + self.log_full_name + " 2>&1 "
      
      print ">>",cmd
      
      return cmd
   
   def get_output_filename(self):
      filename = self.systematic + "_" + self.sampleName + "_" + self.channel
      if(self.exe.find("compToys") >= 0 or self.exe.find("migrationToys") >= 0):
         filename += "_toy" + str( self.nToys )
      
      if (not (self.systematic.find("bayesian")  >= 0 or
               self.systematic.find("binbybin")  >= 0 or
               self.systematic.find('inversion') >= 0
               )
         ):
         if(self.method.find("-s") >= 0):
            filename += "_svd_reg" + str( self.regValue )
         elif(self.method.find("-b") >= 0):
            filename += "_bay_iter" + str( self.nIter )
         elif(self.method.find("-c") >= 0):
            filename += "_bin"
         elif(self.method.find("-f") >= 0):
            filename += "_inv"
      
      return filename
   
   def get_mc_filename(self):
      #create mc file name
      mc_name = ""
      if (self.MCmodel.find('McAtNlo')           >= 0):
         mc_name      =  "_T1_McAtNlo_Jimmy_"
      elif self.MCmodel.find('Alpgen')           >= 0:
         mc_name      =  "_AlpgenJimmyttbarlnqq_"
      
      # for qcd/PowHeg/PDFs/ISFR/unfoldingMethods just use the nominal MC files
      if (self.systematic.find('qcd')           >= 0 or
          self.systematic.find('PowHeg_')       >= 0 or
          self.systematic.find('MRSTMCal')      >= 0 or
          self.systematic.find('CTEQ66')        >= 0 or
          self.systematic.find('MRST2007')      >= 0 or
          self.systematic.find('NNPDF20')       >= 0 or
          self.systematic.find('IFsr')          >= 0 or
          self.systematic.find('unfolding')     >= 0 or
          self.systematic.find('migrationToys') >= 0 or
          self.systematic.find('bgstat')        >= 0 or
          self.systematic.find('wjets_')        >= 0 or
          self.systematic.find('bkgd_jes')      >= 0 or
          self.systematic.find('oppositeGenerator') >= 0):
         self.mc_file = self.data_path + "/nominal/MC_" + self.channel + mc_name + "nominal.root"
      elif self.systematic.find('hadronization')  >= 0:
         self.mc_file = self.data_path + '/nominal/MC_' + self.channel + '_AlpGenPythia_P2011_CTEQ5L_ttbarlnqq_nominal.root'
      else:
         self.mc_file  = self.data_path + "/" + self.systematic + "/MC_" + self.channel + mc_name + self.systematic + ".root"
   
   
   def get_data_filename(self):
      #data file name
      mc_name = ''
      oppositeGenerator = ''
      if self.MCmodel.find('McAtNlo')            >= 0:
         mc_name      =  "_McAtNloJimmy"
         oppositeGenerator = '_AlpgenJimmy'
      elif self.MCmodel.find('Alpgen')           >= 0:
         mc_name      =  "_AlpgenJimmy"
         oppositeGenerator = '_McAtNloJimmy'
      
      # for PowHeg and IFSR use an MC file in the nominal folder to unfold
      if self.systematic.find('PowHeg_') >= 0:
         self.data_file       = self.data_path + "/nominal/MC_" + self.channel + "_AF2_mc11b_TTbar_" + self.systematic + "_nominal.root"
      elif (self.systematic.find('IFsrMinus') >= 0):
         #self.data_file       = self.data_path + "/nominal/MC_" + self.channel + "_AcerMCttbar_Perugia2011C_LessPS_nominal.root"
         self.data_file       = self.data_path + "/nominal/MC_" + self.channel + "_AlpGenPythia_P2011radLo_KTFac2CTEQ5L_ttbarlnqq_nominal.root"
      elif (self.systematic.find('IFsrPlus') >= 0):
         #self.data_file       = self.data_path + "/nominal/MC_" + self.channel + "_AcerMCttbar_Perugia2011C_MorePS_nominal.root"
         self.data_file       = self.data_path + "/nominal/MC_" + self.channel + "_AlpGenPythia_P2011radHi_KTFac05CTEQ5L_ttbarlnqq_nominal.root"        
      elif self.systematic.find('parton_shower_up') >= 0:
         self.data_file       = self.data_path + '/nominal/MC_' + self.channel + '_AF2_Top_moreISR_nominal.root'
      #self.data_file       = self.data_path + '/nominal/MC_' + self.channel + '_AF2_mc11b_TTbar_PowHeg_Jimmy_nominal.root'
      elif self.systematic.find('parton_shower_down') >= 0:
         self.data_file       = self.data_path + '/nominal/MC_' + self.channel + '_AF2_Top_lessISR_nominal.root'
      #self.data_file       = self.data_path + '/nominal/MC_' + self.channel + '_AF2_mc11b_TTbar_PowHeg_Pythia_nominal.root'
      else:
         
         # use the nominal data to unfold
         if (self.systematic.find('MRSTMCal')      >= 0 or
             self.systematic.find('CTEQ66')        >= 0 or
             self.systematic.find('MRST2007')      >= 0 or
             self.systematic.find('NNPDF20')       >= 0 or
             self.systematic.find('unfolding')     >= 0 or
             self.systematic.find('migrationToys') >= 0 or
             IsJESComponentSyst(self.systematic)):
            self.data_file       = self.data_path + "/nominal/ToUnfold_nominal" + mc_name + "/"
         elif self.systematic.find('hadronization') >= 0:
            self.data_file       = self.data_path + "/nominal/ToUnfold_nominal_Alpgen_Pythia/"
         elif self.systematic.find('oppositeGenerator') >= 0:
            self.data_file       = self.data_path + '/nominal/ToUnfold_nominal' + oppositeGenerator + '/'
         elif 'bkgd_jes_up' in self.systematic:
            self.data_file       = self.data_path + '/jesu/ToUnfold_jesu' + mc_name + '/'
         elif 'bkgd_jes_down' in self.systematic:
            self.data_file       = self.data_path + '/jesd/ToUnfold_jesd' + mc_name + '/'
         else:
            self.data_file       = self.data_path + "/" + self.systematic + "/ToUnfold_" + self.systematic + mc_name +"/"
         
         # Likelihood or not
         if (self.sampleName.find('SystemMass')      >= 0 or
             self.sampleName.find('SystemPt')        >= 0 or
             self.sampleName.find('SystemRapidity')  >= 0 or
             self.sampleName.find('lhood')           >= 0):
            self.data_file  += "lhood_tagged_"
         else:
            self.data_file  += "tagged_" 
         
         # add trailing bit
         if(self.channel.find("el") >= 0):
            self.data_file += "ejets.root"
         elif(self.channel.find("mu") >= 0):
            self.data_file += "mujets.root"
         elif(self.channel.find("co") >= 0):
            self.data_file += "co.root" 
   
   def get_efficiency_filename(self):
      # build efficiency filename
      mc_name = ''
      if (self.MCmodel.find('McAtNlo')           >= 0):
         mc_name      =  "_McAtNloJimmy"
      elif self.MCmodel.find('Alpgen')           >= 0:
         mc_name      =  "_AlpgenJimmy"
      
      self.eff_file           = self.data_path + "/" + self.systematic +  "/ForEfficiency/hist/efficiencies_"
      if (self.systematic.find('PowHeg_')          >= 0 or
          self.systematic.find('IFsr')           >= 0 or
          self.systematic.find('hadronization')  >= 0 or
          self.systematic.find("unfolding")      >= 0 or
          self.systematic.find("migrationToys")  >= 0 or
          self.systematic.find('MRSTMCal')       >= 0 or
          self.systematic.find('NNPDF20')        >= 0 or
          self.systematic.find('CTEQ66')         >= 0 or
          self.systematic.find('MRST2007')       >= 0 or
          self.systematic.find('AlpgenJimmy')    >= 0 or
          self.systematic.find('McAtNloJimmy')   >= 0 or
          self.systematic.find('qcd')            >= 0 or
          self.systematic.find('wjets_')         >= 0 or
          self.systematic.find('bkgd_jes')       >= 0 or
          self.systematic.find('oppositeGenerator') >= 0):
         self.eff_file           = self.data_path + "/nominal/ForEfficiency/hist/efficiencies_"
      
      # Likelihood or not
      if (self.sampleName.find('SystemMass')      >= 0 or
          self.sampleName.find('SystemPt')        >= 0 or
          self.sampleName.find('SystemRapidity')  >= 0 or
          self.sampleName.find('lhood')           >= 0):
         self.eff_file += "lhood_"
      
      #particular cases    
      if (self.systematic.find('qcd')           >= 0 or
          self.systematic.find('bgstat')        >= 0 or
          self.systematic.find('PowHeg_')       >= 0 or
          self.systematic.find('IFsr')          >= 0 or
          self.systematic.find('parton_shower') >= 0 or
          self.systematic.find("unfolding")     >= 0 or
          self.systematic.find("migrationToys") >= 0 or
          self.systematic.find("wjets_")        >= 0 or
          self.systematic.find("bkgd_jes")        >= 0 or
          self.systematic.find("oppositeGenerator") >= 0):
         self.eff_file += "nominal" + mc_name +".root"
      elif self.systematic.find('hadronization') >= 0:
         self.eff_file += "nominal_AlpGenPythia_P2011_CTEQ5L_ttbarlnqq.root"
      else:
         self.eff_file += self.systematic + mc_name + ".root"
      

def IsJESComponentSyst(systematic):
   if('SingleParticle-HighPt'          in systematic or
      'BJesUnc'                        in systematic or
      'closeby'                        in systematic or
      'EffectiveNP-DET1'               in systematic or
      'EffectiveNP-DET2'               in systematic or
      'EffectiveNP-MIXED1'             in systematic or
      'EffectiveNP-MIXED2'             in systematic or
      'EffectiveNP-MODEL1'             in systematic or
      'EffectiveNP-MODEL2'             in systematic or
      'EffectiveNP-MODEL3'             in systematic or
      'EffectiveNP-MODEL4'             in systematic or
      'EffectiveNP-STAT1'              in systematic or
      'EffectiveNP-STAT2'              in systematic or
      'EffectiveNP-STAT3'              in systematic or
      'EtaIntercalibration-Theory'     in systematic or
      'EtaIntercalibration-TotalStat'  in systematic or
      'flavor-comp'                    in systematic or
      'flavor-response'                in systematic or
      'Pileup-OffsetMu'                in systematic or
      'Pileup-OffsetNPV'               in systematic or
      'RelativeNonClosure-MC11b'       in systematic):
      return True
   return False

if __name__ == "__main__":
   main()
