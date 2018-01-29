#! /usr/bin/env python26

import os,sys,subprocess,shlex,math
from array import array
from multiprocessing import Pool
from ROOT import TFile,TH1D,TH1F,TCanvas,TLegend,gStyle,TLatex
from ROOT import kBlack,kBlue,kRed,kGreen,kYellow
from ROOT import TGraphAsymmErrors,TF1,gDirectory,gROOT

def main():
   
   gROOT.SetBatch(1)
   
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
   
   channels = ['el','mu']
   
   generators  = ['Alpgen','McAtNlo']
   
   toys       = 'toy5000'
   systematic = 'nominal'
   method_forFile = 'svd'
   method_forHist = 'SVD'
   regValue   = 'reg4'
   baseRecoFileName = 'RecoClosure'
   
   baseFilePath = '../data/March_4/Alpgen/MCClosure'
   
   can = TCanvas("can","can",0,0,800,600)
   can.SetMargin(0.2,0.05,0.15,0.03)
   
   
   # loop over the sample names since each one needs its own plots
   for sampleName in sampleNames:
      print 'loop',sampleName
      
      x_title = ''
      y_title = "ratio of unfolded over truth #frac{dN}{d"
      y_max = 1.2
      y_min = 0.8
      latex_label = ''
      if sampleName is 'Tag1_SystemMass':
         x_title = 'M_{t#bar{t}} [GeV]'
         y_title += 'M_{t#bar{t}}}'
         latex_label = 't\\bar{t} Mass: '
      elif sampleName is 'Tag1_SystemPt':
         x_title = 'p_{T}^{t#bar{t}} [GeV]'
         y_title += 'p_{T}^{t#bar{t}}}'
         latex_label = 't\\bar{t} p_{T}: '
         y_max = 1.6
         y_min = 0.6
      elif sampleName is 'Tag1_SystemRapidity':
         x_title = 'y_{t#bar{t}}'
         y_title += 'y_{t#bar{t}}}'
         latex_label = 't\\bar{t} Rapidity: '
      elif sampleName is 'Tag1_Top1_lhood':
         x_title = 'leptonic top p_{T} [GeV]'
         y_title += 'p_{T}^{t}}'
         latex_label = 'top p_{T}: '
      elif sampleName is 'Tag1_Top2_lhood':
         x_title = 'hadronic top p_{T} [GeV]'
         y_title += 'p_{T}^{t}}'
         latex_label = 'top p_{T}: '
      
      # loop over the channels since they will also each get their own plots
      for channel in channels:
         print 'loop',channel
         
         outFilename = baseFilePath + '/alpgenMcAtNloClosureTest_' + sampleName + '_' + channel + '.eps'
         
         #loop over the generator files and load plots
         genhistos = {}
         for generator in generators:
            # filename where plots are stored
            inFilename = (baseFilePath+'/'+
                           baseRecoFileName+'_'+sampleName+'_'+
                           channel+'_unfoldWith'+generator+'_'+toys+'_'+
                           method_forFile+'_'+regValue+'.root')
            inFile = TFile(inFilename)
            if not inFile.IsOpen():
               print 'ERROR opening input file:',inFilename
               return
            
            # build histogram names to retrieve
            baseHistoPath = ('unfolding/toys/'+systematic+'/'+channel+'/'+
                           sampleName+'/'+method_forHist+'/'+regValue+'/'+toys+'/')
            baseHistoName = ('H_'+channel+'_'+sampleName+'_'+method_forHist+'_'+
                           regValue+'_'+toys+'_')
            
            
            histos = {}
            truth_name = baseHistoPath+baseHistoName+'mc_truth'
            histos['truth'] = inFile.Get(truth_name)
            if not histos['truth']:
               print 'ERROR could not load histogram',truth_name,'from file',inFilename
               return
            histos['truth'].SetDirectory(0)
            
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
            
            
            diffnx_name = baseHistoPath+baseHistoName+'data_unfolded'
            histos['diffnx'] = inFile.Get(diffnx_name)
            if not histos['diffnx']:
               print 'ERROR could not load histogram',diffnx_name,'from file',inFilename
               return
            histos['diffnx'].SetDirectory(0)
            
            
            
            genhistos[generator] = histos
            
            inFile.Close()
            # end generators
         
         # create ratio plot for Alpgen[0] unfolding MC@NLO[1], divided by MC@NLO truth
         ratio01 = TH1F(genhistos[generators[0]]['truth'])
         ratio01.SetDirectory(0)
         ratio01.SetName('ratio_'+generator[0]+'Unfolding'+generator[1]+'_over_'+generator[1]+'Truth')
         # divide the unfolded result by the truth from the generator that cooresponds
         # to the distribution that was unfolded
         ratio01.Divide(genhistos[generators[0]]['diffnx'],genhistos[generators[1]]['truth'])
         ratio01.GetYaxis().SetTitle(y_title)
         ratio01.GetXaxis().SetTitle(x_title)
         ratio01.SetMaximum(y_max)
         ratio01.SetMinimum(y_min)
         
         # create ratio plot for MC@NLO[1] unfolding Alpgen[0], divided by Alpgen truth
         ratio10 = TH1F(genhistos[generators[0]]['truth'])
         ratio10.SetDirectory(0)
         ratio01.SetName('ratio_'+generator[1]+'Unfolding'+generator[0]+'_over_'+generator[0]+'Truth')
         ratio10.Divide(genhistos[generators[1]]['diffnx'],genhistos[generators[0]]['truth'])
         ratio10.GetYaxis().SetTitle(y_title)
         ratio10.GetXaxis().SetTitle(x_title)
         ratio10.SetMaximum(y_max)
         ratio10.SetMinimum(y_min)
         
         can.cd()
         ratio01.SetMarkerColor(kBlack)
         ratio01.SetMarkerStyle(20)
         ratio01.SetLineWidth(2)
         ratio01.Draw()
         ratio10.SetMarkerColor(kRed)
         ratio10.SetLineColor(kRed)
         ratio10.SetLineWidth(1)
         ratio10.SetMarkerStyle(23)
         ratio10.Draw('same')
         
         
         
         
         channel_label = latex_label
         if channel is 'el':
            channel_label += ' e+jets'
         elif channel is 'mu':
            channel_label += ' \\mu+jets'
         
         legend = TLegend(0.22,0.95,0.7,0.75,channel_label)
         legend.SetFillStyle(0)
         legend.SetBorderSize(0)
         legend.AddEntry(ratio01,generators[0]+' unfolding '+generators[1],"lp")
         legend.AddEntry(ratio10,generators[1]+' unfolding '+generators[0],"lp")
         legend.Draw('same')
         
         #tex = TLatex()
         #tex.SetNDC()
         #tex.SetTextFont(62)
         #tex.DrawLatex(0.23,0.88,channel_label)
         
         can.SaveAs(outFilename)
         
         
         
         
         # end channel
      
      
      # end sampleName
   
   




if __name__ == "__main__":
   main()


