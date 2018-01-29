#! /usr/bin/env python26

import os,sys,subprocess,shlex,math
from array import array
from multiprocessing import Pool
from ROOT import TFile,TH1D,TH1F,TCanvas,TLegend,gStyle
from ROOT import kBlack,kBlue,kRed,kGreen,kYellow
from ROOT import TGraphAsymmErrors,TF1,gROOT
   
from AtlasStyle import SetAtlasStyle

def main():
   
   gROOT.SetBatch(1)
   SetAtlasStyle()
   
   #channels = ["ejets","mujets"]
   channels = ["ejets"]
   
   variables = ["Top1_lhood", "Top2_lhood", "SystemMass", "SystemPt", "SystemRapidity"]
   
   for j in range(len(variables)):
      variable = variables[j]    
      print j,variable
      
      can = TCanvas(variable,variable,0,0,800,600)
      can.SetMargin(0.2,0.05,0.15,0.03)
   
      ratioPlots = []
      
      label = ''
      latexlabel = ""
      if variable is 'Top1_lhood':
         label = 'Leptonic top p^{t}_{T}'
         latexlabel = "#frac{d#sigma}{dp^{t}_{T}}_{rew} #times #left[#frac{d#sigma}{dp^{t}_{T}}_{std}#right]^{-1}"
      elif variable is 'Top2_lhood':
         label = 'Hadronic top p^{t}_{T}'
         latexlabel = "#frac{d#sigma}{dp_{T}^{t}}_{rew} #times #left[#frac{d#sigma}{dp_{T}^{t}}_{std}#right]^{-1}"
      elif variable is 'SystemMass':
         label = 't#bar{t} System mass'
         latexlabel = "#frac{d#sigma}{dM_{t#bar{t}}}_{rew} #times #left[#frac{d#sigma}{dM_{t#bar{t}}}_{std}#right]^{-1}"
      elif variable is 'SystemRapidity':
         label = 't#bar{t} System rapidity'
         latexlabel = "#frac{d#sigma}{dY_{t#bar{t}}}_{rew} #times #left[#frac{d#sigma}{dY_{t#bar{t}}}_{std}#right]^{-1}"
      elif variable is 'SystemPt':
         label = 't#bar{t} System p_{T}'
         latexlabel = "#frac{d#sigma}{dp^{t#bar{t}}_{T}}_{rew} #times #left[#frac{d#sigma}{d}p^{t#bar{t}}_{T}_{std}#right]^{-1}"            
   
      legend = TLegend(0.25,0.95,0.5,0.75,label)
      legend.SetFillStyle(0)
      legend.SetBorderSize(0)
      
      for i in range(len(channels)):
         channel = channels[i]
      
         print i,channel
      
         postfix = ''
         if channel is 'ejets':
            postfix = '_el'
         elif channel is 'mujets':
            postfix = '_mu'
         elif channel is 'combined':
            postfix = ''
      
         filename = "../data/data_Unfolded_14_02_2013/Alpgen/SVD/unfoldedResult_Tag1_" + variable + postfix + ".root"
         file_std = TFile(filename)
         if not file_std.IsOpen():
            print 'ERROR opening ',filename
         filename = "../data/test_Unfolded_08_03_2013/Alpgen/SVD/unfoldedResult_Tag1_" + variable + postfix + ".root"
         file_rew = TFile(filename)
         if not file_rew.IsOpen():
            print 'ERROR opening ',filename
      
         diffxs_std = file_std.Get("diffxs_stat")
         diffxs_std.SetName("diffxs_stat_std"+postfix)
         measured_std = file_std.Get("measured")
         measured_std.SetName("measured_std"+postfix)
         diffxs_rew = file_rew.Get("diffxs_stat")
         measured_rew = file_rew.Get("measured")
         measured_rew.SetName("measured_rew"+postfix)
         diffxs_rew.SetName("diffxs_stat_rew"+postfix)      
      
         ratio = diffxs_rew.Clone("diffxs_stat_ratio"+postfix)
         ratio.SetDirectory(0)
         ratio.GetYaxis().SetTitle(latexlabel)
         ratio.GetYaxis().SetTitleOffset(1.6)
         ratio.Divide(diffxs_std)
         ratio.GetYaxis().SetRangeUser(0.9,1.2)
         ratio.SetMarkerColor(kBlack+i)
         ratio.SetMarkerStyle(20+i)
         ratio.SetLineColor(kBlack+i)
         
         ratio1 = measured_rew.Clone("measured_ratio"+postfix)
         ratio1.SetDirectory(0)
         ratio1.GetYaxis().SetTitle(latexlabel)
         ratio1.GetYaxis().SetTitleOffset(1.6)
         ratio1.Divide(measured_std)
         ratio1.GetYaxis().SetRangeUser(0.7,1.2)
         if variable is 'SystemRapidity':
               ratio.GetYaxis().SetRangeUser(-0.07,0.04)
         ratio1.SetMarkerColor(kRed+i)
         ratio1.SetMarkerStyle(20+i)
         ratio1.SetLineColor(kRed+i)
      
         ratioPlots.append(ratio)
         ratioPlots.append(ratio1)
         
         legend.AddEntry(ratio,"unfolded","p")
         legend.AddEntry(ratio1,"measured","p")
      
         can.cd()
         if i > 0:
            ratio.Draw('same')
         else:
            ratio.Draw()
            ratio1.Draw('same')
      
      can.cd()
      horizontal = TF1("line","pol1",200,2800)
      horizontal.SetParameters(1,0)
      horizontal.SetLineColor(kBlack)
      horizontal.SetLineStyle(2)
      horizontal.Draw('same')
   
      legend.Draw('same')
   
      can.SaveAs(variable+".eps")
      
   controlPlots = ['InclusiveJetBinLeptonPt', 'Top1Pt', 'Top2Pt', 'SystemMass', 'SystemPt']
   
   ratioPlots1 = []
   
   for j in range(len(controlPlots)):
      controlPlot = controlPlots[j]
        
      can1 = TCanvas(controlPlot,controlPlot,0,0,800,600)
      can1.SetMargin(0.2,0.05,0.15,0.03)
      
      legend1 = TLegend(0.45,0.95,0.90,0.75,"")
      legend1.SetFillStyle(0)
      legend1.SetBorderSize(0)

      labelx = ''
      if controlPlot is 'InclusiveJetBinLeptonPt':
         labelx = 'lepton p_{T}, all-jet bin[GeV/c]'
      elif controlPlot is 'Top1Pt':
         labelx = 'Leptonic top p^{t}_{T}[GeV/c]'
      elif controlPlot is 'Top2Pt':
         labelx = 'Hadronic top p^{t}_{T}[GeV/c]'
      elif controlPlot is 'SystemMass':
         labelx = 'M_{t#bar{t}}[GeV/c^{2}]'
      elif controlPlot is 'SystemPt':
         labelx = 'p^{t#bar{t}}_{T}[GeV/c]'
      
      for i in range(len(channels)):
         channel = channels[i]
         print i,channel
      
         postfix = ''
         if channel is 'ejets':
            postfix = '_el'
         elif channel is 'mujets':
            postfix = '_mu'
         elif channel is 'combined':
            postfix = ''
         
         filename = "../data/data_14_02_2013/nominal/ToUnfold_nominal_AlpgenJimmy/tagged_" + channel + ".root"
         cfile_std = TFile(filename)
         if not cfile_std.IsOpen():
            print 'ERROR opening ',filename
         filename = "../data/test_08_03_2013/nominal/ToUnfold_nominal_AlpgenJimmy/tagged_" + channel + ".root"
         cfile_rew = TFile(filename)
         if not cfile_rew.IsOpen():
            print 'ERROR opening ',filename
         
         control_std = cfile_std.Get(controlPlot+"_Data")
         control_std.SetName("control_std"+postfix)
         control_rew = cfile_rew.Get(controlPlot+"_Data")
         control_rew.SetName("control_rew"+postfix)
                  
         control_std.SetDirectory(0)         
         control_std.SetTitle("")
         control_std.GetYaxis().SetTitle("Events")
         control_std.GetXaxis().SetTitle(labelx)                 
         control_std.GetYaxis().SetTitleOffset(1.6)
         #control_std.GetYaxis().SetRangeUser(0.7,1.3)
         control_std.SetMarkerColor(kBlack+i)
         control_std.SetMarkerStyle(20+i)
         control_std.SetLineColor(kBlack+i)
         
         control_rew.SetMarkerColor(kRed+i)
         control_rew.SetMarkerStyle(20+i)
         control_rew.SetLineColor(kRed+i)
         
         if i > 0:
            control_std.Draw('same')
            control_rew.Draw('same')            
         else:
            control_std.Draw()
            control_rew.Draw('same')
            
         legend1.AddEntry(control_std,"nominal","p")
         legend1.AddEntry(control_rew,"Lepton p_{T} reweighting","p")
         legend1.Draw('same')
      
      can1.SetLogy()
      can1.SaveAs(controlPlot+"_log.eps")
      
      

def DivideTGraphAsymmErrors(numerator,denominator,name='ratio'):
   
   ratio = numerator.Clone(name)
   
   for i in range(numerator.GetN()):
      
      num      = numerator.GetY()[i]
      num_up   = numerator.GetErrorYhigh(i)
      num_down = numerator.GetErrorYlow(i)
      den      = denominator.GetY()[i]
      den_up   = denominator.GetErrorYhigh(i)
      den_down = denominator.GetErrorYlow(i)
      
      content = num / den
      
      error_up   = content * math.sqrt( (num_up*num_up)/(num*num) + (den_up*den_up)/(den*den) )
      error_down = content * math.sqrt( (num_down*num_down)/(num*num) + (den_down*den_down)/(den*den) )
      
      ratio.SetPoint(i,numerator.GetX()[i],content)
      ratio.SetPointEYhigh(i,error_up)
      ratio.SetPointEYlow(i,error_down)
      
   
   return ratio




if __name__ == "__main__":
   main()


