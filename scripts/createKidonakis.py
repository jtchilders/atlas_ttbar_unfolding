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
   histoname                  = 'h_theory'
   baseFilePath               = '../data/data_14_02_2013'
   
   filename_topMass           = baseFilePath + '/pttopnnloapprox7lhc173m.root'
   file_topMass               = TFile(filename_topMass)
   histo_m                    = file_topMass.Get(histoname)
   if not histo_m:
      print 'ERROR retreiving',histoname,'from',filename_topMass
      return
   histo_m.SetName('kidonakis_m')
   norm_histo_m = histo_m.Clone('norm_'+histo_m.GetName())
   
   filename_topTransverseMass = baseFilePath + '/pttopnnloapprox7lhc173mT.root'
   file_topTransverseMass     = TFile(filename_topTransverseMass)
   histo_mT                  = file_topTransverseMass.Get(histoname)
   if not histo_mT:
      print 'ERROR retreiving',histoname,'from',filename_topTransverseMass
      return
   histo_mT.SetName('kidonakis_mT')
   norm_histo_mT = histo_mT.Clone('norm_'+histo_mT.GetName())
   
   filename_twiceTopMass      = baseFilePath + '/pttopnnloapprox7lhc1732m.root'
   file_twiceTopMass          = TFile(filename_twiceTopMass)
   histo_2m                   = file_twiceTopMass.Get(histoname)
   if not histo_2m:
      print 'ERROR retreiving',histoname,'from',filename_twiceTopMass
      return
   histo_2m.SetName('kidonakis_2m')
   norm_histo_2m = histo_2m.Clone('norm_'+histo_2m.GetName())
   
   filename_halfTopMass       = baseFilePath + '/pttopnnloapprox7lhc173halfm.root'
   file_halfTopMass           = TFile(filename_halfTopMass)
   histo_halfm                = file_halfTopMass.Get(histoname)
   if not histo_halfm:
      print 'ERROR retreiving',histoname,'from',filename_halfTopMass
      return
   histo_halfm.SetName('kidonakis_halfm')
   norm_histo_halfm = histo_halfm.Clone('norm_'+histo_halfm.GetName())
   
   output_filename = baseFilePath + '/kidonakis.root'
   
   outFile = TFile(output_filename,'UPDATE')
   
   histo_m.Write(histo_m.GetName(),TObject.kOverwrite)
   histo_mT.Write(histo_mT.GetName(),TObject.kOverwrite)
   histo_2m.Write(histo_2m.GetName(),TObject.kOverwrite)
   histo_halfm.Write(histo_halfm.GetName(),TObject.kOverwrite)
   
   # create plot of kidonakis using m as nominal, then others are systematic
   h_m_syst = histo_m.Clone('kidonakis_m_withSyst')
   
   for bin in range(1,h_m_syst.GetNbinsX()+1):
      diff_mT = math.fabs(histo_m.GetBinContent(bin)    - histo_mT.GetBinContent(bin))
      diff_2m = math.fabs(histo_m.GetBinContent(bin)    - histo_2m.GetBinContent(bin))
      diff_halfm = math.fabs(histo_m.GetBinContent(bin) - histo_halfm.GetBinContent(bin))
      
      if(diff_mT > diff_2m and 
         diff_mT > diff_halfm):
         h_m_syst.SetBinError(bin,diff_mT)
      elif(diff_2m > diff_mT and 
           diff_2m > diff_halfm):
         h_m_syst.SetBinError(bin,diff_2m)
      elif(diff_halfm > diff_2m and 
           diff_halfm > diff_mT):
         h_m_syst.SetBinError(bin,diff_halfm)
      
   
   h_m_syst.SetMarkerStyle(20)
   h_m_syst.SetMarkerColor(kBlack)
   h_m_syst.SetLineColor(kBlack)
   h_m_syst.SetTitle(';p_{T}^{t} [GeV];#frac{d#sigma}{dp_{T}^{t}} #left[#frac{pb}{GeV}#right]')
   h_m_syst.Write(h_m_syst.GetName(),TObject.kOverwrite)
   
   can.cd()
   h_m_syst.Draw()
   can.SaveAs('kidonakis.eps')
   
   # create normalized version
   h_m_syst.Scale(1./h_m_syst.Integral('width'))
   h_m_syst.SetTitle(';p_{T}^{t} [GeV];#frac{1}{#sigma} #frac{d#sigma}{dp_{T}^{t}}#left[#frac{1}{GeV}#right]')
   h_m_syst.GetYaxis().SetTitleOffset(1.7)
   h_m_syst.Write(h_m_syst.GetName()+'_norm',TObject.kOverwrite)
   
   can.cd()
   h_m_syst.Draw()
   can.SaveAs('kidonakis_norm.eps')
   
   outFile.Close()
   



if __name__ == "__main__":
   main()


