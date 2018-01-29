#! /usr/bin/env python26

import os,sys
import subprocess
import shlex
from multiprocessing import Pool
from globals import *


###############################################
### REMEMBER!!!!
### Set globals::useGausSmearing to TRUE!!!
###############################################


# need current path for batch submission
PWD = os.getenv("PWD")

# this script runs the compToys executable for
# each unfolding method and for all combinations
# of MC@NLO half0 and half1 subsamples


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
   
   test = False
   
   for unfoldingMethod in unfoldingMethods:
      for half in halves:
         
         for sampleName in sampleNames:
            
            regValue = regValues[sampleName]
            
            for channel in channels:
               
               
               entry = Systematic(
                                  regValue = regValue,
                                  sampleName = sampleName,
                                  channel = channel,
                                  runOnLxbatch = runOnLxbatch,
                                  unfoldingMethod = unfoldingMethod,
                                  half = half
                                 )
               
               cmd_list.append(entry)
               
               if test: break;
            if test: break;
         if test: break;
      if test: break;
   
   
   return cmd_list
   


class Systematic:
   """ class to define a single command to runToys for a systematic """
   def __init__(self,systematic        = 'nominal', 
                     exe               = '../bin/compToys',
                     data_path         = '/afs/cern.ch/work/c/carolina/data_14_02_2013',
                     output_file_path  = '/afs/cern.ch/work/c/childers/unfoldedData/March_4/McAtNlo/UnfoldingMethodTest',
                     log_file_path     = '../logFiles',
                     nToys             = 5000,
                     nIter             = 4,
                     regValue          = 4,
                     sampleName        = LeptonicTop,
                     channel           = 'el',
                     dataHistoName     = '',
                     dataFilename      = '',
                     mcFilename        = '',
                     runOnLxbatch      = False,
                     half              = half0,
                     unfoldingMethod   = SVD
      ):
      self.systematic         = systematic
      self.exe                = exe
      self.queue              = "1nh"
      self.half               = half
      self.runOnLxbatch       = runOnLxbatch
      if runOnLxbatch:
         self.exe = "bsub -q " + self.queue + " " + PWD + "/" + self.exe
      
      self.sampleName         = sampleName
      self.channel            = channel
      self.nToys              = nToys
      self.nIter              = nIter
      self.regValue           = regValue
      
      self.mc_name            = 'McAtNloJimmy'
      self.data_path          = data_path
      self.output_file_path   = output_file_path
      self.log_file_path      = log_file_path
      self.unfoldingMethod    = unfoldingMethod
      
      # MC File
      self.mc_file            = self.data_path + '/' + self.systematic + '/subsamples/MC_' + self.channel + '_T1_McAtNlo_Jimmy_' + self.systematic + '_' + half + '.root'
      # Data File
      self.data_file          = self.data_path + '/' + self.systematic + '/subsamples/MC_' + self.channel + '_T1_McAtNlo_Jimmy_' + self.systematic + '_' + oppositeHalves[halves.index(half)] + '.root'
      
      #output filename
      self.output_file        = self.output_file_path + '/' + self.get_output_filename() + '.root'
      
      # Log File
      self.log_full_name      = self.log_file_path + '/' + self.get_output_filename() + '.txt'
      
      self.eff_file           = self.data_path + '/' + self.systematic + '/ForEfficiency/hist/efficiencies_lhood_' + self.systematic + '_' + self.mc_name + '.root'
      self.eff_hist_name      = ''
      if(self.sampleName.find('Tag0') >= 0):
         self.eff_hist_name   = 'pretag_'
      elif(self.sampleName.find('Tag1') >= 0):
         self.eff_hist_name   = 'tagged_'
      self.eff_hist_name     += channel + '_lhood_' + self.systematic
      
      
      if(self.sampleName.find("Top1") >= 0):
         self.eff_hist_name += "_Top1Pt"
      elif(self.sampleName.find("Top2") >= 0):
         self.eff_hist_name += "_Top2Pt"
      elif(self.sampleName.find("TopLepHad") >= 0):
         self.eff_hist_name += "_TopLepHadPt"
      elif(self.sampleName.find("SystemPt") >= 0):
         self.eff_hist_name += "_SystemPt"
      elif(self.sampleName.find("SystemMass") >= 0):
         self.eff_hist_name += "_SystemMass"
      elif(self.sampleName.find("SystemRapidity") >= 0):
         self.eff_hist_name += "_SystemRapidity"
      
      self.dataHistoName      = dataHistoName
      
      
   
   def get_command(self):
      
      cmd = self.exe
      cmd += ' -m ' + self.mc_file
      cmd += ' -d ' + self.data_file
      cmd += ' -n ' + self.sampleName
      
      cmd += ' -t ' + str(self.nToys)
      
      cmd += ' -o ' + self.output_file
      
      if self.unfoldingMethod == SVD:
         cmd += ' -r ' + str(self.regValue)
         cmd += ' -s '
      elif self.unfoldingMethod == Bayesian:
         cmd += ' -i ' + str(self.nIter)
         cmd += ' -b '
      elif self.unfoldingMethod == binByBin:
         cmd += ' -c '
      elif self.unfoldingMethod == inversion:
         cmd += ' -f '
      
      if( len(self.dataHistoName) > 0):
         cmd += ' -u ' + self.dataHistoName
      
      cmd += ' -g ' + self.eff_file
      cmd += ' -j ' + self.eff_hist_name
      
      if not self.runOnLxbatch:
         cmd += " > " + self.log_full_name + " 2>&1 "
      
      print '>>',cmd
      print ''
      
      return cmd
   
   def get_output_filename(self):
      filename = self.systematic + "_" + self.sampleName + "_" + self.channel
      filename += "_toy" + str( self.nToys )
      
      if(self.unfoldingMethod is SVD):
         filename += "_svd_reg" + str( self.regValue )
      elif(self.unfoldingMethod is Bayesian):
         filename += "_bay_iter" + str( self.nIter )
      elif(self.unfoldingMethod is binByBin):
         filename += "_bin"
      elif(self.unfoldingMethod is inversion):
         filename += "_inv"
      
      filename += '_' + self.half + oppositeHalves[halves.index(self.half)][-1]
      
      return filename
   

if __name__ == "__main__":
   main()
