#! /usr/bin/env python26

import os,sys,subprocess,shlex,math
from array import array
from multiprocessing import Pool
from ROOT import TFile,TH1D,TH1F,TCanvas,TLegend,gStyle
from ROOT import kBlack,kBlue,kRed,kGreen,kYellow
from ROOT import TGraphAsymmErrors,TF1,gROOT,TObject

from AtlasStyle import SetAtlasStyle

def main():
   
   gROOT.SetBatch(1)
   SetAtlasStyle()
   
   can = TCanvas("can","can",0,0,800,600)
   can.SetMargin(0.2,0.05,0.15,0.03)
   
   # open files, get the histograms
   histoname                  = 'h_theoryMtt'
   baseFilePath               = '../data/data_14_02_2013'
   
   filename          = baseFilePath + '/Mtt_7000_Mtt_fin.root'
   file              = TFile(filename)
   
   histos = []
   
   for i in range(7):
      var = i
      if var==0:
         histos.append(file.Get(histoname+'_'))
      elif var>0:
         histos.append(file.Get(histoname+'_'+str(i)))

      if not histos[i]:
         print 'ERROR retreiving',histoname+'_'+str(i),'from',filename
         return
      histos[i].SetName('yangPecjak_'+str(i))
   
   
   output_filename = baseFilePath + '/yangPecjak.root'
   
   outFile = TFile(output_filename,'UPDATE')
   
   nominal = histos[0].Clone()
   nominal.SetName('yangPecjak_withSyst')
   
   # add other plots 1-6 in quadrature as error
   for histo in histos:
      for bin in range(1,nominal.GetNbinsX()+1):
         
         diff = math.fabs(nominal.GetBinContent(bin)-histo.GetBinContent(bin))
         
         nominal.SetBinError(bin,nominal.GetBinError(bin)+diff*diff)
         
   
   # take squareroot
   for bin in range(1,nominal.GetNbinsX()+1):
      nominal.SetBinError(bin,math.sqrt(nominal.GetBinError(bin)))
   
   nominal.SetMarkerStyle(20)
   nominal.SetMarkerColor(kBlack)
   nominal.SetLineColor(kBlack)
   nominal.SetTitle(';M_{t#bar{t}} [GeV]; #frac{d#sigma}{dM_{t#bar{t}}}#left[#frac{pb}{GeV}#right]')
   nominal.Write(nominal.GetName(),TObject.kOverwrite)
   
   can.cd()
   nominal.Draw()
   can.SaveAs("yangPecjak.eps")
   
   outFile.Close()
   



if __name__ == "__main__":
   main()


