#! /usr/bin/env python26

import os,sys,subprocess,shlex
from multiprocessing import Pool

# need current path for batch submission
PWD = os.getenv("PWD")
batchQueue = '8nh'

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

def getOppositeHalf(half):
   if half.find('half0') >= 0:
      half = half[0:-1]
      half += '1'
   elif half.find('half1') >= 0:
      half = half[0:-1]
      half += '0'
   return half

def run_cmd(systematic):
   subprocess.call(systematic.get_command(),shell=True)

def get_cmd_list(runOnLxbatch): 

   sampleNames = [
                  #'Tag1_Top1',
                  #'Tag1_Top2',
                  'Tag1_Top1_lhood',
                  'Tag1_Top2_lhood',
                  'Tag1_SystemPt',
                  'Tag1_SystemMass',
                  'Tag1_SystemRapidity',
                  ]
   channels    = [
                  'el',
                  'mu'
                  ]
   halves      = [
                  'half0',
                  'half1'
                 ]
   
   adjust      = [
                  'root500_down',
                  'root900_down',
                  'root500_up',
                  'root900_up',
                  'rootBmp_2',
                  'rootBmp_half',
                  'rootNom',
                 ]
   
   regValues   = {'Tag1_Top1_lhood':4,
                  'Tag1_Top2_lhood':4,
                  'Tag1_SystemPt':3,
                  'Tag1_SystemMass':3,
                  'Tag1_SystemRapidity':4
                 }
   
   cmd_list = list()
   
   for sampleName in sampleNames:
      
      regValue = regValues[sampleName]
      
      for channel in channels:
         
         for half in halves:
            for adjustLabel in adjust:
            
               entry = Systematic(sampleName = sampleName, channel = channel, runOnLxbatch = runOnLxbatch, adjustLabel = adjustLabel, half = half, regValue = regValue)
               
               cmd_list.append(entry)
   
   return cmd_list



class Systematic:
   """ class to define a single command to runToys for a systematic """
   def __init__(self,systematic        = "nominal", 
                     exe               = "../bin/compToys",
                     nominal_data_path = "/afs/cern.ch/work/c/childers/unfoldedData/adjustedSpectrum/dec_5",
                     adjusted_data_path  = "/afs/cern.ch/work/c/childers/unfoldedData/adjustedSpectrum/dec_5",
                     method            = '-s',
                     nToys             = 1000,
                     nIter             = 4,
                     regValue          = 4,
                     sampleName        = 'Tag1_Top1',
                     channel           = 'el',
                     runOnLxbatch      = False,
                     adjustLabel       = 'root500_down',
                     half              = 'half0'
      ):
      self.systematic         = systematic
      self.exe                = exe
      if runOnLxbatch:
         self.exe = "bsub -q " + batchQueue + " " + PWD + "/" + exe
      
      
      # set the unfolding method
      # -s   >   SVD
      # -b   >   Bayesian
      # -c   >   BinByBin
      # -f   >   Inversion
      self.method             = method
      if(self.systematic.find("bayesian") >= 0):
         self.method = " -b "
      if(self.systematic.find("binbybin") >= 0):
         self.method = " -c "
      if(self.systematic.find('inversion') >= 0):
         self.method = " -f "
         
      self.sampleName         = sampleName
      self.channel            = channel
      self.nToys              = nToys
      self.nIter              = nIter
      self.regValue           = regValue
      self.runOnLxbatch       = runOnLxbatch
      
      self.adjustLabel        = adjustLabel
      self.half               = half
      
      self.nominal_data_path  = nominal_data_path
      self.adjusted_data_path = adjusted_data_path
      
      #mc file name
      self.mc_file            = self.nominal_data_path + "/rootNom/MC_" + self.channel + "_T1_McAtNlo_Jimmy_" + self.systematic + "_" + self.half + ".root"
      
      #data file name
      if self.adjustLabel.find('rootNom') >= 0: # for 'rootNom'
         self.data_file = self.nominal_data_path + "/rootNom/MC_" + self.channel + "_T1_McAtNlo_Jimmy_" + self.systematic + "_" + getOppositeHalf(self.half) + ".root"
      else:
         self.data_file          = self.adjusted_data_path + "/" + self.adjustLabel + "/MC_" + self.channel + "_T1_McAtNlo_Jimmy_" + self.systematic + "_" + getOppositeHalf(self.half) + ".root"
      
      # output file name
      self.output_file        = self.adjusted_data_path + "/" + self.adjustLabel + "/" + self.get_output_filename() + ".root"
      
      # log file name
      self.log_full_name      = self.adjusted_data_path + "/" + self.adjustLabel + "/" + self.get_output_filename() + ".txt"
      
      # build efficiency filename
      self.eff_file           = self.nominal_data_path + "/rootNom/efficiencies_lhood_nominal_McAtNloJimmy.root"
      
      # build efficiency histogram name
      self.eff_hist_name      = ""
      if(self.sampleName.find("Tag0") >= 0):
         self.eff_hist_name = "pretag_" + channel
      elif(self.sampleName.find("Tag1") >= 0):
         self.eff_hist_name = "tagged_" + channel
      
      self.eff_hist_name += "_lhood_nominal"
      
      if(self.sampleName.find("Top1") >= 0):
         self.eff_hist_name += "_Top1Pt"
      elif(self.sampleName.find("Top2") >= 0):
         self.eff_hist_name += "_Top2Pt"
      elif(self.sampleName.find("SystemPt") >= 0):
         self.eff_hist_name += "_ttbarPt"
      elif(self.sampleName.find("SystemMass") >= 0):
         self.eff_hist_name += "_ttbarMass"
      elif(self.sampleName.find("SystemRapidity") >= 0):
         self.eff_hist_name += "_ttbarRapidity"
      
   
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
      filename = self.sampleName + "_" + self.channel
      if(self.exe.find("compToys") >= 0):
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
      
      if(self.half.find('half0') >= 0):
         filename += "_half01"
      else:
         filename += "_half10"
      
      return filename
   

if __name__ == "__main__":
   main()
