#! /usr/bin/env python26

import os,sys,subprocess,shlex,math
from multiprocessing import Pool
from ROOT import TFile,TCanvas,TLegend,TPaveText
from ROOT import TH1D,TH1F,TH1,TGraphAsymmErrors,TF1
from ROOT import kBlue,kRed,kGreen,kBlack
from ROOT import gROOT,gStyle
from AtlasStyle import SetAtlasStyle
from globals import *

###############################################
### REMEMBER!!!!
### Set globals::useGausSmearing to TRUE!!!
###############################################



def main():
   
   gROOT.SetBatch(True)
   SetAtlasStyle()
   
   base_path = '../data/March_4/McAtNlo/UnfoldingMethodTest'
   output_eps_path = base_path + '/eps'
   base_filename = 'unfoldingMethodsTest_mc'
   
   systematic  = 'nominal'
   toys        = 5000
   iterValue   = 4
   histogram_names = ['mc_truth_diffxs','data_unfolded_diffxs']
   
   
   output_filename = base_path + '/unfoldingMethodsPlots'
   output_filename_ext = '.eps'
   can = TCanvas('can','can',0,0,800,600)
   can.SetMargin(0.2,0.05,0.15,0.05)
   
   
   horizontalAt0 = TF1("horizontalAt0","[0]+x*[1]",-100000,100000)
   horizontalAt0.SetParameter(0,0)
   horizontalAt0.SetParameter(1,0)
   horizontalAt0.SetLineColor(kBlack)
   horizontalAt0.SetLineStyle(9)
   horizontalAt0.SetLineWidth(1)
   
   y_title = "#left(#frac{d#sigma}{dX}|_{unf} - #frac{d#sigma}{dX}|_{tru}#right)#times#left(#frac{d#sigma}{dX}|_{tru}#right)^{-1}"
   
   # load histograms
   for sampleName in sampleNames:
      x_title = ''
      
      latex_label = ''
      if sampleName is 'Tag1_SystemMass':
         x_title = 'M_{t#bar{t}} [GeV]'
      elif sampleName is 'Tag1_SystemPt':
         x_title = 'p_{T}^{t#bar{t}} [GeV]'
      elif sampleName is 'Tag1_Top1_lhood':
         x_title = 'leptonic top p_{T} [GeV]'
      elif sampleName is 'Tag1_Top2_lhood':
         x_title = 'hadronic top p_{T} [GeV]'
      
      for channel in channels:
         
         diffxs = {}
         truth  = {}
         ratios = {}
         gratios = {}
         for half in halves:
            
            
            # create canvas and legend
            can.cd()
            legA = TLegend(0.23,0.93,0.53,0.75)
            legA.SetBorderSize(0)
            legA.SetFillStyle(0)
            legB = TLegend(0.53,0.93,0.83,0.75)
            legB.SetBorderSize(0)
            legB.SetFillStyle(0)
            
            # loop over methods and get histograms
            diffxs[half] = {}
            truth[half] = {}
            ratios[half] = {}
            gratios[half] = {}
            for unfoldingMethod in unfoldingMethods:
               
               # get the method value
               methodValue = -1
               if unfoldingMethod == SVD:
                  methodValue = regValues[sampleName]
               elif unfoldingMethod == Bayesian:
                  methodValue = iterValue
               
               # open file with histograms
               in_filename = base_path + '/' + get_input_filename(systematic,sampleName,channel,unfoldingMethod,half,toys,methodValue) + '.root'
               in_file = TFile(in_filename)
               if not in_file.IsOpen():
                  print 'ERROR opening file',in_filename
                  return
               
               # retrieve histograms
               histoname_base = get_histoname_base(systematic,sampleName,channel,unfoldingMethod,half,toys,methodValue)
               
               # get truth
               histoname = histoname_base + '_mc_truth_diffxs'
               histo = in_file.Get(histoname)
               if not histo:
                  print 'ERROR retrieving histogram,\n',histoname,',\n from file,\n',in_filename
                  return
               histo.SetDirectory(0)
               histo.SetName(histoname_base + '_' + unfoldingMethod + '_mc_truth_diffxs')
               
               truth[half][unfoldingMethod] = histo
               
               # get unfolded distribution
               histoname = histoname_base + '_data_unfolded_diffxs'
               histo = in_file.Get(histoname)
               if not histo:
                  print 'ERROR retrieving histogram,\n',histoname,',\n from file,\n',in_filename
                  return
               histo.SetDirectory(0)
               histo.SetName(histoname_base + '_' + unfoldingMethod + '_data_unfolded_diffxs')
               
               diffxs[half][unfoldingMethod] = histo
               
               in_file.Close()
               # end unfoldingMethods
            # end halves
         
         for half in halves:
            for unfoldingMethod in unfoldingMethods:
               # calculate ratio of unfolded-truth over truth
               ratio = GetRatioPlot(diffxs[half][unfoldingMethod],
                       truth[oppositeHalves[halves.index(half)]][unfoldingMethod])
               histoname_base = get_histoname_base(systematic,sampleName,channel,unfoldingMethod,half,toys,methodValue)
               ratio.SetName(histoname_base + '_' + unfoldingMethod + '_ratio')
               ratio.SetDirectory(0)
               
               ratios[half][unfoldingMethod] = ratio
               
               axis_length = ratio.GetXaxis().GetXmax() - ratio.GetXaxis().GetXmin()
               offset = (unfoldingMethods.index(unfoldingMethod)-1.5)*0.005*axis_length
               gratio = GetTGraphAsymmErrors(ratio,offset)
               
               # plot ratio
               gratio.SetMarkerColor(unfoldingMethods.index(unfoldingMethod) + 1)
               gratio.SetMarkerStyle(unfoldingMethods.index(unfoldingMethod) + 21)
               gratio.SetLineColor(unfoldingMethods.index(unfoldingMethod) + 1)
               gratio.SetLineWidth(2)
               
               # if this is the first method, need to set proper axis parameters
               if unfoldingMethods.index(unfoldingMethod) == 0:
                  if sampleName is LeptonicTop:
                     gratio.SetMaximum(0.11)
                     gratio.SetMinimum(-0.09)
                  elif sampleName is HadronicTop:
                     gratio.SetMaximum(0.08)
                     gratio.SetMinimum(-0.1)
                  elif sampleName is SystemMass:
                     gratio.SetMaximum(0.1)
                     gratio.SetMinimum(-0.08)
                  elif sampleName is SystemPt:
                     gratio.SetMaximum(0.15)
                     gratio.SetMinimum(-0.3)
                  elif sampleName is SystemRapidity:
                     gratio.SetMaximum(0.07)
                     gratio.SetMinimum(-0.05)
                  
                  gratio.GetXaxis().SetTitle(x_title)
                  gratio.GetYaxis().SetTitle(y_title)
                  gratio.GetYaxis().SetTitleOffset(1.6)
                  gratio.Draw('ap')
               else:
                  gratio.Draw('p')
               
               # add to legend
               if unfoldingMethods.index(unfoldingMethod) >= 2:
                  legB.AddEntry(gratio,unfoldingMethod,'pl')
               else:
                  legA.AddEntry(gratio,unfoldingMethod,'pl')
               
               gratios[half][unfoldingMethod] = gratio
               
               horizontalAt0.Draw('same')
               
               
               legA.Draw('same')
               legB.Draw('same')
               
               text = TPaveText(0.2,0.25,0.4,0.18,"NDC")
               text.SetFillStyle(0)
               text.SetBorderSize(0)
               text.AddText(half + ', ' + channel)
               #    text.Draw('same')
               can.SaveAs(output_filename+'_'+channel+'_'+half+'_'+sampleName+output_filename_ext)
               
               
         
         
         
   
   
   

def GetXaxisTitle(sampleName):
   if sampleName is LeptonicTop:
      return 'leptonic p_{T}^{t} [GeV]'
   elif sampleName is HadronicTop:
      return 'hadronic p_{T}^{t} [GeV]'
   elif sampleName is SystemMass:
      return 'M_{tt} [GeV]'
   elif sampleName is SystemPt:
      return 'p_{T}^{tt} [GeV]'
   elif sampleName is SystemRapidity:
      return 'y_{tt}'
   return 'x-axis'

def GetYaxisTitle():
   return '#left(#frac{d#sigma}{dX}_{unf} - #frac{d#sigma}{dX}_{tru}#right)#times#left(#frac{d#sigma}{dX}_{tru}#right)^{-1}'


def GetTGraphAsymmErrors(histo,offset):
   
   graph_in = TGraphAsymmErrors(histo)
   graph_in.SetName(histo.GetName() + '_tmp')
   graph_out = TGraphAsymmErrors(histo)
   graph_out.SetName(histo.GetName() + '_offset')
   
   for i in range(graph_in.GetN()):
      graph_out.GetX()[i] = graph_in.GetX()[i] + offset
      graph_out.GetEXlow()[i] = graph_in.GetEXlow()[i] + offset
      graph_out.GetEXhigh()[i] = graph_in.GetEXhigh()[i] - offset
      
   
   x_min = graph_out.GetX()[0]- graph_out.GetEXlow()[0]
   if(x_min > 0): x_min = 0
   x_max = graph_out.GetX()[graph_out.GetN()-1] + graph_out.GetEXhigh()[graph_out.GetN()-1]
   graph_out.GetXaxis().SetLimits(x_min,x_max)
   
   return graph_out

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

def getRatio(diffxs,truth):
   
   histo = TH1F(diffxs)
   histo.SetDirectory(0)
   
   histo.Add(truth,-1)
   histo.Divide(truth)
   
   return histo

def get_input_filename(systematic,sampleName,channel,unfoldingMethod,half,toys,methodValue = -1):
   filename = systematic + "_" + sampleName + "_" + channel
   filename += "_toy" + str(toys )
   
   if(unfoldingMethod is SVD):
      filename += "_svd_reg" + str( methodValue )
   elif(unfoldingMethod is Bayesian):
      filename += "_bay_iter" + str( methodValue )
   elif(unfoldingMethod is binByBin):
      filename += "_bin"
   elif(unfoldingMethod is inversion):
      filename += "_inv"
   
   filename += '_' + half + oppositeHalves[halves.index(half)][-1]
   
   return filename

def get_histoname_base(systematic,sampleName,channel,unfoldingMethod,half,toys,methodValue = -1):
   histoname = 'unfolding/toys/' + systematic + '/' + channel + '/' + sampleName
   if unfoldingMethod == SVD:
      histoname += '/SVD/reg' + str(methodValue)
   elif unfoldingMethod == Bayesian:
      histoname += '/Bayes'
   elif unfoldingMethod == binByBin:
      histoname += '/' + binByBin
   elif unfoldingMethod == inversion:
      histoname += '/' + inversion
   histoname += '/toy' + str(toys)
   histoname += '/H_' + channel + '_' + sampleName
   
   if unfoldingMethod == SVD:
      histoname += '_SVD_reg' + str(methodValue)
   elif unfoldingMethod == Bayesian:
      histoname += '_Bayes'
   elif unfoldingMethod == binByBin:
      histoname += '_' + binByBin
   elif unfoldingMethod == inversion:
      histoname += '_' + inversion
   histoname += '_toy' + str(toys)
   return histoname

if __name__ == "__main__":
   main()


