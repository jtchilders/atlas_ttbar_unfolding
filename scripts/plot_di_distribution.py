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

   
   channels = ["ejets","mujets"]
   #channels = ["mujets"]
   
   variables = ["Top1_lhood", "Top2_lhood", "SystemMass", "SystemPt", "SystemRapidity"]
   #variables = ["Top1_lhood"]
   
   for j in range(len(variables)):
      variable = variables[j]    
      print j,variable
      
      can = TCanvas(variable,variable,0,0,800,600)
      can.SetMargin(0.2,0.05,0.15,0.03)
      
      label = ''
      regnumber = ''
      
      if variable is 'Top1_lhood':
         label = 'Leptonic top p^{t}_{T}'
         regnumber = "reg4"
      elif variable is 'Top2_lhood':
         label = 'Hadronic top p^{t}_{T}'
         regnumber = "reg4"
      elif variable is 'SystemMass':
         label = 't#bar{t} System mass'
         regnumber = "reg3"
      elif variable is 'SystemPt':
         label = 't#bar{t} System p_{T}'
         regnumber = "reg3"
      elif variable is 'SystemRapidity':
         label = 't#bar{t} System rapidity'
         regnumber = "reg4"
   
      legend = TLegend(0.6,0.95,0.95,0.75,label)
      legend.SetFillStyle(0)
      legend.SetBorderSize(0)

      ratioPlots = []

      
      for i in range(len(channels)):
         channel = channels[i]
      
         print i,channel
      
         postfix = ''
         labelChannel = ''
         if channel is 'ejets':
            postfix = 'el'
            labelChannel = 'e + jets'
         elif channel is 'mujets':
            postfix = 'mu'
            labelChannel = '#mu + jets'
         elif channel is 'combined':
            postfix = 'co'
      
         filename = "../data/data_Unfolded_14_02_2013/Alpgen/nominal_Tag1_" + variable +"_"+ postfix + "_toy5000_svd_" +regnumber + ".root"
         fileDi = TFile(filename)
         if not fileDi.IsOpen():
            print 'ERROR opening ',filename
      
         m_histo = fileDi.Get("unfolding/toys/nominal/"+postfix+"/Tag1_"+variable+"/SVD/"+regnumber+"/toy5000/H_"+postfix+"_Tag1_"+variable+"_SVD_"+regnumber+"_toy5000_Regularization")
         m_histo.SetName("diFactor"+variable+postfix)       
         nbins = m_histo.GetNbinsX()-1

         diFactor=m_histo.Clone();
         diFactor.SetDirectory(0)
         diFactor.SetNdivisions(508);
         diFactor.GetXaxis().SetRangeUser(1,nbins+1)
         
         for bin in range(nbins):
            bindiFactor = m_histo.GetBinContent(bin+1);
            print bin,bindiFactor
            diFactor.SetBinContent(bin+2,bindiFactor)
            
         diFactor.SetTitle("")
         diFactor.SetMarkerColor(kBlack+i)
         diFactor.SetMarkerStyle(20+i)
         diFactor.SetLineColor(kBlack+i)
         diFactor.GetYaxis().SetRangeUser(0.005,400)
         diFactor.GetYaxis().SetTitle("|d_{i}|")
         diFactor.GetXaxis().SetTitle("bin number")

         ratioPlots.append(diFactor)
         
         legend.AddEntry(diFactor,labelChannel,"p")

         if i > 0:
            diFactor.Draw('histosame')
         else:
            diFactor.Draw("histo")
      
      can.cd()
      horizontal = TF1("line","pol1",0,2800)
      horizontal.SetParameters(1,0)
      horizontal.SetLineColor(kBlack)
      horizontal.SetLineStyle(2)
      horizontal.Draw('same')
   
      legend.Draw('same')
      can.SetLogy()
      can.SaveAs("regularizationValue_"+variable+".eps")      
      

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


