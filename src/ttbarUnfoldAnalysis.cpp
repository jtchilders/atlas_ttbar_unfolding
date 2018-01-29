#include "ttbarUnfoldAnalysis.h"
#include "FileList.h"
#include "FileHandler.h"
#include "SystematicSample.h"
#include "globals.h"
#include "Th1PlusLegend.h"

#include "TH1D.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TDirectory.h"
#include "TLegend.h"
#include "TStyle.h"
#include "MeanValue.h"
#include "KinematicVariable.h"
#include "TopDecayChannel.h"

#include <cmath>
#include <map>

ttbarUnfoldAnalysis::ttbarUnfoldAnalysis(void){
   m_rootFile                 = new FileHandler;
   
   m_ensembleDeviation        = 0;
   m_ensembleDeviationRMS     = 0;
   
   m_rmsResidualsOverTruth    = 0;
   m_meanResidualsOverTruth   = 0;
   m_rmsComparison            = 0;
   
}

ttbarUnfoldAnalysis::~ttbarUnfoldAnalysis(void){
   
   if(m_ensembleDeviation){
      std::cout << "[ttbarUnfoldAnalysis::~ttbarUnfoldAnalysis] deleting ensembleDeviation\n";
      delete m_ensembleDeviation;
      m_ensembleDeviation = 0;
   }
   
   if(m_rmsResidualsOverTruth){
      std::cout << "[ttbarUnfoldAnalysis::~ttbarUnfoldAnalysis] deleting rmsResidualsOverTruth\n";
      delete m_rmsResidualsOverTruth;
      m_rmsResidualsOverTruth = 0;
   }
   
   if(m_meanResidualsOverTruth){
      std::cout << "[ttbarUnfoldAnalysis::~ttbarUnfoldAnalysis] deleting meanResidualsOverTruth\n";
      delete m_meanResidualsOverTruth;
      m_meanResidualsOverTruth = 0;
   }
   
   if(m_rmsComparison){
      std::cout << "[ttbarUnfoldAnalysis::~ttbarUnfoldAnalysis] deleting rmsComparison\n";
      delete m_rmsComparison;
      m_rmsComparison = 0;
   }
   
   if(m_rootFile){
      m_rootFile->Close();
      std::cout << "[ttbarUnfoldAnalysis::~ttbarUnfoldAnalysis] deleting outputFile\n";
      delete m_rootFile;
      m_rootFile = 0;
   }
   
}

bool ttbarUnfoldAnalysis::RunOverEnsembleDataSets(FileList* mc_files,FileList* data_files, const std::string sampleName, const unsigned int regValue){
   

   std::vector<TH1D*> m_deviations;
   std::vector<TH1D*> m_ensembleDeviationPerBin;
   
   //    the number of files in each list should be equal OR
   //    the mc files list can have only one file which will
   //    be compared against all the data files.
   bool globalMC=false;
   if( mc_files->size() != data_files->size()){
      std::cout << " Number of files is not the same.\n";
      if( mc_files->size() != 1){
         std::cerr << " If number of files is not the same, the number of MC files is only allowed to be one, but list contains " << mc_files->size() << " mc files, and " << data_files->size() << " data files.\n\n";
         return false;
      }
      else
         globalMC = true;
   }
   
   // determine if this is e+jets or mu+jets
   TopDecayChannel channel;
   channel.SetChannelUsingFilename((*mc_files)[0]);
   
   // use the systematic sample for the root folder naming structure
   SystematicSample syst(SystematicSample::NOMINAL);
   if(m_outputFilename.size() > 0)
      m_rootFile->SetOutputFilename(m_outputFilename);
   m_rootFile->Open();
   
   // make ps output filename with regValue
   std::stringstream BaseName;
   BaseName << channel.GetChannelString() << "_" << sampleName << "_reg" << regValue;
   std::stringstream psfilenamestream;
   psfilenamestream << "Ensembles_" << BaseName.str() << ".ps";
   std::string psfilename = psfilenamestream.str();
   
   
   
   for(unsigned int i=0;i<data_files->size();++i){
      
      std::string mcFilename;
      if(globalMC)
         mcFilename = (*mc_files)[0];
      else
         mcFilename = (*mc_files)[i];
      
      std::string dataFilename = (*data_files)[i];
      
      std::cout << " Comparing: \n";
      std::cout << "  data: " << dataFilename << std::endl;
      std::cout << "  mc:   " << mcFilename << std::endl;
      
      ttbarUnfold* analysis = new ttbarUnfold(dataFilename,mcFilename,sampleName);
      analysis->SetRegularization(regValue);
      analysis->SetToysForUnfolding(500);
      // get base histogram name
      std::string baseHistoName = GetHistogramNameEnsembles(channel.GetChannelString(),sampleName,analysis->GetUnfoldingMethod(),regValue,i,"") + "_";
      analysis->SetHistoBaseName(baseHistoName);
      
      analysis->doUnfolding();
      
      // move to directory for this ensemble
      m_rootFile->toEnsembles(syst,channel.GetChannelString(),sampleName,analysis->GetUnfoldingMethod(),regValue,i);
      
      gStyle->SetPadTopMargin(1.4);
      gStyle->SetPadRightMargin(2.4);

      if(i == 0) {
         std::string canvasName = GetCanvasNameEnsembles(channel.GetChannelString(),sampleName,analysis->GetUnfoldingMethod(),regValue,i,"migration");
         TCanvas *can = new TCanvas(canvasName.c_str(),canvasName.c_str(),0,0,800,600);
         
         analysis->DrawMigrationMatrix(sampleName);
         
         std::stringstream pdfname;
         pdfname << canvasName <<".pdf";
         can->SaveAs(pdfname.str().c_str()); 
         
         can->Write();
         
         delete can;
         can = 0;
         
      }
      
      std::string canvasName = GetCanvasNameEnsembles(channel.GetChannelString(),sampleName,analysis->GetUnfoldingMethod(),regValue,i,"");
      TCanvas *can = new TCanvas(canvasName.c_str(),canvasName.c_str(),0,0,800,600);
      can->Divide(2,2);
      can->cd(1);
      TLatex *   tex = new TLatex(0.8,0.8,canvasName.c_str());
      analysis->DrawPtSpectraScale(sampleName);
      can->cd(2);
      analysis->DrawDeviationReco();
      can->cd(3);
      analysis->DrawDeviation();
      can->cd(4);
      can->GetPad(4)->SetLogy();
      analysis->DrawDistributionD();
      
      if(i == 0){
         std::string tmp = psfilename + "(";
         can->SaveAs(tmp.c_str());
      }
      else if( i == data_files->size()-1 ){
         std::string tmp = psfilename + ")";
         can->SaveAs(tmp.c_str());
      }
      else
         can->SaveAs(psfilename.c_str());

      // when we write we have to make sure ROOT
      // will put our things in the right place
      can->Write();
      if(i>0) {
         std::string deviationName = GetHistogramNameEnsembles(channel.GetChannelString(),sampleName,analysis->GetUnfoldingMethod(),regValue,i-1,"deviation");
         m_deviations.push_back((TH1D*)analysis->GetDeviation()->Clone(deviationName.c_str()));
         m_deviations.back()->AddDirectory(false);
      }

            
      // when we write we have to make sure ROOT
      // will put our things in the right place
      if(analysis->GetMcTruth())
         analysis->GetMcTruth()->Write(analysis->GetMcTruth()->GetName(),TObject::kOverwrite);
      if(analysis->GetMcTruthNorm())
         analysis->GetMcTruthNorm()->Write(analysis->GetMcTruthNorm()->GetName(),TObject::kOverwrite);
      if(analysis->GetMcTruthScaled())
         analysis->GetMcTruthScaled()->Write(analysis->GetMcTruthScaled()->GetName(),TObject::kOverwrite);
      if(analysis->GetMcReco())
         analysis->GetMcReco()->Write(analysis->GetMcReco()->GetName(),TObject::kOverwrite);
      if(analysis->GetMcRecoNorm())
         analysis->GetMcRecoNorm()->Write(analysis->GetMcRecoNorm()->GetName(),TObject::kOverwrite);
      if(analysis->GetMcRecoScaled())
         analysis->GetMcRecoScaled()->Write(analysis->GetMcRecoScaled()->GetName(),TObject::kOverwrite);
      if(analysis->GetMcMigration()) 
         analysis->GetMcMigration()->Write(analysis->GetMcMigration()->GetName(),TObject::kOverwrite);
      if(analysis->GetDataMeasured())
         analysis->GetDataMeasured()->Write(analysis->GetDataMeasured()->GetName(),TObject::kOverwrite);
      if(analysis->GetDataUnfolded())
         analysis->GetDataUnfolded()->Write(analysis->GetDataUnfolded()->GetName(),TObject::kOverwrite);
      if(analysis->GetDataUnfoldedNorm())
         analysis->GetDataUnfoldedNorm()->Write(analysis->GetDataUnfoldedNorm()->GetName(),TObject::kOverwrite);
      if(analysis->GetDataMeasuredNorm())
         analysis->GetDataMeasuredNorm()->Write(analysis->GetDataMeasuredNorm()->GetName(),TObject::kOverwrite);
      if(analysis->GetRegularizationScan())
         analysis->GetRegularizationScan()->Write(analysis->GetRegularizationScan()->GetName(),TObject::kOverwrite);
      if(analysis->GetDeviation())
         analysis->GetDeviation()->Write(analysis->GetDeviation()->GetName(),TObject::kOverwrite);  
      if(analysis->GetDeviationReco())
         analysis->GetDeviationReco()->Write(analysis->GetDeviationReco()->GetName(),TObject::kOverwrite);   
      delete analysis;
      analysis = 0;
      
      delete can;
      can = 0;
      
   }
   
   // save summary plots in top directory ( no ensemble number )
   m_rootFile->toEnsembles(syst,channel.GetChannelString(),sampleName,ttbarUnfold::SVD,regValue,0);
   
   std::string nameDev = GetHistogramNameEnsembles(channel.GetChannelString(),sampleName,ttbarUnfold::SVD,regValue,0,"mean");
   m_ensembleDeviation = (TH1D *) m_deviations[0]->Clone(nameDev.c_str());
   m_ensembleDeviation->SetDirectory(0);
   m_ensembleDeviation->Reset();

   std::string nameDevRMS = GetHistogramNameEnsembles(channel.GetChannelString(),sampleName,ttbarUnfold::SVD,regValue,0,"rms");
   m_ensembleDeviationRMS = (TH1D *) m_deviations[0]->Clone(nameDevRMS.c_str());
   m_ensembleDeviationRMS->SetDirectory(0);
   m_ensembleDeviationRMS->Reset();
   
   for(unsigned int i=0;i<=m_deviations[0]->GetNbinsX();++i){
      TString perbinName("Deviation per bin ");
      perbinName+=i;
      TH1D* graph_name = new TH1D (perbinName,perbinName, 1000,-100,100);
      m_ensembleDeviationPerBin.push_back(graph_name);

      for(unsigned int j=0;j<(m_deviations.size());++j){
         m_ensembleDeviationPerBin[i]->Fill(m_deviations[j]->GetBinContent(i), m_deviations[j]->GetBinError(i));
      }
  }

   double min=999., max=-999., rms_max=-999.;
   for(unsigned int j=0;j<m_ensembleDeviationPerBin.size();++j){
      double mean = m_ensembleDeviationPerBin[j]->GetMean();
      double rms  = m_ensembleDeviationPerBin[j]->GetRMS();
      double emean = mean / (sqrt(m_ensembleDeviationPerBin[j]->GetEntries()));
      double erms = rms / (sqrt(m_ensembleDeviationPerBin[j]->GetEntries()));
      
      m_ensembleDeviation->SetBinContent(j,mean);
      m_ensembleDeviation->SetBinError(j,emean);
      m_ensembleDeviationRMS->SetBinContent(j,rms);
      m_ensembleDeviationRMS->SetBinError(j,erms);

      if(max<mean) max=mean+0.2*fabs(mean);
      if(min>mean) min=mean-0.2*fabs(mean);
      if(rms_max<rms) rms_max = rms+(0.2*rms);
   }

   // when we write we have to make sure ROOT
   // will put our things in the right place
   m_ensembleDeviation->Write(m_ensembleDeviation->GetName(),TObject::kOverwrite);
   m_ensembleDeviationRMS->Write(m_ensembleDeviationRMS->GetName(),TObject::kOverwrite);

   std::string canvasName = GetCanvasNameEnsembles(channel.GetChannelString(),sampleName,ttbarUnfold::SVD,regValue,0,"allDeviations");
   TCanvas *can = new TCanvas(canvasName.c_str(),canvasName.c_str(),0,0,800,600);
   
   for(unsigned int i=0;i<m_deviations.size();++i){
      m_deviations[i]->SetLineColor(i+1);
      m_deviations[i]->SetMarkerColor(i+1);
      
      if(i == 0){
         m_deviations[i]->SetMinimum(-1.0);
         m_deviations[i]->SetMaximum(1.0);
         m_deviations[i]->Draw("e");
      }
      else
         m_deviations[i]->Draw("e same");
   }
   
   can->Write(can->GetName(),TObject::kOverwrite);
   can->SaveAs("tmp.ps");
   
   canvasName = GetCanvasNameEnsembles(channel.GetChannelString(),sampleName,ttbarUnfold::SVD,regValue,0,"mean");
   TCanvas* can1 = new TCanvas(canvasName.c_str(),canvasName.c_str(),0,0,800,600);
   m_ensembleDeviation->SetMinimum(min);
   m_ensembleDeviation->SetMaximum(max);
   m_ensembleDeviation->Draw("e");
   
   can1->Write(can1->GetName(),TObject::kOverwrite);  
   can1->SaveAs("tmp1.ps");   
   
   canvasName = GetCanvasNameEnsembles(channel.GetChannelString(),sampleName,ttbarUnfold::SVD,regValue,0,"rms");
   TCanvas *can2 = new TCanvas(canvasName.c_str(),canvasName.c_str(),0,0,800,600);
   m_ensembleDeviationRMS->SetMinimum(0.);
   m_ensembleDeviationRMS->SetMaximum(rms_max);
   m_ensembleDeviationRMS->GetYaxis()->SetTitle("RMS_{(x^{Unfolded}-x^{Truth})/x^{Truth}}");
   m_ensembleDeviationRMS->Draw();  
   
   can2->Write(can2->GetName(),TObject::kOverwrite);  
   can2->SaveAs("tmp2.ps");
   
   // merge to the top of the other post script
   psmerge("newtmp.ps","tmp.ps",psfilename);
   rmfile(psfilename);
   rmfile("tmp.ps");
   mvfile("newtmp.ps",psfilename);

   // merge to the top of the other post script
   psmerge("newtmp.ps","tmp1.ps",psfilename);
   rmfile(psfilename);
   rmfile("tmp1.ps");
   mvfile("newtmp.ps",psfilename);

   // merge to the top of the other post script
   psmerge("newtmp.ps","tmp2.ps",psfilename);
   rmfile(psfilename);
   rmfile("tmp2.ps");
   mvfile("newtmp.ps",psfilename);

   for(unsigned int i=0;i<m_deviations.size();++i){
      if(m_deviations[i]){
         std::cout << "[ttbarUnfoldAnalysis::~ttbarUnfoldAnalysis] deleting deviations\n";
         delete m_deviations[i];
         m_deviations[i] = 0;
      }
   }

   for(unsigned int i=0;i<m_ensembleDeviationPerBin.size();++i){
      if(m_ensembleDeviationPerBin[i]){
         std::cout << "[ttbarUnfoldAnalysis::~ttbarUnfoldAnalysis] deleting Deviation per Bin\n";
         delete m_ensembleDeviationPerBin[i];
         m_ensembleDeviationPerBin[i] = 0;
      }
   }
   

   delete can;
   can = 0;
   delete can1;
   can1 = 0;
   delete can2;
   can2 = 0;

   
   m_rootFile->Close();
   
   return true;
   
 
}

bool ttbarUnfoldAnalysis::OptimizeRegValueWithEnsembles(FileList* mc,FileList* data,const std::string sampleName,const unsigned int mregValue){
   
   std::vector<TH1D*> deviationPerRegValue;
   std::vector<TH1D*> RMSdeviationPerRegValue;
   
   ttbarUnfold::UnfoldMethod unfoldMethod = ttbarUnfold::SVD;

   int MK=9;
   if(mregValue>=2) MK = mregValue;

   // loop over possible regularization values
   double min=999., max=-999., rms_max=-999.;

   for(unsigned int i=2;i<=MK;++i){

      m_ensembleDeviation        = 0;
      m_ensembleDeviationRMS     = 0;
      
      if( !RunOverEnsembleDataSets(mc,data,sampleName,i) ){
         std::cerr << " ERROR running over ensemble data sets\n";
         return false;
      }
      
      std::string name = m_ensembleDeviation->GetName();
      name += "Clone";
      deviationPerRegValue.push_back((TH1D*)m_ensembleDeviation->Clone(name.c_str()));
      double m = m_ensembleDeviation->GetMinimum();
      double M = m_ensembleDeviation->GetMaximum();

      std::string name1 = m_ensembleDeviationRMS->GetName();
      name1 += "Clone";
      RMSdeviationPerRegValue.push_back((TH1D*)m_ensembleDeviationRMS->Clone(name1.c_str()));
      double MRMS = m_ensembleDeviationRMS->GetMaximum();

      if(min>m) min=m-1.15*fabs(m);
      if(max<M) max=M+1.15*fabs(M);
      if(rms_max<MRMS) rms_max=MRMS+0.1*fabs(MRMS);
      
   }
   
   // determine if this is e+jets or mu+jets
   TopDecayChannel channel;
   channel.SetChannelUsingFilename((*mc)[0]);
   
   if(m_outputFilename.size() > 0)
      m_rootFile->SetOutputFilename(m_outputFilename);
   m_rootFile->Open();
   SystematicSample syst(SystematicSample::NOMINAL);
   m_rootFile->toEnsembles(syst,channel.GetChannelString(),sampleName,unfoldMethod);
   
   std::string canvasName = GetCanvasNameEnsembles(channel.GetChannelString(),sampleName,unfoldMethod,-1,-1,"mean_allReg");
   TCanvas* canM = new TCanvas(canvasName.c_str(),canvasName.c_str(),0,0,800,600);
   TLegend *leg = new TLegend(0.28, 0.68, 0.38, 0.93);
   leg->SetFillColor(0);
   leg->SetLineColor(0);
   leg->SetBorderSize(0);
   TLegend *leg1 = new TLegend(0.4, 0.68, 0.48, 0.93);
   leg1->SetFillColor(0);
   leg1->SetLineColor(0);
   leg1->SetBorderSize(0);

   unsigned int size = deviationPerRegValue.size();

   for(unsigned int i=0;i<size;++i){
      float hsize = (float) (size)/2.;
      if(i<hsize){
         leg -> AddEntry(deviationPerRegValue[i], Form("k=%i", i+2), "PL");
      }
      if(i>=hsize && i<size){
         leg1 -> AddEntry(deviationPerRegValue[i], Form("k=%i", i+2), "PL");
      }
      deviationPerRegValue[i]->SetLineColor(i+1);
      deviationPerRegValue[i]->SetMarkerColor(i+1);
      deviationPerRegValue[i]->SetMarkerStyle(i+20);
      if(i == 0){
         deviationPerRegValue[i]->SetMinimum(min);
         deviationPerRegValue[i]->SetMaximum(max);
         deviationPerRegValue[i]->Draw("P");
      }
      else deviationPerRegValue[i]->Draw("P same");
   }
   leg ->Draw();
   leg1->Draw();

   std::string psnameM = canvasName + ".pdf";
   canM->SaveAs(psnameM.c_str());
   canM->Write(canM->GetName(),TObject::kOverwrite);  

   canvasName = GetCanvasNameEnsembles(channel.GetChannelString(),sampleName,unfoldMethod,-1,-1,"rms_allReg");
   TCanvas* canR = new TCanvas(canvasName.c_str(),canvasName.c_str(),0,0,800,600);

   for(unsigned int i=0;i<size;++i){
      RMSdeviationPerRegValue[i]->SetLineColor(i+1);
      RMSdeviationPerRegValue[i]->SetMarkerColor(i+1);
      RMSdeviationPerRegValue[i]->SetMarkerStyle(i+20);
      if(i == 0){
         RMSdeviationPerRegValue[i]->SetMinimum(0.0);
         RMSdeviationPerRegValue[i]->SetMaximum(rms_max);
         RMSdeviationPerRegValue[i]->Draw("P");
      }
      else RMSdeviationPerRegValue[i]->Draw("P same");
   }
   leg -> Draw();
   leg1->Draw();

   std::string psnameR = canvasName + ".pdf";
   canR->SaveAs(psnameR.c_str());
   canR->Write(canR->GetName(),TObject::kOverwrite);

   delete canM;
   canM = 0;
   delete canR;
   canR = 0;

   m_rootFile->Close();
}

bool ttbarUnfoldAnalysis::ComparisonEnsembleDataSets(const std::string  mc,const std::string  data, FileList* sample,const std::string flag,const unsigned int regValue){
   
   std::vector<TH1D*> Mean_PerSample;
   std::vector<TH1D*> Rms_PerSample;
   
   ttbarUnfold::UnfoldMethod unfoldMethod = ttbarUnfold::SVD;

   std::string sampleName0 = (*sample)[0];

   std::cout << "  mc: " << mc << std::endl;
   std::cout << "  data: " << data << std::endl;

   // determine if this is e+jets or mu+jets
   TopDecayChannel channel;

   // loop over possible samples to be compared
   double min=999., max=-999., rms_max=-999.;

   for(unsigned int i=0;i<sample->size();++i){
      std::string sampleName = (*sample)[i];
      std::stringstream mc_sample, data_sample;
      if(flag=="yes"){
         mc_sample << mc <<"_"<<sampleName<<".txt";
         data_sample << data <<"_"<<sampleName<<".txt";
      }
      else {
         mc_sample << mc <<".txt";
         data_sample << data <<".txt";
      }
      
      FileList* mc_files = new FileList(mc_sample.str());
      FileList* data_files = new FileList(data_sample.str());

      if(i==0)
         channel.SetChannelUsingFilename((*mc_files)[0]);

      if( !RunOverEnsembleDataSets(mc_files,data_files,sampleName,regValue) ){
         std::cerr << " ERROR running over ensemble data sets\n";
         return false;
      }
      
      std::string name = m_ensembleDeviation->GetName();
      name += "Clone";
      Mean_PerSample.push_back((TH1D*)m_ensembleDeviation->Clone(name.c_str()));
      double m = m_ensembleDeviation->GetMinimum();
      double M = m_ensembleDeviation->GetMaximum();

      std::string name1 = m_ensembleDeviationRMS->GetName();
      name1 += "Clone";
      Rms_PerSample.push_back((TH1D*)m_ensembleDeviationRMS->Clone(name1.c_str()));
      double MRMS = m_ensembleDeviationRMS->GetMaximum();

      if(min>m) {
         if(m<0) min=m*1.5;
         if(m>0) min=m*0.5;
      }
      if(max<M) {
         if(M>0) max=M*1.5;
         if(M<0) max=M*0.5;
      }
      if(rms_max<MRMS) rms_max=MRMS+0.1*fabs(MRMS);
      
   }
   
   if(m_outputFilename.size() > 0)
      m_rootFile->SetOutputFilename(m_outputFilename);
   m_rootFile->Open();
   SystematicSample syst(SystematicSample::NOMINAL);
   std::string allSamples = "allSamples";
   m_rootFile->toEnsembles(syst,channel.GetChannelString(),allSamples,unfoldMethod,regValue);

   std::string canvasName = GetCanvasNameEnsembles(channel.GetChannelString(),allSamples,unfoldMethod,regValue,-1,"mean");
   TCanvas* canM = new TCanvas(canvasName.c_str(),canvasName.c_str(),0,0,800,600);
   TLegend *leg = new TLegend(0.20,0.62,0.50,0.89);
   leg->SetFillColor(0);
   leg->SetLineColor(0);
   leg->SetBorderSize(0);

   unsigned int size = Mean_PerSample.size();

   for(unsigned int i=0;i<size;++i){
      std::string sampleName = (*sample)[i];
      leg -> AddEntry(Mean_PerSample[i], Form("%s", sampleName.c_str()), "PL");

      Mean_PerSample[i]->SetLineColor(i+1);
      Mean_PerSample[i]->SetMarkerSize(1.2);
      Mean_PerSample[i]->SetMarkerColor(i+1);
      Mean_PerSample[i]->SetMarkerStyle(i+20);
      if(i == 0){
         Mean_PerSample[i]->SetMinimum(min);
         //Mean_PerSample[i]->SetMaximum(max);
         Mean_PerSample[i]->SetMaximum(0.06);
         Mean_PerSample[i]->Draw("P");
      }
      else Mean_PerSample[i]->Draw("P same");
   }
   leg ->Draw();

   std::string psnameM = canvasName + ".pdf";
   canM->SaveAs(psnameM.c_str());
   m_rootFile->toEnsembles(syst,channel.GetChannelString(),allSamples,unfoldMethod,regValue);
   canM->Write(canM->GetName(),TObject::kOverwrite);  

   canvasName = GetCanvasNameEnsembles(channel.GetChannelString(),allSamples,unfoldMethod,regValue,-1,"rms");
   TCanvas* canR = new TCanvas(canvasName.c_str(),canvasName.c_str(),0,0,800,600);
   TLegend *legR = new TLegend(0.20,0.58,0.50,0.85);
   legR->SetFillColor(0);
   legR->SetLineColor(0);
   legR->SetBorderSize(0);

   for(unsigned int i=0;i<size;++i){
      std::string sampleName = (*sample)[i];
      legR -> AddEntry(Rms_PerSample[i], Form("%s", sampleName.c_str()), "PL");

      Rms_PerSample[i]->SetLineColor(i+1);
      Rms_PerSample[i]->SetMarkerColor(i+1);
      Rms_PerSample[i]->SetMarkerSize(1.2);
      Rms_PerSample[i]->SetMarkerStyle(i+20);
      if(i == 0){
         Rms_PerSample[i]->SetMinimum(0.0);
         Rms_PerSample[i]->SetMaximum(rms_max);
         Rms_PerSample[i]->Draw("P");
      }
      else Rms_PerSample[i]->Draw("P same");
   }
   legR -> Draw();

   std::string psnameR = canvasName + ".pdf";
   canR->SaveAs(psnameR.c_str());
   m_rootFile->toEnsembles(syst,channel.GetChannelString(),allSamples,unfoldMethod,regValue);
   canR->Write(canR->GetName(),TObject::kOverwrite);

   delete canM;
   canM = 0;
   delete canR;
   canR = 0;

   m_rootFile->Close();
}


// Run the unfolding with the toy models
bool ttbarUnfoldAnalysis::RunToyUnfolding(const std::string mc_filename,
                                          const std::string data_filename,
                                          const SystematicSample syst,
                                          const std::string sampleName,
                                          const std::string measuredHistoName,
                                          const ttbarUnfold::UnfoldMethod unfoldMethod,
                                          const unsigned int regValue,
                                          const unsigned int iterations,
                                          const unsigned int nToys,
                                          std::vector<TH1D*>* meanOverTruth,
                                          std::vector<TH1D*>* rmsOverTruth,
                                          std::vector<TH1D*>* compRms
                                         )
{
   
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding] Comparing: \n";
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  data: " << data_filename << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  mc:   " << mc_filename << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  syst:   " << syst.str() << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  sample:   " << sampleName << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  measured Histo Name: " << measuredHistoName << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  unfold method: " << ttbarUnfold::GetMethodString(unfoldMethod) << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  nToys:   " << nToys << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  regValue:   " << regValue << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  iterations: " << iterations << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  efficiency file: " << m_efficiencyFilename << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  efficiency histo name: " << m_efficiencyHist << std::endl;
   
   // determine if this is e+jets or mu+jets
   TopDecayChannel channel;
   channel.SetChannelUsingFilename(mc_filename);
   
   // retrieve efficiency histogram and pass it to the unfolding
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  Getting Efficiency File.\n";
   TH1* efficiencyHisto = 0;
   if(m_efficiencyFilename.size() > 0 && m_efficiencyHist.size() > 0)
      efficiencyHisto = globals::GetEfficiencyHistogram(m_efficiencyFilename,m_efficiencyHist);
   if(!efficiencyHisto){
      std::cerr << "[ttbarUnfoldAnalysis::RunToyUnfolding] ERROR: Could not retrieve efficiency Histogram\n";
//       return false;
   }
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  done.\n";
   
   // get right labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string kinVarLabel  = kinVar->GetKinVarLabel();
   std::string UkinVarLabel = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   std::string NkinVarLabel = kinVar->GetNKinVarLabel();
   
   delete kinVar; kinVar = 0;
   
   ttbarUnfold* unfolded = new ttbarUnfold(data_filename,mc_filename,sampleName,efficiencyHisto);
   unfolded->SetToysForUnfolding(nToys);
   unfolded->SetMeasuredDataMinusBkgdHistogramName(measuredHistoName);
   unfolded->SetUnfoldingMethod(unfoldMethod);
   
   std::string histoBaseName = GetHistogramNameToys(channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys,"") + "_";
   unfolded->SetHistoBaseName(histoBaseName.c_str());
   if(unfolded->GetUnfoldingMethod() == ttbarUnfold::SVD)
      unfolded->SetRegularization(regValue);
   else if(unfolded->GetUnfoldingMethod() == ttbarUnfold::Bayes)
      unfolded->SetBayesianIterations(iterations);
   
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding] doUnfolding \n";
   if(!unfolded->doUnfolding()){
      std::cerr << "[ttbarUnfoldAnalysis::RunToyUnfolding] ERROR running doUnfolding.\n";
      return false;
   }
   
   if(m_outputFilename.size() > 0)
      m_rootFile->SetOutputFilename(m_outputFilename);
   m_rootFile->Open();
   m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys);
   
   if(unfolded->GetMcTruth()){
      unfolded->GetMcTruth()->Write(unfolded->GetMcTruth()->GetName(),TObject::kOverwrite);
   }
   if(unfolded->GetMcTruthNorm())
      unfolded->GetMcTruthNorm()->Write(unfolded->GetMcTruthNorm()->GetName(),TObject::kOverwrite);
   if(unfolded->GetMcTruthScaled())
      unfolded->GetMcTruthScaled()->Write(unfolded->GetMcTruthScaled()->GetName(),TObject::kOverwrite);
   if(unfolded->GetMcTruthDiffxs())
      unfolded->GetMcTruthDiffxs()->Write(unfolded->GetMcTruthDiffxs()->GetName(),TObject::kOverwrite);
   if(unfolded->GetMcTruthDiffxsNorm())
      unfolded->GetMcTruthDiffxsNorm()->Write(unfolded->GetMcTruthDiffxsNorm()->GetName(),TObject::kOverwrite);
   
   
   m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys);
   
   if(unfolded->GetMcReco())
      unfolded->GetMcReco()->Write(unfolded->GetMcReco()->GetName(),TObject::kOverwrite);
   if(unfolded->GetMcRecoDiffxs())
      unfolded->GetMcRecoDiffxs()->Write(unfolded->GetMcRecoDiffxs()->GetName(),TObject::kOverwrite);
   if(unfolded->GetMcRecoDiffxsNorm())
      unfolded->GetMcRecoDiffxsNorm()->Write(unfolded->GetMcRecoDiffxsNorm()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetMcRecoNorm())
      unfolded->GetMcRecoNorm()->Write(unfolded->GetMcRecoNorm()->GetName(),TObject::kOverwrite);
   if(unfolded->GetMcRecoScaled())
      unfolded->GetMcRecoScaled()->Write(unfolded->GetMcRecoScaled()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetDataMeasuredMinusBkgd())
      unfolded->GetDataMeasuredMinusBkgd()->Write(unfolded->GetDataMeasuredMinusBkgd()->GetName(),TObject::kOverwrite);
   
   m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys);
   
   if(unfolded->GetDataMeasuredMinusBkgdNorm())
      unfolded->GetDataMeasuredMinusBkgdNorm()->Write(unfolded->GetDataMeasuredMinusBkgdNorm()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetMcMigration())
      unfolded->GetMcMigration()->Write(unfolded->GetMcMigration()->GetName(),TObject::kOverwrite);
   
   
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding] runToys \n";
   if(!unfolded->runToys()){
      std::cerr << "[ttbarUnfoldAnalysis::RunToyUnfolding] ERROR running runToys.\n";
      return false;
   }
   
   // keep diffxs and diffxsNorm so we can apply the correct rms errors to the plot
   // and then write it to the file
   TH1* diffxs = 0;
   TH1* diffxsNorm = 0;
   if(unfolded->GetRmsResiduals()){
      TH1* rms = unfolded->GetRmsResiduals();
      rms->Write(rms->GetName(),TObject::kOverwrite);
      
      
      if(unfolded->GetDataUnfolded()){
         TH1* dataUnfolded = unfolded->GetDataUnfolded();
         
         // copy rms residuals as the statistical errors on the unfolded plot
         for(unsigned int bin = 1; bin <= dataUnfolded->GetNbinsX();++bin){
            dataUnfolded->SetBinError(bin,rms->GetBinContent(bin));
         }
         
         m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys);
         dataUnfolded->Write(dataUnfolded->GetName(),TObject::kOverwrite);
         
         std::string name = dataUnfolded->GetName();
         name += "Norm";
         TH1* dataUnfoldedNorm = (TH1*)dataUnfolded->Clone(name.c_str()); 
         dataUnfoldedNorm->Scale(1.0/dataUnfolded->Integral("width"));
         dataUnfoldedNorm->GetYaxis()->SetTitle("#frac{1}{#SigmaN^{t}} N^{t}");
         dataUnfoldedNorm->Write(dataUnfoldedNorm->GetName(),TObject::kOverwrite);
         
         
         // convert counts to diff xs values
         if(efficiencyHisto){
            diffxs = globals::LumiEfficiencyCorrectedSpectra(dataUnfolded,sampleName,efficiencyHisto);
            name = GetHistogramNameToys(channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys,"data_unfolded_diffxs");
            diffxs->SetName(name.c_str());
            
            
            // normalize diffxs
            name = GetHistogramNameToys(channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys,"data_unfolded_diffxsNorm");
            diffxsNorm = (TH1*)diffxs->Clone(name.c_str());
         
            diffxsNorm->Scale(1.0/diffxs->Integral("width"));
            std::stringstream ntitle;
            ntitle <<";" <<UkinVarLabel<<";"<<NkinVarLabel;

            diffxsNorm->SetTitle(ntitle.str().c_str());
         }
         
         
         delete dataUnfoldedNorm;
      }
      
   }
   
   m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys);
   
   if(unfolded->GetMeanResiduals())
      unfolded->GetMeanResiduals()->Write(unfolded->GetMeanResiduals()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetRmsResidualsDiffxsNorm()){
      TH1* rmsDiffxsNorm = unfolded->GetRmsResidualsDiffxsNorm();
      rmsDiffxsNorm->Write(rmsDiffxsNorm->GetName(),TObject::kOverwrite);
      
      
      if(diffxsNorm){
         
         // copy RMS errors to the normalized diffxs plot for statistical errors
         for(unsigned int bin=1;bin<=diffxsNorm->GetNbinsX();++bin){
            diffxsNorm->SetBinError(bin,rmsDiffxsNorm->GetBinContent(bin));
         }
         
         
         m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys);
         diffxsNorm->Write(diffxsNorm->GetName(),TObject::kOverwrite);
         
         delete diffxsNorm;
      }
   }
   
   if(unfolded->GetMeanResidualsDiffxsNorm())
      unfolded->GetMeanResidualsDiffxsNorm()->Write(unfolded->GetMeanResidualsDiffxsNorm()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetRmsResidualsDiffxs()){
      TH1* rmsDiffxs = unfolded->GetRmsResidualsDiffxs();
      rmsDiffxs->Write(rmsDiffxs->GetName(),TObject::kOverwrite);
      
      if(diffxs){
         
         // copy RMS errors to the diffxs plot for statistical errors
         for(unsigned int bin=1;bin<=diffxs->GetNbinsX();++bin){
            diffxs->SetBinError(bin,rmsDiffxs->GetBinContent(bin));
         }
         
         
         m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys);
         diffxs->Write(diffxs->GetName(),TObject::kOverwrite);
         std::string effName = GetHistogramNameToys(channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys,"efficiency");
         efficiencyHisto->Write(effName.c_str(),TObject::kOverwrite);
         
         delete diffxs;
      }
   }
   
   m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys);
   
   if(unfolded->GetMeanResidualsDiffxs())
      unfolded->GetMeanResidualsDiffxs()->Write(unfolded->GetMeanResidualsDiffxs()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetUnfoldingError())
      unfolded->GetUnfoldingError()->Write(unfolded->GetUnfoldingError()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetMeanDeviation())
      unfolded->GetMeanDeviation()->Write(unfolded->GetMeanDeviation()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetRmsDeviation())
      unfolded->GetRmsDeviation()->Write(unfolded->GetRmsDeviation()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetMeanDeviationScaled())
      unfolded->GetMeanDeviationScaled()->Write(unfolded->GetMeanDeviationScaled()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetRmsDeviationScaled())
      unfolded->GetRmsDeviationScaled()->Write(unfolded->GetRmsDeviationScaled()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetRegularizationScan())
      unfolded->GetRegularizationScan()->Write(unfolded->GetRegularizationScan()->GetName(),TObject::kOverwrite);
   
   /// scale RMS by truth
   if( unfolded->GetRmsResiduals() && unfolded->GetMcTruthScaled()){
      TH1* rms = unfolded->GetRmsResiduals();
      TH1* truth = unfolded->GetMcTruthScaled();
      
      /// create histogram using rms and clear it
      std::string name = GetHistogramNameToys(channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys,"rmsResidualOverTruth");
      TH1D* rmsResidualsOverTruth = (TH1D*)rms->Clone(name.c_str());
      rmsResidualsOverTruth->Reset();
      rmsResidualsOverTruth->SetTitle("");
      rmsResidualsOverTruth->SetYTitle("(RMS of Toys)/Truth");
      rmsResidualsOverTruth->SetDirectory(false);
      
      for(unsigned int bin=1;bin<=rms->GetNbinsX();++bin){
         double RMS = rms->GetBinContent(bin);
         double eRMS = rms->GetBinError(bin);
         double Truth = truth->GetBinContent(bin);
         double eTruth = truth->GetBinError(bin);
         double binContent = RMS/Truth;
         double a = eRMS*eRMS;
         double b = (eTruth*eTruth*RMS*RMS)/(Truth*Truth);
         double binError = (1./Truth)*sqrt(a + b);
         
         rmsResidualsOverTruth->SetBinContent(bin,binContent);
         rmsResidualsOverTruth->SetBinError(bin,binError);
      }
      
      rmsResidualsOverTruth->Write(rmsResidualsOverTruth->GetName(),TObject::kOverwrite);
      
      if(rmsOverTruth) rmsOverTruth->push_back(rmsResidualsOverTruth);
      else{delete rmsResidualsOverTruth;rmsResidualsOverTruth=0;}
   }
   
   /// compare RMS to Errors on Unfolded result
   if( unfolded->GetRmsResiduals() && unfolded->GetDataUnfolded() ){
      TH1* rms = unfolded->GetRmsResiduals();
      TH1* unfoldedReco = unfolded->GetDataUnfolded();
      
      /// create histogram using rms and clear it
      std::string name = GetHistogramNameToys(channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys,"rmsComparison");
      TH1D* rmsComparison = (TH1D*)rms->Clone(name.c_str());
      rmsComparison->Reset();
      std::stringstream title;
      title << "RMS of Toys minus RMS of unfolded result for regularisation value " << regValue;
      rmsComparison->SetTitle(title.str().c_str());
      rmsComparison->SetYTitle("(Mean Toy RMS - Unfolded RMS)/Unfolded Result");
      rmsComparison->SetDirectory(false);
      
      for(unsigned int bin=1;bin<=rmsComparison->GetNbinsX();++bin){
         double toyRms = rms->GetBinContent(bin);
         double unfoldedRms = unfoldedReco->GetBinError(bin);
         double unfoldedResult = unfoldedReco->GetBinContent(bin);
         
         rmsComparison->SetBinContent(bin,fabs(toyRms-unfoldedRms)/unfoldedResult);
      }
      
      rmsComparison->Write(rmsComparison->GetName(),TObject::kOverwrite);
      
      if(compRms) compRms->push_back(rmsComparison);
      else{delete rmsComparison;rmsComparison=0;}
   }
   
   
   /// plot Mean-Truth/Truth
   if( unfolded->GetMeanResiduals() && unfolded->GetMcTruthScaled()){
      TH1* mean = unfolded->GetMeanResiduals();
      TH1* truth = unfolded->GetMcTruthScaled();
      
      /// create histogram using mean and clear it
      std::string name = GetHistogramNameToys(channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),regValue,nToys,"meanResidualOverTruth");
      TH1D* meanResidualsOverTruth= (TH1D*)mean->Clone(name.c_str());
      meanResidualsOverTruth->Reset();
      meanResidualsOverTruth->SetMinimum(-1);
      meanResidualsOverTruth->SetMaximum(1);
      meanResidualsOverTruth->SetTitle("");
      meanResidualsOverTruth->SetYTitle("(Unfolded-Truth)/Truth");
      meanResidualsOverTruth->SetDirectory(false);
      
      for(unsigned int i=1;i<=meanResidualsOverTruth->GetNbinsX();++i){
         double Mean = mean->GetBinContent(i);
         double eMean = Mean/sqrt(nToys);
         double Truth = truth->GetBinContent(i);
         double eTruth = truth->GetBinError(i);
         
         double binContent = (Mean-Truth)/Truth;
         double a = eMean*eMean;
         double b = (eTruth*eTruth*Mean*Mean)/(Truth*Truth);
         double binError = (1./Truth)*sqrt(a + b);
         
         meanResidualsOverTruth->SetBinContent(i,binContent);
         meanResidualsOverTruth->SetBinError(i,binError);
      }
      
      meanResidualsOverTruth->Write(meanResidualsOverTruth->GetName(),TObject::kOverwrite);
      
      if(meanOverTruth) meanOverTruth->push_back(meanResidualsOverTruth);
      else{delete meanResidualsOverTruth;meanResidualsOverTruth=0;}
   }
   
   
   m_rootFile->Close();
   
   delete unfolded;
   unfolded = 0;
   
   delete kinVar;
   kinVar = 0;
   
   return true;
}

bool ttbarUnfoldAnalysis::RunToyUnfoldingForRegScan(
                                          const std::string mc_filename,
                                          const std::string data_filename,
                                          const SystematicSample syst,
                                          const std::string sampleName,
                                          const std::string measuredHistoName,
                                          const ttbarUnfold::UnfoldMethod unfoldMethod,
                                          const unsigned int regValue,
                                          const unsigned int nToys)
{
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding] Comparing: \n";
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  data: " << data_filename << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  mc:   " << mc_filename << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  syst:   " << syst.str() << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  sample:   " << sampleName << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  measured Histo Name: " << measuredHistoName << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  unfold method: " << ttbarUnfold::GetMethodString(unfoldMethod) << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  nToys:   " << nToys << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  regValue:   " << regValue << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  efficiency file: " << m_efficiencyFilename << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfolding]  efficiency histo name: " << m_efficiencyHist << std::endl;
   
   
   std::vector<TH1D*> histoListMean;
   std::vector<TH1D*> histoListRms;
   std::vector<TH1D*> histoListRmsComp;
   
   // loop over possible regularization values
   unsigned int minRegValue = 2,maxRegValue = 5;
   for(unsigned int i=minRegValue;i<=maxRegValue;++i){
      
      if( !RunToyUnfolding(mc_filename,data_filename,syst,sampleName,measuredHistoName,unfoldMethod,i,0,nToys,&histoListMean,&histoListRms,&histoListRmsComp) ){
         std::cerr << " [ttbarUnfoldAnalysis::RunToyUnfoldingForRegScan] ERROR running over toys for regularization parameter " << i << "\n";
         return false;
      }
      
   }
   
   // determine if this is e+jets or mu+jets
   TopDecayChannel channel;
   channel.SetChannelUsingFilename(mc_filename);
   
   // need to open the root file down here since the RunOverEnsembleDataSets
   // function also uses the root file
   if(m_outputFilename.size() > 0)
      m_rootFile->SetOutputFilename(m_outputFilename);
   m_rootFile->Open();
   m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfoldMethod,-1,nToys);
   
   // loop over vector and create the new plots and write to Root file.
   std::string canvasRmsName = GetCanvasNameToys(channel.GetChannelString(),sampleName,unfoldMethod,-1,nToys,"rmsResidualOverUnfoldedCanvas");
   TCanvas canrms(canvasRmsName.c_str(),"can1",0,0,800,600);
   std::string canvasMeanName = GetCanvasNameToys(channel.GetChannelString(),sampleName,unfoldMethod,-1,nToys,"meanResidualOverUnfoldedCanvas");
   TCanvas canmean(canvasMeanName.c_str(),"can2",0,0,800,600);
   
   TLegend legendA(0.3,0.9,0.5,0.7);
   legendA.SetFillColor(kWhite);
   TLegend legendB(0.5,0.9,0.7,0.7);
   legendB.SetFillColor(kWhite);
   
   for(unsigned int i=0;i<histoListMean.size();++i){
      histoListMean[i]->SetMarkerColor(i+1);
      histoListMean[i]->SetLineColor(i+1);
      histoListMean[i]->SetMarkerStyle(20+i);
      
      histoListRms[i]->SetMarkerColor(i+1);
      histoListRms[i]->SetLineColor(i+1);
      histoListRms[i]->SetMarkerStyle(20+i);
      
      if(i == 0){
         canrms.cd();
         histoListRms[i]->SetMinimum(0.);
         histoListRms[i]->SetMaximum(0.1);
         histoListRms[i]->Draw("P");
         
         canmean.cd();
         histoListMean[i]->SetMinimum(-0.2);
         histoListMean[i]->SetMaximum(0.2);
         histoListMean[i]->SetTitle("");
         histoListMean[i]->Draw("P");
      }
      else{
         canrms.cd();
         histoListRms[i]->Draw("P same");
         canmean.cd();
         histoListMean[i]->Draw("P same");
      }
      
      std::stringstream legendtitle;
      legendtitle << minRegValue+i;
      if(i < histoListMean.size()/2.)
         legendA.AddEntry(histoListRms[i],legendtitle.str().c_str());
      else
         legendB.AddEntry(histoListRms[i],legendtitle.str().c_str());
   }
   
   canrms.cd();
   legendA.Draw("same");
   legendB.Draw("same");
   canmean.cd();
   legendA.Draw("same");
   legendB.Draw("same");
   
   m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfoldMethod,-1,nToys);
   canrms.Write(canrms.GetName(),TObject::kOverwrite);
   canrms.SaveAs(std::string(canvasRmsName + ".ps").c_str());
   canmean.Write(canmean.GetName(),TObject::kOverwrite);
   canmean.SaveAs(std::string(canvasMeanName + ".ps").c_str());
   
   
   m_rootFile->Close();
   
   for(unsigned int i=0;i<histoListMean.size();++i){
      delete histoListMean[i];
      histoListMean[i] = 0;
      delete histoListRms[i];
      histoListRms[i] = 0;
      delete histoListRmsComp[i];
      histoListRmsComp[i] = 0;
   }
   
   std::cout << "[ttbarUnfoldAnalysis::RunToyUnfoldingForRegScan] Done.\n";
   
   return true;
}


bool ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods(
            const std::string mc,
            const std::string data,
            const std::string sampleName,
            const std::string measuredHistoName,
            const unsigned int nToys,
            const unsigned int SvdRegValue,
            const unsigned int BayesIterations)
{
   
   std::cout << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] Comparing: \n";
   std::cout << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods]  data:     " << data << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods]  mc:       " << mc << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods]  sample:   " << sampleName << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods]  measuredHisto:   " << measuredHistoName << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods]  nToys:    " << nToys << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods]  svd reg value:   " << SvdRegValue << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods]  bayes iteration value:   " << BayesIterations << std::endl;
   
   
   // determine if this is e+jets or mu+jets
   TopDecayChannel channel;
   channel.SetChannelUsingFilename(mc);
   
   // retrieve efficiency histogram which is used to convert dN/dpT to dsigma/dpT
   TH1* efficiencyHisto = globals::GetEfficiencyHistogram(m_efficiencyFilename,m_efficiencyHist);
   
   if(!efficiencyHisto){
      std::cerr << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] ERROR: Could not retrieve efficiency Histogram\n";
      return false;
   }
   
   // get right labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string kinVarLabel  = kinVar->GetKinVarLabel();
   std::string UkinVarLabel  = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   std::string NkinVarLabel = kinVar->GetNKinVarLabel();
   std::stringstream ntitle;
       
   std::string tagged,topSample;
   if(sampleName.find("Tag0") != std::string::npos)
      tagged = "#geq 0 b-tag";
   else if(sampleName.find("Tag1") != std::string::npos)
      tagged = "#geq 1 b-tag";
   else if(sampleName.find("Tag2") != std::string::npos)
      tagged = "#geq 2 b-tag";
   if(sampleName.find("Top1") != std::string::npos)
      topSample = "leptonic top";
   else if(sampleName.find("Top2") != std::string::npos)
      topSample = "hadronic top";
   else if(sampleName.find("TopLepHad") != std::string::npos)
      topSample = "lep+had top";
   
   
   std::string channelText;
   if(channel.GetChannelString().find("el") != std::string::npos)
      channelText = "e+jets";
   else if(channel.GetChannelString().find("mu") != std::string::npos)
      channelText = "mu+jets";
   
   std::string descriptiveText = channelText + ", ";
   descriptiveText += tagged + ", ";
   descriptiveText += topSample;
   
   std::string mcOrDataText;
   if(mc.find("half0") != std::string::npos && data.find("half1") != std::string::npos)
      mcOrDataText = "mc_h01";
   else if(mc.find("half1") != std::string::npos && data.find("half0") != std::string::npos)
      mcOrDataText = "mc_h10";
   else
      mcOrDataText = "data";
   
   
   // ps file output
   std::stringstream outputstream;
   outputstream << mcOrDataText << "_" << sampleName << "_" << channel.GetChannelString() << "_toy" << nToys;
   std::string output_ps = outputstream.str() + ".ps";
   
   // x1,y1 = top,left corner, x2,y2 = bottom, right corner
   // x=0,y=0 >> bottom, left corner
   double dev_x1_data=0.3,dev_y1_data=0.9,dev_x2_data=0.65,dev_y2_data=0.73;
   double dev_x1_mc=0.15,dev_y1_mc=0.95,dev_x2_mc=0.6,dev_y2_mc=0.65;
   // want to plot deviation results for each method
   Th1PlusLegend* deviationPlots;
   if(mcOrDataText.find("data") != std::string::npos)
      deviationPlots = new Th1PlusLegend(dev_x1_mc,dev_y1_mc,dev_x2_mc,dev_y2_mc,descriptiveText);
   else
      deviationPlots = new Th1PlusLegend(dev_x1_data,dev_y1_data,dev_x2_data,dev_y2_data,descriptiveText);
   
   // want to plot deviation diffxs for each method
   Th1PlusLegend* deviationDiffxsPlots;
   if(mcOrDataText.find("data") != std::string::npos)
      deviationDiffxsPlots = new Th1PlusLegend(dev_x1_mc,dev_y1_mc,dev_x2_mc,dev_y2_mc,descriptiveText);
   else
      deviationDiffxsPlots = new Th1PlusLegend(dev_x1_data,dev_y1_data,dev_x2_data,dev_y2_data,descriptiveText);
   
   // x1,y1 = top,left corner, x2,y2 = bottom, right corner
   // x=0,y=0 >> bottom, left corner
   double spectra_x1=0.5,spectra_y1=0.9,spectra_x2=0.9,spectra_y2=0.55;
   // want to plot unfolded results for each method
   Th1PlusLegend* unfoldedPlots = new Th1PlusLegend(spectra_x1,spectra_y1,spectra_x2,spectra_y2,descriptiveText);
   Th1PlusLegend* unfoldedPlotsLog = new Th1PlusLegend(dev_x1_data,dev_y1_data,dev_x2_data,dev_y2_data,descriptiveText);
   // want to plot the unfolded diffxs for each method
   Th1PlusLegend* unfoldedDiffxsPlots = new Th1PlusLegend(spectra_x1,spectra_y1,spectra_x2,spectra_y2,descriptiveText);
   Th1PlusLegend* unfoldedDiffxsPlotsLog = new Th1PlusLegend(dev_x1_data,dev_y1_data,dev_x2_data,dev_y2_data,descriptiveText);
   // want to plot the normalized unfolded diffxs for each method
   Th1PlusLegend* unfoldedDiffxsNormPlots = new Th1PlusLegend(spectra_x1,spectra_y1,spectra_x2,spectra_y2,descriptiveText);
   Th1PlusLegend* unfoldedDiffxsNormPlotsLog = new Th1PlusLegend(dev_x1_data,dev_y1_data,dev_x2_data,dev_y2_data,descriptiveText);
   
   
   bool isData = false;
   if( measuredHistoName.size() > 0) isData = true;
   
   // hold global plots
   TH1* truth = 0;
   TH1* truth_diffxs = 0;
   TH1* truth_diffxsNorm = 0;
   TH1* reco = 0;
   TH1* measured = 0;
   std::map<ttbarUnfold::UnfoldMethod,TH1*> unfoldedResult;
   std::map<ttbarUnfold::UnfoldMethod,TH1*> unfoldedXS;
   std::map<ttbarUnfold::UnfoldMethod,TH1*> deviations;
   std::map<ttbarUnfold::UnfoldMethod,TH1*> diffxs_devs;
   
   for(ttbarUnfold::UnfoldMethod method = (ttbarUnfold::UnfoldMethod)0;
            (int)method<ttbarUnfold::NUM_OF_METHODS;
            method = (ttbarUnfold::UnfoldMethod)((int)method + 1))
   {
      
      std::cout << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] Perform unfolding with " << ttbarUnfold::GetMethodString(method) << " method. \n";
      
      ttbarUnfold* unfolded = new ttbarUnfold(data,mc,sampleName,(TH1*)efficiencyHisto->Clone("efficiencyClone"));
      // set the number of toys used to derive the deviation
      unfolded->SetToysForUnfolding(nToys);
      // set the number of toys used to derive the error matrix
      unfolded->SetToysForErrors(nToys);
      
      // set method specific values
      if(method == ttbarUnfold::SVD)
         unfolded->SetRegularization(SvdRegValue);
      else if(method == ttbarUnfold::Bayes)
         unfolded->SetBayesianIterations(BayesIterations);
      
      // set the method to use
      unfolded->SetUnfoldingMethod(method);
      
      // set the histogram name base
      std::string histoBase;
      if(method == ttbarUnfold::SVD)
         histoBase = GetHistogramNameToys(channel.GetChannelString(),sampleName,method,SvdRegValue,nToys,"");
      else if(method == ttbarUnfold::Bayes){
         std::stringstream temp;
         temp << "itr" << BayesIterations;
         histoBase = GetHistogramNameToys(channel.GetChannelString(),sampleName,method,-1,nToys,temp.str());
      }
      else
         histoBase = GetHistogramNameToys(channel.GetChannelString(),sampleName,method,-1,nToys,"");
      histoBase += "_";
      
      unfolded->SetHistoBaseName(histoBase.c_str());
      
      if(isData)
         unfolded->SetMeasuredDataMinusBkgdHistogramName(measuredHistoName);
      
      //// UNFOLD
      if(!unfolded->doUnfolding()){
         std::cerr << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] ERROR running SVD doUnfolding.\n";
         return false;
      }
      
      //// RUN TOYS
      if(!unfolded->runToys()){
         std::cerr << "[ttbarUnfoldAnalysis::RunToyUnfolding] ERROR running runToys.\n";
         return false;
      }
      
      if(m_outputFilename.size() > 0)
         m_rootFile->SetOutputFilename(m_outputFilename);
      m_rootFile->Open();
      m_rootFile->toMethods(method);
      
      if(unfolded->GetMcTruth()){
         unfolded->GetMcTruth()->Write(unfolded->GetMcTruth()->GetName(),TObject::kOverwrite);
         if(isData && !truth)
            truth = (TH1*)unfolded->GetMcTruth()->Clone("truthClone");
      }
      if(unfolded->GetMcTruthNorm())
         unfolded->GetMcTruthNorm()->Write(unfolded->GetMcTruthNorm()->GetName(),TObject::kOverwrite);
      if(unfolded->GetMcTruthScaled()){
         unfolded->GetMcTruthScaled()->Write(unfolded->GetMcTruthScaled()->GetName(),TObject::kOverwrite);
         if(!isData && !truth)
            truth = (TH1*)unfolded->GetMcTruthScaled()->Clone("truthScaledClone");
      }
      if(unfolded->GetMcTruthDiffxs()){
         unfolded->GetMcTruthDiffxs()->Write(unfolded->GetMcTruthDiffxs()->GetName(),TObject::kOverwrite);
         if(!truth_diffxs)
            truth_diffxs = (TH1*)unfolded->GetMcTruthDiffxs()->Clone("truthDiffxsClone");
      }
      if(unfolded->GetMcTruthDiffxsNorm()){
         unfolded->GetMcTruthDiffxsNorm()->Write(unfolded->GetMcTruthDiffxsNorm()->GetName(),TObject::kOverwrite);
         if(!truth_diffxsNorm)
            truth_diffxsNorm = (TH1*)unfolded->GetMcTruthDiffxsNorm()->Clone("truthDiffxsClone");
      }
      
      if(unfolded->GetMcReco()){
         unfolded->GetMcReco()->Write(unfolded->GetMcReco()->GetName(),TObject::kOverwrite);
         if(isData && !reco)
            reco = (TH1*)unfolded->GetMcReco()->Clone("recoClone");
      }
      if(unfolded->GetMcRecoNorm())
         unfolded->GetMcRecoNorm()->Write(unfolded->GetMcRecoNorm()->GetName(),TObject::kOverwrite);
      if(unfolded->GetMcRecoScaled()){
         unfolded->GetMcRecoScaled()->Write(unfolded->GetMcRecoScaled()->GetName(),TObject::kOverwrite);
         if(!isData && !reco)
            reco = (TH1*)unfolded->GetMcRecoScaled()->Clone("recoScaledClone");
      }
      if(unfolded->GetMcRecoDiffxs())
         unfolded->GetMcRecoDiffxs()->Write(unfolded->GetMcRecoDiffxs()->GetName(),TObject::kOverwrite);
      if(unfolded->GetMcRecoDiffxsNorm())
         unfolded->GetMcRecoDiffxsNorm()->Write(unfolded->GetMcRecoDiffxsNorm()->GetName(),TObject::kOverwrite);
      
      
      if(unfolded->GetDataMeasuredMinusBkgd()){
         unfolded->GetDataMeasuredMinusBkgd()->Write(unfolded->GetDataMeasuredMinusBkgd()->GetName(),TObject::kOverwrite);
         if(!measured)
            measured = (TH1*)unfolded->GetDataMeasuredMinusBkgd()->Clone("measuredClone");
      }
      if(unfolded->GetDataMeasuredMinusBkgdNorm())
         unfolded->GetDataMeasuredMinusBkgdNorm()->Write(unfolded->GetDataMeasuredMinusBkgdNorm()->GetName(),TObject::kOverwrite);
      
      if(unfolded->GetMcMigration())
         unfolded->GetMcMigration()->Write(unfolded->GetMcMigration()->GetName(),TObject::kOverwrite);
      
      if(unfolded->GetDataUnfolded()){
         unfolded->GetDataUnfolded()->Write(unfolded->GetDataUnfolded()->GetName(),TObject::kOverwrite);
         
         // draw unfolded result on one plot
         std::string tempName = unfolded->GetDataUnfolded()->GetName();
         tempName += "Clone";
         TH1* temp = (TH1*)unfolded->GetDataUnfolded()->Clone(tempName.c_str());
         temp->SetMarkerColor(1+(int)method);
         temp->SetLineColor(1+(int)method);
         temp->SetMarkerStyle(20+(int)method);
         temp->SetTitle("");
         unfoldedPlots->AddHisto(temp,ttbarUnfold::GetMethodString(method).c_str());
         unfoldedPlotsLog->AddHisto(temp,ttbarUnfold::GetMethodString(method).c_str());
         
         unfoldedResult[method] = temp;
         
      }
      
      if(unfolded->GetDataUnfoldedDiffxs()){
         unfolded->GetDataUnfoldedDiffxs()->Write(unfolded->GetDataUnfoldedDiffxs()->GetName(),TObject::kOverwrite);
         
         std::string tempName = unfolded->GetDataUnfoldedDiffxs()->GetName();
         tempName += "Clone";
         TH1* temp = (TH1*)unfolded->GetDataUnfoldedDiffxs()->Clone(tempName.c_str());
         temp->SetMarkerColor(1+(int)method);
         temp->SetLineColor(1+(int)method);
         temp->SetMarkerStyle(20+(int)method);
         temp->SetTitle("");
         unfoldedDiffxsPlots->AddHisto(temp,ttbarUnfold::GetMethodString(method).c_str());
         unfoldedDiffxsPlotsLog->AddHisto(temp,ttbarUnfold::GetMethodString(method).c_str());
         
         unfoldedXS[method] = temp;
         
      }
      if(unfolded->GetDeviation()){
         unfolded->GetDeviation()->Write(unfolded->GetDeviation()->GetName(),TObject::kOverwrite);
         
         std::string tempName = unfolded->GetDeviation()->GetName();
         tempName += "Clone";
         TH1* temp = (TH1*)unfolded->GetDeviation()->Clone(tempName.c_str());
         temp->SetMarkerColor(1+(int)method);
         temp->SetLineColor(1+(int)method);
         temp->SetMarkerStyle(20+(int)method);
         
         deviationPlots->AddHisto(temp,ttbarUnfold::GetMethodString(method).c_str());
         
         deviations[method] = temp;
      }
      if(unfolded->GetDeviationDiffxs()){
         unfolded->GetDeviationDiffxs()->Write(unfolded->GetDeviationDiffxs()->GetName(),TObject::kOverwrite);
         
         std::string tempName = unfolded->GetDeviationDiffxs()->GetName();
         tempName += "Clone";
         TH1* temp = (TH1*)unfolded->GetDeviationDiffxs()->Clone(tempName.c_str());
         temp->SetMarkerColor(1+(int)method);
         temp->SetLineColor(1+(int)method);
         temp->SetMarkerStyle(20+(int)method);
         
         deviationDiffxsPlots->AddHisto(temp,ttbarUnfold::GetMethodString(method).c_str());
         
         diffxs_devs[method] = temp;
      }
      
      if(unfolded->GetDataUnfoldedNorm())
         unfolded->GetDataUnfoldedNorm()->Write(unfolded->GetDataUnfoldedNorm()->GetName(),TObject::kOverwrite);
      
      if(unfolded->GetDataUnfoldedDiffxsNorm()){
         unfolded->GetDataUnfoldedDiffxsNorm()->Write(unfolded->GetDataUnfoldedDiffxsNorm()->GetName(),TObject::kOverwrite);
         
         std::string tempName = unfolded->GetDataUnfoldedDiffxsNorm()->GetName();
         tempName += "Clone";
         TH1* temp = (TH1*)unfolded->GetDataUnfoldedDiffxsNorm()->Clone(tempName.c_str());
         temp->SetMarkerColor(1+(int)method);
         temp->SetLineColor(1+(int)method);
         temp->SetMarkerStyle(20+(int)method);
         
         unfoldedDiffxsNormPlots->AddHisto(temp,ttbarUnfold::GetMethodString(method).c_str());
         
         unfoldedDiffxsNormPlotsLog->AddHisto(temp,ttbarUnfold::GetMethodString(method).c_str());
      }
      
      
      if(unfolded->GetRmsResiduals())
         unfolded->GetRmsResiduals()->Write(unfolded->GetRmsResiduals()->GetName(),TObject::kOverwrite);
      
      if(unfolded->GetMeanResiduals())
         unfolded->GetMeanResiduals()->Write(unfolded->GetMeanResiduals()->GetName(),TObject::kOverwrite);
      
      if(unfolded->GetUnfoldingError())
         unfolded->GetUnfoldingError()->Write(unfolded->GetUnfoldingError()->GetName(),TObject::kOverwrite);
      
      if(unfolded->GetMeanDeviation())
         unfolded->GetMeanDeviation()->Write(unfolded->GetMeanDeviation()->GetName(),TObject::kOverwrite);
      
      if(unfolded->GetRmsDeviation())
         unfolded->GetRmsDeviation()->Write(unfolded->GetRmsDeviation()->GetName(),TObject::kOverwrite);
      
      if(unfolded->GetMeanDeviationScaled())
         unfolded->GetMeanDeviationScaled()->Write(unfolded->GetMeanDeviationScaled()->GetName(),TObject::kOverwrite);
      
      if(unfolded->GetRmsDeviationScaled())
         unfolded->GetRmsDeviationScaled()->Write(unfolded->GetRmsDeviationScaled()->GetName(),TObject::kOverwrite);
      
      if(unfolded->GetRegularizationScan())
         unfolded->GetRegularizationScan()->Write(unfolded->GetRegularizationScan()->GetName(),TObject::kOverwrite);
      
      /// scale RMS by truth
      if( unfolded->GetRmsResiduals() && unfolded->GetMcTruthScaled()){
         TH1* rms = unfolded->GetRmsResiduals();
         TH1* truth = unfolded->GetMcTruthScaled();
         
         /// create histogram using rms and clear it
         std::string name = histoBase + "rmsResidualOverTruth";
         TH1D* rmsResidualsOverTruth = (TH1D*)rms->Clone(name.c_str());
         rmsResidualsOverTruth->Reset();
         rmsResidualsOverTruth->SetTitle("");
         rmsResidualsOverTruth->SetYTitle("(RMS of Toys)/Truth");
         rmsResidualsOverTruth->SetDirectory(false);
         
         for(unsigned int bin=1;bin<=rms->GetNbinsX();++bin){
            double RMS = rms->GetBinContent(bin);
            double eRMS = rms->GetBinError(bin);
            double Truth = truth->GetBinContent(bin);
            double eTruth = truth->GetBinError(bin);
            double binContent = RMS/Truth;
            double a = eRMS*eRMS;
            double b = (eTruth*eTruth*RMS*RMS)/(Truth*Truth);
            double binError = (1./Truth)*sqrt(a + b);
            
            rmsResidualsOverTruth->SetBinContent(bin,binContent);
            rmsResidualsOverTruth->SetBinError(bin,binError);
         }
         
         rmsResidualsOverTruth->Write(rmsResidualsOverTruth->GetName(),TObject::kOverwrite);
         
      }
       
      /// compare RMS to Errors on Unfolded result
      if( unfolded->GetRmsResiduals() && unfolded->GetDataUnfolded() ){
         TH1* rms = unfolded->GetRmsResiduals();
         TH1* unfoldedReco = unfolded->GetDataUnfolded();
         
         /// create histogram using rms and clear it
         std::string name = histoBase + "rmsComparison";
         TH1D* rmsComparison = (TH1D*)rms->Clone(name.c_str());
         rmsComparison->Reset();
         rmsComparison->SetTitle("");
         rmsComparison->SetYTitle("(Mean Toy RMS - Unfolded RMS)/Unfolded Result");
         rmsComparison->SetDirectory(false);
         
         for(unsigned int bin=1;bin<=rmsComparison->GetNbinsX();++bin){
            double toyRms = rms->GetBinContent(bin);
            double unfoldedRms = unfoldedReco->GetBinError(bin);
            double unfoldedResult = unfoldedReco->GetBinContent(bin);
            
            rmsComparison->SetBinContent(bin,fabs(toyRms-unfoldedRms)/unfoldedResult);
         }
         
         rmsComparison->Write(rmsComparison->GetName(),TObject::kOverwrite);
         
      }
      
      
      /// plot Mean-Truth/Truth
      if( unfolded->GetMeanResiduals() && unfolded->GetMcTruthScaled()){
         TH1* mean = unfolded->GetMeanResiduals();
         TH1* truth = unfolded->GetMcTruthScaled();
         
         /// create histogram using mean and clear it
         std::string name = histoBase + "meanResidualOverTruth";
         TH1D* meanResidualsOverTruth= (TH1D*)mean->Clone(name.c_str());
         meanResidualsOverTruth->Reset();
         meanResidualsOverTruth->SetMinimum(-1);
         meanResidualsOverTruth->SetMaximum(1);
         meanResidualsOverTruth->SetTitle("");
         meanResidualsOverTruth->SetYTitle("(Unfolded-Truth)/Truth");
         meanResidualsOverTruth->SetDirectory(false);
         
         for(unsigned int i=1;i<=meanResidualsOverTruth->GetNbinsX();++i){
            double Mean = mean->GetBinContent(i);
            double eMean = Mean/sqrt(nToys);
            double Truth = truth->GetBinContent(i);
            double eTruth = truth->GetBinError(i);
            
            double binContent = (Mean-Truth)/Truth;
            double a = eMean*eMean;
            double b = (eTruth*eTruth*Mean*Mean)/(Truth*Truth);
            double binError = (1./Truth)*sqrt(a + b);
            
            meanResidualsOverTruth->SetBinContent(i,binContent);
            meanResidualsOverTruth->SetBinError(i,binError);
         }
         
         meanResidualsOverTruth->Write(meanResidualsOverTruth->GetName(),TObject::kOverwrite);
         
      }
      
      
      std::cout << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] completed unfolding for method " << ttbarUnfold::GetMethodString(method) << std::endl;
      
      delete unfolded;
      unfolded = 0;
      
   }
   
   m_rootFile->toMethods();
   
   std::cerr << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] plot truth/reco/measured.\n";
   
   // Plot truth/reco/measured all on the same plot.
   {
      TCanvas canSpectra(GetCanvasNameToys(channel.GetChannelString(),sampleName,ttbarUnfold::NUM_OF_METHODS,-1,nToys,"spectra").c_str(),"canSpectra",0,0,800,600);
      Th1PlusLegend spectraPlots(spectra_x1,spectra_y1,spectra_x2,spectra_y2,descriptiveText);
      
      Th1PlusLegend spectraPlotsLog(dev_x1_data,dev_y1_data,dev_x2_data,dev_y2_data,descriptiveText);
      
      canSpectra.cd();
      truth->SetMarkerColor(2+(int)ttbarUnfold::NUM_OF_METHODS);
      truth->SetLineColor(2+(int)ttbarUnfold::NUM_OF_METHODS);
      truth->SetMarkerStyle(20+(int)ttbarUnfold::NUM_OF_METHODS);
      truth_diffxs->SetMarkerColor(2+(int)ttbarUnfold::NUM_OF_METHODS);
      truth_diffxs->SetLineColor(2+(int)ttbarUnfold::NUM_OF_METHODS);
      truth_diffxs->SetMarkerStyle(20+(int)ttbarUnfold::NUM_OF_METHODS);
      truth_diffxsNorm->SetMarkerColor(2+(int)ttbarUnfold::NUM_OF_METHODS);
      truth_diffxsNorm->SetLineColor(2+(int)ttbarUnfold::NUM_OF_METHODS);
      truth_diffxsNorm->SetMarkerStyle(20+(int)ttbarUnfold::NUM_OF_METHODS);
      
      reco->SetMarkerColor(3+(int)ttbarUnfold::NUM_OF_METHODS);
      reco->SetLineColor(3+(int)ttbarUnfold::NUM_OF_METHODS);
      reco->SetMarkerStyle(21+(int)ttbarUnfold::NUM_OF_METHODS);
      
      measured->SetMarkerColor(4+(int)ttbarUnfold::NUM_OF_METHODS);
      measured->SetLineColor(4+(int)ttbarUnfold::NUM_OF_METHODS);
      measured->SetMarkerStyle(22+(int)ttbarUnfold::NUM_OF_METHODS);
      
      if(isData){
         spectraPlots.AddHisto(truth,"Truth MC@NLO (nominal)");
         spectraPlots.AddHisto(reco,"Reco MC@NLO (nominal)");
         spectraPlots.AddHisto(measured,"Measured (Data-BG)");
         
         spectraPlotsLog.AddHisto(truth,"Truth MC@NLO (nominal)");
         spectraPlotsLog.AddHisto(reco,"Reco MC@NLO (nominal)");
         spectraPlotsLog.AddHisto(measured,"Measured (Data-BG)");
      }
      else{
         spectraPlots.AddHisto(truth,"Scaled Truth MC@NLO (half0)");
         spectraPlots.AddHisto(reco,"Scaled Reco MC@NLO (half0)");
         spectraPlots.AddHisto(measured,"Reco MC@NLO (half1)");
         
         spectraPlotsLog.AddHisto(truth,"Scaled Truth MC@NLO (half0)");
         spectraPlotsLog.AddHisto(reco,"Scaled Reco MC@NLO (half0)");
         spectraPlotsLog.AddHisto(measured,"Reco MC@NLO (half1)");
      }
      spectraPlots.Draw();
      canSpectra.Write(canSpectra.GetName(),TObject::kOverwrite);
      canSpectra.SaveAs(std::string(output_ps + "(").c_str());
      
      spectraPlotsLog.SetMinimum(10);
      spectraPlotsLog.SetMaximum(10000);
      spectraPlotsLog.Draw();
      
      canSpectra.cd()->SetLogy(true);
      canSpectra.SaveAs(output_ps.c_str());
   }
   
   std::cerr << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] plot unfolded.\n";
   
   // plot unfolded histograms
   {
      TCanvas canUnfold(GetCanvasNameToys(channel.GetChannelString(),sampleName,ttbarUnfold::NUM_OF_METHODS,-1,nToys,"data_unfolded").c_str(),"data_unfolded",0,0,800,600);
      canUnfold.cd();
      if(isData){
         unfoldedPlots->AddHisto(truth,"Truth MC@NLO");
         unfoldedPlotsLog->AddHisto(truth,"Truth MC@NLO");
      }
      else{
         unfoldedPlots->AddHisto(truth,"Scaled Truth MC@NLO (half0)");
         unfoldedPlotsLog->AddHisto(truth,"Scaled Truth MC@NLO (half0)");
      }
      unfoldedPlots->Draw();
      canUnfold.Write(canUnfold.GetName(),TObject::kOverwrite);
      canUnfold.SaveAs(output_ps.c_str());
      
      unfoldedPlotsLog->SetMinimum(10);
      unfoldedPlotsLog->SetMaximum(10000);
      unfoldedPlotsLog->Draw();
      
      canUnfold.SetLogy(true);
      canUnfold.SaveAs(output_ps.c_str());
   }
   
   std::cerr << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] plot diffxs.\n";
   
   // plot unfolded diffxs histograms
   {
      TCanvas canUnfoldDiffxs(GetCanvasNameToys(channel.GetChannelString(),sampleName,ttbarUnfold::NUM_OF_METHODS,-1,nToys,"data_unfolded_diffxs").c_str(),"data_unfolded_diffxs",0,0,800,600);
      canUnfoldDiffxs.cd();
      
      unfoldedDiffxsPlots->AddHisto(truth_diffxs,"Truth MC@NLO");
      unfoldedDiffxsPlotsLog->AddHisto(truth_diffxs,"Truth MC@NLO");
      
      ntitle << ";"<<UkinVarLabel<<";"<<DkinVarLabel;
      unfoldedDiffxsPlots->GetStack()->SetTitle(ntitle.str().c_str());
      unfoldedDiffxsPlots->Draw();
      canUnfoldDiffxs.Write(canUnfoldDiffxs.GetName(),TObject::kOverwrite);
      canUnfoldDiffxs.SaveAs(output_ps.c_str());
      std::string outputFile = outputstream.str() + "_unfoldedDiffxs.eps";
      canUnfoldDiffxs.SaveAs(outputFile.c_str());
      
      unfoldedDiffxsPlotsLog->SetMinimum(0.00001);
      unfoldedDiffxsPlotsLog->SetMaximum(1);
      unfoldedDiffxsPlotsLog->GetStack()->SetTitle(ntitle.str().c_str());
      unfoldedDiffxsPlotsLog->Draw();
      
      canUnfoldDiffxs.SetLogy(true);
      canUnfoldDiffxs.SaveAs(output_ps.c_str());
      
      std::string outputFile2 = outputstream.str() + "_unfoldedDiffxsLog.eps";
      canUnfoldDiffxs.SaveAs(outputFile2.c_str());
      
   }
   
   std::cerr << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] plot diffxsNorm.\n";
   
   // plot normalized unfolded diffxs histograms
   {
      TCanvas canUnfoldDiffxs(GetCanvasNameToys(channel.GetChannelString(),sampleName,ttbarUnfold::NUM_OF_METHODS,-1,nToys,"data_unfolded_diffxsNorm").c_str(),"data_unfolded_diffxsNorm",0,0,800,600);
      canUnfoldDiffxs.cd();
      
      unfoldedDiffxsNormPlots->AddHisto(truth_diffxsNorm,"Truth MC@NLO");
      unfoldedDiffxsNormPlotsLog->AddHisto(truth_diffxsNorm,"Truth MC@NLO");
      
      ntitle.str("");
      ntitle << ";"<<UkinVarLabel<<";"<<NkinVarLabel;
      unfoldedDiffxsNormPlots->GetStack()->SetTitle(ntitle.str().c_str());
      unfoldedDiffxsNormPlots->Draw();
      canUnfoldDiffxs.Write(canUnfoldDiffxs.GetName(),TObject::kOverwrite);
      canUnfoldDiffxs.SaveAs(output_ps.c_str());
      std::string outputFile = outputstream.str() + "_unfoldedDiffxsNorm.eps";
      canUnfoldDiffxs.SaveAs(outputFile.c_str());
      
      
      unfoldedDiffxsNormPlots->SetMinimum(0.00001);
      unfoldedDiffxsNormPlots->SetMaximum(1);
      unfoldedDiffxsNormPlots->GetStack()->SetTitle(ntitle.str().c_str());
      unfoldedDiffxsNormPlots->Draw();
      
      canUnfoldDiffxs.SetLogy(true);
      canUnfoldDiffxs.SaveAs(output_ps.c_str());
      
      std::string outputFile2 = outputstream.str() + "_unfoldedDiffxsNormLog.eps";
      canUnfoldDiffxs.SaveAs(outputFile2.c_str());
      
   }
   
   std::cerr << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] plot deviation.\n";
   
   // plot deviation histograms
   {
      TCanvas canDev(GetCanvasNameToys(channel.GetChannelString(),sampleName,ttbarUnfold::NUM_OF_METHODS,-1,nToys,"deviation").c_str(),"canDev",0,0,800,600);
      canDev.cd();
      // ATLAS Style sets the error caps to 0 width
      gStyle->SetEndErrorSize(5);
      deviationPlots->Draw();
      canDev.Write(canDev.GetName(),TObject::kOverwrite);
      canDev.SaveAs(output_ps.c_str());
      gStyle->SetEndErrorSize(0);
      
   }
   
   std::cerr << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] plot deviation diffxs.\n";
   
   // plot deviation diffxs histograms
   {
      TCanvas can_dev_diffxs(GetCanvasNameToys(channel.GetChannelString(),sampleName,ttbarUnfold::NUM_OF_METHODS,-1,nToys,"diffxs_dev").c_str(),"can_dev_diffxs",0,0,800,600);
      can_dev_diffxs.cd();
      can_dev_diffxs.SetMargin(0.2,0.8,0.2,0.8);
      // ATLAS Style sets the error caps to 0 width
      gStyle->SetEndErrorSize(5);
      gStyle->SetTitleOffset(1.6,"y");
      deviationDiffxsPlots->Draw();
      can_dev_diffxs.Write(can_dev_diffxs.GetName(),TObject::kOverwrite);
      can_dev_diffxs.SaveAs(output_ps.c_str());
      std::string outputFile = outputstream.str() + "_devDiffxs.eps";
      can_dev_diffxs.SaveAs(outputFile.c_str());
      gStyle->SetEndErrorSize(0);
      
   }
   
   std::cerr << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] plot diff between bayes and svd.\n";
   
   /// Plot the difference between the unfolded Bayesian and SVD
   {
      TCanvas can(GetCanvasNameToys(channel.GetChannelString(),sampleName,ttbarUnfold::NUM_OF_METHODS,-1,nToys,"unfoldingSyst").c_str(),"can",0,0,800,600);
      
      std::string name = GetHistogramNameToys(channel.GetChannelString(),sampleName,ttbarUnfold::NUM_OF_METHODS,-1,nToys,"unfoldingSyst");
      TH1* syst = (TH1*)truth->Clone(name.c_str());
      syst->Reset();
      syst->SetDirectory(false);
      ntitle.str("");
      ntitle << ";"<<UkinVarLabel<<";#||{#frac{d#sigma}{d"<<kinVarLabel<<"}}_{SVD} - #||{#frac{d#sigma}{d"<<kinVarLabel<<"}}_{Bay}";
      syst->SetTitle(ntitle.str().c_str());
      
      TH1* svd = unfoldedXS[ttbarUnfold::SVD];
      TH1* bayes = unfoldedXS[ttbarUnfold::Bayes];
      
      for(unsigned int bin=1;bin<=syst->GetNbinsX();++bin){
         double content = fabs(svd->GetBinContent(bin) - bayes->GetBinContent(bin));
         double error   = sqrt(svd->GetBinError(bin)*svd->GetBinError(bin) +
                  bayes->GetBinError(bin)*bayes->GetBinError(bin));
         syst->SetBinContent(bin,content);
         syst->SetBinError(bin,error);
      }
      
      can.cd();
      syst->Draw();
      can.SaveAs(output_ps.c_str());
      syst->Write(syst->GetName(),TObject::kOverwrite);
   
      /// divide the systematic by SVD method to compare
      name = GetHistogramNameToys(channel.GetChannelString(),sampleName,ttbarUnfold::NUM_OF_METHODS,-1,nToys,"unfoldingSystOverSVD");
      TH1* systOverSVD = (TH1*)truth->Clone(name.c_str());
      systOverSVD->SetDirectory(false);
      systOverSVD->Reset();
      ntitle.str("");
      ntitle << ";"<<UkinVarLabel<<";#left(#||{#frac{d#sigma}{d"<<kinVarLabel<<"}}_{SVD} - #||{#frac{d#sigma}{d"<<kinVarLabel<<"}}_{Bay}#right) / #||{#frac{d#sigma}{d"<<kinVarLabel<<"}}_{SVD}";
      systOverSVD->SetTitle(ntitle.str().c_str());
      
      for(unsigned int bin=1;bin<=systOverSVD->GetNbinsX();++bin){
         double s = syst->GetBinContent(bin);
         double ds = syst->GetBinError(bin);
         double t = svd->GetBinContent(bin);
         double dt = svd->GetBinError(bin);
         double content = t != 0 ? (s/t) : 0;
         double error   = t != 0 ? sqrt( (ds/t)*(ds/t) + (s*dt/(t*t))*(s*dt/(t*t))) : 0;
         systOverSVD->SetBinContent(bin,content);
         systOverSVD->SetBinError(bin,error);
      }
      
      can.cd();
      can.Clear();
      systOverSVD->Draw();
      can.SaveAs(std::string(output_ps + ")").c_str());
      systOverSVD->Write(systOverSVD->GetName(),TObject::kOverwrite);
      
      delete syst;
      syst = 0;
      
      delete systOverSVD;
      systOverSVD = 0;
      
   }
   
   m_rootFile->Close();
   
   delete efficiencyHisto;
   efficiencyHisto = 0;
   
   delete kinVar;
   kinVar = 0;
   
   std::cerr << "[ttbarUnfoldAnalysis::RunUnfoldingWithDifferentMethods] Done. \n";
   
   return true;
}


bool ttbarUnfoldAnalysis::RunMigrationSmearedToys(const std::string mc,
                        const std::string data,
                        const std::string sampleName,
                        const unsigned int nToys,
                        const unsigned int svdRegValue,
                        const unsigned int bayesIterator,
                        const std::string measuredHistoName,
                        const ttbarUnfold::UnfoldMethod unfoldMethod)
{
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys] Comparing: \n";
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys]  data:                " << data << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys]  mc:                  " << mc << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys]  nToys:               " << nToys << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys]  regValue:            " << svdRegValue << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys]  iterator:            " << bayesIterator << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys]  sampleName:          " << sampleName << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys]  measuredHistoName:   " << measuredHistoName << std::endl;
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys]  unfold method:       " << ttbarUnfold::GetMethodString(unfoldMethod) << std::endl;
   
   
   // determine if this is e+jets or mu+jets
   TopDecayChannel channel;
   channel.SetChannelUsingFilename(mc);
   
   // retrieve efficiency histogram and pass it to the unfolding
   TH1* efficiencyHisto = globals::GetEfficiencyHistogram(m_efficiencyFilename,m_efficiencyHist);
   if(!efficiencyHisto){
      std::cerr << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys] ERROR: Could not retrieve efficiency Histogram\n";
      return false;
   }
   
   // get right labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string kinVarLabel  = kinVar->GetKinVarLabel();
   std::string UkinVarLabel  = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   std::string NkinVarLabel = kinVar->GetNKinVarLabel();
   std::stringstream ntitle;
   
   SystematicSample syst(SystematicSample::MIGRATIONTOYS);
   
   ttbarUnfold* unfolded = new ttbarUnfold(data,mc,sampleName,efficiencyHisto);
   unfolded->SetToysForUnfolding(nToys);
   unfolded->SetToysForErrors(nToys);
   unfolded->SetUnfoldingMethod(unfoldMethod);
   
   std::stringstream nToysString;
   nToysString << nToys;
   std::stringstream regValueString;
   regValueString << svdRegValue;
   std::string histoBase,canvasBase;
   
   
   // set method specific values
   if(unfoldMethod == ttbarUnfold::SVD){
      unfolded->SetRegularization(svdRegValue);
      histoBase = GetHistogramNameToys(channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),svdRegValue,nToys,"") + "_";
      canvasBase = GetCanvasNameToys(channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),svdRegValue,nToys,"") + "_";
   }
   else if(unfoldMethod == ttbarUnfold::Bayes){
      unfolded->SetBayesianIterations(bayesIterator);
      histoBase = GetHistogramNameToys(channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),bayesIterator,nToys,"") + "_";
      canvasBase = GetCanvasNameToys(channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),bayesIterator,nToys,"") + "_";
      
   }
   
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys] histoBase: " << histoBase << std::endl;
   
   unfolded->SetHistoBaseName(histoBase.c_str());
   unfolded->SetUnfoldingMethod(unfoldMethod);
   unfolded->SetMeasuredDataMinusBkgdHistogramName(measuredHistoName);
   
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys] doUnfolding \n";
   if(!unfolded->doUnfolding()){
      std::cerr << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys] ERROR running doUnfolding.\n";
      return false;
   }
   
   if(m_outputFilename.size() > 0)
      m_rootFile->SetOutputFilename(m_outputFilename);
   m_rootFile->Open();
   if(unfoldMethod == ttbarUnfold::SVD)
      m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),svdRegValue,nToys);
   else if(unfoldMethod == ttbarUnfold::Bayes)
      m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),bayesIterator,nToys);
   
   if(unfolded->GetMcTruth())
      unfolded->GetMcTruth()->Write(unfolded->GetMcTruth()->GetName(),TObject::kOverwrite);
   if(unfolded->GetMcTruthDiffxs())
      unfolded->GetMcTruthDiffxs()->Write(unfolded->GetMcTruthDiffxs()->GetName(),TObject::kOverwrite);
   if(unfolded->GetMcTruthDiffxsNorm())
      unfolded->GetMcTruthDiffxsNorm()->Write(unfolded->GetMcTruthDiffxsNorm()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetMcReco())
      unfolded->GetMcReco()->Write(unfolded->GetMcReco()->GetName(),TObject::kOverwrite);
   if(unfolded->GetMcRecoDiffxs())
      unfolded->GetMcRecoDiffxs()->Write(unfolded->GetMcRecoDiffxs()->GetName(),TObject::kOverwrite);
   if(unfolded->GetMcRecoDiffxsNorm())
      unfolded->GetMcRecoDiffxsNorm()->Write(unfolded->GetMcRecoDiffxsNorm()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetDataMeasuredMinusBkgd())
      unfolded->GetDataMeasuredMinusBkgd()->Write(unfolded->GetDataMeasuredMinusBkgd()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetMcMigration())
      unfolded->GetMcMigration()->Write(unfolded->GetMcMigration()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetDataUnfolded())
      unfolded->GetDataUnfolded()->Write(unfolded->GetDataUnfolded()->GetName(),TObject::kOverwrite);
   if(unfolded->GetDataUnfoldedNorm())
      unfolded->GetDataUnfoldedNorm()->Write(unfolded->GetDataUnfoldedNorm()->GetName(),TObject::kOverwrite);
   
   if(unfolded->GetEfficiencyHistogram())
      unfolded->GetEfficiencyHistogram()->Write(unfolded->GetEfficiencyHistogram()->GetName(),TObject::kOverwrite);
   
   if(!unfolded->GetMcTruth()){
      std::cerr << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys] Truth histogram not set. Truth is needed for following calculations.\n";
      return false;
   }
   
   std::vector<MeanValue> binMeanDiffxs(unfolded->GetMcTruth()->GetNbinsX(),MeanValue::MeanValue());
   std::vector<MeanValue> binMeanDiffxsNorm(unfolded->GetMcTruth()->GetNbinsX(),MeanValue::MeanValue());
   
   std::vector<MeanValue> binMeanDeviation(unfolded->GetMcTruth()->GetNbinsX(),MeanValue::MeanValue());
   std::vector<MeanValue> binMeanDeviationScaled(unfolded->GetMcTruth()->GetNbinsX(),MeanValue::MeanValue());
   
   
   // loop over toy migration matrices
   
   for(unsigned int toyNumber=0;toyNumber<nToys;++toyNumber){
      std::cout << " Migration Toy # " << toyNumber << "\n";
      
      ttbarUnfold* toy = unfolded->GetSmearedMigrationToy(channel);
      if(toy == (ttbarUnfold*)0){
         std::cerr << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys] Error getting toy model\n";
         continue;
      }
      
      
      if(toy->GetDataUnfoldedDiffxs()){
         TH1* diffxs = toy->GetDataUnfoldedDiffxs();
         TH1* diffxsNorm = toy->GetDataUnfoldedDiffxsNorm();
         
         for(unsigned int bin=1;bin<=diffxs->GetNbinsX();++bin){
            binMeanDiffxs[bin-1].AddEntry(diffxs->GetBinContent(bin));
            binMeanDiffxsNorm[bin-1].AddEntry(diffxsNorm->GetBinContent(bin));
         
            // calculate the deviation from truth
            if(unfolded->GetMcTruthDiffxs()){
               TH1* truth = unfolded->GetMcTruthDiffxs();
         
            
               double truthVal = truth->GetBinContent(bin);
               double toyVal   = diffxs->GetBinContent(bin);
               double deviation = (truthVal == 0) ? 0. : (toyVal-truthVal)/truthVal;
               binMeanDeviation[bin-1].AddEntry(deviation);
            
               double scaler = diffxs->Integral("width")/truth->Integral("width");
            
               double truthValScaled = truthVal*scaler;
               double deviationScaled = (truthValScaled == 0) ? 0. : (toyVal-truthValScaled)/truthValScaled;
               binMeanDeviationScaled[bin-1].AddEntry(deviationScaled);
            
            }
         }// end for(bin)
         
         
         // histogram deleted by ttbarUnfold destructor
         diffxs = 0;
         diffxsNorm = 0;
         
      }
      else{
         std::cerr << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys] Error retrieving toy unfolded histogram.\n";
      }
      
      delete toy;
      toy = 0;
   }// end for(toy)
   
   
   std::string name = histoBase + "meanResidual";
   ntitle << ";"<<UkinVarLabel<<";#frac{1}{N_{toys}} #sum_{i}^{N_{toys}} #left[#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{i} #left[#frac{pb}{GeV}#right]";
   TH1F* meanResidual = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   meanResidual->SetDirectory(false);
   meanResidual->Reset();
   meanResidual->SetTitle(ntitle.str().c_str());
   
   name = histoBase;
   name += "meanResidualOverTruth";
   ntitle.str("");
   ntitle << ";"<<UkinVarLabel<<";#left[#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{MC@NLO}^{-1} #times #frac{1}{N_{toys}} #sum_{i}^{N_{toys}} #left[#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{i}";
   TH1F* meanResidualOverTruth = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   meanResidualOverTruth->SetDirectory(false);
   meanResidualOverTruth->Reset();
   meanResidualOverTruth->SetTitle(ntitle.str().c_str());
   
   name = histoBase;
   name += "rmsResidual";
   ntitle.str("");
   ntitle << ";"<<UkinVarLabel<<";RMS of #frac{1}{N_{toys}} #sum_{i}^{N_{toys}} #left[#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{i} #left[#frac{pb}{GeV}#right]";
   TH1F* rmsResidual = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   rmsResidual->SetDirectory(false);
   rmsResidual->Reset();
   rmsResidual->SetTitle(ntitle.str().c_str());
   
   name = histoBase;
   name += "rmsResidualOverTruth";
   ntitle.str("");
   ntitle << ";"<<UkinVarLabel<<";#left[#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{MC@NLO}^{-1} #times RMS of #frac{1}{N_{toys}} #sum_{i}^{N_{toys}} #left[#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{i}";
   TH1F* rmsResidualOverTruth = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   rmsResidualOverTruth->SetDirectory(false);
   rmsResidualOverTruth->Reset();
   rmsResidualOverTruth->SetTitle(ntitle.str().c_str());
   
   name = histoBase;
   name += "meanResidualNorm";
   ntitle.str("");
   ntitle << ";"<<UkinVarLabel<<";#frac{1}{N_{toys}} #sum_{i}^{N_{toys}} #left[#frac{1}{#sigma^{t}}#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{i} #left[#frac{pb}{GeV}#right]";
   TH1F* meanResidualNorm = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   meanResidualNorm->SetDirectory(false);
   meanResidualNorm->Reset();
   meanResidualNorm->SetTitle(ntitle.str().c_str());
   
   name = histoBase;
   name += "meanResidualNormOverTruth";
   ntitle.str("");
   ntitle << ";"<<UkinVarLabel<<";#left[#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{MC@NLO}^{-1} #times #frac{1}{N_{toys}} #sum_{i}^{N_{toys}} #left[#frac{1}{#sigma^{t}}#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{i}";
   TH1F* meanResidualNormOverTruth = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   meanResidualNormOverTruth->SetDirectory(false);
   meanResidualNormOverTruth->Reset();
   meanResidualNormOverTruth->SetTitle(ntitle.str().c_str());
   
   name = histoBase;
   name += "rmsResidualNorm";
   ntitle.str("");
   ntitle << ";"<<UkinVarLabel<<";RMS of #frac{1}{N_{toys}} #sum_{i}^{N_{toys}} #left[#frac{1}{#sigma^{t}}#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{i} #left[#frac{pb}{GeV}#right]";
   TH1F* rmsResidualNorm = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   rmsResidualNorm->SetDirectory(false);
   rmsResidualNorm->Reset();
   rmsResidualNorm->SetTitle(ntitle.str().c_str());
   
   name = histoBase;
   name += "rmsResidualOverTruth";
   ntitle.str("");
   ntitle << ";"<<UkinVarLabel<<";#left[#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{MC@NLO}^{-1} #times RMS of #frac{1}{N_{toys}} #sum_{i}^{N_{toys}} #left[#frac{1}{#sigma^{t}}#frac{d#sigma}{d"<<kinVarLabel<<"}#right]_{i}";
   TH1F* rmsResidualNormOverTruth = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   rmsResidualNormOverTruth->SetDirectory(false);
   rmsResidualNormOverTruth->Reset();
   rmsResidualNormOverTruth->SetTitle(ntitle.str().c_str());
   
   name = histoBase;
   name += "meanDeviation";
   TH1F* meanDeviation = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   meanDeviation->SetDirectory(false);
   meanDeviation->Reset();
   meanDeviation->SetTitle("");
   meanDeviation->SetXTitle(UkinVarLabel.c_str());
   meanDeviation->SetYTitle("Mean Deviation of all Toy Model");
   
   name = histoBase;
   name += "rmsDeviation";
   TH1F* rmsDeviation = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   rmsDeviation->SetDirectory(false);
   rmsDeviation->Reset();
   rmsDeviation->SetTitle("");
   rmsDeviation->SetXTitle(UkinVarLabel.c_str());
   rmsDeviation->SetYTitle("RMS of Deviation of all Toy Model");
   
   name = histoBase;
   name += "meanDeviationScaled";
   TH1F* meanDeviationScaled = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   meanDeviationScaled->SetDirectory(false);
   meanDeviationScaled->Reset();
   meanDeviationScaled->SetTitle("");
   meanDeviationScaled->SetXTitle(UkinVarLabel.c_str());
   meanDeviationScaled->SetYTitle("Scaled(toy/truth) Mean Deviation of all Toy Model");
   name = histoBase;
   name += "rmsDeviationScaled";
   TH1F* rmsDeviationScaled = (TH1F*)unfolded->GetMcTruth()->Clone(name.c_str());
   rmsDeviationScaled->SetDirectory(false);
   rmsDeviationScaled->Reset();
   rmsDeviationScaled->SetTitle("");
   rmsDeviationScaled->SetXTitle(UkinVarLabel.c_str());
   rmsDeviationScaled->SetYTitle("RMS of Scaled(toy/truth) Deviation of all Toy Model");
   
   
   for(unsigned int bin=1;bin<=meanResidualOverTruth->GetNbinsX();++bin){
      
      double mean  = binMeanDiffxs[bin-1].GetMean();
      double sigma = binMeanDiffxs[bin-1].GetSigma();
      double truth = unfolded->GetMcTruthDiffxs()->GetBinContent(bin);
      
      double meanOverTruth = truth == 0 ? 0 : mean/truth;
      double sigmaOverTruth  = truth == 0 ? 0 : sigma/truth;
      
      meanResidual->SetBinContent(bin,mean);
      meanResidualOverTruth->SetBinContent(bin,meanOverTruth);
      
      rmsResidual->SetBinContent(bin,sigma);
      rmsResidualOverTruth->SetBinContent(bin,sigmaOverTruth);
      
      // now the normalized values
      mean  = binMeanDiffxsNorm[bin-1].GetMean();
      sigma = binMeanDiffxsNorm[bin-1].GetSigma();
      
      meanOverTruth = truth == 0 ? 0 : mean/truth;
      sigmaOverTruth  = truth == 0 ? 0 : sigma/truth;
      
      meanResidualNorm->SetBinContent(bin,mean);
      meanResidualNormOverTruth->SetBinContent(bin,meanOverTruth);
      
      rmsResidualNorm->SetBinContent(bin,sigma);
      rmsResidualNormOverTruth->SetBinContent(bin,sigmaOverTruth);
      
      // now the deviation
      mean = binMeanDeviation[bin-1].GetMean();
      sigma= binMeanDeviation[bin-1].GetSigma();
      
      meanDeviation->SetBinContent(bin,mean);
      rmsDeviation->SetBinContent(bin,sigma);
      
      // now the deviationScaled
      mean = binMeanDeviationScaled[bin-1].GetMean();
      sigma= binMeanDeviationScaled[bin-1].GetSigma();
      
      meanDeviationScaled->SetBinContent(bin,mean);
      rmsDeviationScaled->SetBinContent(bin,sigma);
      
      // Set diffxs errors to the rms residuals
      if(unfolded->GetDataUnfoldedDiffxs())
         unfolded->GetDataUnfoldedDiffxs()->SetBinError(bin,rmsResidual->GetBinContent(bin));
      if(unfolded->GetDataUnfoldedDiffxsNorm())
         unfolded->GetDataUnfoldedDiffxsNorm()->SetBinError(bin,rmsResidualNorm->GetBinContent(bin));
      
   }
   
   if(unfoldMethod == ttbarUnfold::SVD)
      m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),svdRegValue,nToys);
   else if(unfoldMethod == ttbarUnfold::Bayes)
      m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),bayesIterator,nToys);
   
   // write diffxs plots to file with the proper errors
   if(unfolded->GetDataUnfoldedDiffxs())
      unfolded->GetDataUnfoldedDiffxs()->Write(unfolded->GetDataUnfoldedDiffxs()->GetName(),TObject::kOverwrite);
   if(unfolded->GetDataUnfoldedDiffxsNorm())
      unfolded->GetDataUnfoldedDiffxsNorm()->Write(unfolded->GetDataUnfoldedDiffxsNorm()->GetName(),TObject::kOverwrite);
   
   
   
   // draw to a canvas and save to the root file
   
   meanResidual->Write(meanResidual->GetName(),TObject::kOverwrite);
   meanResidualOverTruth->Write(meanResidualOverTruth->GetName(),TObject::kOverwrite);
   rmsResidual->Write(rmsResidual->GetName(),TObject::kOverwrite);
   rmsResidualOverTruth->Write(rmsResidualOverTruth->GetName(),TObject::kOverwrite);
   
   meanResidualNorm->Write(meanResidualNorm->GetName(),TObject::kOverwrite);
   meanResidualNormOverTruth->Write(meanResidualNormOverTruth->GetName(),TObject::kOverwrite);
   rmsResidualNorm->Write(rmsResidualNorm->GetName(),TObject::kOverwrite);
   rmsResidualNormOverTruth->Write(rmsResidualNormOverTruth->GetName(),TObject::kOverwrite);
   
   meanDeviation->Write(meanDeviation->GetName(),TObject::kOverwrite);
   meanDeviationScaled->Write(meanDeviationScaled->GetName(),TObject::kOverwrite);
   rmsDeviation->Write(rmsDeviation->GetName(),TObject::kOverwrite);
   rmsDeviationScaled->Write(rmsDeviationScaled->GetName(),TObject::kOverwrite);
   
   
   name = canvasBase;
   name += "migrationToys";
   TCanvas can(name.c_str(),name.c_str(),0,0,800,600);
   can.Divide(2,2);
   
   can.cd(1);
   meanResidual->Draw();
   can.cd(2);
   rmsResidual->Draw();
   can.cd(3);
   meanResidualOverTruth->Draw();
   can.cd(4);
   rmsResidualOverTruth->Draw();
   
   can.SaveAs("migrationSmearing.ps");
   
   if(unfoldMethod == ttbarUnfold::SVD)
      m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),svdRegValue,nToys);
   else if(unfoldMethod == ttbarUnfold::Bayes)
      m_rootFile->toToys(syst,channel.GetChannelString(),sampleName,unfolded->GetUnfoldingMethod(),bayesIterator,nToys);
   can.Write(can.GetName(),TObject::kOverwrite);
   
   name = canvasBase;
   name += "migrationToysNorm";
   TCanvas canNorm(name.c_str(),name.c_str(),0,0,800,600);
   canNorm.Divide(2,2);
   
   canNorm.cd(1);
   meanResidualNorm->Draw();
   canNorm.cd(2);
   rmsResidualNorm->Draw();
   canNorm.cd(3);
   meanResidualNormOverTruth->Draw();
   canNorm.cd(4);
   rmsResidualNormOverTruth->Draw();
   
   canNorm.SaveAs("migrationSmearingNorm.ps");
   
   canNorm.Write(can.GetName(),TObject::kOverwrite);
   m_rootFile->Close();
   
   delete meanResidual;
   meanResidual = 0;
   delete meanResidualNorm;
   meanResidualNorm = 0;
   
   delete meanResidualOverTruth;
   meanResidualOverTruth = 0;
   delete meanResidualNormOverTruth;
   meanResidualNormOverTruth = 0;
   
   delete rmsResidual;
   rmsResidual = 0;
   delete rmsResidualNorm;
   rmsResidualNorm = 0;
   
   delete rmsResidualOverTruth;
   rmsResidualOverTruth = 0;
   delete rmsResidualNormOverTruth;
   rmsResidualNormOverTruth = 0;
   
   delete meanDeviation;
   meanDeviation = 0;
   delete rmsDeviation;
   rmsDeviation = 0;
   
   delete meanDeviationScaled;
   meanDeviationScaled = 0;
   delete rmsDeviationScaled;
   rmsDeviationScaled = 0;
   
   delete unfolded;
   unfolded = 0;
   
   delete kinVar;
   kinVar = 0;
   
   
   std::cout << "[ttbarUnfoldAnalysis::RunMigrationSmearedToys] Done.\n";
   
   return true;
}

void ttbarUnfoldAnalysis::psmerge(const std::string outFilename,const std::string file1, const std::string file2){
   std::string cmd = "psmerge ";
   cmd += "-o";
   cmd += outFilename;
   cmd += " ";
   cmd += file1;
   cmd += " ";
   cmd += file2;
   
   system(cmd.c_str());
}

void ttbarUnfoldAnalysis::rmfile(const std::string filename){
   std::string cmd = "rm ";
   cmd += filename;
   
   system(cmd.c_str());
}

void ttbarUnfoldAnalysis::mvfile(const std::string from,const std::string to){
   std::string cmd = "mv ";
   cmd += from;
   cmd += " ";
   cmd += to;
   
   system(cmd.c_str());
   
}


std::string ttbarUnfoldAnalysis::GetHistogramNameToys(const std::string channel,
      const std::string sampleName,
      const ttbarUnfold::UnfoldMethod unfoldMethod,
      const int regValue,
      const int nToys,
      const std::string additional)
{
   std::stringstream name;
   name << "H_" << channel << "_" << sampleName << "_" << ttbarUnfold::GetMethodString(unfoldMethod);
   // check unfoldMethod
   if(unfoldMethod == ttbarUnfold::SVD && regValue >= 0)
      name << "_reg" << regValue;
   
   // add number of toys
   if(nToys >= 0)
      name << "_toy" << nToys;
   
   if(additional.size() > 0)
      name << "_" << additional;
   
   return name.str();
}

std::string ttbarUnfoldAnalysis::GetCanvasNameToys(const std::string channel,
      const std::string sampleName,
      const ttbarUnfold::UnfoldMethod unfoldMethod,
      const int regValue,
      const int nToys,
      const std::string additional)
{
   std::stringstream name;
   name << "C_" << channel << "_" << sampleName << "_" << ttbarUnfold::GetMethodString(unfoldMethod);
   // check unfoldMethod
   if(unfoldMethod == ttbarUnfold::SVD && regValue >= 0)
      name << "_reg" << regValue;
   
   // add number of toys
   if(nToys >= 0)
      name << "_toy" << nToys;
   
   if(additional.size() > 0)
      name << "_" << additional;
   
   return name.str();
}

std::string ttbarUnfoldAnalysis::GetHistogramNameEnsembles(const std::string channel,
      const std::string sampleName,
      const ttbarUnfold::UnfoldMethod unfoldMethod,
      const int regValue,
      const int ensembleNumber,
      const std::string additional
      )
{
   std::stringstream name;
   name << "H_" << channel << "_" << sampleName << "_" << ttbarUnfold::GetMethodString(unfoldMethod);
   // check unfoldMethod
   if(unfoldMethod == ttbarUnfold::SVD && regValue >= 0)
      name << "_reg" << regValue;
   
   if(ensembleNumber >= 0)
      name << "_ens" << ensembleNumber;
   
   if(additional.size() > 0)
      name << "_" << additional;
   
   return name.str();
}


std::string ttbarUnfoldAnalysis::GetCanvasNameEnsembles(const std::string channel,
      const std::string sampleName,
      const ttbarUnfold::UnfoldMethod unfoldMethod,
      const int regValue,
      const int ensembleNumber,
      const std::string additional
      )
{
   std::stringstream name;
   name << "C_" << channel << "_" << sampleName << "_" << ttbarUnfold::GetMethodString(unfoldMethod);
   // check unfoldMethod
   if(unfoldMethod == ttbarUnfold::SVD && regValue >= 0)
      name << "_reg" << regValue;
   
   if(ensembleNumber >= 0)
      name << "_ens" << ensembleNumber;
   if(additional.size() > 0)
      name << "_" << additional;
   
   return name.str();
}




