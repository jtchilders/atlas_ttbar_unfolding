#! /usr/bin/env python26

import os,sys,subprocess,shlex,math
from array import array
from multiprocessing import Pool
from ROOT import TFile,TH1D,TH1F,TCanvas,TLegend,gStyle
from ROOT import kBlack,kBlue,kRed,kGreen,kYellow
from ROOT import TGraphAsymmErrors,TF1,gROOT

def main():
   
   gROOT.SetBatch(1)
   
   channels = ["ejets","mujets","combined"]
   
   can = TCanvas("can","can",0,0,800,600)
   can.SetMargin(0.2,0.05,0.15,0.03)
   
   canErrors = TCanvas("canErrors","canErrors",0,0,800,600)
   canErrors.SetMargin(0.2,0.05,0.15,0.03)
   
   
   legend = TLegend(0.25,0.95,0.5,0.75,"hadronic top")
   legend.SetFillStyle(0)
   legend.SetBorderSize(0)
   
   
   
   ratioPlots = []
   ratioErrorPlots = []
   
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
      
      filename = "../data/January_19/Alpgen/Bayesian/unfoldedResult_Tag1_Top2_lhood" + postfix + ".root"
      file_bay = TFile(filename)
      if not file_bay.IsOpen():
         print 'ERROR opening ',filename
      filename = "../data/January_19/Alpgen/SVD/unfoldedResult_Tag1_Top2_lhood" + postfix + ".root"
      file_svd = TFile(filename)
      if not file_svd.IsOpen():
         print 'ERROR opening ',filename
      
      diffxs_bay = file_bay.Get("diffxs_stat")
      diffxs_bay.SetName("diffxs_stat_bay"+postfix)
      diffxs_svd = file_svd.Get("diffxs_stat")
      diffxs_svd.SetName("diffxs_stat_svd"+postfix)
      
      
      ratio = diffxs_svd.Clone("diffxs_stat_ratio"+postfix)
      ratio.SetDirectory(0)
      ratio.GetYaxis().SetTitle("#frac{d#sigma}{dp_{T}^{t}}_{SVD} #times #left[#frac{d#sigma}{dp_{T}^{t}}_{Bay}#right]^{-1}")
      ratio.GetYaxis().SetTitleOffset(1.6)
      ratio.Divide(diffxs_bay)
      ratio.SetMarkerColor(kBlack+i)
      ratio.SetMarkerStyle(20+i)
      ratio.SetLineColor(kBlack+i)
      ratio.SetMinimum(0.8)
      ratio.SetMaximum(1.5)
      
      ratioPlots.append(ratio)
      
      legend.AddEntry(ratio,channel,"p")
      
      can.cd()
      if i > 0:
         ratio.Draw('same')
      else:
         ratio.Draw()
      
      
      if not (channel is 'combined'):
         truth = file_bay.Get("measured")
         truth.SetName("diffxs_truth"+postfix)
         
         legendErr = TLegend(0.25,0.95,0.5,0.75,"hadronic top")
         legendErr.SetFillStyle(0)
         legendErr.SetBorderSize(0)
         
         ratioErrBay = diffxs_svd.Clone("ratio_errors_bay"+postfix)
         ratioErrBay.Reset()
         ratioErrBay.SetDirectory(0)
         
         ratioErrSvd = diffxs_svd.Clone("ratio_errors_svd"+postfix)
         ratioErrSvd.Reset()
         ratioErrSvd.SetDirectory(0)
         
         for bin in range(1,ratioErrSvd.GetNbinsX()+1):
            
            tru   = truth.GetBinContent(bin)
            tErr  = truth.GetBinError(bin)
            
            bayes = diffxs_bay.GetBinContent(bin)
            bErr  = diffxs_bay.GetBinError(bin)
            svd   = diffxs_svd.GetBinContent(bin)
            sErr  = diffxs_svd.GetBinError(bin)
            
            binContent = (sErr/svd)/(tErr/tru)
            ratioErrSvd.SetBinContent(bin,binContent)
            
            binContent = (bErr/bayes)/(tErr/tru)
            ratioErrBay.SetBinContent(bin,binContent)
            
         
         ratioErrorPlots.append(ratioErrBay)
         ratioErrorPlots.append(ratioErrSvd)
         
         canErrors.cd()
         ratioErrBay.SetMarkerStyle(22)
         ratioErrBay.SetMarkerColor(kRed)
         ratioErrBay.SetLineColor(kRed)
         ratioErrBay.GetYaxis().SetTitle("ratio of fractional errors")
         ratioErrBay.SetMinimum(0)
         ratioErrBay.SetMaximum(2.)
         ratioErrBay.Draw()
         
         ratioErrSvd.SetMarkerColor(kBlue)
         ratioErrSvd.SetLineColor(kBlue)
         
         ratioErrSvd.Draw('same')
         
         legendErr.AddEntry(ratioErrBay,"Bayesian","l")
         legendErr.AddEntry(ratioErrSvd,"SVD","l")
         legendErr.Draw('same')
         
         canErrors.SaveAs("errors"+postfix+".eps")
      
   
   can.cd()
   horizontal = TF1("line","pol1",0,1000)
   horizontal.SetParameters(1,0)
   horizontal.SetLineColor(kBlack)
   horizontal.SetLineStyle(2)
   horizontal.Draw('same')
   
   legend.Draw('same')
   
   can.SaveAs("test.eps")
   
   
   

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


