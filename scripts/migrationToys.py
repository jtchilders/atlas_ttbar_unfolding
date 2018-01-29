#! /usr/bin/env python26

import os
import subprocess
import shlex
from multiprocessing import Pool

def main():
   # create processing pool with up to 3 concurrent running threads
   pool = Pool(processes=2)
   
   # get the list of command line commands needed to run all the tests
   cmd_list = get_cmd_list()
   
   # run all commands through the subprocess package
   pool.map(run_cmd,cmd_list)
   
   #sys = Systematic(systematic = "nominal")
   #sys.get_command()

def run_cmd(systematic):
   subprocess.call(systematic.get_command(),shell=True)

def get_cmd_list(): 
   systematics = ['nominal','jesu','jesd','btagsfup','btagsfdown','eeru','eerd','eesu','eesd','jeff','jer','muidu','muidd','musc','mumsu','mumsd','celloutu','celloutd','pileupu','pileupd','lep_id_sfup','lep_id_sfdown','lep_reco_sfup','lep_reco_sfdown','lep_trig_sfup','lep_trig_sfdown','wjets_iqopt3','wjets_ptjmin10']
   
   sampleNames = ['Tag1_Top1','Tag1_Top2']
   channels    = ['el','mu']
   
   cmd_list = list()
   
   
   for sampleName in sampleNames:
      
      for channel in channels:
         
         entry = Systematic(sampleName = sampleName, channel = channel)
         
         cmd_list.append(entry)
      
   
   
   return cmd_list
   


class Systematic:
   """ class to define a single command to runToys for a systematic """
   def __init__(self,systematic        = "nominal", 
                     exe               = "../bin/migrationToys",
                     data_path         = "/mnt/ctpfs/carolina/ttbardata/July_11",
                     output_file_path  = "/mnt/ctpfs/carolina/ttbardata/unfolded",
                     log_file_path     = "../logFiles",
                     method            = '-s',
                     nToys             = 1000,
                     nIter             = 4,
                     regValue          = 4,
                     sampleName        = 'Tag1_Top1',
                     channel           = 'el',
                     dataHistoName     = '',
      ):
      self.systematic         = systematic
      self.exe                = exe
      self.exeName            = exe.split('/')[-1]
      
      # set the unfolding method
      # -s   >   SVD
      # -b   >   Bayesian
      # -c   >   BinByBin
      # -f   >   Inversion
      self.method             = method
      self.sampleName         = sampleName
      self.channel            = channel
      self.nToys              = nToys
      self.nIter              = nIter
      self.regValue           = regValue
      
      
      self.data_path          = data_path
      self.mc_file            = self.data_path + "/" + self.systematic + "/subsamples/MC_" + self.channel + "_T1_McAtNlo_Jimmy_" + self.systematic + ".root"
      self.data_file          = self.data_path + "/" + self.systematic + "/ToUnfold_" + self.systematic + "/tagged_"
      
      if(self.channel.find("el") >= 0):
         self.data_file += "ejets.root"
      else:
         self.data_file += "mujets.root"
      
      self.output_file_path   = output_file_path
      self.output_file        = self.output_file_path + "/" + self.exeName + "_" + self.get_output_filename() + ".root"
      
      self.log_file_path      = log_file_path
      self.log_full_name      = self.log_file_path + "/" + self.exeName + "_" + self.get_output_filename() + ".txt"
      
      self.eff_file           = self.data_path + "/" + self.systematic + "/ForEfficiency/hist/efficiencies_" + self.systematic + ".root"
      self.eff_hist_name      = ""
      if(self.sampleName.find("Tag0") >= 0):
         self.eff_hist_name = "pretag_"
      elif(self.sampleName.find("Tag1") >= 0):
         self.eff_hist_name = "tagged_"
      self.eff_hist_name += channel + "_" + self.systematic
      
      self.dataHistoName      = dataHistoName
      if(self.sampleName.find("Top1") >= 0):
         self.dataHistoName = "Top1Pt_DataMinusBG"
         self.eff_hist_name += "_Top1Pt"
      elif(self.sampleName.find("Top2") >= 0):
         self.dataHistoName = "Top2Pt_DataMinusBG"
         self.eff_hist_name += "_Top2Pt"
   
   def get_command(self):
      
      cmd = self.exe
      cmd += " -m " + self.mc_file
      cmd += " -d " + self.data_file
      cmd += " " + self.method + " "
      cmd += " -n " + self.sampleName
      
      if(self.exe.find("compToys") >= 0 or self.exe.find("migrationToys") >= 0):
         cmd += " -t " + str(self.nToys)
      
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
      
      cmd += " > " + self.log_full_name + " 2>&1 "
      
      print ">>",cmd
      
      return cmd
   
   def get_output_filename(self):
      filename = self.systematic + "_" + self.sampleName + "_" + self.channel
      if(self.exe.find("compToys") >= 0 or self.exe.find("migrationToys") >= 0):
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
