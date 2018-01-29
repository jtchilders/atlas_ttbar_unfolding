#! /usr/bin/env python26

import os,sys,subprocess,shlex,math
from array import array
from multiprocessing import Pool
from ROOT import TFile,TH1D,TH1F,TCanvas,TLegend,gStyle,TLatex
from ROOT import kBlack,kBlue,kRed,kGreen,kYellow,kGray
from ROOT import TGraphAsymmErrors,TF1,gDirectory,gROOT
from AtlasStyle import SetAtlasStyle
from globals import *


###############################################
### REMEMBER!!!!
### Set globals::useGausSmearing to TRUE!!!
###############################################



def main():
   
   gROOT.SetBatch(1)
   SetAtlasStyle()
   
   toys       = 'toy5000'
   systematic = 'nominal'
   method_forFile = 'svd'
   method_forHist = 'SVD'
   regValue   = 'reg'
   baseRecoFileName = 'mcatnloHalfClosure'
   
   baseFilePath = '../data/March_4/McAtNlo/MCClosure'
   
   can = TCanvas("can","can",0,0,800,600)
   can.SetMargin(0.2,0.05,0.15,0.03)
   
   legend = TLegend(0.25,0.93,0.5,0.6,"leptonic top")
   legend.SetFillStyle(0)
   legend.SetBorderSize(0)
   firstEntry = True
   
   
   horizontalAt0 = TF1("horizontalAt0","[0]+x*[1]",-100000,100000)
   horizontalAt0.SetParameter(0,0)
   horizontalAt0.SetParameter(1,0)
   horizontalAt0.SetLineColor(kBlack)
   horizontalAt0.SetLineStyle(9)
   horizontalAt0.SetLineWidth(1)
   
   # loop over the sample names since each one needs its own plots
   for sampleName in sampleNames:
      print 'loop',sampleName
      
      x_title = ''
      y_title = "#left(#frac{d#sigma}{dX}|_{unf} - #frac{d#sigma}{dX}|_{tru}#right)#times#left(#frac{d#sigma}{dX}|_{tru}#right)^{-1}"
      latex_label = ''
      if sampleName is 'Tag1_SystemMass':
         x_title = 'M_{t#bar{t}} [GeV]'
      elif sampleName is 'Tag1_SystemPt':
         x_title = 'p_{T}^{t#bar{t}} [GeV]'
      elif sampleName is 'Tag1_Top1_lhood':
         x_title = 'leptonic top p_{T} [GeV]'
      elif sampleName is 'Tag1_Top2_lhood':
         x_title = 'hadronic top p_{T} [GeV]'
      
      # loop over the channels since they will also each get their own plots
      for channel in channels:
         print 'loop',channel
         
         outFilename = baseFilePath + '/mcatnloHalfClosure_' + sampleName + '_' + channel + '.eps'
         
         #loop over the subsamples and load plots
         half_histos = {}
         for half in halves:
            inFilename = (baseFilePath+'/'+
                           baseRecoFileName+'_'+sampleName+'_'+
                           channel+'_'+half+'_'+toys+'_'+
                           method_forFile+'_'+regValue+str(regValues[sampleName])+'.root')
            inFile = TFile(inFilename)
            if not inFile.IsOpen():
               print 'ERROR opening input file:',inFilename
               return
            
            baseHistoPath = ('unfolding/toys/'+systematic+'/'+channel+'/'+
                           sampleName+'/'+method_forHist+'/'+regValue+str(regValues[sampleName])+'/'+toys+'/')
            baseHistoName = ('H_'+channel+'_'+sampleName+'_'+method_forHist+'_'+
                           regValue+str(regValues[sampleName])+'_'+toys+'_')
            
            
            histos = {}
            
            truth_name = baseHistoPath+baseHistoName+'mc_truth_diffxs'
            histos['truth'] = inFile.Get(truth_name)
            if not histos['truth']:
               print 'ERROR could not load histogram',truth_name,'from file',inFilename
               return
            histos['truth'].SetDirectory(0)
            
            truth_norm_name = baseHistoPath+baseHistoName+'mc_truth_diffxsNorm'
            histos['truthNorm'] = inFile.Get(truth_norm_name)
            if not histos['truthNorm']:
               print 'ERROR could not load histogram',truth_norm_name,'from file',inFilename
               return
            histos['truthNorm'].SetDirectory(0)
            
            reco_name = baseHistoPath+baseHistoName+'mc_reco_diffxs'
            histos['reco'] = inFile.Get(reco_name)
            if not histos['reco']:
               print 'ERROR could not load histogram',reco_name,'from file',inFilename
               return
            histos['reco'].SetDirectory(0)
            
            measured_name = baseHistoPath+baseHistoName+'data_measured'
            histos['measured'] = inFile.Get(measured_name)
            if not histos['measured']:
               print 'ERROR could not load histogram',measured_name,'from file',inFilename
               return
            histos['measured'].SetDirectory(0)
            
            diffxs_name = baseHistoPath+baseHistoName+'data_unfolded_diffxs'
            histos['diffxs'] = inFile.Get(diffxs_name)
            if not histos['diffxs']:
               print 'ERROR could not load histogram',diffxs_name,'from file',inFilename
               return
            histos['diffxs'].SetDirectory(0)
            
            
            diffxs_norm_name = baseHistoPath+baseHistoName+'data_unfolded_diffxsNorm'
            histos['diffxsNorm'] = inFile.Get(diffxs_norm_name)
            if not histos['diffxsNorm']:
               print 'ERROR could not load histogram',diffxs_norm_name,'from file',inFilename
               return
            histos['diffxsNorm'].SetDirectory(0)
            
            half_histos[half] = histos
            
            inFile.Close()
            # end half
         
         
         # ratio of reco from half1 unfolded using migration matrix from half0
         # over the truth from half1
         ratio0 = GetRatioPlot(half_histos[half0]['diffxs'],half_histos[half1]['truth'])
         ratio0.SetName('ratio0')
         ratio0.GetXaxis().SetTitle(x_title)
         ratio0.GetYaxis().SetTitle(y_title)
         # ratio of reco from half0 unfolded using migration matrix from half1
         # over the truth from half0
         ratio1 = GetRatioPlot(half_histos[half1]['diffxs'],half_histos[half0]['truth'])
         ratio1.SetName('ratio1')
         ratio1.GetXaxis().SetTitle(x_title)
         ratio1.GetYaxis().SetTitle(y_title)
         
         can.cd();
         
         if sampleName is LeptonicTop:
            ratio0.SetMaximum(0.14)
            ratio0.SetMinimum(-0.08)
         elif sampleName is HadronicTop:
            if channel is ejets:
               ratio0.SetMaximum(0.08)
               ratio0.SetMinimum(-0.08)
            elif channel is mujets:
               ratio0.SetMaximum(0.12)
               ratio0.SetMinimum(-0.12)
         elif sampleName is SystemMass:
            ratio0.SetMaximum(0.08)
            ratio0.SetMinimum(-0.06)
         elif sampleName is SystemPt:
            ratio0.SetMaximum(0.15)
            ratio0.SetMinimum(-0.15)
         elif sampleName is SystemRapidity:
            ratio0.SetMaximum(0.06)
            ratio0.SetMinimum(-0.04)
         
         ratio0.SetLineWidth(3)
         ratio0.Draw()
         ratio1.SetMarkerColor(kRed)
         ratio1.SetLineColor(kRed)
         ratio1.SetLineWidth(2)
         ratio1.Draw('same')
         
         horizontalAt0.DrawCopy('same')
         
         channel_label = latex_label
         if channel is 'el':
            channel_label += ' e+jets'
         elif channel is 'mu':
            channel_label += ' \\mu+jets'
         
         
         leg = TLegend(0.23,0.92,0.55,0.75,channel_label)
         leg.SetBorderSize(0)
         leg.SetFillStyle(0)
         leg.AddEntry(ratio0,'M_{half0}*Reco_{half1}','lp')
         leg.AddEntry(ratio1,'M_{half1}*Reco_{half0}','lp')
         #leg.AddEntry(ratio,'combined','lp')
         
         leg.Draw('same')
         
         can.SaveAs(outFilename)
         
         
         
         
         # end channel
      
      
      # end sampleName
   
   


def getRatio(diffxs,truth):
   
   histo = TH1F(diffxs)
   histo.SetDirectory(0)
   
   histo.Add(truth,-1)
   histo.Divide(truth)
   
   return histo

def GetRatioValue(unfolded,truth):
   return (unfolded-truth)/truth

def GetRatioUncertainty(unfolded,unfoldedUncertainty,truth,truthUncertainty):
   a = unfoldedUncertainty/truth
   b = truthUncertainty*(-unfolded/(truth*truth))
   return math.sqrt(a*a + b*b)

def GetRatioPlot(unfolded,truth):
   output = unfolded.Clone()
   
   for bin in range(1,output.GetNbinsX()+1):
      output.SetBinContent(bin,GetRatioValue(unfolded.GetBinContent(bin),truth.GetBinContent(bin)))
      output.SetBinError(bin,GetRatioUncertainty(unfolded.GetBinContent(bin),unfolded.GetBinError(bin),truth.GetBinContent(bin),truth.GetBinError(bin)))
   
   return output

if __name__ == "__main__":
   main()


