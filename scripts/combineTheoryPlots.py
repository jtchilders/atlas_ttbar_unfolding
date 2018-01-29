#! /usr/bin/env python26

import os
from ROOT import gROOT, TCanvas, TF1, TFile, TObject


def main():
   gROOT.Reset()
   
   outputFilename = 'Theory_MCFM.root'
   
   histoBase = '_MCFM_scale'
   massScales = ['172','345','690']
   kinVar_histobases = ['pttop','pttt','Mtt']
   
   kinVar_newhistobases = ['toppT_MCFM_','pTtt_MCFM_','Mtt_MCFM_']
   newMassScales = ['mtop','2mtop','4mtop']
   
   fileLocation = 'root://eosatlas//eos/atlas/user/c/childers/September_4'
   
   filenameBase = 'Theory_MCFM_'
   kinVar_filenames = ['pTt','pTtt','Mtt']
   filenamePostfix = '.root'
   
   outputFile = TFile(outputFilename,'UPDATE')
   
   for i in range(len(kinVar_histobases)):
      filename = fileLocation + '/' + filenameBase + kinVar_filenames[i] + filenamePostfix
      file = openEosFile(filename)
      if not file:
         print 'error opening file',filename
         return
      
      histobase = kinVar_histobases[i] + histoBase
      newHistobase = kinVar_newhistobases[i]
      for j in range(len(massScales)):
         histogram = file.Get(histobase + massScales[j])
         histogram.SetName(newHistobase + newMassScales[j])
         outputFile.cd()
         histogram.Write(histogram.GetName(),TObject.kOverwrite)
   
   
   
   
   

def openEosFile(filename):
   file = TFile.Open(filename)
   for i in range(5):
      if not file:
         file = TFile.Open(filename)
      else:
         break
   
   return file


if __name__ == "__main__":
   main()
