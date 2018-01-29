#! /usr/bin/env python26

import os,sys,subprocess,shlex
from multiprocessing import Pool
from globals import *

# need current path for batch submission
PWD = os.getenv("PWD")



###############################################
### REMEMBER!!!!
### Set globals::useGausSmearing to TRUE!!!
###############################################


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
   
   cmd_list = list()
   
   for half in halves:
      for sampleName in sampleNames:
         regValue = regValues[sampleName]
         for channel in channels:
            entry = Systematic(sampleName = sampleName, 
                                 channel = channel,
                                 runOnLxbatch = runOnLxbatch,
                                 half = half,
                                 regValue = regValue)
            cmd_list.append(entry)
            
   
   return cmd_list



class Systematic:
   """ class to define a single command to runToys for a systematic """
   def __init__(self,exe               = "../bin/compToys",
                     data_path         = "/afs/cern.ch/work/c/carolina/data_14_02_2013",
                     output_file_path  = "/afs/cern.ch/work/c/childers/unfoldedData/March_4/McAtNlo/MCClosure",
                     log_file_path     = "../logFiles",
                     method            = '-s',
                     nToys             = 5000,
                     nIter             = 4,
                     regValue          = 4,
                     sampleName        = 'Tag1_Top1',
                     channel           = 'el',
                     half              = 'half0',
                     dataHistoName     = '',
                     runOnLxbatch      = False
      ):
      self.exe                = exe
      if runOnLxbatch:
         self.exe = "bsub -q 8nh " + PWD + "/" + exe
      
      
      self.sampleName         = sampleName
      self.channel            = channel
      self.nToys              = nToys
      self.nIter              = nIter
      self.regValue           = regValue
      self.half               = half
      self.runOnLxbatch       = runOnLxbatch
      self.method             = method
      
      self.data_path          = data_path
      
      #mc file name
      self.mc_file            = self.data_path + "/nominal/subsamples/MC_" + self.channel + "_T1_McAtNlo_Jimmy_nominal_" +  self.half + ".root"     

      #data file name
      self.data_file          = self.data_path + "/nominal/subsamples/MC_" + self.channel + "_T1_McAtNlo_Jimmy_nominal_"
      if   self.half is half0:
         self.data_file       += half1
      elif self.half is half1:
         self.data_file       += half0
      self.data_file += ".root"
      
      
      self.output_file_path   = output_file_path
      self.output_file        = self.output_file_path + "/" + self.get_output_filename() + ".root"
      
      self.log_file_path      = log_file_path
      self.log_full_name      = self.log_file_path + "/" + self.get_output_filename() + ".txt"
      
      
      # build efficiency filename
      self.eff_file           = self.data_path + "/nominal/ForEfficiency/hist/efficiencies_"
      
      # Likelihood or not
      if (self.sampleName.find('SystemMass')      >= 0 or
          self.sampleName.find('SystemPt')        >= 0 or
          self.sampleName.find('SystemRapidity')  >= 0 or
          self.sampleName.find('lhood')           >= 0):
         self.eff_file += "lhood_"
         
      self.eff_file += "nominal_McAtNloJimmy.root"
      
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
      
      self.eff_hist_name += "_nominal"
      
      self.dataHistoName      = dataHistoName
      # Likelihood or not
      #if (self.sampleName.find('SystemMass')      >= 0 or
          #self.sampleName.find('SystemPt')        >= 0 or
          #self.sampleName.find('SystemRapidity')  >= 0 or
          #self.sampleName.find('lhood')           >= 0):
         #self.dataHistoName += "_likelihoodCut"
      
      if(self.sampleName.find("Top1") >= 0):
         #self.dataHistoName += "/TopFit_NTag1/Perm0Top1/Perm0Top1PtSpectrum"
         self.eff_hist_name += "_Top1Pt"
      elif(self.sampleName.find("Top2") >= 0):
         #self.dataHistoName += "/TopFit_NTag1/Perm0Top2/Perm0Top2PtSpectrum"
         self.eff_hist_name += "_Top2Pt"
      elif(self.sampleName.find("SystemPt") >= 0):
         #self.dataHistoName += "/TopFit_NTag1/Perm0System/Perm0SystemPtSpectrum"
         self.eff_hist_name += "_SystemPt"
      elif(self.sampleName.find("SystemMass") >= 0):
         #self.dataHistoName += "/TopFit_NTag1/Perm0System/Perm0SystemMassSpectrum"
         self.eff_hist_name += "_SystemMass"
      elif(self.sampleName.find("SystemRapidity") >= 0):
         #self.dataHistoName += "/TopFit_NTag1/Perm0System/Perm0SystemRapiditySpectrum"
         self.eff_hist_name += "_SystemRapidity"
   
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
      
      cmd += " -e " + "nominal"
      
      cmd += " -g " + self.eff_file
      cmd += " -j " + self.eff_hist_name
      
      cmd += " -t " + str(self.nToys)
      
      if not self.runOnLxbatch:
         cmd += " > " + self.log_full_name + " 2>&1 "
      
      print ">>",cmd
      
      return cmd
   
   def get_output_filename(self):
      filename = "mcatnloHalfClosure_" + self.sampleName + "_" + self.channel + "_" + self.half
      
      filename += "_toy" + str( self.nToys )
      
      if(self.method.find("-s") >= 0):
         filename += "_svd_reg" + str( self.regValue )
      elif(self.method.find("-b") >= 0):
         filename += "_bay_iter" + str( self.nIter )
      elif(self.method.find("-c") >= 0):
         filename += "_bin"
      elif(self.method.find("-f") >= 0):
         filename += "_inv"
      
      return filename
   

if __name__ == "__main__":
   main()
