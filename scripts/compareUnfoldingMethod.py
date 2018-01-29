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
   
   methods = ["Inversion","BinByBin","Bayes","SVD" ]
   #methods = ["SVD"]
   
   for j in range(len(variables)):
      variable = variables[j]    
      print j,variable
      
      for i in range(len(channels)):
         channel = channels[i]
         print i,channel
      
         postfix = ''
         if channel is 'ejets':
            postfix = 'el'
         elif channel is 'mujets':
            postfix = 'mu'
         elif channel is 'combined':
            postfix = ''
      
         can = TCanvas(variable+"_"+channel,variable+"_"+channel,0,0,800,600)
         can.SetMargin(0.2,0.05,0.15,0.03)
   
         ratioPlots = []
         
         label = ''
         latexlabel = ""
         if variable is 'Top1_lhood':
            label = 'Leptonic top p^{t}_{T}'
            regnumber = "reg4"
            latexlabel = "#left(#frac{d#sigma}{dp^{t}_{T}}_{unf} - #frac{d#sigma}{dp^{t}_{T}}_{truth}#right) #times #left(#frac{d#sigma}{dp^{t}_{T}}_{truth}#right)^{-1}"
         elif variable is 'Top2_lhood':
            label = 'Hadronic top p^{t}_{T}'
            regnumber = "reg4"
            latexlabel = "#left(#frac{d#sigma}{dp^{t}_{T}}_{unf} - #frac{d#sigma}{dp^{t}_{T}}_{truth}#right) #times #left(#frac{d#sigma}{dp^{t}_{T}}_{truth}#right)^{-1}"
         elif variable is 'SystemMass':
            label = 't#bar{t} System mass'
            regnumber = "reg3"
            latexlabel = "#left(#frac{d#sigma}{dM_{t#bar{t}}}_{unf} - #frac{d#sigma}{dM_{t#bar{t}}}_{truth}#right) #times #left(#frac{d#sigma}{dM_{t#bar{t}}}_{truth}#right)^{-1}"
         elif variable is 'SystemPt':
            label = 't#bar{t} System p_{T}'
            regnumber = "reg3"
            latexlabel = "#left(#frac{d#sigma}{dp^{t#bar{t}}_{T}}_{unf} - #frac{d#sigma}{dp^{t#bar{t}}_{T}}_{truth}#right) #times #left(#frac{d#sigma}{dp^{t#bar{t}}_{T}}_{truth}#right)^{-1}" 
         elif variable is 'SystemRapidity':
            label = 't#bar{t} System rapidity'
            regnumber = "reg4"
            latexlabel = "#left(#frac{d#sigma}{dY_{t#bar{t}}_{T}}_{unf} - #frac{d#sigma}{dY_{t#bar{t}}_{T}}_{truth}#right) #times #left(#frac{d#sigma}{dY_{t#bar{t}}_{T}}_{truth}#right)^{-1}" 
   
         legend = TLegend(0.5,0.95,0.85,0.7,label)
         legend.SetFillStyle(0)
         legend.SetBorderSize(0)
            
         for k in range(len(methods)):
            method = methods[k]
            print k,method 
            
            metfix = ''
            histfix = ''
            addfix = ''
            if method is 'SVD':
               metfix = "_svd_"+regnumber
               histfix = "_SVD_"+regnumber
               addfix = regnumber + "/"
            elif method is 'BinByBin':
               metfix = "_bin"
               histfix = "_BinByBin"
               addfix = ""
            elif method is 'Bayes':
               metfix = "_bay_iter4"
               histfix = "_Bayes"
               addfix = ""
            elif method is 'Inversion':
               metfix = "_inv"               
               histfix = "_Inversion"
               addfix = ""
            
            
            filename = "../data/data_Unfolded_14_02_2013/McAtNlo/MCClosure/RecoClosure_Tag1_" + variable +"_"+postfix + "_half0_toy5000" + metfix +".root"
            file = TFile(filename)
            if not file.IsOpen():
               print 'ERROR opening ',filename 
         
            #open the histograms to do the %
            name = "unfolding/toys/nominal/" + postfix + "/Tag1_" + variable + "/" + method + "/" + addfix + "toy5000/H_" + postfix + "_Tag1_" + variable + histfix + "_toy5000_data_unfolded_diffxs"
            print name
            m_unf =file.Get(name)
            m_unf.SetName("Unfolded_"+method+"_"+variable+"_"+postfix)   
      
            m_truth =file.Get("unfolding/toys/nominal/" + postfix + "/Tag1_" + variable + "/" + method + "/" + addfix + "toy5000/H_" + postfix + "_Tag1_" + variable + histfix + "_toy5000_mc_truth_diffxs")
            m_truth.SetName("Truth_"+method+"_"+variable+"_"+postfix)
      
            nbins = m_unf.GetNbinsX()
            
            ratio = m_unf.Clone("Ratio_"+method+"_"+variable+"_"+postfix)
            
            ratio.SetDirectory(0)
            
            for bin in range(nbins):
               binratio = (m_unf.GetBinContent(bin+1)-m_truth.GetBinContent(bin+1));
               print bin,binratio,m_unf.GetBinContent(bin+1),m_truth.GetBinContent(bin+1)
               ratio.SetBinContent(bin+1,binratio)
            
            ratio.GetYaxis().SetTitle(latexlabel)
            ratio.GetYaxis().SetTitleOffset(1.6)
            ratio.Divide(m_truth)
            ratio.GetYaxis().SetRangeUser(-0.25,0.25)
            if variable is 'SystemRapidity':
               ratio.GetYaxis().SetRangeUser(-0.07,0.06)
            ratio.SetMarkerColor(kBlack+k)
            ratio.SetMarkerStyle(20+k)
            ratio.SetLineColor(kBlack+k)
      
            ratioPlots.append(ratio)
      
            can.cd()
            if k > 0:
               ratio.Draw('same')
            else:
               ratio.Draw()
               #ratio1.Draw('same')
               
            legend.AddEntry(ratio,method,"p")
      
         can.cd()
         horizontal = TF1("line","pol1",-200,2800)
         horizontal.SetParameters(1,0)
         horizontal.SetLineColor(kBlack)
         horizontal.SetLineStyle(2)
         horizontal.Draw('same')
   
         legend.Draw('same')
   
         can.SaveAs("ComparisonMethods_"+variable+"_"+channel+".eps")
 

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


