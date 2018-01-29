#! /usr/bin/env python26

import os,sys,subprocess,shlex
from array import array
#from multiprocessing import Pool
from ROOT import TFile,TH1,TCanvas,TLegend,gStyle,gROOT,TF1,TPaveText,TGraphErrors


sampleNames = [
                  #'Tag1_Top1',
                  #'Tag1_Top2',
#                  'Tag1_Top1_lhood',
#                  'Tag1_Top2_lhood',
              'Tag1_SystemPt',
#                  'Tag1_SystemMass',
#                  'Tag1_SystemRapidity',
              ]

channels    = [
               'el',
               'mu'
               ]
halves      = [
               'half01',
               'half10',
               ]

adjust      = [
              'systemPtStressTest/rootSystemPtDown',
              'systemPtStressTest/rootSystemPtUp',
#                  'systemMassStressTest/rootMassUp',
#                  'systemMassStressTest/rootMassDown',
#                  'systemRapidityStressTest/rootRapidityUp',
#                  'systemRapidityStressTest/rootRapidityDown',
#                  'topPtStressTest/rootDown500',
#                  'topPtStressTest/rootDown900',
#                  'topPtStressTest/rootUp500',
#                  'topPtStressTest/rootUp900',
              ]

regValues   = {'Tag1_Top1_lhood':4,
               'Tag1_Top2_lhood':4,
               'Tag1_SystemPt':3,
               'Tag1_SystemMass':3,
               'Tag1_SystemRapidity':4
              }

def main():
   
   gROOT.SetBatch(True)
   gStyle.SetOptStat(0)
   
   
   fileList = get_file_list()
   
   filenameA = "rwSpectraLinearShift.ps"
   can_linear = TCanvas("can_linear","can_linear",0,0,800,600)
   can_linear.SetMargin(0.13,0.05,0.15,0.05);
   firstLinear = True
   
   filenameB = "rwSpectraStepFuncShift.ps"
   can_bmp = TCanvas("can_bmp","can_bmp",0,0,800,600)
   can_bmp.SetMargin(0.13,0.00,0.15,0.05);
   firstBmp = True
   
   fits = {}
   
   
   for i in range(len(fileList)):
      fileObj = fileList[i]
      
      #print 'i = ',i
      
      scale = False
      
      yaxis_title_offset = 1.6
      nom_tru = fileObj.nominal_mc_truth.Clone(fileObj.nominal_mc_truth.GetName()+"Norm")
      if scale: nom_tru.Scale(1/fileObj.nominal_mc_truth.Integral("width"))
      nom_tru.SetMarkerStyle(20)
      nom_tru.SetMarkerColor(1)
      nom_tru.SetTitle(';p_{T}^{t} [GeV];#frac{d#sigma_{t#bar{t}}}{dp_{T}^{t}} #left[#frac{pb}{GeV}#right]')
      nom_tru.GetYaxis().SetTitleOffset(yaxis_title_offset)
      nom_rec = fileObj.nominal_mc_reco.Clone(fileObj.nominal_mc_reco.GetName()+"Norm")
      if scale: nom_rec.Scale(1/fileObj.nominal_mc_truth.Integral("width"))
      nom_rec.SetMarkerStyle(21)
      nom_rec.SetMarkerColor(2)
      nom_rec.SetTitle(';p_{T}^{t} [GeV];#frac{d#sigma_{t#bar{t}}}{dp_{T}^{t}} #left[#frac{pb}{GeV}#right]')
      nom_rec.GetYaxis().SetTitleOffset(yaxis_title_offset)
      nom_mea = fileObj.nominal_measured.Clone(fileObj.nominal_measured.GetName()+"Norm")
      if scale: nom_mea.Scale(1/fileObj.nominal_mc_truth.Integral("width"))
      nom_mea.SetMarkerStyle(22)
      nom_mea.SetMarkerColor(3)
      nom_mea.SetTitle(';p_{T}^{t} [GeV];#frac{d#sigma_{t#bar{t}}}{dp_{T}^{t}} #left[#frac{pb}{GeV}#right]')
      nom_mea.GetYaxis().SetTitleOffset(yaxis_title_offset)
      nom_unf = fileObj.nominal_unfolded.Clone(fileObj.nominal_unfolded.GetName()+"Norm")
      if scale: nom_unf.Scale(1/fileObj.nominal_mc_truth.Integral("width"))
      nom_unf.SetMarkerStyle(23)
      nom_unf.SetMarkerColor(4)
      nom_unf.SetTitle(';p_{T}^{t} [GeV];#frac{d#sigma_{t#bar{t}}}{dp_{T}^{t}} #left[#frac{pb}{GeV}#right]')
      nom_unf.GetYaxis().SetTitleOffset(yaxis_title_offset)
      
      adj_tru = fileObj.adjusted_mc_truth.Clone(fileObj.adjusted_mc_truth.GetName()+"Norm")
      if scale: adj_tru.Scale(1/fileObj.nominal_mc_truth.Integral("width"))
      adj_tru.SetMarkerStyle(29)
      adj_tru.SetMarkerColor(6)
      adj_tru.SetTitle(';p_{T}^{t} [GeV];#frac{d#sigma_{t#bar{t}}}{dp_{T}^{t}} #left[#frac{pb}{GeV}#right]')
      adj_tru.GetYaxis().SetTitleOffset(yaxis_title_offset)
      adj_rec = fileObj.adjusted_mc_reco.Clone(fileObj.adjusted_mc_reco.GetName()+"Norm")
      if scale: adj_rec.Scale(1/fileObj.nominal_mc_truth.Integral("width"))
      adj_rec.SetMarkerStyle(31)
      adj_rec.SetMarkerColor(7)
      adj_rec.SetTitle(';p_{T}^{t} [GeV];#frac{d#sigma_{t#bar{t}}}{dp_{T}^{t}} #left[#frac{pb}{GeV}#right]')
      adj_rec.GetYaxis().SetTitleOffset(yaxis_title_offset)
      adj_mea = fileObj.adjusted_measured.Clone(fileObj.adjusted_measured.GetName()+"Norm")
      if scale: adj_mea.Scale(1/fileObj.nominal_mc_truth.Integral("width"))
      adj_mea.SetMarkerStyle(33)
      adj_mea.SetMarkerColor(8)
      adj_mea.SetTitle(';p_{T}^{t} [GeV];#frac{d#sigma_{t#bar{t}}}{dp_{T}^{t}} #left[#frac{pb}{GeV}#right]')
      adj_mea.GetYaxis().SetTitleOffset(yaxis_title_offset)
      adj_unf = fileObj.adjusted_unfolded.Clone(fileObj.adjusted_unfolded.GetName()+"Norm")
      if scale: adj_unf.Scale(1/fileObj.nominal_mc_truth.Integral("width"))
      adj_unf.SetMarkerStyle(34)
      adj_unf.SetMarkerColor(9)
      adj_unf.SetTitle(';p_{T}^{t} [GeV];#frac{d#sigma_{t#bar{t}}}{dp_{T}^{t}} #left[#frac{pb}{GeV}#right]')
      adj_unf.GetYaxis().SetTitleOffset(yaxis_title_offset)
      
      legendString = fileObj.adjustLabel + ',' + fileObj.sampleName + ',' + fileObj.channel + ',' + fileObj.half
      legendStringFit = fileObj.sampleName + ',' + fileObj.channel + ',' + fileObj.half
      
      filenameBase = fileObj.adjustLabel + '_' + fileObj.channel + '_' + fileObj.sampleName + '_' + fileObj.half
      
      nomRec_nomTru = nom_rec.Clone("nomRec_nomTru")
      nomRec_nomTru.Divide(nom_tru)
      
      adjRec_nomTru = adj_rec.Clone("rwRec_nomTru")
      adjRec_nomTru.Divide(nom_tru)
      
      adjUnf_nomTru = adj_unf.Clone("rwUnf_nomTru")
      adjUnf_nomTru.Divide(nom_tru)
      adjUnf_nomTru.SetMarkerColor(1)
      adjUnf_nomTru.SetMarkerStyle(24)
      
      adjTru_nomTru = adj_tru.Clone("rwTru_nomTru")
      adjTru_nomTru.Divide(nom_tru)
      adjTru_nomTru.SetMarkerColor(4)
      adjTru_nomTru.SetMarkerStyle(24)
      
      nomUnf_nomTru = nom_unf.Clone("nomUnf_nomTru")
      nomUnf_nomTru.Divide(nom_tru)
      nomUnf_nomTru.SetMarkerColor(2)
      nomUnf_nomTru.SetMarkerSize(0.8)
      nomUnf_nomTru.SetMarkerStyle(20)
      
      adjUnf_adjTru = adj_unf.Clone("rwUnf_rwTru")
      adjUnf_adjTru.Divide(adj_tru)
      adjUnf_adjTru.SetMarkerStyle(24)
      adjUnf_adjTru.SetMarkerColor(2)
      
      if fileObj.adjustLabel.find('Bmp') >= 0:
         # print bump plots in different canvas/file
         can_bmp.cd()
         can_bmp.Clear()
         
         adjUnf_nomTru.SetMaximum(2.0);
         adjUnf_nomTru.SetMinimum(0.0);
         adjUnf_nomTru.GetYaxis().SetTitle('ratio of diff xs')
         adjUnf_nomTru.GetYaxis().SetTitleOffset(1.)
         adjUnf_nomTru.Draw();
         adjTru_nomTru.Draw('same');
         nomUnf_nomTru.Draw('same');
         adjUnf_adjTru.Draw('same');
         
         legend = TLegend(0.56,0.38,0.86,0.18,legendString)
         legend.SetFillStyle(0)
         legend.SetBorderSize(0)
         legend.AddEntry(adjTru_nomTru,"rwTruth/nomTruth")
         legend.AddEntry(nomUnf_nomTru,"nomUnfold/nomTruth")
         legend.AddEntry(adjUnf_nomTru,"rwUnfold/nomTruth")
         legend.AddEntry(adjUnf_adjTru,"rwUnfold/rwTruth")
         
         
         legend.Draw("same")
         
         if firstBmp:
            can_bmp.SaveAs(filenameB+"(")
            firstBmp = False
         elif (fileObj.adjustLabel.find(adjust[-1]) >= 0 and
               fileObj.adjustLabel.find(channels[-1]) >= 0 and
               fileObj.adjustLabel.find(sampleNames[-1]) >= 0 and
               fileObj.adjustLabel.find(halves[-1]) >= 0):
            can_bmp.SaveAs(filenameB+")")
         else:
            can_bmp.SaveAs(filenameB)
         
         can_bmp.SaveAs(filenameBase + ".eps")
         
         
      else:
         # put linear plots in different canvas/file
         can_linear.cd()
         can_linear.Clear()
         
         # fit the rwUnf/nomTru and rwTru/nomTru
         fit_unf = TF1("fit_unf_"+str(i),"pol1",100,500)
         adjUnf_nomTru.Fit(fit_unf,"rmq0")
         
         fit_tru = TF1("fit_tru_"+str(i),"pol1",100,500)
         adjTru_nomTru.Fit(fit_tru,"rmq0")
         
         # fill fits dictionary with fits so we can loop over them later
         tmpFits = [fit_tru,fit_unf]
         if fileObj.channel not in fits:
            adjustLabelDict = { fileObj.adjustLabel : tmpFits }
            halfDict = { fileObj.half : adjustLabelDict }
            sampleDict = { fileObj.sampleName : halfDict }
            fits[fileObj.channel] = sampleDict
         else:
            sampleDict = fits[fileObj.channel]
            if fileObj.sampleName not in sampleDict:
               adjustLabelDict = { fileObj.adjustLabel : tmpFits }
               halfDict = { fileObj.half : adjustLabelDict }
               sampleDict[fileObj.sampleName] = halfDict
            else:
               halfDict = sampleDict[fileObj.sampleName]
               if fileObj.half not in halfDict:
                  adjustLabelDict = { fileObj.adjustLabel : tmpFits }
                  halfDict[fileObj.half] = adjustLabelDict
               else:
                  adjustLabelDict = halfDict[fileObj.half]
                  adjustLabelDict[fileObj.adjustLabel] = tmpFits
         
         # plot ratios
         adjUnf_nomTru.SetMaximum(2.0);
         adjUnf_nomTru.SetMinimum(0.0);
         adjUnf_nomTru.GetYaxis().SetTitle('ratio of diff xs')
         adjUnf_nomTru.GetYaxis().SetTitleOffset(1.)
         adjUnf_nomTru.Draw();
         adjTru_nomTru.Draw('same');
         nomUnf_nomTru.Draw('same');
         adjUnf_adjTru.Draw('same');
         
         legend = TLegend(0.16,0.38,0.46,0.18,legendString)
         legend.SetFillStyle(0)
         legend.SetBorderSize(0)
         legend.AddEntry(adjTru_nomTru,"rwTruth/nomTruth")
         legend.AddEntry(nomUnf_nomTru,"nomUnfold/nomTruth")
         legend.AddEntry(adjUnf_nomTru,"rwUnfold/nomTruth")
         legend.AddEntry(adjUnf_adjTru,"rwUnfold/rwTruth")
         
         
         legend.Draw("same")
         
         # save canvas
         if firstLinear:
            can_linear.SaveAs(filenameA+"(")
            firstLinear = False
         else:
            can_linear.SaveAs(filenameA)
         
         can_linear.SaveAs(filenameBase + "_ratios.eps")
         
         # if this is the last of the linear plots
         # plot the fit slopes
         if fileObj.adjustLabel.find(adjust[3]) >= 0:
            
            can_linear.cd()
            
            x = []
            y = []
            dx = []
            dy = []
            for adjustLabel in adjust[0:3]:
               fit_tru = fits[fileObj.channel][fileObj.sampleName][fileObj.half][adjustLabel][0]
               fit_unf = fits[fileObj.channel][fileObj.sampleName][fileObj.half][adjustLabel][1]
               
               #print 'tru:',fit_tru.GetParameter(1),'+/-',fit_tru.GetParError(1)
               #print 'unf:',fit_unf.GetParameter(1),'+/-',fit_unf.GetParError(1)
               
               x.append(fit_tru.GetParameter(1))
               dx.append(fit_tru.GetParError(1))
               y.append(fit_unf.GetParameter(1))
               dy.append(fit_unf.GetParError(1))
            
            ax = array('d',x)
            ay = array('d',y)
            adx = array('d',dx)
            ady = array('d',dy)
            
            slope_tru_vs_unf = TGraphErrors(len(x),ax,ay,adx,ady)
            slope_tru_vs_unf.SetName("slope_tru_vs_unf")
            slope_tru_vs_unf.SetTitle(";fitted slope from rwTru/nomTru;fitted slope from rwUnf/nomTru")
            
            #slope_tru_vs_unf.GetXaxis().SetLimits(-1e-4,1e-4)
            slope_tru_vs_unf.GetYaxis().SetTitleOffset(1.25)
            slope_tru_vs_unf.Draw("ap")
            
            # add diagonal line for reference
            diagonal = TF1('diagonal','pol1',-1,1)
            diagonal.SetParameter(0,0.)
            diagonal.SetParameter(1,1.)
            diagonal.SetLineColor(1)
            diagonal.SetLineStyle(2)
            diagonal.Draw('same')
            
            # fit slopes and show results
            slope_fit = TF1('slope_fit','pol1',-1,1)
            slope_tru_vs_unf.Fit(slope_fit,'rq')
            
            slope_fit_info = TPaveText(0.4,0.3,0.9,0.2,"NDCbr")
            slope_fit_info.SetFillStyle(0)
            slope_fit_info.SetBorderSize(0)
            slope_fit_info.AddText(legendStringFit)
            slope_fit_info.AddText('y = %.3e + %2.4f x ' % (slope_fit.GetParameter(0),slope_fit.GetParameter(1)))
            slope_fit_info.Draw('same')
            
            if i == len(fileList) - 1:
               can_linear.SaveAs(filenameA+")")
            else:
               can_linear.SaveAs(filenameA)
            
            can_linear.SaveAs(filenameBase + "_slopes.eps")
   
   
   
   

def get_file_list(): 

   file_list = list()
   
   for sampleName in sampleNames:
      regValue = regValues[sampleName]
      
      for channel in channels:
         
         for half in halves:
            for adjustLabel in adjust:
            
               entry = File    (sampleName = sampleName,
                                channel = channel,
                                adjustLabel = adjustLabel,
                                half = half,
                                regValue = regValue)
               
               file_list.append(entry)
   
   return file_list


class File:
   """ class to define a single command to runToys for a systematic """
   def __init__(self,systematic        = "nominal", 
                nominal_data_path = "/afs/cern.ch/work/c/clbernar/private/topDifferential/stressTest/clbernar",
                adjusted_data_path = "/afs/cern.ch/work/c/clbernar/private/topDifferential/stressTest/clbernar", 
                method            = '-s',
                nToys             = 5000,
                nIter             = 4,
                regValue          = 4,
                sampleName        = 'Tag1_Top1',
                channel           = 'el',
                adjustLabel       = '500',
                half              = 'half01'
                ):
      self.systematic         = systematic
      
      self.sampleName         = sampleName
      self.channel            = channel
      self.nIter              = nIter
      self.nToys              = nToys
      self.regValue           = regValue
      self.method             = method
      
      self.adjustLabel        = adjustLabel
      self.half               = half
      
      self.nominal_data_path  = nominal_data_path
      self.adjusted_data_path = adjusted_data_path
      
      # get the adjusted histograms
      self.adjusted_filename  = self.adjusted_data_path + "/" + self.adjustLabel + "/" + self.get_output_filename()
      
      self.adjusted_file      = TFile(self.adjusted_filename)
      if not self.adjusted_file.IsOpen():
         print 'ERROR opening',self.adjusted_file.GetName()
         return
      else:
         print 'get nominal truth/reco and adjusted measured/unfolded from file: ',self.adjusted_file.GetName()
         self.nominal_mc_truth  = self.adjusted_file.Get(self.get_histoname("_mc_truth"))
         self.nom_truth_int     = self.nominal_mc_truth.Integral('width')
         self.nominal_mc_reco   = self.adjusted_file.Get(self.get_histoname("_mc_reco"))
         self.adjusted_measured = self.adjusted_file.Get(self.get_histoname("_data_measured"))
         self.adjusted_unfolded = self.adjusted_file.Get(self.get_histoname("_data_unfolded"))
         
         if(self.nominal_mc_truth.Integral('width') > 0):
            self.nominal_mc_truth_norm = self.nominal_mc_truth.Clone(self.nominal_mc_truth.GetName() + '_norm')
            self.nominal_mc_reco_norm = self.nominal_mc_reco.Clone(self.nominal_mc_reco.GetName() + '_norm')
            self.adjusted_measured_norm = self.adjusted_measured.Clone(self.adjusted_measured.GetName() + '_norm')
            self.adjusted_measured_norm.Scale(self.adjusted_measured.Integral('width')/self.nom_truth_int)
            self.adjusted_unfolded_norm = self.adjusted_unfolded.Clone(self.adjusted_unfolded.GetName() + '_norm')
            self.adjusted_unfolded_norm.Scale(self.adjusted_unfolded.Integral('width')/self.nom_truth_int)
         
      
      # also need the nominal unfolding for comparison
      self.nominal_filename   = self.adjusted_data_path + "/rootNom/" + self.get_output_filename()
      self.nominal_file       = TFile(self.nominal_filename)
      if not self.nominal_file.IsOpen():
         print 'ERROR opening',self.nominal_file.GetName()
         return
      else:
         print 'get nominal measured/unfolded from file: ',self.nominal_file.GetName()
         self.nominal_measured   = self.nominal_file.Get(self.get_histoname("_data_measured"))
         self.nominal_unfolded   = self.nominal_file.Get(self.get_histoname("_data_unfolded"))
         
         if(self.nominal_mc_truth.Integral('width') > 0):
            self.nominal_measured_norm = self.nominal_measured.Clone(self.nominal_measured.GetName() + '_norm')
            self.nominal_measured_norm.Scale(self.nominal_measured.Integral('width')/self.nom_truth_int)
            self.nominal_unfolded_norm = self.nominal_unfolded.Clone(self.nominal_unfolded.GetName() + '_norm')
            self.nominal_unfolded_norm.Scale(self.nominal_unfolded.Integral('width')/self.nom_truth_int)
      
      # also need to the adjusted truth/reco for comparison
      self.adjusted_raw_filename = self.adjusted_data_path + "/" + self.adjustLabel + "/MC_" + self.channel + "_T1_McAtNlo_Jimmy_" + self.systematic + "_" + self.half[0:-1] + ".root"
      self.adjusted_raw_file     = TFile(self.adjusted_raw_filename)
      if not self.adjusted_raw_file.IsOpen():
         print 'ERROR opening',self.adjusted_raw_file.GetName()
         return
      else:
         print 'get adjusted truth/reco from file: ',self.adjusted_raw_file.GetName()
         
         truthName = ''
         recoName  = ''
         if self.sampleName.find('Tag1_Top1_lhood') >= 0:
            truthName = 'Kinem_likelihoodCut/TopFit_NTag1/PartonTop1/PartonTop1PtSpectrum'
            recoName  = 'Kinem_likelihoodCut/TopFit_NTag1/Perm0Top1/Perm0Top1PtSpectrum'
         elif self.sampleName.find('Tag1_Top2_lhood') >= 0:
            truthName = 'Kinem_likelihoodCut/TopFit_NTag1/PartonTop2/PartonTop2PtSpectrum'
            recoName  = 'Kinem_likelihoodCut/TopFit_NTag1/Perm0Top2/Perm0Top2PtSpectrum'
         elif self.sampleName.find('Tag1_SystemMass') >=0:
            truthName = 'Kinem_likelihoodCut/TopFit_NTag1/PartonSystem/PartonSystemMassSpectrum'
            recoName = 'Kinem_likelihoodCut/TopFit_NTag1/Perm0System/Perm0SytemMassSpectrum'
         elif self.sampleName.find('Tag1_SystemPt') >=0:
            truthName = 'Kinem_likelihoodCut/TopFit_NTag1/PartonSystem/PartonSystemPtSpectrum'
            recoName = 'Kinem_likelihoodCut/TopFit_NTag1/Perm0System/Perm0SystemPtSpectrum'
         elif self.sampleName.find('Tag1_SystemRapidity') >=0:
            truthName = 'Kinem_likelihoodCut/TopFit_NTag1/PartonSystem/PartonSystemRapiditySpectrum'
            recoName = 'Kinem_likelihoodCut/TopFit_NTag1/Perm0System/Perm0SystemRapiditySpectrum'
         
         print 'sample: ',sampleName,', truthName: ',truthName
         
         self.adjusted_mc_truth   = self.adjusted_raw_file.Get(truthName)
         print 'adjusted_mc_truth: ',self.adjusted_mc_truth
         if not self.adjusted_mc_truth:
            print '[File::__init__] ERROR retrieving histogram',truthName,'from file',self.adjusted_raw_filename
         self.adjusted_mc_reco    = self.adjusted_raw_file.Get(recoName)
         print 'adjusted_mc_reco: ',self.adjusted_mc_reco
         if not self.adjusted_mc_reco:
            print '[File::__init__] ERROR retrieving histogram',recoName,'from file',self.adjusted_raw_filename
         
         if(self.nominal_mc_truth.Integral('width') > 0):
            self.adjusted_mc_truth_norm = self.adjusted_mc_truth.Clone(self.adjusted_mc_truth.GetName() + '_norm')
            self.adjusted_mc_truth_norm.Scale(self.adjusted_mc_truth.Integral('width')/self.nom_truth_int)
            self.adjusted_mc_reco_norm = self.adjusted_mc_reco.Clone(self.adjusted_mc_reco.GetName() + '_norm')
            self.adjusted_mc_reco_norm.Scale(self.adjusted_mc_reco.Integral('width')/self.nom_truth_int)
      
      
      
      # set the unfolding method
      # -s   >   SVD
      # -b   >   Bayesian
      # -c   >   BinByBin
      # -f   >   Inversion
      #self.method             = method
      #if(self.systematic.find("bayesian") >= 0):
         #self.method = " -b "
      #if(self.systematic.find("binbybin") >= 0):
         #self.method = " -c "
      #if(self.systematic.find('inversion') >= 0):
         #self.method = " -f "
      
   
   def get_output_filename(self):
      filename = self.sampleName + "_" + self.channel
      if(self.nToys >= 0):
         filename += "_toy" + str( self.nToys )
      
      if (not (self.systematic.find("bayesian")  >= 0 or
               self.systematic.find("binbybin")  >= 0 or
               self.systematic.find('inversion') >= 0
               )
         ):
         if(self.method.find("-s") >= 0):
            filename += "_svd_reg" + str( self.regValue )
         elif(self.method.find("-b") >= 0):
            filename += "_bay_iter" + str( self.nIter )
         elif(self.method.find("-c") >= 0):
            filename += "_bin"
         elif(self.method.find("-f") >= 0):
            filename += "_inv"
      
      filename += "_" + self.half
      
      filename += ".root"
      
      return filename
   
   def get_histoname(self,additional):
      "unfolding/toys/"+ self.systematic + "/" + self.channel + "/" + self.sampleName + "/SVD/reg4/toy100/H_el_Tag1_Top1_lhood_SVD_reg4_toy100_mc_truth"
      
      histoname = "unfolding/toys/"
      histoname += self.systematic
      histoname += "/" + self.channel
      histoname += "/" + self.sampleName
      
      
      if (not (self.systematic.find("bayesian")  >= 0 or
               self.systematic.find("binbybin")  >= 0 or
               self.systematic.find('inversion') >= 0
               )
         ):
         if(self.method.find("-s") >= 0):
            histoname += "/SVD/reg" + str( self.regValue )
         elif(self.method.find("-b") >= 0):
            histoname += "/Bayes/iter" + str( self.nIter )
         elif(self.method.find("-c") >= 0):
            histoname += "/BinByBin"
         elif(self.method.find("-f") >= 0):
            histoname += "/Inversion"
      
      if(self.nToys >= 0):
         histoname += "/toy" + str( self.nToys )
      
      histoname += "/H"
      
      histoname += "_" + self.channel
      histoname += "_" + self.sampleName
      
      if (not (self.systematic.find("bayesian")  >= 0 or
               self.systematic.find("binbybin")  >= 0 or
               self.systematic.find('inversion') >= 0
               )
         ):
         if(self.method.find("-s") >= 0):
            histoname += "_SVD_reg" + str( self.regValue )
         elif(self.method.find("-b") >= 0):
            histoname += "_Bayes_iter" + str( self.nIter )
         elif(self.method.find("-c") >= 0):
            histoname += "_BinByBin"
         elif(self.method.find("-f") >= 0):
            histoname += "_Inversion"
      
      if(self.nToys >= 0):
         histoname += "_toy" + str( self.nToys )
      
      histoname += additional
      
      return histoname


if __name__ == "__main__":
   main()


