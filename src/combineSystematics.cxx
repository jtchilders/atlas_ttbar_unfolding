#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <map>
#include <sstream>
#include <string>

#include "TDatime.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TTimeStamp.h"
#include "TMarker.h"
#include "Rtypes.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TLatex.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TF1.h"
#include "TH2.h"
#include "TH1.h"

#include "AtlasStyle.h"
#include "SystematicSample.h"
#include "SystematicHistoGroup.h"
#include "Th1PlusLegend.h"
#include "Th1PlotAndRatio.h"
#include "globals.h"
#include "MCFMHisto.h"
#include "MCFMWithPDF.h"
#include "KidonakisNNLO.h"
#include "YangPecjakNNLO.h"
#include "McAtNloBeforeCuts.h"
#include "AlpgenMC.h"
#include "PowHegMC.h"
#include "TexTable.h"
#include "SystTable.h"
#include "CombSystTable.h"
#include "SystematicBin.h"
#include "SystematicBinList.h"
#include "SystematicList.h"
#include "TopDecayChannel.h"
#include "PdfSystematic.h"
#include "PlotLabel.h"
#include "PValueTable.h"
#include "PValueList.h"
#include "MasterSystList.h"
#include "LatexDoc.h"
#include "KinematicVariable.h"
#include "GeneratorLabel.h"



void output_latex_table(SystematicList& errors,SystematicList& errorsNorm,
                        SystematicHistoGroup* nominalHistos,
                        const TopDecayChannel& channel,
                        std::string sampleName,
                        PValueList& pValues,
                        PValueList& pValuesNorm,
                        std::string filename,
                        std::string tagged,std::string taggedLatex, 
                        std::string topSample,std::string outputPath);

void output_combineChannels(SystematicList& errors,TH1* nominal,std::string filename);

void plot_XS(TGraphAsymmErrors  nominalSyst,const TopDecayChannel& channel,
             std::string sampleName,
             std::string filename,
             std::string description,std::string texdescription,
             LatexDoc* const latexDoc,
             PValueList& pValues,
             bool norm,bool log);

void plot_XS_withClosure(TGraphAsymmErrors  nominalSyst,const TopDecayChannel& channel,
             std::string sampleName,TH1* unfoldedReco,
             std::string filename,
             std::string description,std::string texdescription,
             LatexDoc* const latexDoc,bool norm,bool log);

void plot_XS_withGenerators(TGraphAsymmErrors nominalSyst,const TopDecayChannel& channel,
                            std::string sampleName,
                            std::string filename,
                            std::string description,std::string texDescription,
                            LatexDoc* const latexDoc, bool norm,bool log);

void plot_XS_withTheory(TGraphAsymmErrors nominalSyst,std::string filename,
                        std::string sampleName,
                        std::string description,std::string texDescription,
                        LatexDoc* const latexDoc,bool norm,bool log);




std::string get_histo_name(const std::string& basePath,const std::string& systematic,
                           const std::string& channel,const std::string& sampleName,
                           const unsigned int& regOrIter,const unsigned int& nToys,
                           const std::string& tail);

void plot_systematics(SystematicList& errors,TH1* data,std::string filename,std::string sampleName,
                      std::string description,
                      std::string texDescription,LatexDoc* const latexDoc,bool norm);

double GetIntegral(TGraphAsymmErrors* graph);

void WritePlotsToFile(const std::string& sampleName,const TopDecayChannel& channel,
                      const SystematicList& errors, const SystematicList& errorsNorm,
                      const TH1* nominalScaledByBR,
                      const TH1* nominalNorm,
                      const TGraphAsymmErrors* nominalWithSystematics,
                      const TGraphAsymmErrors* nominalNormWithSystematics,
                      const TH1* truthScaledByBR,
                      const TH1* measured);

std::string GetInputFilename(const std::string& systematic,
                             const std::string& sampleName,
                             const std::string& channel,
                             const std::string& nToysStr,
                             const GeneratorLabel::Generator genLabelEnum,
                             const unsigned int regOrIter);


int main(int argc,char** argv){
   
   const std::string funcName = "[combineSystematics]";
   
   std::cout << "[combineSystematics] Begin.\n";
   
   SetAtlasStyle();
   
   const unsigned int nToys = 5000;
   std::stringstream nToysStr;
   nToysStr << nToys;
   
   const std::string baseHistoPath = "unfolding/toys";
   
   const unsigned int reg_toppt = 4;
   const unsigned int reg_systemPt = 3;
   const unsigned int reg_systemMass = 3;
   const unsigned int reg_systemRapidity = 4;
   
   const unsigned int bayes_iter = 4;
   
   // retrieve the list of systematics to run over
   MasterSystList masterSystList;
   const std::vector<std::string>& systematics = masterSystList.SystematicsToRun();
   
   const std::string filename_systSep = globals::GetLatexEpsPath() +  "/systematicsSeparated.ps";
   
   std::vector<std::string> sampleNames;
   sampleNames.push_back("Tag1_Top1_lhood");
   sampleNames.push_back("Tag1_Top2_lhood");
   sampleNames.push_back("Tag1_SystemMass");
   sampleNames.push_back("Tag1_SystemPt");
   sampleNames.push_back("Tag1_SystemRapidity");
   
   std::vector<TopDecayChannel> channels;
   channels.push_back(TopDecayChannel(TopDecayChannel::EJETS));
   channels.push_back(TopDecayChannel(TopDecayChannel::MUJETS));
//    channels.push_back(TopDecayChannel(TopDecayChannel::COMBINED));
   
   ///////////
   /// READ Histograms
   ///////////
   
   // some typedefs for this huge map of a map of a map
   // indices: [sampleName][Channel][systematicSample]
   typedef std::map<std::string,std::map<std::string,std::map<std::string,SystematicHistoGroup*> > > tripleMap;
   typedef std::map<std::string,std::map<std::string,std::map<std::string,SystematicHistoGroup*> > >::iterator tripleMapItr;
   typedef std::map<std::string,std::map<std::string,SystematicHistoGroup*> > doubleMap;
   typedef std::map<std::string,std::map<std::string,SystematicHistoGroup*> >::iterator doubleMapItr;
   typedef std::map<std::string,SystematicHistoGroup*> singleMap;
   typedef std::map<std::string,SystematicHistoGroup*>::iterator singleMapItr;
   
   tripleMap* histoList = new tripleMap;
   
   TCanvas* can = new TCanvas("can","can",0,0,800,600);
   const unsigned int can_max = sampleNames.size()*channels.size()*systematics.size();
   unsigned int can_counter = 0;
   
   // legend positions
   double spectra_x1 = 0.5;
   double spectra_y1 = 0.9;
   double spectra_x2 = 0.97;
   double spectra_y2 = 0.6;
   
   {
      
      /// determine which generator we are using ALPGEN or MCATNLO
      GeneratorLabel* genLabel = new GeneratorLabel(globals::GetOutputPath());
      std::string generator = genLabel->GetLabel();
      const GeneratorLabel::Generator genLabelEnum = genLabel->GetGenerator();
      std::stringstream gentitle;
      
      
//       std::string input_filename;
//       if(argc > 1) input_filename = argv[1];
//       else{
//          input_filename = globals::outputFilesPath + "/dataMinusBG.toy";
//          input_filename += nToysStr.str() + ".root";
//       }
      
      /// this is a little hard coding, but need to be fast right now
      /// idealy the filename could be set on the command line
//       std::string input_filename_generator;
//       input_filename_generator = input_filename; // copy input filename
      // if current generator is MCATNLO, replace last part of path '/McAtNlo'
      // with '/Alpgen'
//       if(genLabelEnum == GeneratorLabel::MCATNLO){
//          std::string mcatnlo = "McAtNlo";
//          std::string alpgen = "Alpgen";
//          unsigned int index = input_filename_generator.rfind(mcatnlo);
//          if(index == std::string::npos){
//             std::cerr << "[combineSystematics] ERROR could not build filename for generator systematics\n";
//             return -1;
//          }
//          input_filename_generator = input_filename_generator.replace(index,mcatnlo.size(),alpgen);
//          input_filename_generator += "/dataMinusBG.toy";
//          input_filename_generator += nToysStr.str() + ".root";
//       }
//       // and if Alpgen is the generator, replace with McAtNlo
//       else if(genLabelEnum == GeneratorLabel::ALPGEN){
//          std::string mcatnlo = "McAtNlo";
//          std::string alpgen = "Alpgen";
//          unsigned int index = input_filename_generator.rfind(alpgen);
//          if(index == std::string::npos){
//             std::cerr << "[combineSystematics] ERROR could not build filename for generator systematics\n";
//             return -1;
//          }
//          input_filename_generator = input_filename_generator.replace(index,alpgen.size(),mcatnlo);
//       }
      
//       std::cout << "[combineSystematics] Reading in histograms from file " << input_filename << std::endl;
//       
//       TFile* inputFile = TFile::Open(input_filename.c_str());
//       if(!inputFile){
//          std::cerr << "[combineSystematics] ERROR Opening file: " << input_filename << "\n";
//          return -1;
//       }
//       
//       TFile* inputFileGenerator = TFile::Open(input_filename_generator.c_str());
//       if(!inputFileGenerator){
//          std::cerr << "[combineSystematics] ERROR Opening file: " << input_filename_generator << "\n";
//          return -1;
//       }
      
      if(!globals::CreatePath(globals::GetLatexEpsSplitPath())){
         std::cerr << "[combineSystematics] ERROR creating path: " << globals::GetLatexEpsSplitPath() << "\n";
         return 0;
      }
      if(!globals::CreatePath(globals::GetLatexPdfSplitPath())){
         std::cerr << "[combineSystematics] ERROR creating path: " << globals::GetLatexPdfSplitPath() << "\n";
         return 0;
      }
      
      // set color and style numbers for each systematic sample
      std::map<std::string,unsigned int> styles;
      std::map<std::string,unsigned int> colors;
      const unsigned int style_min = 20;
      const unsigned int style_max = 34;
      unsigned int style_counter = style_min;
      for(unsigned int i=0;i<systematics.size();++i){
         styles[systematics[i]] = style_counter;
         style_counter++;
         if(style_counter > style_max)
            style_counter = style_min;
      }
      
      
      int truth_color = kBlack;
      int reco_color = kRed;
      int measured_color = kBlue;
      int unfolded_color = kGreen;
      int diffxs_color = kOrange;
      int deviation_color = kCyan;
      
      double maximum = 9000.;
      double maximum_diffxs = 0.6;
      double maximum_meanDev = 0.6;
      double minimum_meanDev = -0.6;
      double maximum_rmsDev = 0.07;
      double minimum_rmsDev = 0.0;
      
      double leg_x1 = 0.5;
      double leg_y1 = 0.93;
      double leg_x2 = 0.95;
      double leg_y2 = 0.45;
      
      double leg2_x1 = 0.3;
      double leg2_y1 = 0.93;
      double leg2_x2 = 0.75;
      double leg2_y2 = 0.70;
      
      
      // Load all the plots first
      for(unsigned int sampleNum = 0;sampleNum<sampleNames.size();++sampleNum){
         std::string sampleName = sampleNames[sampleNum];
         
         tripleMapItr sampleItr = histoList->find(sampleName);
         if(sampleItr == histoList->end()){
            (*histoList)[sampleName] = doubleMap();
            sampleItr = histoList->find(sampleName);
         }
         
         // get right labels
         KinematicVariable* kinVar = new KinematicVariable(sampleName);
         std::string UkinVarLabel = kinVar->GetUKinVarLabel();
         std::string DkinVarLabel = kinVar->GetDKinVarLabel();
         
         
         
         
         for(unsigned int channelNum = 0;channelNum<channels.size();++channelNum){
            TopDecayChannel& channel = channels[channelNum];
            std::string channelRootText = channel.GetChannelStringROOT();
            
            doubleMapItr channelItr = sampleItr->second.find(channel.GetChannelString());
            if(channelItr == sampleItr->second.end()){
               (sampleItr->second)[channel.GetChannelString()] = singleMap();
               channelItr = sampleItr->second.find(channel.GetChannelString());
            }
            
            for(unsigned int sysNum = 0;sysNum<systematics.size();++sysNum){
               std::string systematic = systematics[sysNum];
               
               
               // skip systematics that do not use both el and mu
               if(MasterSystList::skipSystematic(systematic,channel.GetChannelString())) continue;
               
               singleMapItr systematicItr = channelItr->second.find(systematic);
               if(systematicItr == channelItr->second.end()){
                  (channelItr->second)[systematic] = new SystematicHistoGroup;
                  systematicItr = channelItr->second.find(systematic);
               }
               
               
               unsigned int regOrIter = 0;
               std::string tagged,topSample;
               if(sampleName.find("Tag0") != std::string::npos)
                  tagged = globals::btag0;
               else if(sampleName.find("Tag1") != std::string::npos)
                  tagged = globals::btag1;
               else if(sampleName.find("Tag2") != std::string::npos)
                  tagged = globals::btag2;
               
               {
                  // make a temporary variable to hold the current unfolding method
                  // being used.
                  globals::unf_methods unfMethod = globals::globalUnfoldingMethod;
                  
                  // if this is the unfolding systematic, the plots come from the
                  // opposite unfolding method so swap which method
                  // that is being used
                  if(systematic.find("unfolding") != std::string::npos){
                     if(globals::globalUnfoldingMethod == globals::SVD)
                        unfMethod = globals::BAYESIAN;
                     else if(globals::globalUnfoldingMethod == globals::BAYESIAN)
                        unfMethod = globals::SVD;
                  }
                  
                  // now fill the regularization or iteration value
                  // based on which method is being used.
                  if(sampleName.find("Top1") != std::string::npos){
                     topSample = globals::leptonicTop;
                     if(unfMethod == globals::SVD)
                        regOrIter = reg_toppt;
                     else if(unfMethod == globals::BAYESIAN)
                        regOrIter = bayes_iter;
                  }
                  else if(sampleName.find("Top2") != std::string::npos){
                     topSample = globals::hadronicTop;
                     if(unfMethod == globals::SVD)
                        regOrIter = reg_toppt;
                     else if(unfMethod == globals::BAYESIAN)
                        regOrIter = bayes_iter;
                  }
                  else if(sampleName.find("TopLepHad") != std::string::npos){
                     topSample = globals::lepHadTop;
                     if(unfMethod == globals::SVD)
                        regOrIter = reg_toppt;
                     else if(unfMethod == globals::BAYESIAN)
                        regOrIter = bayes_iter;
                  }
                  else if(sampleName.find("SystemMass") != std::string::npos){
                     if(unfMethod == globals::SVD)
                        regOrIter = reg_systemMass;
                     else if(unfMethod == globals::BAYESIAN)
                        regOrIter = bayes_iter;
                  }
                  else if(sampleName.find("SystemPt") != std::string::npos){
                     if(unfMethod == globals::SVD)
                        regOrIter = reg_systemPt;
                     else if(unfMethod == globals::BAYESIAN)
                        regOrIter = bayes_iter;
                  }
                  else if(sampleName.find("SystemRapidity") != std::string::npos){
                     if(unfMethod == globals::SVD)
                        regOrIter = reg_systemRapidity;
                     else if(unfMethod == globals::BAYESIAN)
                        regOrIter = bayes_iter;
                  }
                  
               }
               
               // get histogram names
               std::string base_name = get_histo_name(baseHistoPath,systematic,channel.GetChannelString(),sampleName,regOrIter,nToys,"");
               
               // build input filename
               std::string inputFilename = GetInputFilename(systematic,sampleName,
                     channel.GetChannelString(),nToysStr.str(),genLabelEnum,regOrIter);
               
               TFile* inFile = globals::OpenFile(funcName,inputFilename);
               
//                TFile* inFile = inputFile;
               if(systematic.find("mc_generator") != std::string::npos){
//                   inFile = inputFileGenerator;
                  base_name = get_histo_name(baseHistoPath,"nominal",channel.GetChannelString(),sampleName,regOrIter,nToys,"");
               }
               
               std::cout << " Loading plots for: " << sampleName << "/" << channel.GetChannelString() << "/" << systematic << "\n";
               
               std::string leg_label = systematic;
               leg_label += ",";
               leg_label += channelRootText;
               leg_label += ",";
               leg_label += tagged;
               leg_label += ",";
               leg_label += topSample;
               
               std::stringstream ntitle;
               ntitle <<";"<<UkinVarLabel;
               Th1PlusLegend plots(spectra_x1,spectra_y1,spectra_x2,spectra_y2,leg_label);
               plots.SetTitle(ntitle.str().c_str());
               
               SystematicHistoGroup* hg = systematicItr->second;
               
               SystematicSample sample(systematic);
               hg->sample = sample;
               
               can->Clear();
               
               
               std::string name =  base_name + "_mc_truth";
               hg->truth = (TH1*)inFile->Get(name.c_str());
               if(!hg->truth){
                  std::cerr << "[combineSystematics] ERROR retrieving histogram " << name << " from file " << inFile->GetName() << "\n";
                  return 0;
               }
               
               gentitle <<"truth ("<<generator<<"MC@NLO)"; 
               plots.SetTitle(gentitle.str().c_str());
               
               hg->truth->SetMarkerColor(truth_color);
               hg->truth->SetLineColor(truth_color);
               hg->truth->SetMarkerStyle(styles[systematic]);
               hg->truth->SetTitle("");
               hg->truth->SetDirectory(false);
               hg->truth->Draw();
               plots.AddHisto(hg->truth,gentitle.str().c_str());
               //plots.AddHisto(hg->truth,"truth (MC@NLO)");
               
               name =  base_name + "_mc_truth_diffxs";
               hg->truth_diffxs = (TH1*)inFile->Get(name.c_str());
               if(!hg->truth_diffxs){
                  std::cerr << "[combineSystematics] ERROR retrieving histogram " << name << " from file " << inFile->GetName() << "\n";
                  return 0;
               }
               hg->truth_diffxs->SetMarkerColor(truth_color);
               hg->truth_diffxs->SetLineColor(truth_color);
               hg->truth_diffxs->SetMarkerStyle(styles[systematic]);
               hg->truth_diffxs->SetTitle("");
               hg->truth_diffxs->SetDirectory(false);
               hg->truth_diffxs->Draw();
               
               name = base_name + "_mc_reco";
               hg->reco = (TH1*)inFile->Get(name.c_str());
               if(!hg->reco){
                  std::cerr << "[combineSystematics] ERROR retrieving histogram " << name << " from file " << inFile->GetName() << "\n";
                  return 0;
               }
               hg->reco->SetMarkerColor(reco_color);
               hg->reco->SetLineColor(reco_color);
               hg->reco->SetMarkerStyle(styles[systematic]);
               hg->reco->SetTitle("");
               hg->reco->SetDirectory(false);
               hg->reco->Draw("same");
               plots.AddHisto(hg->reco,"reco (MC@NLO)");
               
               name = base_name + "_data_measured";
               hg->measured = (TH1*)inFile->Get(name.c_str());
               if(!hg->measured){
                  std::cerr << "[combineSystematics] ERROR retrieving histogram " << name << " from file " << inFile->GetName() << "\n";
                  return 0;
               }
               hg->measured->SetMarkerColor(measured_color);
               hg->measured->SetLineColor(measured_color);
               hg->measured->SetMarkerStyle(styles[systematic]);
               hg->measured->SetTitle("");
               hg->measured->SetDirectory(false);
               hg->measured->Draw("same");
               plots.AddHisto(hg->measured,"measured (Data-BG)");
               
               name = base_name + "_data_unfolded";
               hg->unfolded = (TH1*)inFile->Get(name.c_str());
               if(!hg->unfolded){
                  std::cerr << "[combineSystematics] ERROR retrieving histogram " << name << " from file " << inFile->GetName() << "\n";
                  return 0;
               }
               hg->unfolded->SetMarkerColor(unfolded_color);
               hg->unfolded->SetLineColor(unfolded_color);
               hg->unfolded->SetMarkerStyle(styles[systematic]);
               hg->unfolded->SetTitle("");
               hg->unfolded->SetDirectory(false);
               hg->unfolded->Draw("same");
               plots.AddHisto(hg->unfolded,"unfolded (Data-BG)");
               
               plots.Draw();
               
               if(can_counter == 0){
                  can->SaveAs(std::string(filename_systSep + "(").c_str());
               }
               else{
                  can->SaveAs(filename_systSep.c_str());
               }
               
               //save eps
               std::stringstream comFile;
               comFile <<channel.GetChannelString()<<"_"<<sampleName;
               if(globals::globalUnfoldingMethod == globals::SVD)
                  comFile <<"svd_reg" << regOrIter << "_toy" << nToys;
               else if(globals::globalUnfoldingMethod == globals::BAYESIAN)
                  comFile <<"bayes_itr" << regOrIter << "_toy" << nToys;
               else if(globals::globalUnfoldingMethod == globals::BINBYBIN)
                  comFile <<"binbybin_toy" << nToys;
               else if(globals::globalUnfoldingMethod == globals::INVERSION)
                  comFile <<"inversion_toy" << nToys;
               comFile <<"_syst_"<<systematic;
               
               const std::string epsComFile = globals::GetLatexEpsSplitPath() + "/combine_" + comFile.str() + ".eps";
               can->SaveAs(epsComFile.c_str());
               
               // save pdf
               const std::string pdfComFile = globals::GetLatexPdfSplitPath() + "/combine_" + comFile.str() + ".pdf";
               can->SaveAs(pdfComFile.c_str());
               
               //we included the efficiency and luminosity corrections
               can->Clear();
               name = base_name + "_data_unfolded_diffxs";
               hg->diffxs = (TH1*)inFile->Get(name.c_str());
               if(!hg->diffxs){
                  std::cerr << "[combineSystematics] ERROR retrieving histogram " << name << " from file " << inFile->GetName() << "\n";
                  return 0;
               }
               hg->diffxs->SetMarkerColor(diffxs_color);
               hg->diffxs->SetLineColor(diffxs_color);
               hg->diffxs->SetMarkerStyle(styles[systematic]);
               hg->diffxs->SetTitle("");
               hg->diffxs->SetDirectory(false);
               hg->diffxs->GetXaxis()->SetTitle(UkinVarLabel.c_str());
               hg->diffxs->GetYaxis()->SetTitle(DkinVarLabel.c_str());
               hg->diffxs->Draw();
               
               std::stringstream integral;
               integral.precision(4);
               integral.width(5);
               integral << hg->diffxs->Integral("width");
               
               // get error on integral
               double squareError=0;
               for(unsigned int bin=1;bin<=hg->diffxs->GetNbinsX();++bin)
                  squareError += hg->diffxs->GetBinError(bin)*hg->diffxs->GetBinError(bin);
               integral << " #pm " << sqrt(squareError) << " pb";
               
               TPaveText label(spectra_x1-0.05,spectra_y1+0.05,spectra_x2-0.05,spectra_y2,"brNDC");
               label.SetFillStyle(0);
               label.SetBorderSize(0);
               label.AddText(leg_label.c_str());
               label.AddText(integral.str().c_str());
               label.AddText("theoretical: (73.4 #pm 5.0) pb");
               label.Draw("same");
               
               can->SaveAs(filename_systSep.c_str());
               
               //save eps,pdf
               const std::string diffxs_epsComFile = globals::GetLatexEpsSplitPath() + "/diffxs_" + comFile.str() + ".eps";
               can->SaveAs(diffxs_epsComFile.c_str());
               
               const std::string diffxs_pdfComFile = globals::GetLatexPdfSplitPath() + "/diffxs_" + comFile.str() + ".pdf";
               can->SaveAs(diffxs_pdfComFile.c_str());
               
               /// plot the 1/sigma*dsigma/dpt
               can->Clear();
               
               TLegend normalizedLeg(spectra_x1,spectra_y1,spectra_x2,spectra_y2,leg_label.c_str());
               normalizedLeg.SetFillStyle(0);
               normalizedLeg.SetBorderSize(0);
               
               name = base_name + "_data_unfolded_diffxsNorm";
               hg->diffxsNorm = (TH1*)inFile->Get(name.c_str());
               if(!hg->diffxsNorm){
                  std::cerr << "[combineSystematics] ERROR retrieving histogram " << name << " from file " << inFile->GetName() << "\n";
                  return 0;
               }
               hg->diffxsNorm->SetMarkerColor(diffxs_color);
               hg->diffxsNorm->SetLineColor(diffxs_color);
               hg->diffxsNorm->SetMarkerStyle(styles[systematic]);
               hg->diffxsNorm->SetTitle("");
               hg->diffxsNorm->SetDirectory(false);
               hg->diffxsNorm->Draw();
               normalizedLeg.AddEntry(hg->diffxsNorm,"unfolded (Data-BG)");
               
               name =  base_name + "_mc_truth_diffxsNorm";
               hg->truthNorm_diffxs = (TH1*)inFile->Get(name.c_str());
               if(!hg->truthNorm_diffxs){
                  std::cerr << "[combineSystematics] ERROR retrieving histogram " << name << " from file " << inFile->GetName() << "\n";
                  return 0;
               }
               hg->truthNorm_diffxs->SetMarkerColor(truth_color);
               hg->truthNorm_diffxs->SetLineColor(truth_color);
               hg->truthNorm_diffxs->SetMarkerStyle(styles[systematic]);
               hg->truthNorm_diffxs->SetTitle("");
               hg->truthNorm_diffxs->SetDirectory(false);
               hg->truthNorm_diffxs->Draw("same");
               normalizedLeg.AddEntry(hg->truthNorm_diffxs,"truth (MC@NLO)");
               
               normalizedLeg.Draw("same");
   
               can->SaveAs(filename_systSep.c_str());
   
               //save eps,pdf
               const std::string diffxsNorm_epsComFile = globals::GetLatexEpsSplitPath() + "/diffxsNorm_" + comFile.str() + ".eps";
               can->SaveAs(diffxsNorm_epsComFile.c_str());
   
               const std::string diffxsNorm_pdfComFile = globals::GetLatexPdfSplitPath() + "/diffxsNorm_" + comFile.str() + ".pdf";
               can->SaveAs(diffxsNorm_pdfComFile.c_str());
   
               
               //Normalized unfolded
               can->Clear();
               
               TLegend unormalizedLeg(spectra_x1,spectra_y1,spectra_x2,spectra_y2,leg_label.c_str());
               unormalizedLeg.SetFillStyle(0);
               unormalizedLeg.SetBorderSize(0);
               
               name =  base_name + "_data_unfoldedNorm";
               hg->unfoldedNorm = (TH1*)inFile->Get(name.c_str());
               if(!hg->unfoldedNorm){
                  std::cerr << "[combineSystematics] ERROR retrieving histogram " << name << " from file " << inFile->GetName() << "\n";
                  return 0;
               }
               hg->unfoldedNorm->SetMarkerColor(unfolded_color);
               hg->unfoldedNorm->SetLineColor(unfolded_color);
               hg->unfoldedNorm->SetMarkerStyle(styles[systematic]);
               hg->unfoldedNorm->SetTitle("");
               hg->unfoldedNorm->SetDirectory(false);
               hg->unfoldedNorm->Draw();
               unormalizedLeg.AddEntry(hg->unfoldedNorm,"normalized unfolded");
               
               unormalizedLeg.Draw("same");
               
               can->SaveAs(filename_systSep.c_str());
               
               can->Clear();
               can->Divide(1,2);
               
               can->cd(1);
               name = base_name + "_meanDeviation";
               hg->meanDeviation = (TH1*)inFile->Get(name.c_str());
               if(!hg->meanDeviation){
                  std::cerr << "[combineSystematics] ERROR retrieving histogram " << name << " from file " << inFile->GetName() << "\n";
                  return 0;
               }
               hg->meanDeviation->SetMarkerColor(deviation_color);
               hg->meanDeviation->SetLineColor(deviation_color);
               hg->meanDeviation->SetMarkerStyle(styles[systematic]);
               hg->meanDeviation->SetTitle("");
               hg->meanDeviation->SetDirectory(false);
               hg->meanDeviation->Draw();
               
               label.Clear();
               label.AddText(leg_label.c_str());
               label.Draw("same");
               
               
               //save eps,pdf
               const std::string mean_epsComFile = globals::GetLatexEpsSplitPath() + "/mean_" + comFile.str() + ".eps";
               can->cd(1)->SaveAs(mean_epsComFile.c_str());
               
               const std::string mean_pdfComFile = globals::GetLatexPdfSplitPath() + "/mean_" + comFile.str() + ".pdf";
               can->cd(1)->SaveAs(mean_pdfComFile.c_str());
               
               can->cd(2);
               name = base_name + "_rmsDeviation";
               hg->rmsDeviation = (TH1*)inFile->Get(name.c_str());
               if(!hg->rmsDeviation){
                  std::cerr << "[combineSystematics] ERROR retrieving histogram " << name << " from file " << inFile->GetName() << "\n";
                  return 0;
               }
               hg->rmsDeviation->SetMarkerColor(deviation_color);
               hg->rmsDeviation->SetLineColor(deviation_color);
               hg->rmsDeviation->SetMarkerStyle(styles[systematic]);
               hg->rmsDeviation->SetTitle("");
               hg->rmsDeviation->SetDirectory(false);
               hg->rmsDeviation->Draw();
               label.Draw("same");
   
                                       
               //save eps,pdf
               const std::string rms_epsComFile = globals::GetLatexEpsSplitPath() + "/rms_" + comFile.str() + ".eps";
               can->cd(2)->SaveAs(rms_epsComFile.c_str());
   
               const std::string rms_pdfComFile = globals::GetLatexPdfSplitPath() + "/rms_" + comFile.str() + ".pdf";
               can->cd(2)->SaveAs(rms_pdfComFile.c_str());
               
               
               if( can_counter == can_max - 1 ){
                  can->SaveAs(std::string(filename_systSep + ")").c_str());
               }
               else{
                  can->SaveAs(filename_systSep.c_str());
               }
               
               can_counter++;
               
               inFile->Close();
               globals::set_chmod(inFile->GetName());
               delete inFile;
               inFile = 0;
               
               
            } // for(systematics)
            
         }// for(channels)
            
         delete kinVar;
         kinVar = 0;
      }// for(sampleNames)
      
      delete genLabel;
      genLabel = 0;
      
//       inputFile->Close();
//       inputFileGenerator->Close();
//       globals::set_chmod(inputFile->GetName());
   }
   
   
   
   
   
   // variables to hold pvalues and chi2's
   PValueList pValues,pValuesNorm;
   
   
   
   
   /// now plot on a 2x2 canvas the truth, reco, measured and unfolded
   /// with the nominal + each systematic(u/d)
   {
      std::string filename_nomPlusSyst = globals::GetLatexEpsPath() + "/nominalSyst_2x2.ps";
      
      can_counter = 0;
      bool firstCan = true;
      can->Clear();
      can->Divide(2,2);
      
      // need to keep track of systematics to skip due to
      // systematic pairs (up and down)
      std::vector<std::string> systToSkip;
      
      // loop over sample names
      tripleMapItr sampleItr;
      for(sampleItr = (*histoList).begin();sampleItr != (*histoList).end();++sampleItr){
         std::string sampleName = sampleItr->first;
         doubleMap& sampleHistoMap = sampleItr->second;
         
         // get right labels
         KinematicVariable* kinVar = new KinematicVariable(sampleName);
         std::string kinVarLabel  = kinVar->GetKinVarLabel();
         std::string UkinVarLabel = kinVar->GetUKinVarLabel();
         std::string DkinVarLabel = kinVar->GetDKinVarLabel();
         std::string NkinVarLabel = kinVar->GetNKinVarLabel(); 
         
         unsigned int regOrIter = 0;
         std::string tagged,topSample,taggedLatex;
         if(sampleName.find("Tag0") != std::string::npos){
            tagged = globals::btag0;
            taggedLatex = globals::btag0Latex;
         }
         else if(sampleName.find("Tag1") != std::string::npos){
            tagged = globals::btag1;
            taggedLatex = globals::btag1Latex;
         }
         else if(sampleName.find("Tag2") != std::string::npos){
            tagged = globals::btag2;
            taggedLatex = globals::btag2Latex;
         }
         
         if(sampleName.find("Top1") != std::string::npos){
            topSample = globals::leptonicTop;
            if(globals::globalUnfoldingMethod == globals::SVD)
               regOrIter = reg_toppt;
            else if(globals::globalUnfoldingMethod == globals::BAYESIAN)
               regOrIter = bayes_iter;
         }
         else if(sampleName.find("Top2") != std::string::npos){
            topSample = globals::hadronicTop;
            if(globals::globalUnfoldingMethod == globals::SVD)
               regOrIter = reg_toppt;
            else if(globals::globalUnfoldingMethod == globals::BAYESIAN)
               regOrIter = bayes_iter;
         }
         else if(sampleName.find("TopLepHad") != std::string::npos){
            topSample = globals::lepHadTop;
            if(globals::globalUnfoldingMethod == globals::SVD)
               regOrIter = reg_toppt;
            else if(globals::globalUnfoldingMethod == globals::BAYESIAN)
               regOrIter = bayes_iter;
         }
         else if(sampleName.find("SystemMass") != std::string::npos){
            if(globals::globalUnfoldingMethod == globals::SVD)
               regOrIter = reg_systemMass;
            else if(globals::globalUnfoldingMethod == globals::BAYESIAN)
               regOrIter = bayes_iter;
         }
         else if(sampleName.find("SystemPt") != std::string::npos){
            if(globals::globalUnfoldingMethod == globals::SVD)
               regOrIter = reg_systemPt;
            else if(globals::globalUnfoldingMethod == globals::BAYESIAN)
               regOrIter = bayes_iter;
         }
         
         
         
         
         // loop over the channels (el/mu)
         doubleMapItr channelItr;
         for(channelItr = sampleHistoMap.begin();channelItr != sampleHistoMap.end();++channelItr){
            TopDecayChannel channel(channelItr->first);
            singleMap& channelHistoMap = channelItr->second;
            
            SystematicList systList;
            SystematicList normSystList;
            
            // want to keep track of the nominal plots
            SystematicHistoGroup* nominal = 0;
            singleMapItr nominalItr;
            // retreive the histogram group for the partner
            nominalItr = channelHistoMap.find(SystematicSample::GetSampleString(SystematicSample::NOMINAL));
            if(nominalItr == channelHistoMap.end()){
               std::cerr << " [combineSystematics] ERROR: nominal cannot be found in the (*histoList) map.\n";
               return -1;
            }
            
            nominal = nominalItr->second;
            
            
            
            ///////////////
            /// Need to save systematic shapes to file so open the file
      
            std::string out_filename = globals::GetOutputPath() 
                  + "/unfoldedResult_" 
                  + sampleName + "_" 
                  + channel.GetChannelString() + ".root";
            // open an output file to save the results to disk for later manipulation
            TFile* outFile = new TFile(out_filename.c_str(),"UPDATE");
            if(!outFile->IsOpen()){
               std::cerr << "[output_latex_table] ERROR opening file for output: " << out_filename << "\n";
            }
            ///////////////
            ////////
            
            // loop over the systematic samples
            singleMapItr systItr;
            for(systItr = channelHistoMap.begin();systItr != channelHistoMap.end();++systItr){
               std::string systematic = systItr->first;
               SystematicHistoGroup* hg = systItr->second;
               
               // skip systematics that do not use both el and mu
               if(MasterSystList::skipSystematic(systematic,channel.GetChannelString())) continue;
               
               
               std::cout << " Calculating Systematic Uncertainties for: " << sampleName << "/" << channel.GetChannelString() << "/" << systematic << "\n";
               
               
               // skip nominal sample since it is plotted along with each sytematic
               SystematicSample::Sample systSampleEnum = SystematicSample::GetSampleEnum(systematic);
               if(systSampleEnum == SystematicSample::NOMINAL){
                  // increment to keep counter in sync
                  can_counter++;
                  continue;
               }
               
               // skip any systematic partners which have been already plotted
               {
                  bool skip = false;
                  for(unsigned int i=0;i<systToSkip.size();++i){
                     if(systematic.find(systToSkip[i]) != std::string::npos){
                        // remove syst from list
                        systToSkip.erase(systToSkip.begin() + i);
                        skip = true;
                        break;
                     }
                  }
                  if(skip) continue;
               }
               
               std::string baseSystematic = hg->sample.GetSampleBaseString(hg->sample);
               
               
               // string to label this combination of things
               std::string combinationText = channel.GetChannelStringROOT();;
               combinationText += ",";
               combinationText += tagged;
               combinationText += ",";
               combinationText += topSample;
               
               std::string truth_label = "#splitline{Truth (MC@NLC)}{";
               truth_label += combinationText + "}";
               std::string reco_label = "#splitline{Reco (MC@NLC)}{";
               reco_label += combinationText + "}";
               std::string measured_label = "#splitline{Measured (Data-BG)}{";
               measured_label += combinationText + "}";
               std::string unfolded_label = "#splitline{Unfolded (Data-BG)}{";
               unfolded_label += combinationText + "}";
               std::string unfoldedNorm_label = "#splitline{UnfoldedNorm (Data-BG)}{";
               unfoldedNorm_label += combinationText + "}";
               std::string diffxs_label = "#splitline{#frac{d#sigma}{d" + kinVarLabel +"} (Data-BG)}{";
               diffxs_label += combinationText + "}";
               
               std::stringstream ntitle;
               ntitle <<";"<<UkinVarLabel; 
               Th1PlusLegend* truth = new Th1PlusLegend   (spectra_x1,spectra_y1,spectra_x2,spectra_y2,truth_label);
               truth->SetTitle(ntitle.str().c_str());
               if((systSampleEnum != SystematicSample::FSRMINUS) && 
                   (systSampleEnum != SystematicSample::FSRPLUS) && 
                   (systSampleEnum != SystematicSample::IFSRMINUS) && 
                   (systSampleEnum != SystematicSample::IFSRPLUS) && 
                   (systSampleEnum != SystematicSample::ISRMINUS) && 
                   (systSampleEnum != SystematicSample::ISRPLUS)) 
                  truth->AddHisto(nominal->truth,"nominal");
               truth->AddHisto(hg->truth,systematic);
               
               Th1PlusLegend* reco = new Th1PlusLegend (spectra_x1,spectra_y1,spectra_x2,spectra_y2,reco_label);
               reco->SetTitle(ntitle.str().c_str());
               if((systSampleEnum != SystematicSample::FSRMINUS) && 
                   (systSampleEnum != SystematicSample::FSRPLUS) && 
                   (systSampleEnum != SystematicSample::IFSRMINUS) && 
                   (systSampleEnum != SystematicSample::IFSRPLUS) && 
                   (systSampleEnum != SystematicSample::ISRMINUS) && 
                   (systSampleEnum != SystematicSample::ISRPLUS))    
                  reco->AddHisto(nominal->reco,"nominal");
               reco->AddHisto(hg->reco,systematic);
               
               Th1PlusLegend* measured = new Th1PlusLegend (spectra_x1,spectra_y1,spectra_x2,spectra_y2,measured_label);
               measured->SetTitle(ntitle.str().c_str());
               if((systSampleEnum != SystematicSample::FSRMINUS) && 
                   (systSampleEnum != SystematicSample::FSRPLUS) && 
                   (systSampleEnum != SystematicSample::IFSRMINUS) && 
                   (systSampleEnum != SystematicSample::IFSRPLUS) && 
                   (systSampleEnum != SystematicSample::ISRMINUS) && 
                   (systSampleEnum != SystematicSample::ISRPLUS)) 
                  measured->AddHisto(nominal->measured,"nominal");
               measured->AddHisto(hg->measured,systematic);
               
               Th1PlusLegend* unfolded = new Th1PlusLegend (spectra_x1,spectra_y1,spectra_x2,spectra_y2,unfolded_label);
               unfolded->SetTitle(ntitle.str().c_str());
               if((systSampleEnum != SystematicSample::FSRMINUS) && 
                   (systSampleEnum != SystematicSample::FSRPLUS) && 
                   (systSampleEnum != SystematicSample::IFSRMINUS) && 
                   (systSampleEnum != SystematicSample::IFSRPLUS) && 
                   (systSampleEnum != SystematicSample::ISRMINUS) && 
                   (systSampleEnum != SystematicSample::ISRPLUS))  
                  unfolded->AddHisto(nominal->unfolded,"nominal");
               unfolded->AddHisto(hg->unfolded,systematic);
               
               Th1PlusLegend* unfoldedNorm = new Th1PlusLegend (spectra_x1,spectra_y1,spectra_x2,spectra_y2,unfoldedNorm_label);
               unfoldedNorm->SetTitle(ntitle.str().c_str());
               if((systSampleEnum != SystematicSample::FSRMINUS) && 
                   (systSampleEnum != SystematicSample::FSRPLUS) && 
                   (systSampleEnum != SystematicSample::IFSRMINUS) && 
                   (systSampleEnum != SystematicSample::IFSRPLUS) && 
                   (systSampleEnum != SystematicSample::ISRMINUS) && 
                   (systSampleEnum != SystematicSample::ISRPLUS)) 
                  unfoldedNorm->AddHisto(nominal->unfoldedNorm,"nominal");
               unfoldedNorm->AddHisto(hg->unfoldedNorm,systematic);
               
               ntitle.str("");
               ntitle <<";"<<UkinVarLabel<<";"<<DkinVarLabel; 
               Th1PlusLegend* diffxs = new Th1PlusLegend (spectra_x1,spectra_y1,spectra_x2,spectra_y2,combinationText);
               diffxs->SetTitle(ntitle.str().c_str());
               if((systSampleEnum != SystematicSample::FSRMINUS) && 
                   (systSampleEnum != SystematicSample::FSRPLUS) && 
                   (systSampleEnum != SystematicSample::IFSRMINUS) && 
                   (systSampleEnum != SystematicSample::IFSRPLUS) && 
                   (systSampleEnum != SystematicSample::ISRMINUS) && 
                   (systSampleEnum != SystematicSample::ISRPLUS)) 
                  diffxs->AddHisto(nominal->diffxs,"nominal");
               diffxs->AddHisto(hg->diffxs,systematic);
               
               Th1PlusLegend* meanDeviation = new Th1PlusLegend (0.7,0.9,0.95,0.7,combinationText);
               meanDeviation->SetTitle(UkinVarLabel.c_str(),nominal->meanDeviation->GetYaxis()->GetTitle());            
               if((systSampleEnum != SystematicSample::FSRMINUS) && 
                   (systSampleEnum != SystematicSample::FSRPLUS) && 
                   (systSampleEnum != SystematicSample::IFSRMINUS) && 
                   (systSampleEnum != SystematicSample::IFSRPLUS) && 
                   (systSampleEnum != SystematicSample::ISRMINUS) && 
                   (systSampleEnum != SystematicSample::ISRPLUS)) 
                  meanDeviation->AddHisto(nominal->meanDeviation,"nominal");
               meanDeviation->AddHisto(hg->meanDeviation,systematic);
               
               Th1PlusLegend* rmsDeviation = new Th1PlusLegend (0.25,0.9,0.6,0.7,combinationText);
               rmsDeviation->SetTitle(UkinVarLabel.c_str(),nominal->rmsDeviation->GetYaxis()->GetTitle());
               if((systSampleEnum != SystematicSample::FSRMINUS) && 
                   (systSampleEnum != SystematicSample::FSRPLUS) && 
                   (systSampleEnum != SystematicSample::IFSRMINUS) && 
                   (systSampleEnum != SystematicSample::IFSRPLUS) && 
                   (systSampleEnum != SystematicSample::ISRMINUS) && 
                   (systSampleEnum != SystematicSample::ISRPLUS))  
                  rmsDeviation->AddHisto(nominal->rmsDeviation,"nominal");
               rmsDeviation->AddHisto(hg->rmsDeviation,systematic);
               
               
               SystematicBinList binErrors;
               SystematicBinList binErrorsNorm;
               
               
               // check to see if this is a systematic with an up and down pair
               SystematicSample::Sample systPartnerEnum = SystematicSample::GetPartner(systSampleEnum);
               SystematicHistoGroup* partnerHg = 0;
               if(systPartnerEnum != SystematicSample::NUM_OF_SAMPLES)
               { // this systematic has a partner
                  
                  singleMapItr partnerItr;
                  // retreive the histogram group for the partner
                  std::string systPartnerString = SystematicSample::GetSampleString(systPartnerEnum);
                  partnerItr = channelHistoMap.find(systPartnerString);
                  if(partnerItr == channelHistoMap.end()){
                     std::cerr << " [combineSystematics] ERROR: systematic partner, " << systPartnerString << ", cannot be found in the (*histoList) map for " << systematic << " " << (int)systSampleEnum << " " << (int)systPartnerEnum << "\n";
                     return -1;
                  }
                  
                  SystematicHistoGroup* partnerHg = partnerItr->second;
                  
                  //////////////////////////////
                  ///// Need to write systematic diffxs and diffxsNorm to file so that
                  ///// we can use them in the correlation matrices calculation
                  bool systIsUp = true;
                  {
                     outFile->cd();
                     std::string outHistoName = baseSystematic + "_shape";
                     std::string partnerHistoName = outHistoName;
                     if(SystematicSample::GetUp(systematic)){
                        outHistoName += "_up";
                        systIsUp = true;
                        partnerHistoName += "_down";
                     }
                     else if(SystematicSample::GetDown(systematic)){
                        outHistoName += "_down";
                        systIsUp = false;
                        partnerHistoName += "_up";
                     }
                     
                     TH1* diffxs = (TH1*)hg->diffxs->Clone("tmp");
                     diffxs->Scale(1./globals::BR);
                     diffxs->Write(outHistoName.c_str(),TObject::kOverwrite);
                     delete diffxs; diffxs = 0;
                     diffxs = (TH1*)partnerHg->diffxs->Clone("tmp");
                     diffxs->Scale(1./globals::BR);
                     diffxs->Write(partnerHistoName.c_str(),TObject::kOverwrite);
                     delete diffxs; diffxs = 0;
                     
                     
                     outHistoName += "_norm";
                     hg->diffxsNorm->Write(outHistoName.c_str(),TObject::kOverwrite);
                     partnerHistoName += "_norm";
                     partnerHg->diffxsNorm->Write(partnerHistoName.c_str(),TObject::kOverwrite);
                  }
                  //////////////////////////
                  /////////
                  
                  // add partner histograms
                  truth->AddHisto(partnerHg->truth,systPartnerString);
                  reco->AddHisto(partnerHg->reco,systPartnerString);
                  measured->AddHisto(partnerHg->measured,systPartnerString);
                  unfolded->AddHisto(partnerHg->unfolded,systPartnerString);
                  //unfoldedNorm->AddHisto(partnerHg->unfoldedNorm,systPartnerString);
                  diffxs->AddHisto(partnerHg->diffxs,systPartnerString);
                  meanDeviation->AddHisto(partnerHg->meanDeviation,systPartnerString);
                  rmsDeviation->AddHisto(partnerHg->rmsDeviation,systPartnerString);
                  
                  // calculate the combined systematics for this systematic
                  for(unsigned int bin=1;bin<=nominal->truth->GetNbinsX();++bin){
                     double x1 = nominal->diffxs->GetBinContent(bin)/globals::BR;
                     double x1Norm = nominal->diffxsNorm->GetBinContent(bin);
                     double x2 = hg->diffxs->GetBinContent(bin)/globals::BR;
                     double x2Norm = hg->diffxsNorm->GetBinContent(bin);
                     double x3 = partnerHg->diffxs->GetBinContent(bin)/globals::BR;
                     double x3Norm = partnerHg->diffxsNorm->GetBinContent(bin);
                     
                     SystematicBin err,errNorm;
                     // Save shape for later use to add signs in tables
                     // if this systematic is the up systematic, then save
                     // it's value to the shape_up and the partner to shape_down
                     if(systIsUp){
                        err.shape_up(x2);
                        err.shape_down(x3);
                        errNorm.shape_up(x2Norm);
                        errNorm.shape_down(x3Norm);
                     }
                     else{ // otherwise, swap them
                        err.shape_up(x3);
                        err.shape_down(x2);
                        errNorm.shape_up(x3Norm);
                        errNorm.shape_down(x2Norm);
                     }
                     
                     if( (systSampleEnum == SystematicSample::FSRMINUS) ||
                         (systSampleEnum == SystematicSample::FSRPLUS) ||
                         (systSampleEnum == SystematicSample::IFSRMINUS) ||
                         (systSampleEnum == SystematicSample::IFSRPLUS) ||
                         (systSampleEnum == SystematicSample::ISRMINUS) ||
                         (systSampleEnum == SystematicSample::ISRPLUS) ||
                         (systSampleEnum == SystematicSample::POWHEG_JIMMY) ||
                         (systSampleEnum == SystematicSample::POWHEG_PYTHIA)
                       )
                     {
                        
                        double meanSyst = (x2+x3)/2.;
                        double meanSystNorm = (x2Norm+x3Norm)/2.;
                        
                        double diffSyst = 0., diffSystNorm = 0.;
                        
                        if((systSampleEnum == SystematicSample::POWHEG_JIMMY) || (systSampleEnum == SystematicSample::POWHEG_PYTHIA)) {
                           // do not divide by two
                           diffSyst = fabs(x3-x2);
                           diffSystNorm = fabs(x3Norm-x2Norm);
                        }
                        else { // divide by two
                           diffSyst = fabs(x3-x2)/2.;
                           diffSystNorm = fabs(x3Norm-x2Norm)/2.;
                        }
                        
                        // error is calculated as a percentage of the ratio to the mean
                        double dx = (x1 * diffSyst)/meanSyst;
                        double dxNorm = (x1Norm * diffSystNorm)/meanSystNorm;
                        
                        err.up(dx);
                        err.down(dx);
                        
                        errNorm.up(dxNorm);
                        errNorm.down(dxNorm);
                        
                     }
                     // here is just the standard comparison to nominal
                     else {
                        // first absolute spectrum
                        
                        // nominal minus current systematic
                        double dx12 = x1-x2;
                        
                        // if the difference is positive, the nominal is larger
                        // so this is the down shift
                        if(dx12 >= 0){
                           err.down(dx12);
                        }
                        // otherwise it is the up shift
                        else{
                           err.up(fabs(dx12));
                        }
                        
                        // nominal minus the partner systematic
                        double dx13= x1-x3;
                        // if positive this is the down shift
                        if(dx13 >= 0){
                           // if the down shift has already been assigned
                           // use the bigger of the two
                           if(err.down() != 0.){
                              if(err.down() < dx13)
                                 err.down(dx13);
                           }
                           // otherwise just assign the value
                           else
                              err.down(dx13);
                        }
                        else{
                           dx13 = fabs(dx13);
                           if(err.up() != 0.){
                              if(err.up() < dx13)
                                 err.up(dx13);
                           }
                           else
                              err.up(dx13);
                        }
                        
                        //Systematics errors for the normalized cross-section
                        double dx12Norm = x1Norm-x2Norm;
                        
                        if(dx12Norm >= 0){
                           errNorm.down(fabs(dx12Norm));
                        }
                        else{
                           errNorm.up(fabs(dx12Norm));
                        }
                     
                        double dx13Norm= x1Norm-x3Norm;
                        if(dx13Norm >= 0){
                           if(errNorm.down() != 0.){
                              if(errNorm.down() < dx13Norm)
                                 errNorm.down(dx13Norm);
                           }
                           else
                              errNorm.down(fabs(dx13Norm));
                        }
                        else{
                           dx13Norm = fabs(dx13Norm);
                           if(errNorm.up() != 0.){
                              if(errNorm.up() < dx13Norm)
                                 errNorm.up(dx13Norm);
                           }
                           else
                              errNorm.up(fabs(dx13Norm));
                        }
                        
                     }// end else
                     
                     // add bin edges to variables
                     err.content(nominal->diffxs->GetBinContent(bin)/globals::BR);
                     err.stat(nominal->diffxs->GetBinError(bin)/globals::BR);
                     err.low(nominal->diffxs->GetBinLowEdge(bin));
                     err.high(nominal->diffxs->GetBinLowEdge(bin)+nominal->diffxs->GetBinWidth(bin));
                     
                     errNorm.content(nominal->diffxsNorm->GetBinContent(bin));
                     errNorm.stat(nominal->diffxsNorm->GetBinError(bin));
                     errNorm.low(nominal->diffxsNorm->GetBinLowEdge(bin));
                     errNorm.high(nominal->diffxsNorm->GetBinLowEdge(bin)+nominal->diffxs->GetBinWidth(bin));
                     
                     
                     binErrors.push_back(err);
                     binErrorsNorm.push_back(errNorm);
                     
                  }// end for(bin)
                  
                  /// ACCOUNTING
                  // want to skip this partner when we come to it 
                  // in the loop since we have already plotted it
                  systToSkip.push_back(systPartnerString);
                  // increment counter for this partner since we will skip it
                  // in the next loop
                  // incrementing here, also ensures that if the partner was
                  // the last on the list of plots, that the canvas ps file
                  // gets closed with a ")"
                  can_counter++;
                  
               }
               else
               { // for systematics without a partner
                  
                  //////////////////////////////
                  ///// Need to write systematic diffxs and diffxsNorm to file so that
                  ///// we can use them in the correlation matrices calculation
                  {
                     outFile->cd();
                     std::string histo_name = baseSystematic + "_shape";
                     
                     TH1* diffxs = (TH1*)hg->diffxs->Clone("tmp");
                     diffxs->Scale(1./globals::BR);
                     TH1* diffxsNorm = (TH1*)hg->diffxsNorm->Clone("tmpNorm");
                     
                     // for migration toys the uncertainties are in the errors
                     // so just add them to the bin contents to get the "shifted"
                     // shape
                     if(systSampleEnum == SystematicSample::MIGRATIONTOYS){
                        for(unsigned int bin=1;bin<=diffxs->GetNbinsX();++bin){
                           diffxs->SetBinContent(bin,diffxs->GetBinError(bin)+diffxs->GetBinContent(bin));
                           diffxsNorm->SetBinContent(bin,diffxsNorm->GetBinError(bin)+diffxsNorm->GetBinContent(bin));
                        }
                     }
                     
                     diffxs->Write(histo_name.c_str(),TObject::kOverwrite);
                     delete diffxs; diffxs = 0;
                     
                     histo_name += "_norm";
                     diffxsNorm->Write(histo_name.c_str(),TObject::kOverwrite);
                     delete diffxsNorm; diffxsNorm = 0;
                  }
                  //////////////////////////
                  /////////
                  
                  
                  // calculate the combined systematics for this systematic
                  for(unsigned int bin=1;bin<=nominal->truth->GetNbinsX();++bin){
                     double x1 = nominal->diffxs->GetBinContent(bin)/globals::BR;
                     double x1Norm = nominal->diffxsNorm->GetBinContent(bin);
                     double x2 = hg->diffxs->GetBinContent(bin)/globals::BR;
                     double x2Norm = hg->diffxsNorm->GetBinContent(bin);
                     
                     SystematicBin err,errNorm;
                     
                     if((systSampleEnum == SystematicSample::ALPGENJIMMY)){
                        //x1 = nominal->diffxs->GetBinContent(bin);
                        //x2 = nominal->unfolded->GetBinContent(bin);
                        //double x3 = hg->unfolded->GetBinContent(bin);
                        
                        double average = (x1+x2)/2.;
                        double fractionalError = fabs(x1-x2)/average;
                        
                        double dx = x1 * fractionalError;
                        
                        err.up(fabs(dx));
                        err.down(fabs(dx));
                        
                        average = (x1Norm+x2Norm)/2;
                        fractionalError = fabs(x1Norm-x2Norm)/average;
                        
                        double dxNorm = x1Norm*fractionalError;
                        
                        errNorm.up(dxNorm);
                        errNorm.down(dxNorm);
                        
                     }
                     else {
                        
                        // migration toys are not the say and the errors come from the
                        // errors on the diffxs plot
                        double dx=0,dxNorm=0;
                        double shape_up=0,shape_down=0,shape_up_norm=0,shape_down_norm=0;
                        if(systSampleEnum == SystematicSample::MIGRATIONTOYS){
                           dx = hg->diffxs->GetBinError(bin);
                           shape_up = x1+dx;
                           shape_down = x1-dx;
                           dxNorm = hg->diffxsNorm->GetBinError(bin);
                           shape_up_norm = x1Norm+dxNorm;
                           shape_down_norm = x1Norm-dxNorm;
                        }
                        else{
                           dx = x1-x2;
                           shape_up = x1 + dx;
                           shape_down = x1 - dx;
                           dx = fabs(dx);
                           
                           dxNorm = x1Norm-x2Norm;
                           shape_up_norm = x1Norm + dxNorm;
                           shape_down_norm = x1Norm - dxNorm;
                           dxNorm = fabs(dxNorm);
                        }
                        
                        err.down(dx);
                        err.up(dx);
                        
                        errNorm.up(dxNorm);
                        errNorm.down(dxNorm);
                        
                        // set shape information for later use in tables
                        err.shape_up(shape_up);
                        err.shape_down(shape_down);
                        errNorm.shape_up(shape_up_norm);
                        errNorm.shape_down(shape_down_norm);
                     }
                     
                     // copy nominal data to systematicBin
                     err.content(nominal->diffxs->GetBinContent(bin)/globals::BR);
                     err.stat(nominal->diffxs->GetBinError(bin)/globals::BR);
                     err.low(nominal->diffxs->GetBinLowEdge(bin));
                     err.high(nominal->diffxs->GetBinLowEdge(bin)+nominal->diffxs->GetBinWidth(bin));
                     err.shape_down(0);
                     err.shape_up(0);
                     
                     errNorm.content(nominal->diffxsNorm->GetBinContent(bin));
                     errNorm.stat(nominal->diffxsNorm->GetBinError(bin));
                     errNorm.low(nominal->diffxsNorm->GetBinLowEdge(bin));
                     errNorm.high(nominal->diffxsNorm->GetBinLowEdge(bin)+nominal->diffxsNorm->GetBinWidth(bin));
                     errNorm.shape_down(0);
                     errNorm.shape_up(0);
                     
                     binErrors.push_back(err);
                     binErrorsNorm.push_back(errNorm);
                     
                  }// end for(bin)
                  
               }// end else(systPartner)
               
               // add systematics to list
               systList[baseSystematic] = binErrors;
               normSystList[baseSystematic] = binErrorsNorm;
               
               
               can->Clear();
               can->Divide(2,2);
               
               can->cd(1);
               truth->Draw();
               can->cd(2);
               reco->Draw();
               can->cd(3);
               unfolded->Draw();
               can->cd(4);
               measured->Draw();
               
               if(firstCan){
                  can->SaveAs(std::string(filename_nomPlusSyst + "(").c_str());
                  firstCan = false;
               }
               else
                  can->SaveAs(filename_nomPlusSyst.c_str());
               
               can->Clear();
               
               can->cd();
               diffxs->Draw();
               can->SaveAs(filename_nomPlusSyst.c_str());
               
               //save eps,pdf
               std::stringstream comFile;
               comFile <<channel.GetChannelString()<<"_"<<sampleName;
               if(globals::globalUnfoldingMethod == globals::SVD)
                  comFile <<"svd_reg" << regOrIter << "_toy" << nToys;
               else if(globals::globalUnfoldingMethod == globals::BAYESIAN)
                  comFile <<"bayes_itr" << regOrIter << "_toy" << nToys;
               else if(globals::globalUnfoldingMethod == globals::BINBYBIN)
                  comFile <<"binbybin_toy" << nToys;
               else if(globals::globalUnfoldingMethod == globals::INVERSION)
                  comFile <<"inversion_toy" << nToys;
               comFile <<"_syst_"<<systematic;
               
               const std::string diffxs_epsComFile = globals::GetLatexEpsSplitPath() + "/comb_diffxs_" + comFile.str() + ".eps";
               can->SaveAs(diffxs_epsComFile.c_str());
   
               const std::string diffxs_pdfComFile = globals::GetLatexPdfSplitPath() + "/comb_diffxs_" + comFile.str() + ".pdf";
               can->SaveAs(diffxs_pdfComFile.c_str());
               
               can->Clear();
               can->Divide(1,2);
               
               can->cd(1);
               meanDeviation->Draw();
               can->cd(2);
               rmsDeviation->Draw();
               
               
               if(can_counter == can_max -1){
                  can->SaveAs(std::string(filename_nomPlusSyst + ")").c_str());
               }
               else
                  can->SaveAs(filename_nomPlusSyst.c_str());
               
               can_counter++;
               
               delete hg;
               hg = 0;
               
               delete truth; truth = 0;
               delete reco; reco = 0;
               delete measured; measured = 0;
               delete unfolded; unfolded = 0;
               delete diffxs; diffxs = 0;
               delete meanDeviation; meanDeviation = 0;
               delete rmsDeviation; rmsDeviation = 0;
               
               if(partnerHg){
                  delete partnerHg;partnerHg = 0;
               }
            
            } // for(systematics)
            
            // add LUMINOSITY ERRORS to list of systematics
            {
               
               TH1* nominalHist = nominal->diffxs;
               TH1* nominalNormHist = nominal->diffxsNorm;
               SystematicBin err,errNorm;
               SystematicBinList binErrors;
               SystematicBinList binErrorsNorm;
               
               for(unsigned int bin=1;bin<=nominalHist->GetNbinsX();++bin){
                  
                  // the error is just proportional to the bin content
                  double error = nominalHist->GetBinContent(bin)*globals::LUMI_FRACTIONAL_ERROR;
                  err.up(error/globals::BR);
                  err.down(error/globals::BR);
                  
                  // since we normalize by the Luminosity there is a cancelation
                  // so there is no contribution to the errors in this case.
                  errNorm.up(0.);
                  errNorm.down(0.);
                  
                  
                  // copy nominal data to systematicBin
                  err.content(nominalHist->GetBinContent(bin)/globals::BR);
                  err.stat(nominalHist->GetBinError(bin)/globals::BR);
                  err.low(nominalHist->GetBinLowEdge(bin));
                  err.high(nominalHist->GetBinLowEdge(bin)+nominalHist->GetBinWidth(bin));
                  
                  errNorm.content(nominalNormHist->GetBinContent(bin));
                  errNorm.stat(nominalNormHist->GetBinError(bin));
                  errNorm.low(nominalNormHist->GetBinLowEdge(bin));
                  errNorm.high(nominalNormHist->GetBinLowEdge(bin)+nominalNormHist->GetBinWidth(bin));
                  
                  
                  binErrors.push_back(err);
                  binErrorsNorm.push_back(errNorm);
               }
               
               
               // add systematics to list
               std::string baseSystematic = SystematicSample::GetSampleBaseString(SystematicSample::LUMI);
               systList[baseSystematic] = binErrors;
               normSystList[baseSystematic] = binErrorsNorm;
            }
            
            // add PDF ERRORS to list of systematics
            {
               
               TH1* nominalHist = nominal->diffxs;
               TH1* nominalNormHist = nominal->diffxsNorm;
               
               // retreive PDF values
               PdfSystematic pdf(sampleName,channel);
               
               // if there are not enough PDF systematics then skip this.
               if(pdf.size() >= nominalHist->GetNbinsX()){
                  SystematicBinList binErrors;
                  SystematicBinList binErrorsNorm;
                  
                  // first need to create normalized histograms
                  // with the PDF up and down
                  TH1* hUp = (TH1*)nominalHist->Clone("pdf_up");
                  hUp->SetDirectory(0);
                  TH1* hDown = (TH1*)nominalHist->Clone("pdf_down");
                  hUp->SetDirectory(0);
                  
                  for(unsigned int bin=1;bin<=nominalHist->GetNbinsX();++bin){
                     hUp  ->SetBinContent(bin,hUp  ->GetBinContent(bin)*(1.+pdf[bin-1]));
                     hDown->SetBinContent(bin,hDown->GetBinContent(bin)*(1.-pdf[bin-1]));
                  }
                  
                  TH1* hUpNorm = (TH1*)hUp->Clone("pdf_up_norm");
                  TH1* hDownNorm = (TH1*)hDown->Clone("pdf_down_norm");
                  
                  hUp->Scale(1./globals::BR);
                  hDown->Scale(1./globals::BR);
                  
                  // normalize
                  hUpNorm->Scale(1./hUpNorm->Integral("width"));
                  hDownNorm->Scale(1./hDownNorm->Integral("width"));
                  
                  for(unsigned int bin=1;bin<=nominalHist->GetNbinsX();++bin){
                     SystematicBin err,errNorm;
                     
                     // the error is just proportional to the bin content
                     double error = 0.;
                     if(bin-1 < pdf.size())
                        error = nominalHist->GetBinContent(bin)*pdf[bin-1];
                     err.up(error/globals::BR);
                     err.down(error/globals::BR);
                     
                     double errorNorm = hUpNorm->GetBinContent(bin) - nominalNormHist->GetBinContent(bin);
                     // assign based on sign
                     if(errorNorm > 0)
                        errNorm.up(errorNorm);
                     else
                        errNorm.down(fabs(errorNorm));
                     errorNorm = hDownNorm->GetBinContent(bin) - nominalNormHist->GetBinContent(bin);
                     // assign based on sign
                     if(errorNorm > 0){
                        // if up has already been set, choose bigger one
                        if(errNorm.up() < errorNorm)
                           errNorm.up(errorNorm);
                     }
                     else{
                        errorNorm = fabs(errorNorm);
                        // if up has already been set, choose bigger one
                        if(errNorm.down() < errorNorm)
                           errNorm.down(errorNorm);
                     }
                     // copy nominal data to systematicBin
                     err.content(nominalHist->GetBinContent(bin)/globals::BR);
                     err.stat(nominalHist->GetBinError(bin)/globals::BR);
                     err.low(nominalHist->GetBinLowEdge(bin));
                     err.high(nominalHist->GetBinLowEdge(bin)+nominalHist->GetBinWidth(bin));
                     
                     errNorm.content(nominalNormHist->GetBinContent(bin));
                     errNorm.stat(nominalNormHist->GetBinError(bin));
                     errNorm.low(nominalNormHist->GetBinLowEdge(bin));
                     errNorm.high(nominalNormHist->GetBinLowEdge(bin)+nominalNormHist->GetBinWidth(bin));
                     
                     binErrors.push_back(err);
                     binErrorsNorm.push_back(errNorm);
                     
                     
                     
                  }
                  
                  //////////////////////////////
                  ///// Need to write systematic diffxs and diffxsNorm to file so that
                  ///// we can use them in the correlation matrices calculation
                  {
                     outFile->cd();
                     std::string histo_name = "pdf_shape";
                     
                     hUp->Write(histo_name.c_str(),TObject::kOverwrite);
                     
                     histo_name += "_norm";
                     hUpNorm->Write(histo_name.c_str(),TObject::kOverwrite);
                  }
                  //////////////////////////
                  /////////
                  
                  
                  // add systematics to list
                  std::string baseSystematic = SystematicSample::GetSampleBaseString(SystematicSample::PDF);
                  systList[baseSystematic] = binErrors;
                  normSystList[baseSystematic] = binErrorsNorm;
                  
                  delete hUp; hUp = 0;
                  delete hDown; hDown = 0;
               }
            }
            
            outFile->Close();
            delete outFile;
            outFile = 0;
            
            // build output file name
            std::string filename = sampleName + "_";
            filename += channel.GetChannelString();
            
            std::cout << "[combineSystematics] Taking the MC closure " << std::endl;
            
            
            std::cout << "[combineSystematics] Combine channels " << std::endl;
            output_combineChannels(systList,nominal->diffxs,filename);
            output_combineChannels(normSystList,nominal->diffxsNorm,filename+"_norm");
            
            output_latex_table(systList,normSystList,nominal,channel,
                              sampleName,pValues,pValuesNorm,
                              filename,tagged,taggedLatex,
                              topSample,globals::GetLatexPath());
            
         }// for(channels)
         delete kinVar;
         kinVar = 0;
      }// for(sampleNames)
   }
   
   
   
   // output pvalue tables
   std::string pvalue_label   = "tab:pvalues";
   std::string pvalue_caption = "Probability that the $\\chi^{2}$ between the prediction and data is less than that which was calculated. The first column lists the kinematic variable for which the values are calculated, the second column lists the top pair decay channel for with the values are calculated, and the remaining columns list the calculated probablities for the data and the listed prediction. These values are for the absolute spectra.";
   PValueTable pvaluesTable(pValues,pvalue_label,pvalue_caption);
   std::string pvalue_filename = globals::GetLatexPath();
   pvalue_filename += "/pvaluesTable.tex";
   pvaluesTable.WriteTableToFile(pvalue_filename);
   
   
   std::string pvalueNorm_label   = "tab:pvalues_norm";
   std::string pvalueNorm_caption = "Probability that the $\\chi^{2}$ between the prediction and data is less than that which was calculated. The first column lists the kinematic variable for which the values are calculated, the second column lists the top pair decay channel for with the values are calculated, and the remaining columns list the calculated probablities for the data and the listed prediction. These values are for the normalized spectra.";
   PValueTable pvaluesNormTable(pValuesNorm,pvalueNorm_label,pvalueNorm_caption);
   std::string pvalueNorm_filename = globals::GetLatexPath();
   pvalueNorm_filename += "/pvaluesTableNorm.tex";
   pvaluesNormTable.WriteTableToFile(pvalueNorm_filename);
   
   
   // compile latex
   std::string makeCmd = "make -C ";
   makeCmd += globals::GetLatexPath();
   makeCmd += " -f Makefile_channels";
   system(makeCmd.c_str());
   makeCmd += "_lhood";
   system(makeCmd.c_str());
   
   
   delete histoList;
   histoList = 0;
   
   delete can;
   can = 0;
   
   std::cout << "[combineSystematics] Done.\n";
   
   return 0;
}


//Function to store information about systematics into a text file. 
//Then, do the combination in-between channels with: ./bin/combineChannels
void output_combineChannels(SystematicList& errors,TH1* nominal,std::string filename){
   
   
   std::string path = globals::GetCombinationTextFilePath();
   
   if(!globals::CreatePath(path)){
      std::cerr << "[output_combineChannels] ERROR path could not be created: " << path << "\n";
      return;
   }
   
   std::string texCombine = path + "/" + filename + ".tex";
   std::string texCombineUp = path + "/" + filename + "_up.tex";
   std::string texCombineDown = path + "/" + filename + "_down.tex";
   
   std::ofstream fileCombine(texCombine.c_str());
   if(fileCombine.fail()){
      std::cerr << "[output_combineChannels] ERROR failed to open file for writing: " << texCombine << "\n";
      return;
   }
   
   std::ofstream fileCombineUp(texCombineUp.c_str());
   if(fileCombineUp.fail()){
      std::cerr << "[output_combineChannels] ERROR failed to open file for writing: " << texCombineUp << "\n";
      return;
   }
   
   std::ofstream fileCombineDown(texCombineDown.c_str());
   if(fileCombineDown.fail()){
      std::cerr << "[output_combineChannels] ERROR failed to open file for writing: " << texCombineDown << "\n";
      return;
   }
   
   const unsigned int numOfSyst = errors.size();
   const unsigned int numOfBins = nominal->GetNbinsX();
   
   SystematicList::iterator itr;
   
   for(itr = errors.begin();itr!=errors.end();++itr){
      std::string systematic=itr->first;
      SystematicBinList& binErrors = itr->second;
      
      // skip luminosity if doing normalized output
//       if(systematic.find(SystematicSample::GetSampleBaseString(SystematicSample::LUMI)) != std::string::npos && filename.find("_norm") != std::string::npos){
//          continue;
//       }
      
      for(unsigned int bin=1;bin<=numOfBins;++bin){
         double up = binErrors[bin-1].up();
         double down = binErrors[bin-1].down();
         double binContents = binErrors[bin-1].content();
         double binLeft = binErrors[bin-1].low();
         double binRight = binErrors[bin-1].high();
         double binStat = binErrors[bin-1].stat();
         
         fileCombine<<systematic<<"   "<<bin<<"   "<<binLeft<<"   "<<binRight<<"   "<<binContents<<"   "<<binStat<<"   "<<up<<"   "<<down<<"\n";
         
         fileCombineUp<<systematic<<"   "<<bin<<"   "<<binLeft<<"   "<<binRight<<"   "<<binContents<<"   "<<binStat<<"   "<<up<<"\n";
         
         fileCombineDown<<systematic<<"   "<<bin<<"   "<<binLeft<<"   "<<binRight<<"   "<<binContents<<"   "<<binStat<<"   "<<down<<"\n";
      }
   }
   
   fileCombine.close();
   fileCombineUp.close();
   fileCombineDown.close();
   globals::set_chmod(texCombine);
   globals::set_chmod(texCombineUp);
   globals::set_chmod(texCombineDown);
}

//Function to write in a latex tex file the main 
//and systematic tables and also the final plots
void output_latex_table(SystematicList& errors, SystematicList& errorsNorm,
                        SystematicHistoGroup* nominalHistos,
                        const TopDecayChannel& channel,std::string sampleName,
                        PValueList& pValues,
                        PValueList& pValuesNorm,
                        std::string filename,std::string tagged,
                        std::string taggedLatex,std::string topSample, 
                        std::string outputPath){
   
   TH1* nominal     = nominalHistos->diffxs;
   TH1* nominalNorm = nominalHistos->diffxsNorm;
   TH1* truth       = nominalHistos->truth_diffxs;
   std::string name = truth->GetName();
   name += "ScaledByBR";
   TH1* truthScaledByBR = (TH1*)truth->Clone(name.c_str());
   
   const unsigned int numOfSyst = errors.size();
   const unsigned int numOfBins = nominal->GetNbinsX();
   
   //define histogram and asymmetric graph for differential cross-section
   name = nominal->GetName();
   name += "WithSystematics";
   TH1* nominalScaledByBR = (TH1*)nominal->Clone();
   nominalScaledByBR->Scale(1./globals::BR);
   TGraphAsymmErrors nominalWithSystematics(nominalScaledByBR);
   nominalWithSystematics.SetName(name.c_str());
   
   
   //define histogram and asymetric graph for normalized differential cross-section
   std::string nameNorm = nominalNorm->GetName();
   nameNorm += "WithSystematics";
   TGraphAsymmErrors nominalNormWithSystematics(nominalNorm);
   nominalNormWithSystematics.SetName(nameNorm.c_str());
   
   //define tex file names
   if(!globals::CreatePath(outputPath)){
      std::cerr << "[output_latex_table] ERROR could not create path: " << outputPath << "\n";
      return;
   }
   std::string texFilename                   = outputPath + "/" + filename + ".tex";
   std::string texFilenameNorm               = outputPath + "/" + filename + "Norm.tex";
      
   std::string description = tagged;
   std::string texDescription = taggedLatex;
   if(topSample.size() > 0){
      texDescription += ", " + topSample;
      description    += ", " + topSample;
   }
   texDescription += ", " + channel.GetChannelStringLatex();
   description    += ", " + channel.GetChannelStringROOT();
   
   
   
   // get combined systematic errors for the absolute and normalized spectra
   SystematicBinList* totalSystPerBin = errors.GetCombinedBins();
   SystematicBinList* totalSystPerBinNorm = errorsNorm.GetCombinedBins();
   
   // loop over the combined uncertainties and create the nominal 
   // graph with the combined asymmetric errors
   for (unsigned int bin = 1; bin<=numOfBins; ++bin) {
      
      double binLeft = nominal->GetBinLowEdge(bin);
      double binRight = nominal->GetBinLowEdge(bin) + nominal->GetBinWidth(bin);
      double binSystUp=0., binSystDown=0.,binSystUpNorm=0.,binSystDownNorm=0.;
      
      // add systematics to asymmetric plot 
      //for normalized and non normalized differential cross-section
      nominalWithSystematics.SetPointEYhigh(bin-1,(nominal->GetBinError(bin)+(*totalSystPerBin)[bin-1].up()));
      nominalWithSystematics.SetPointEYlow(bin-1,(nominal->GetBinError(bin)+(*totalSystPerBin)[bin-1].down()));
      
      nominalNormWithSystematics.SetPointEYhigh(bin-1,nominalNorm->GetBinError(bin)+(*totalSystPerBinNorm)[bin-1].up());
      nominalNormWithSystematics.SetPointEYlow(bin-1,nominalNorm->GetBinError(bin)+(*totalSystPerBinNorm)[bin-1].down());
      
   }
   
   
   // write systematics to file.
   WritePlotsToFile(sampleName,channel,
                    errors,errorsNorm,
                    nominalScaledByBR,
                    nominalNorm,
                    &nominalWithSystematics,
                    &nominalNormWithSystematics,
                    truthScaledByBR,
                    nominalHistos->measured);
   
   
   // get generator
   GeneratorLabel genLabel(globals::outputFilesPath);
   std::string generator = genLabel.GetLabel();
   
   //define labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string kinVarLabel  = kinVar->GetLatexKinVarLabel();
   std::string UkinVarLabel = kinVar->GetLatexUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetLatexDKinVarLabel();
   std::string NkinVarLabel = kinVar->GetLatexNKinVarLabel();
   
   //Save final tables and results in latex file
   std::cout << "Writing tex file: " << texFilename << "\n";
   LatexDoc* latexDoc = LatexDoc::GetTopDiffInstance();
   
   // combined plot with absolute errors
   std::string label = "table:CombSyst_"+filename;
   std::string capChannel = channel.GetChannelStringROOT();
   CombSystTable combSystTable(errors,label,nominalScaledByBR,sampleName,false);
   std::string caption = "Differential top pair production cross-section as a function of the " + topSample + " $" + kinVarLabel + "$ in the "+channel.GetChannelStringLatex()+" channel with absolute statistical and total systematic errors for each bin. Signal model: \\" + generator + ".";
   //mylatexF << combSystTable.GetTableText(caption,filename);
   
   //mylatexF << "\n";
   
   // combined plot with relative errors
   label = "table:CombSyst_"+filename+"_withRelativeErrors";
   combSystTable.SetLabel(label);
   combSystTable.isRelative(true);
   caption = "Differential top pair production cross-section as a function of the " + topSample + " $" + kinVarLabel + "$ in the "+channel.GetChannelStringLatex()+" channel with percent statistical and total systematic errors for each bin. Signal model: \\" + generator + ".";
   latexDoc->AddBodyText(combSystTable.GetTableText(caption,filename));
   latexDoc->AddBodyText("\n");
   
   
   // ds/dpt systematic table (commented out since not using this in the INT note)
//    label = "table:Syst_"+filename;
   SystTable systTable(errors,label,nominalScaledByBR,sampleName,false,false);
//    caption = "Contribution of each systematic error to the total " +DkinVarLabel+ " spectrum for: ";
//    caption += texDescription;
//    mylatexF << systTable.GetTableText(caption);
//   
//    mylatexF << "\n";
   
   // ds/dpt systematic table (percent)
   label = "table:Syst_rel_"+filename+"_per";
   systTable.SetLabel(label);
   systTable.isNormalized(false);
   systTable.isRelative(true);
   caption = "Contribution of each systematic uncertainty to the total ${\\rm d}\\sigma/{\\rm d}" + kinVarLabel + "$ spectrum as percentage of the cross-section in each bin for ";
   if(topSample.size() > 0)
      caption += topSample;
   caption += ", " + channel.GetChannelStringLatex() + " channel.";
   latexDoc->AddBodyText(systTable.GetTableText(caption));
   latexDoc->AddBodyText("\n");
    
   plot_systematics(errors,nominalScaledByBR,filename,sampleName,description,texDescription,latexDoc,false);
   
   
   
   plot_XS(nominalWithSystematics,channel,sampleName,
           filename,description,texDescription,latexDoc,pValues,false,true);
//    PValueList pValuesLog;
//    plot_XS(nominalWithSystematics,channel,sampleName,
//            filename,description,texDescription,mylatexF,pValuesLog,false,true);
   
//    plot_XS_withClosure(nominalWithSystematics,channel,sampleName,unfoldedReco,
//            filename,description,texDescription,mylatexF,false,false);
//    plot_XS_withClosure(nominalWithSystematics,channel,sampleName,unfoldedReco,
//            filename,description,texDescription,mylatexF,false,true);
   
   ;
   plot_XS_withGenerators(nominalWithSystematics,channel,sampleName,
                          filename,description,texDescription,
                          latexDoc,false,true);
//    plot_XS_withGenerators(nominalWithSystematics,channel,sampleName,
//                           filename,description,texDescription,
//                           mylatexF,false,true);
   
   plot_XS_withTheory(nominalWithSystematics,filename,sampleName,description,
                      texDescription,latexDoc,false,true);
//    plot_XS_withTheory(nominalWithSystematics,filename,sampleName,description,
//                       texDescription,mylatexF,false,true);
   
   latexDoc->AddBodyText("\n");
   
   //We do the same thing with the normalized sample 
   label = "table:CombSyst_"+filename+"_Norm";
   CombSystTable combSystTableNorm(errorsNorm,label,nominalNorm,sampleName,true);
   caption = "Normalized differential top pair production cross-section as a function of the " + topSample + " $" + kinVarLabel + "$ in the "+channel.GetChannelStringLatex()+" channel with absolute statistical and total systematic errors for each bin. Signal model: \\" + generator + ".";
   //mylatexF << combSystTableNorm.GetTableText(caption,filename); 
   
   //mylatexF<<"\n";
   
   // with relative errors
   label = "table:CombSyst_"+filename+"_Norm_withRelativeErrors";
   combSystTableNorm.isRelative(true);
   combSystTableNorm.SetLabel(label);
   caption = "Normalized differential top pair production cross-section as a function of the " + topSample + " $" + kinVarLabel + "$ in the "+channel.GetChannelStringLatex()+" channel with percent statistical and total systematic errors for each bin. Signal model: \\" + generator + ".";
   latexDoc->AddBodyText(combSystTableNorm.GetTableText(caption,filename));
   
   latexDoc->AddBodyText("\n");
   
   // normalized ds/dpt systematic table (commented out since not using in INT note)
//    label = "table:Syst_"+filename+"_Norm";
   SystTable systTableNorm(errorsNorm,label,nominalNorm,sampleName,true,false);
//    caption = "Contribution of each systematic error to the total " + NkinVarLabel + " spectrum for: ";
//    caption += texDescription;
//    mylatexF << systTableNorm.GetTableText(caption);
   
   // normalized ds/dpt systematic table (percent)
   label = "table:Syst_rel_"+filename+"_per_Norm";
   systTableNorm.SetLabel(label);
   systTableNorm.isNormalized(true);
   systTableNorm.isRelative(true);
   caption = "Contribution of each systematic uncertainty to the total $(1/\\sigma)\\cdot d\\sigma/d" + kinVarLabel + "$ spectrum as percentage of the normalized cross-section in each bin for ";
   if(topSample.size() > 0)
      caption += topSample;
   caption += ", " + channel.GetChannelStringLatex() + " channel.";
   latexDoc->AddBodyText(systTableNorm.GetTableText(caption));
   latexDoc->AddBodyText("\n");
   
   std::string filenameNorm = filename + "Norm";
   plot_systematics(errorsNorm,nominalNorm,filenameNorm,sampleName,description,texDescription,latexDoc,true);
   plot_XS(nominalNormWithSystematics,channel,sampleName,
           filenameNorm,description,texDescription,
           latexDoc,pValuesNorm,true,true);
//    PValueList pValuesNormLog;
//    plot_XS(nominalNormWithSystematics,channel,sampleName,
//            filenameNorm,description,texDescription,
//            mylatexF,pValuesNormLog,true,true);
   
//    plot_XS_withClosure(nominalNormWithSystematics,channel,sampleName,unfoldedRecoNorm,
//            filenameNorm,description,texDescription,
//            mylatexF,true,false);
//    plot_XS_withClosure(nominalNormWithSystematics,channel,sampleName,unfoldedRecoNorm,
//            filenameNorm,description,texDescription,
//            mylatexF,true,true);
   
   plot_XS_withGenerators(nominalNormWithSystematics,channel,sampleName,
                          filenameNorm,description,texDescription,
                          latexDoc,true,true);
//    plot_XS_withGenerators(nominalNormWithSystematics,channel,sampleName,
//                           filenameNorm,description,texDescription,
//                           mylatexF,true,true);
   
   plot_XS_withTheory(nominalNormWithSystematics,filenameNorm,sampleName,description,
                      texDescription,latexDoc,true,true);
//    plot_XS_withTheory(nominalNormWithSystematics,filenameNorm,sampleName,description,
//                       texDescription,mylatexF,true,true);
   
   latexDoc->AddBodyText("\n");
   latexDoc->WriteToFile(texFilename);
   globals::set_chmod(texFilename);
   
   delete kinVar;
   kinVar = 0;
   
   delete nominalScaledByBR;
   nominalScaledByBR = 0;
   
   
}



//function to plot the differential and normalized spectrum with and without log scale 
void plot_XS(TGraphAsymmErrors nominalSyst,const TopDecayChannel& channel,
             std::string sampleName,std::string filename,std::string description,
             std::string texDescription,LatexDoc* const latexDoc,
             PValueList& pValues,
             bool norm,bool log){
   
   std::string pdfFilename       = globals::GetLatexPdfPath() + "/" + filename + ".pdf";
   std::string epsFilename       = globals::GetLatexEpsPath() + "/" + filename + ".eps";
   std::string pdfFilename_log   = globals::GetLatexPdfPath() + "/" + filename + "_log.pdf";
   std::string epsFilename_log   = globals::GetLatexEpsPath() + "/" + filename + "_log.eps";
   std::string labelname = "figure:XS_"+filename;
   std::string labelname_Norm = labelname +"_Norm";
   
   // Get Kidonakis NNLO
   TGraphAsymmErrors* NNLO = 0;
   TH1* hNNLO = 0;
   if(sampleName.find("Top1") != std::string::npos || sampleName.find("Top2") != std::string::npos){
      KidonakisNNLO kNNLO;
      hNNLO = kNNLO.GetHisto();
      NNLO = kNNLO.GetGraph();
      if(norm){
         hNNLO = kNNLO.GetHistoNorm();
         NNLO = kNNLO.GetGraphNorm();
      }
   }
   // Get Yang-Pecjak NNLO for SystemMass
   else if(sampleName.find("SystemMass") != std::string::npos){
      YangPecjakNNLO yNNLO;
      NNLO = (TGraphAsymmErrors*)yNNLO.GetGraph()->Clone();
      hNNLO = (TH1*)yNNLO.GetHisto()->Clone();
      if(norm){
         NNLO = (TGraphAsymmErrors*)yNNLO.GetGraphNorm()->Clone();
         hNNLO = (TH1*)yNNLO.GetHistoNorm()->Clone();
      }
   }
   
   // Get MCFM
//    MCFMHisto mcfm(sampleName);
   MCFMWithPDF mcfm(MCFMWithPDF::CT10NLO,sampleName);
   TGraphAsymmErrors* gMcfm = mcfm.GetGraph();
   if(norm){
      gMcfm = mcfm.GetGraphNorm();
   }
   TH1* hMcfm = mcfm.GetStyleHisto();
   
   // get generator
   GeneratorLabel genLabel(globals::outputFilesPath);
   std::string generator = genLabel.GetLabel();
   
   // Get MC@NLO
   McAtNloBeforeCuts MCatNLO(channel,sampleName);
   if(genLabel.GetGenerator() == GeneratorLabel::MCATNLO)
      MCatNLO.SetMainGenerator(true);
   
   TH1* mcAtNlo= MCatNLO.GetHisto();
   if(norm) mcAtNlo = MCatNLO.GetHistoNorm();
   
   
   // Get ALPGEN
   AlpgenMC alpgen(channel.GetChannelString(),sampleName);
   if(genLabel.GetGenerator() == GeneratorLabel::ALPGEN)
      alpgen.SetMainGenerator(true);
   
   TH1* hAlpgen = alpgen.GetHisto();
   if(norm) hAlpgen = alpgen.GetHistoNorm();
   
   // Get POWHEG
   PowHegMC powheg(sampleName);
   if(genLabel.GetGenerator() == GeneratorLabel::POWHEG)
      powheg.SetMainGenerator(true);
   
   TH1* hPowheg = powheg.GetHisto();
   if(norm) hPowheg = powheg.GetHistoNorm();
   
//    std::cerr << "\n";
//    for(unsigned int bin = 1;bin<=mcAtNlo->GetNbinsX();++bin){
//       std::cerr << "bin: " << bin;
//       std::cerr << ", mc@nlo: " << mcAtNlo->GetBinContent(bin) << " +/- " << mcAtNlo->GetBinError(bin);
//       std::cerr << ", alpgen: " << hAlpgen->GetBinContent(bin) << " +/- " << hAlpgen->GetBinError(bin);
//       std::cerr << ", powheg: " << hPowheg->GetBinContent(bin) << " +/- " << hPowheg->GetBinError(bin);
//       std::cerr << "\n";
//    }
//    std::cerr << "\n";
   
   //define labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string kinVarLabel  = kinVar->GetKinVarLabel();
   std::string latexkinVarLabel  = kinVar->GetLatexKinVarLabel();
   std::string latexNkinVarLabel  = kinVar->GetNKinVarLabel();
   std::string UkinVarLabel = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   std::string NkinVarLabel = kinVar->GetNKinVarLabel(); 
   std::stringstream ntitle;
   
   // set color/style for main data
   nominalSyst.SetMarkerSize(1.);
   nominalSyst.SetMarkerColor(kBlack);
   nominalSyst.SetLineColor(kBlack);
   nominalSyst.SetMarkerStyle(20);
   
   //needed to enlarge the error bars 
   gStyle->SetEndErrorSize(3);
   
   
   // create a plot with ratio below
   Th1PlotAndRatio::RATIO_ORDER ratioOrder = Th1PlotAndRatio::NUM_RATIO_ORDER;
   if(globals::DATA_ON_TOP)
      ratioOrder = Th1PlotAndRatio::DATA_AS_NUMERATOR;
   else
      ratioOrder = Th1PlotAndRatio::DATA_AS_DENOMINATOR;
   Th1PlotAndRatio plotAndRatio(description,ratioOrder);
   
   if(sampleName.find("SystemRapidity") != std::string::npos){
      if(norm)
         plotAndRatio.SetYAxisMax(10);
      else
         plotAndRatio.SetYAxisMax(150);
   }
   
   // set axis labels
   if(norm)
      plotAndRatio.SetYAxisTitleTop(latexNkinVarLabel);
   else
      plotAndRatio.SetYAxisTitleTop(DkinVarLabel);
   
   plotAndRatio.SetXAxisTitle(UkinVarLabel);
   if(globals::DATA_ON_TOP){
      plotAndRatio.SetYAxisTitleRatio("Data/MC");
      plotAndRatio.SetCombineGeneratorRatios(false);
   }
   else{
      plotAndRatio.SetYAxisTitleRatio("MC/Data");
      plotAndRatio.SetCombineGeneratorRatios(true);
   }
   
   // add nominal data as reference plot
   std::stringstream label;
   label << "Data";
   label.precision(4);
   if(!norm) label << " #int=" << GetIntegral(&nominalSyst) << "pb";
   nominalSyst.SetFillStyle(3244);
   nominalSyst.SetFillColor(kGray+1);
   plotAndRatio.SetData(&nominalSyst,label.str().c_str());
   
   // add remaining plots
   
   if(hAlpgen){
      label.str("");
      label.precision(4);
      label << "Alpgen"; 
      if(!norm) label << " #int=" << hAlpgen->Integral("width") << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/Alpgen";
      else
         ratio_axis_label = "Alpgen/Data";
      plotAndRatio.AddGenerator(hAlpgen,label.str().c_str(),ratio_axis_label,true,"l");
      
      pValues.CalculateAndFillPValueList(sampleName,channel,"Alpgen",&nominalSyst,hAlpgen,norm);
   }
   if(mcAtNlo){
      label.str("");
      label.precision(4);
      label << "MC@NLO"; 
      if(!norm) label << " #int=" << mcAtNlo->Integral("width") << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/MC@NLO";
      else
         ratio_axis_label = "MC@NLO/Data";
      plotAndRatio.AddGenerator(mcAtNlo,label.str().c_str(),ratio_axis_label,true,"l");
      
      pValues.CalculateAndFillPValueList(sampleName,channel,"McAtNlo",&nominalSyst,mcAtNlo,norm);
   }
   if(hPowheg){
      label.str("");
      label.precision(4);
      label << "PowHeg"; 
      if(!norm) label << " #int=" << hPowheg->Integral("width") << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/PowHeg";
      else
         ratio_axis_label = "PowHeg/Data";
      plotAndRatio.AddGenerator(hPowheg,label.str().c_str(),ratio_axis_label,true,"l");
      
      pValues.CalculateAndFillPValueList(sampleName,channel,"PowHeg",&nominalSyst,hPowheg,norm);
   }
   if(NNLO && hNNLO){
      label.str("");
      label.precision(4);
      label << "Aprox. NNLO"; 
      if(!norm) label << " #int=" << hNNLO->Integral("width") << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/NNLO";
      else
         ratio_axis_label = "NNLO/Data";
      plotAndRatio.AddTheory(NNLO,label.str().c_str(),ratio_axis_label,hNNLO,true,"f");
      
      pValues.CalculateAndFillPValueList(sampleName,channel,"NNLO",&nominalSyst,NNLO,norm);
   }
   if(gMcfm && hMcfm){
      label.str("");
      label.precision(4);
      label << "MCFM (NLO)"; 
      if(!norm) label << " #int=" << hMcfm->Integral("width") << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/MCFM";
      else
         ratio_axis_label = "MCFM/Data";
      plotAndRatio.AddTheory(gMcfm,label.str(),ratio_axis_label,hMcfm,true,"f");
      
      pValues.CalculateAndFillPValueList(sampleName,channel,"MCFM",&nominalSyst,gMcfm,norm);
   }
   
   // Save plots to files and add figure to latex file
   
   plotAndRatio.SaveAs(pdfFilename);
   plotAndRatio.SaveAs(epsFilename);
   
   std::stringstream mylatexF;
   mylatexF<<"\\begin{figure} \n";
   mylatexF<<"\\centering \n";
   mylatexF<<"\\includegraphics[width=0.8\\textwidth]{" << filename << "} \n";
   if(norm){
      mylatexF<<"\\caption{Normalized top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
      mylatexF<<"\\label{"<<labelname_Norm<<"} \n";
   }
   else{
      mylatexF<<"\\caption{Top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
      mylatexF<<"\\label{"<<labelname<<"} \n";
   }
   mylatexF<<"\\end{figure} \n";
   mylatexF<<"\n";
   
   // if log plots are requested, add them too
   
   // save plots to files
   if(log){
      plotAndRatio.SetMainPadLogy(true);
      plotAndRatio.SaveAs(pdfFilename_log);
      plotAndRatio.SaveAs(epsFilename_log);
      
      mylatexF<<"\\begin{figure} \n";
      mylatexF<<"\\centering \n";
      mylatexF<<"\\includegraphics[width=0.8\\textwidth]{" << filename << "_log} \n";
      if(norm){
         mylatexF<<"\\caption{Normalized top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
         mylatexF<<"\\label{"<<labelname_Norm<<"} \n";
      }
      else{
         mylatexF<<"\\caption{Top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
         mylatexF<<"\\label{"<<labelname<<"_log} \n";
      }
      mylatexF<<"\\end{figure} \n";
      mylatexF<<"\n";
   }
   
   latexDoc->AddBodyText(mylatexF.str());
   
   if(NNLO){
      delete NNLO;
      NNLO = 0;
   }
   if(kinVar){
      delete kinVar;
      kinVar = 0;
   }
}



//function to plot the differential and normalized spectrum with and without log scale 
void plot_XS_withClosure(TGraphAsymmErrors nominalSyst,
                         const TopDecayChannel& channel,
                         std::string sampleName,
                         TH1* unfoldedReco,
                         std::string filename,
                         std::string description,
                         std::string texDescription,
                         LatexDoc* const latexDoc,
                         bool norm,bool log){
   
   filename += "_closure";
   std::string pdfFilename       = globals::GetLatexPdfPath() + "/" + filename + ".pdf";
   std::string epsFilename       = globals::GetLatexEpsPath() + "/" + filename + ".eps";
   std::string pdfFilename_log   = globals::GetLatexPdfPath() + "/" + filename + "_log.pdf";
   std::string epsFilename_log   = globals::GetLatexEpsPath() + "/" + filename + "_log.eps";
   std::string labelname = "figure:XS_"+filename;
   std::string labelname_Norm = labelname +"_Norm";
   
   
   //define labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string kinVarLabel  = kinVar->GetKinVarLabel();
   std::string latexkinVarLabel  = kinVar->GetLatexKinVarLabel();
   std::string latexNkinVarLabel  = kinVar->GetNKinVarLabel();
   std::string UkinVarLabel = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   std::string NkinVarLabel = kinVar->GetNKinVarLabel(); 
   std::stringstream ntitle;
   
   // get generator
   GeneratorLabel genLabel(globals::outputFilesPath);
   std::string generator = genLabel.GetLabel();
   
   // Get MC@NLO
   McAtNloBeforeCuts MCatNLO(channel,sampleName);
   if(genLabel.GetGenerator() == GeneratorLabel::MCATNLO)
      MCatNLO.SetMainGenerator(true);
   
   TH1* mcAtNlo= MCatNLO.GetHisto();
   if(norm) mcAtNlo = MCatNLO.GetHistoNorm();
   
   
   // Get ALPGEN
   AlpgenMC alpgen(channel.GetChannelString(),sampleName);
   if(genLabel.GetGenerator() == GeneratorLabel::ALPGEN)
      alpgen.SetMainGenerator(true);
   
   TH1* hAlpgen = alpgen.GetHisto();
   if(norm) hAlpgen = alpgen.GetHistoNorm();
   
   
   // create a plot with ratio below
   Th1PlotAndRatio::RATIO_ORDER ratioOrder = Th1PlotAndRatio::NUM_RATIO_ORDER;
   if(globals::DATA_ON_TOP)
      ratioOrder = Th1PlotAndRatio::DATA_AS_NUMERATOR;
   else
      ratioOrder = Th1PlotAndRatio::DATA_AS_DENOMINATOR;
   Th1PlotAndRatio plotAndRatio(description,ratioOrder);
   
   if(sampleName.find("SystemRapidity") != std::string::npos){
      if(norm)
         plotAndRatio.SetYAxisMax(10);
      else
         plotAndRatio.SetYAxisMax(150);
   }
   
   // set axis labels
   if(norm)
      plotAndRatio.SetYAxisTitleTop(latexNkinVarLabel);
   else
      plotAndRatio.SetYAxisTitleTop(DkinVarLabel);
   
   plotAndRatio.SetXAxisTitle(UkinVarLabel);
   if(globals::DATA_ON_TOP){
      plotAndRatio.SetYAxisTitleRatio("Data/MC");
      plotAndRatio.SetCombineGeneratorRatios(false);
   }
   else{
      plotAndRatio.SetYAxisTitleRatio("MC/Data");
      plotAndRatio.SetCombineGeneratorRatios(true);
   }
   
   
   // set color/style for main data
   nominalSyst.SetMarkerSize(1.);
   nominalSyst.SetMarkerColor(kBlack);
   nominalSyst.SetLineColor(kBlack);
   nominalSyst.SetMarkerStyle(20);
   nominalSyst.SetFillStyle(3244);
   nominalSyst.SetFillColor(kGray+1);
   
   // add nominal data as reference plot
   std::stringstream label;
   label << "Data"; 
   if(!norm) label << " #int=" << GetIntegral(&nominalSyst) << "pb";
   plotAndRatio.SetData(&nominalSyst,label.str().c_str());
   
   if(mcAtNlo){
      label.str("");
      label << "MC@NLO"; 
      if(!norm) label << " #int=" << mcAtNlo->Integral("width") << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/MC@NLO";
      else
         ratio_axis_label = "MC@NLO/Data";
      plotAndRatio.AddGenerator(mcAtNlo,label.str().c_str(),ratio_axis_label,true,"l");
   }
   if(hAlpgen){
      label.str("");
      label << "Alpgen"; 
      if(!norm) label << " #int=" << hAlpgen->Integral("width") << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/Alpgen";
      else
         ratio_axis_label = "Alpgen/Data";
      plotAndRatio.AddGenerator(hAlpgen,label.str().c_str(),ratio_axis_label,true,"l");
   }
   // setup and draw the closure
   if(unfoldedReco){
      label.str("");
      label << "MC Closure"; 
      if(!norm) label << " #int=" << unfoldedReco->Integral("width") << "pb";
      unfoldedReco->SetFillColor(kRed);
      plotAndRatio.AddTheory(unfoldedReco,label.str().c_str(),"",false);
   }
   
   // save plots to files
   
   plotAndRatio.SaveAs(pdfFilename);
   plotAndRatio.SaveAs(epsFilename);
   
   std::stringstream mylatexF;
   mylatexF<<"\\begin{figure} \n";
   mylatexF<<"\\centering \n";
   mylatexF<<"\\includegraphics[width=0.8\\textwidth]{" << filename << "} \n";
   if(norm){
      mylatexF<<"\\caption{Normalized top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
      mylatexF<<"\\label{"<<labelname_Norm<<"} \n";
   }
   else{
      mylatexF<<"\\caption{Top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
      mylatexF<<"\\label{"<<labelname<<"} \n";
   }
   mylatexF<<"\\end{figure} \n";
   mylatexF<<"\n";
   
   // if log plots are requested, add them too
   
   // save plots to files
   if(log){
      plotAndRatio.SetMainPadLogy(true);
      plotAndRatio.SaveAs(pdfFilename_log);
      plotAndRatio.SaveAs(epsFilename_log);
      
      mylatexF<<"\\begin{figure} \n";
      mylatexF<<"\\centering \n";
      mylatexF<<"\\includegraphics[width=0.8\\textwidth]{" << filename << "_log} \n";
      if(norm){
         mylatexF<<"\\caption{Normalized top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
         mylatexF<<"\\label{"<<labelname_Norm<<"_log} \n";
      }
      else{
         mylatexF<<"\\caption{Top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
         mylatexF<<"\\label{"<<labelname<<"_log} \n";
      }
      mylatexF<<"\\end{figure} \n";
      mylatexF<<"\n";
   }
   
   latexDoc->AddBodyText(mylatexF.str());
   
   if(mcAtNlo){
      delete mcAtNlo;
      mcAtNlo = 0;
   }
   
   if(kinVar){
      delete kinVar;
      kinVar = 0;
   }
}

//function to plot the differential and normalized spectrum with and without log scale 
void plot_XS_withGenerators(TGraphAsymmErrors nominalSyst,
                            const TopDecayChannel& channel,
                            std::string sampleName,
                            std::string filename,
                            std::string description,
                            std::string texDescription,
                            LatexDoc* const latexDoc,
                            bool norm,bool log){
   
   
   filename += "_gen";
   std::string pdfFilename       = globals::GetLatexPdfPath() + "/" + filename + ".pdf";
   std::string epsFilename       = globals::GetLatexEpsPath() + "/" + filename + ".eps";
   std::string pdfFilename_log   = globals::GetLatexPdfPath() + "/" + filename + "_log.pdf";
   std::string epsFilename_log   = globals::GetLatexEpsPath() + "/" + filename + "_log.eps";
   std::string labelname = "figure:XS_"+filename;
   std::string labelname_Norm = labelname +"_Norm";
   
   
   // get generator
   GeneratorLabel genLabel(globals::outputFilesPath);
   std::string generator = genLabel.GetLabel();
   
   // Get MC@NLO
   McAtNloBeforeCuts MCatNLO(channel,sampleName);
   if(genLabel.GetGenerator() == GeneratorLabel::MCATNLO)
      MCatNLO.SetMainGenerator(true);
   
   TH1* mcAtNlo= MCatNLO.GetHisto();
   if(norm) mcAtNlo = MCatNLO.GetHistoNorm();
   
   
   // Get ALPGEN
   AlpgenMC alpgen(channel.GetChannelString(),sampleName);
   if(genLabel.GetGenerator() == GeneratorLabel::ALPGEN)
      alpgen.SetMainGenerator(true);
   
   TH1* hAlpgen = alpgen.GetHisto();
   if(norm) hAlpgen = alpgen.GetHistoNorm();
   
   // Get POWHEG
   PowHegMC powheg(sampleName);
   if(genLabel.GetGenerator() == GeneratorLabel::POWHEG)
      powheg.SetMainGenerator(true);
   
   TH1* hPowheg = powheg.GetHisto();
   if(norm) hPowheg = powheg.GetHistoNorm();
   
   
   //define labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string kinVarLabel  = kinVar->GetKinVarLabel();
   std::string latexkinVarLabel  = kinVar->GetLatexKinVarLabel();
   std::string latexNkinVarLabel  = kinVar->GetNKinVarLabel();
   std::string UkinVarLabel = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   std::string NkinVarLabel = kinVar->GetNKinVarLabel(); 
   std::stringstream ntitle;
   
   // set color/style for main data
   nominalSyst.SetMarkerSize(1.);
   nominalSyst.SetMarkerColor(kBlack);
   nominalSyst.SetLineColor(kBlack);
   nominalSyst.SetMarkerStyle(20);
   nominalSyst.SetFillStyle(3244);
   nominalSyst.SetFillColor(kGray+1);
   
   //needed to enlarge the error bars 
   gStyle->SetEndErrorSize(3);
   
   
   // create a plot with ratio below
   Th1PlotAndRatio::RATIO_ORDER ratioOrder = Th1PlotAndRatio::NUM_RATIO_ORDER;
   if(globals::DATA_ON_TOP)
      ratioOrder = Th1PlotAndRatio::DATA_AS_NUMERATOR;
   else
      ratioOrder = Th1PlotAndRatio::DATA_AS_DENOMINATOR;
   Th1PlotAndRatio plotAndRatio(description,ratioOrder);
   
   if(sampleName.find("SystemRapidity") != std::string::npos){
      if(norm)
         plotAndRatio.SetYAxisMax(10);
      else
         plotAndRatio.SetYAxisMax(150);
   }
   
   // set axis labels
   if(norm)
      plotAndRatio.SetYAxisTitleTop(latexNkinVarLabel);
   else
      plotAndRatio.SetYAxisTitleTop(DkinVarLabel);
   
   plotAndRatio.SetXAxisTitle(UkinVarLabel);
   if(globals::DATA_ON_TOP){
      plotAndRatio.SetYAxisTitleRatio("Data/MC");
      plotAndRatio.SetCombineGeneratorRatios(false);
   }
   else{
      plotAndRatio.SetYAxisTitleRatio("MC/Data");
      plotAndRatio.SetCombineGeneratorRatios(true);
   }
   
   
   // set color/style for main data
   nominalSyst.SetMarkerSize(1.);
   nominalSyst.SetMarkerColor(kBlack);
   nominalSyst.SetLineColor(kBlack);
   nominalSyst.SetMarkerStyle(20);
   
   // add nominal data as reference plot
   std::stringstream label;
   label << "Data"; 
   if(!norm) label << " #int=" << GetIntegral(&nominalSyst) << "pb";
   plotAndRatio.SetData(&nominalSyst,label.str().c_str());
   
   if(mcAtNlo){
      label.str("");
      label << "MC@NLO";
      if(!norm) label << " #int=" << mcAtNlo->Integral("width") << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/MC@NLO";
      else
         ratio_axis_label = "MC@NLO/Data";
      plotAndRatio.AddGenerator(mcAtNlo,label.str().c_str(),ratio_axis_label,true,"l");
   }
   if(hAlpgen){
      label.str("");
      label << "Alpgen"; 
      if(!norm) label << " #int=" << hAlpgen->Integral("width") << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/Alpgen";
      else
         ratio_axis_label = "Alpgen/Data";
      plotAndRatio.AddGenerator(hAlpgen,label.str().c_str(),ratio_axis_label,true,"l");
   }
   if(hPowheg){
      label.str("");
      label.precision(4);
      label << "PowHeg"; 
      if(!norm) label << " #int=" << hPowheg->Integral("width") << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/PowHeg";
      else
         ratio_axis_label = "PowHeg/Data";
      plotAndRatio.AddGenerator(hPowheg,label.str().c_str(),ratio_axis_label,true,"l");
   }
   
   // save plots to files
   
   plotAndRatio.SaveAs(pdfFilename);
   plotAndRatio.SaveAs(epsFilename);
   
   std::stringstream mylatexF;
   mylatexF<<"\\begin{figure} \n";
   mylatexF<<"\\centering \n";
   mylatexF<<"\\includegraphics[width=0.8\\textwidth]{" << filename << "} \n";
   if(norm){
      mylatexF<<"\\caption{Normalized top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
      mylatexF<<"\\label{"<<labelname_Norm<<"} \n";
   }
   else{
      mylatexF<<"\\caption{Top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
      mylatexF<<"\\label{"<<labelname<<"} \n";
   }
   mylatexF<<"\\end{figure} \n";
   mylatexF<<"\n";
   
   // if log plots are requested, add them too
   
   // save plots to files
   if(log){
      plotAndRatio.SetMainPadLogy(true);
      plotAndRatio.SaveAs(pdfFilename_log);
      plotAndRatio.SaveAs(epsFilename_log);
      
      mylatexF<<"\\begin{figure} \n";
      mylatexF<<"\\centering \n";
      mylatexF<<"\\includegraphics[width=0.8\\textwidth]{" << filename << "_log} \n";
      if(norm){
         mylatexF<<"\\caption{Normalized top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
         mylatexF<<"\\label{"<<labelname_Norm<<"_log} \n";
      }
      else{
         mylatexF<<"\\caption{Top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
         mylatexF<<"\\label{"<<labelname<<"_log} \n";
      }
      mylatexF<<"\\end{figure} \n";
      mylatexF<<"\n";
   }
   
   latexDoc->AddBodyText(mylatexF.str());
   
   if(mcAtNlo){
      delete mcAtNlo;
      mcAtNlo = 0;
   }
   
   if(kinVar){
      delete kinVar;
      kinVar = 0;
   }
}

//function to plot the differential and normalized spectrum with and without log scale 
void plot_XS_withTheory(TGraphAsymmErrors nominalSyst,
                        std::string filename,
                        std::string sampleName,
                        std::string description,
                        std::string texDescription,
                        LatexDoc* const latexDoc,
                        bool norm,bool log){
   
   
   filename += "_thry";
   std::string pdfFilename       = globals::GetLatexPdfPath() + "/" + filename + ".pdf";
   std::string epsFilename       = globals::GetLatexEpsPath() + "/" + filename + ".eps";
   std::string pdfFilename_log   = globals::GetLatexPdfPath() + "/" + filename + "_log.pdf";
   std::string epsFilename_log   = globals::GetLatexEpsPath() + "/" + filename + "_log.eps";
   std::string labelname = "figure:XS_"+filename;
   std::string labelname_Norm = labelname +"_Norm";
   
   
   //define labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string kinVarLabel  = kinVar->GetKinVarLabel();
   std::string latexkinVarLabel  = kinVar->GetLatexKinVarLabel();
   std::string latexNkinVarLabel  = kinVar->GetNKinVarLabel();
   std::string UkinVarLabel = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   std::string NkinVarLabel = kinVar->GetNKinVarLabel(); 
   std::stringstream ntitle;
   
   
   // Get Kidonakis NNLO
   TGraphAsymmErrors* NNLO = 0;
   TH1* hNNLO = 0;
   if(sampleName.find("Top1") != std::string::npos || sampleName.find("Top2") != std::string::npos){
      KidonakisNNLO kNNLO;
      NNLO = kNNLO.GetGraph();
      hNNLO = kNNLO.GetHisto();
      if(norm){
         NNLO = kNNLO.GetGraphNorm();
         hNNLO = kNNLO.GetHistoNorm();
      }
   }
   // Get Yang-Pecjak NNLO for SystemMass
   else if(sampleName.find("SystemMass") != std::string::npos){
      YangPecjakNNLO yNNLO;
      NNLO = (TGraphAsymmErrors*)yNNLO.GetGraph()->Clone();
      hNNLO = (TH1*)yNNLO.GetHisto()->Clone();
      if(norm){
         NNLO = (TGraphAsymmErrors*)yNNLO.GetGraphNorm()->Clone();
         hNNLO = (TH1*)yNNLO.GetHistoNorm()->Clone();
      }
   }
   
   // Get MCFM
//    MCFMHisto mcfm(sampleName);
   MCFMWithPDF mcfm(MCFMWithPDF::CT10NLO,sampleName);
   TGraphAsymmErrors* gMcfm = mcfm.GetGraph();
   if(norm){
      gMcfm = mcfm.GetGraphNorm();
   }
   TH1* hMcfm = mcfm.GetStyleHisto();
   
   
   // create a plot with ratio below
   Th1PlotAndRatio::RATIO_ORDER ratioOrder = Th1PlotAndRatio::NUM_RATIO_ORDER;
   if(globals::DATA_ON_TOP)
      ratioOrder = Th1PlotAndRatio::DATA_AS_NUMERATOR;
   else
      ratioOrder = Th1PlotAndRatio::DATA_AS_DENOMINATOR;
   Th1PlotAndRatio plotAndRatio(description,ratioOrder);
   
   if(sampleName.find("SystemRapidity") != std::string::npos){
      if(norm)
         plotAndRatio.SetYAxisMax(10);
      else
         plotAndRatio.SetYAxisMax(150);
   }
   
   // set axis labels
   if(norm)
      plotAndRatio.SetYAxisTitleTop(latexNkinVarLabel);
   else
      plotAndRatio.SetYAxisTitleTop(DkinVarLabel);
   
   plotAndRatio.SetXAxisTitle(UkinVarLabel);
   if(globals::DATA_ON_TOP){
      plotAndRatio.SetYAxisTitleRatio("Data/MC");
      plotAndRatio.SetCombineGeneratorRatios(false);
   }
   else{
      plotAndRatio.SetYAxisTitleRatio("MC/Data");
      plotAndRatio.SetCombineGeneratorRatios(true);
   }
   
   
   // set color/style for main data
   nominalSyst.SetMarkerSize(1.);
   nominalSyst.SetMarkerColor(kBlack);
   nominalSyst.SetLineColor(kBlack);
   nominalSyst.SetMarkerStyle(20);
   nominalSyst.SetFillStyle(3244);
   nominalSyst.SetFillColor(kGray+1);
   
   // add nominal data as reference plot
   std::stringstream label;
   label << "Data"; 
   if(!norm) label << " #int=" << GetIntegral(&nominalSyst) << "pb";
   plotAndRatio.SetData(&nominalSyst,label.str().c_str());
   
   if(NNLO && hNNLO){
      label.str("");
      label << "NNLO"; 
      if(!norm) label << " #int=" << hNNLO->Integral("width") << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/NNLO";
      else
         ratio_axis_label = "NNLO/Data";
      plotAndRatio.AddTheory(NNLO,label.str().c_str(),ratio_axis_label,hNNLO,true,"f");
   }
   if(gMcfm && hMcfm){
      label.str("");
      label << "MCFM"; 
      if(!norm) label << " #int=" << GetIntegral(gMcfm) << "pb";
      std::string ratio_axis_label;
      if(globals::DATA_ON_TOP)
         ratio_axis_label = "Data/MCFM";
      else
         ratio_axis_label = "MCFM/Data";
      plotAndRatio.AddTheory(gMcfm,label.str().c_str(),ratio_axis_label,hMcfm,true,"f");
   }
   
   // save plots to files
   
   plotAndRatio.SaveAs(pdfFilename);
   plotAndRatio.SaveAs(epsFilename);
   
   std::stringstream mylatexF;
   mylatexF<<"\\begin{figure} \n";
   mylatexF<<"\\centering \n";
   mylatexF<<"\\includegraphics[width=0.8\\textwidth]{" << filename << "} \n";
   if(norm){
      mylatexF<<"\\caption{Normalized top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
      mylatexF<<"\\label{"<<labelname_Norm<<"} \n";
   }
   else{
      mylatexF<<"\\caption{Top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
      mylatexF<<"\\label{"<<labelname<<"} \n";
   }
   mylatexF<<"\\end{figure} \n";
   mylatexF<<"\n";
   
   
   // if log plots are requested, add them too
   
   // save plots to files
   if(log){
      plotAndRatio.SetMainPadLogy(true);
      plotAndRatio.SaveAs(pdfFilename_log);
      plotAndRatio.SaveAs(epsFilename_log);
      
      mylatexF<<"\\begin{figure} \n";
      mylatexF<<"\\centering \n";
      mylatexF<<"\\includegraphics[width=0.8\\textwidth]{" << filename << "_log} \n";
      if(norm){
         mylatexF<<"\\caption{Normalized top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
         mylatexF<<"\\label{"<<labelname_Norm<<"_log} \n";
      }
      else{
         mylatexF<<"\\caption{Top differential cross section versus top-$"<<latexkinVarLabel<<"$ for: " << texDescription << "} \n";
         mylatexF<<"\\label{"<<labelname<<"_log} \n";
      }
      mylatexF<<"\\end{figure} \n";
      mylatexF<<"\n";
   }
   
   latexDoc->AddBodyText(mylatexF.str());
   
   delete kinVar;
   kinVar = 0;
}

//function to plot the systematics, only considered when one bin has more than 1%
void plot_systematics(SystematicList& errors,
                      TH1* data,
                      std::string filename,
                      std::string sampleName,
                      std::string description,
                      std::string texDescription,
                      LatexDoc* const latexDoc,
                      bool norm){
   
   const unsigned int numOfSyst = errors.size();
   const unsigned int numOfBins = data->GetNbinsX();
   
   const double minUncertaintyPercent = norm ? 3. : 5.;
   
   SystematicList::iterator itr;
   
   std::string pdfFilename   = globals::GetLatexPdfPath() + "/" + filename + "_syst.pdf";
   std::string epsFilename  = globals::GetLatexEpsPath() + "/" + filename + "_syst.eps";
   std::string labelname = "figure:Syst5_"+filename;
   std::string labelname_Norm = labelname +"_Norm";
   
      
   //define labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string kinVarLabel  = kinVar->GetKinVarLabel();
   std::string latexkinVarLabel  = kinVar->GetLatexKinVarLabel();
   std::string UkinVarLabel = kinVar->GetUKinVarLabel();
   std::stringstream ntitle;
   
   TCanvas c("xs log","diffxsPlot log",0,0,800,600);
   c.cd();
   
   TLegend leg(0.18,0.9,0.5,0.7,description.c_str());
   leg.SetFillStyle(0);
   leg.SetBorderSize(0);
   leg.SetTextSize(.04);
   
   TLegend leg1(0.4,0.83,0.6,0.7,"");
   leg1.SetFillStyle(0);
   leg1.SetBorderSize(0);
   leg1.SetTextSize(.04);
   
   TLegend leg2(0.66,0.83,0.86,0.7,"");
   leg2.SetFillStyle(0);
   leg2.SetBorderSize(0);
   leg2.SetTextSize(.04);

   TH1 *h1 = (TH1*)data->Clone("systematics");
   h1->SetDirectory(false);

   //TH1F *h1 = (TH1F*)data->Clone();
   h1->SetName("h1");
   //h1->GetXaxis()->SetRangeUser(0.,850.);
   h1->AddDirectory(false);
   h1->Reset();
   
   h1->GetYaxis()->SetDecimals(kTRUE);
   if(norm){
      h1->SetMinimum(-15.);
      h1->SetMaximum(40.);
   }
   else{
      h1->SetMinimum(-30.);
      h1->SetMaximum(60.);
   }
   if( sampleName.find("Tag1_Top1_lhood") != std::string::npos ){
      if(norm){
         h1->SetMinimum(-15.);
         h1->SetMaximum(30.);
      }
      else{
         h1->SetMinimum(-30.);
         h1->SetMaximum(50.);
      }
   }
   else if( sampleName.find("Tag1_Top2_lhood") != std::string::npos ){
      if(norm){
         h1->SetMinimum(-15.);
         h1->SetMaximum(30.);
      }
      else{
         h1->SetMinimum(-30.);
         h1->SetMaximum(50.);
      }
   }
   else if( sampleName.find("Tag1_SystemPt") != std::string::npos ){
      if(norm){
         h1->SetMinimum(-30.);
         h1->SetMaximum(50.);
      }
   }
   else if( sampleName.find("Tag1_SystemRapidity") != std::string::npos ){
      if(norm){
         h1->SetMinimum(-15.);
         h1->SetMaximum(30.);
      }
      else{
         h1->SetMinimum(-20.);
         h1->SetMaximum(30.);
      }
   }
   else if( sampleName.find("Tag1_SystemMass") != std::string::npos ){
      if(norm){
         h1->SetMinimum(-15.);
         h1->SetMaximum(30.);
      }
   }
   
   h1->SetStats(kFALSE);
   h1->SetTitle("");
   h1->GetXaxis()->SetTitle(UkinVarLabel.c_str());
   h1->GetYaxis()->SetTitle("Systematic as % of bin content");  
   h1->GetXaxis()->SetTitleOffset(1.2);
   h1->GetYaxis()->SetTitleOffset(1.2);
   //h1->GetYaxis()->SetNdivisions(502);;
   h1->GetXaxis()->SetTickLength(0.05);
   h1->Draw("AXIS");
   h1->Draw("AXIG SAME");
         
   std::vector<TH1*> hUp;
   std::vector<TH1*> hDown;
   std::vector<bool> plot;
   std::vector<std::string> name;
   
   std::vector<std::string> jesi = MasterSystList::GetJesComponentBaseNames();
      
   for(itr = errors.begin();itr!=errors.end();++itr){
      std::string systematic = itr->first;
      SystematicBinList&  binErrors = itr->second;
      
      TH1F* histo = (TH1F*)data->Clone(std::string(systematic+"_up").c_str());
      hUp.push_back(histo);
      hUp.back()->AddDirectory(false);
      hUp.back()->Reset();
      
      TH1F* histod = (TH1F*)data->Clone(std::string(systematic+"_down").c_str());
      hDown.push_back(histod);
      hDown.back()->AddDirectory(false);
      hDown.back()->Reset();
      
      plot.push_back(false);
      
      for(unsigned int bin=1;bin<=numOfBins;++bin){
         
         double binContents = data->GetBinContent(bin);
         double up = binErrors[bin-1].up()*100./binContents;
         double down = - binErrors[bin-1].down()*100./binContents;

         double binLeft = data->GetBinLowEdge(bin);
         double binRight = data->GetBinLowEdge(bin) + data->GetBinWidth(bin);
         double binNom = (binLeft + binRight)/2.;
         
         hUp.back()->Fill(binNom,up);
         hDown.back()->Fill(binNom,down);

         hUp.back()->SetBinError(bin,0.001);
         hDown.back()->SetBinError(bin,0.001);
         
         if(up>minUncertaintyPercent || down <-minUncertaintyPercent){
            if(!MasterSystList::IsJesComponentSystBase(systematic) && systematic.find("bkgd_jes") == std::string::npos)
               plot.back() = true;
         }
         
      }
      
   }
   
   int leg_cont=1;
   for(unsigned int i=0;i<hDown.size();++i){
      
      if(plot[i]) {
         std::string hname = hDown[i]->GetName();
         std::string legend_label;
         
         if(hname.find("jes") != std::string::npos){
            hDown[i]->SetMarkerStyle(20);
            hDown[i]->SetMarkerColor(1);
            hDown[i]->SetLineColor(1);
            
            hUp[i]->SetMarkerStyle(20);
            hUp[i]->SetMarkerColor(1);
            hUp[i]->SetLineColor(1);
            
            legend_label = "JES";
         }
         else if(hname.find("unfolding") != std::string::npos){
            hDown[i]->SetMarkerStyle(21);
            hDown[i]->SetMarkerColor(2);
            hDown[i]->SetLineColor(2);
            
            hUp[i]->SetMarkerStyle(21);
            hUp[i]->SetMarkerColor(2);
            hUp[i]->SetLineColor(2);
            
            legend_label = "Unfolding Method";
         }
         else if(hname.find("btagsf") != std::string::npos){
            hDown[i]->SetMarkerStyle(22);
            hDown[i]->SetMarkerColor(3);
            hDown[i]->SetLineColor(3);
            
            hUp[i]->SetMarkerStyle(22);
            hUp[i]->SetMarkerColor(3);
            hUp[i]->SetLineColor(3);
            
            legend_label = "b-tag SF";
         }
         else if(hname.find("generator") != std::string::npos){
            hDown[i]->SetMarkerStyle(23);
            hDown[i]->SetMarkerColor(4);
            hDown[i]->SetLineColor(4);
            
            hUp[i]->SetMarkerStyle(23);
            hUp[i]->SetMarkerColor(4);
            hUp[i]->SetLineColor(4);
            
            legend_label = "Generator";
         }
         else if(hname.find("qcds") != std::string::npos){
            hDown[i]->SetMarkerStyle(29);
            hDown[i]->SetMarkerColor(11);
            hDown[i]->SetLineColor(11);
            
            hUp[i]->SetMarkerStyle(29);
            hUp[i]->SetMarkerColor(11);
            hUp[i]->SetLineColor(11);
            
            legend_label = "QCDShape";
         }
         else if(hname.find("qcd") != std::string::npos){
            hDown[i]->SetMarkerStyle(24);
            hDown[i]->SetMarkerColor(5);
            hDown[i]->SetLineColor(5);
            
            hUp[i]->SetMarkerStyle(24);
            hUp[i]->SetMarkerColor(5);
            hUp[i]->SetLineColor(5);
            
            legend_label = "QCD";
         }
         else if(hname.find("ifsr") != std::string::npos){
            hDown[i]->SetMarkerStyle(25);
            hDown[i]->SetMarkerColor(6);
            hDown[i]->SetLineColor(6);
            
            hUp[i]->SetMarkerStyle(25);
            hUp[i]->SetMarkerColor(6);
            hUp[i]->SetLineColor(6);
            
            legend_label = "IFSR";
         }
         else if(hname.find("jer") != std::string::npos){
            hDown[i]->SetMarkerStyle(26);
            hDown[i]->SetMarkerColor(7);
            hDown[i]->SetLineColor(7);
            
            hUp[i]->SetMarkerStyle(26);
            hUp[i]->SetMarkerColor(7);
            hUp[i]->SetLineColor(7);
            
            legend_label = "JER";
         }
       
         else if(hname.find("powheg") != std::string::npos){
            hDown[i]->SetMarkerStyle(27);
            hDown[i]->SetMarkerColor(8);
            hDown[i]->SetLineColor(8);
            
            hUp[i]->SetMarkerStyle(27);
            hUp[i]->SetMarkerColor(8);
            hUp[i]->SetLineColor(8);
            
            legend_label = "Parton Shower";
         }
         
         else if(hname.find("hadronization") != std::string::npos){
            hDown[i]->SetMarkerStyle(28);
            hDown[i]->SetMarkerColor(9);
            hDown[i]->SetLineColor(9);
            
            hUp[i]->SetMarkerStyle(28);
            hUp[i]->SetMarkerColor(9);
            hUp[i]->SetLineColor(9);
            
            legend_label = "Fragment.";
         }
         else if(hname.find("migrationtoys") != std::string::npos){
            hDown[i]->SetMarkerStyle(30);
            hDown[i]->SetMarkerColor(12);
            hDown[i]->SetLineColor(12);
            
            hUp[i]->SetMarkerStyle(30);
            hUp[i]->SetMarkerColor(12);
            hUp[i]->SetLineColor(12);
            
            legend_label = "MC Stat.";
         }
         else if(hname.find("ees") != std::string::npos){
            hDown[i]->SetMarkerStyle(31);
            hDown[i]->SetMarkerColor(13);
            hDown[i]->SetLineColor(13);
            
            hUp[i]->SetMarkerStyle(31);
            hUp[i]->SetMarkerColor(13);
            hUp[i]->SetLineColor(13);
            
            legend_label = "El. En. Res.";
         }
         else if(hname.find("wjets_ca") != std::string::npos){
            hDown[i]->SetMarkerStyle(32);
            hDown[i]->SetMarkerColor(14);
            hDown[i]->SetLineColor(14);
            
            hUp[i]->SetMarkerStyle(32);
            hUp[i]->SetMarkerColor(14);
            hUp[i]->SetLineColor(14);
            
            legend_label = "W+Jets(ca)";
         }
         else{
            std::cerr << "[plot_systematics] ERROR Should have plotted systematic: " << hname << " but no if-statement included to catch this entry. SampleName is " << sampleName << "\n";
            legend_label = hname;
         }
         
         //gStyle->SetErrorX(0.001);
         hUp[i]->Draw("P same");
         hDown[i]->Draw("P same");
         
         if(1 <= leg_cont && leg_cont <= 2)
            leg.AddEntry(hUp[i],legend_label.c_str(),"p");
         if(3 <= leg_cont && leg_cont <= 4)
            leg1.AddEntry(hUp[i],legend_label.c_str(),"p");
         else if(leg_cont >= 5){
            leg2.AddEntry(hUp[i],legend_label.c_str(),"p");
         }
         
         leg.Draw("same");
         leg1.Draw("same");
         leg2.Draw("same");
         leg_cont++;
      }
   }
   c.SaveAs(pdfFilename.c_str());
   c.SaveAs(epsFilename.c_str());  
    
   for(unsigned int i=0;i<hDown.size();++i){
      delete hDown[i];
      hDown[i] = 0;
      delete hUp[i];
      hUp[i] = 0;
   }
   
   std::stringstream mylatexF;
   mylatexF<<"\\begin{figure} \n";
   mylatexF<<"\\centering \n";
   mylatexF<<"\\includegraphics[width=0.8\\textwidth]{" << filename << "_syst.eps} \n";
   if(norm){
      mylatexF<<"\\caption{Normalized top differential cross section versus top-$" << latexkinVarLabel << "$ for: " << texDescription << "} \n";
      mylatexF<<"\\label{"<<labelname_Norm<<"} \n";
   }
   else{
      mylatexF<<"\\caption{Top differential cross section versus top-$" << latexkinVarLabel << "$ for: " << texDescription << "} \n";
      mylatexF<<"\\label{"<<labelname<<"} \n";
   }
   mylatexF<<"\\end{figure} \n";
   mylatexF<<"\\clearpage    \n";
   
   latexDoc->AddBodyText(mylatexF.str());
   
   delete h1;
   h1 = 0;
   
   delete kinVar;
   kinVar = 0;
}

//function to get the correct name of a histogram
std::string get_histo_name(const std::string& basePath,
                           const std::string& systematic,
                           const std::string& channel,
                           const std::string& sampleName,
                           const unsigned int& regOrIter,
                           const unsigned int& nToys,
                           const std::string& tail
                           )
{
   
   std::stringstream histoPath,histoName;
   if(systematic=="unfolding"){
      histoPath << basePath << "/" << systematic << "/" << channel << "/" << sampleName;
      histoName << "/H_" << channel << "_" << sampleName;
      if(globals::globalUnfoldingMethod == globals::SVD){
         histoPath << "/Bayes";
         histoName << "_Bayes";
      }
      else if(globals::globalUnfoldingMethod == globals::BAYESIAN){
         histoPath << "/SVD/reg" << regOrIter;
         histoName << "_SVD_reg" << regOrIter;
      }
      
      if(nToys > 0){
         histoPath << "/toy" << nToys;
         histoName << "_toy" << nToys;
      }
      
      histoName << tail;
   }
   else{
      histoPath << basePath << "/" << systematic << "/" << channel << "/" << sampleName;
      histoName << "/H_" << channel << "_" << sampleName ;
      if(globals::globalUnfoldingMethod == globals::SVD){
         histoPath << "/SVD/reg" << regOrIter;
         histoName << "_SVD_reg" << regOrIter;
      }
      else if(globals::globalUnfoldingMethod == globals::BAYESIAN){
         histoPath << "/Bayes";
         histoName << "_Bayes";
      }
      else if(globals::globalUnfoldingMethod == globals::BINBYBIN){
         histoPath << "/BinByBin";
         histoName << "_BinByBin";
      }
      else if(globals::globalUnfoldingMethod == globals::INVERSION){
         histoPath << "/Inversion";
         histoName << "_inversion";
      }
      
      if(nToys > 0){
         histoPath << "/toy" << nToys;
         histoName << "_toy" << nToys;
      }
      
      histoName << tail;
   }
   
   std::string path = histoPath.str() + histoName.str();
   return path;
}







double GetIntegral(TGraphAsymmErrors* graph){
   
   double integral = 0;
   for(unsigned int i=0;i<graph->GetN();++i){
      double binWidth = graph->GetErrorXhigh(i) + graph->GetErrorXlow(i);
      double binContent = graph->GetY()[i];
      
//       std::cout << ">>>>> binWidth: " << binWidth << " binContent: " << binContent << std::endl;
      
      integral += binWidth*binContent;
      
   }
   
   return integral;
   
   
}

void WritePlotsToFile(const std::string& sampleName,const TopDecayChannel& channel,
                      const SystematicList& errors, const SystematicList& errorsNorm,
                      const TH1* nominalScaledByBR,
                      const TH1* nominalNorm,
                      const TGraphAsymmErrors* nominalWithSystematics,
                      const TGraphAsymmErrors* nominalNormWithSystematics,
                      const TH1* truthScaledByBR,
                      const TH1* measured)
{
   std::string out_filename = globals::GetOutputPath() + "/unfoldedResult_" + sampleName + "_" + channel.GetChannelString() + ".root";
   // open an output file to save the results to disk for later manipulation
   TFile* outFile = new TFile(out_filename.c_str(),"UPDATE");
   if(!outFile->IsOpen()){
      std::cerr << "[output_latex_table] ERROR opening file for output: " << out_filename << "\n";
   }
   
   // write nominal plots to disk
   outFile->cd();
   nominalScaledByBR->Write("diffxs_stat",TObject::kOverwrite);
   nominalNorm->Write("diffxs_norm_stat",TObject::kOverwrite);
   nominalWithSystematics->Write("diffxs_syst_stat",TObject::kOverwrite);
   nominalNormWithSystematics->Write("diffxs_norm_syst_stat",TObject::kOverwrite);
   truthScaledByBR->Write("diffxs_truth",TObject::kOverwrite);
   measured->Write("measured",TObject::kOverwrite);
   
   // write systematics to disk by filling histograms with systematics
   // and save to disk
   SystematicList::const_iterator errIter = errors.begin();
   SystematicList::const_iterator errNormIter = errorsNorm.begin();
   for(;errIter != errors.end() && errNormIter != errorsNorm.end();++errIter,++errNormIter){
      std::string systematic = errIter->first;
      const SystematicBinList& errList = errIter->second;
      const SystematicBinList& errNormList = errNormIter->second;
      
      TGraphAsymmErrors* error   = (TGraphAsymmErrors*)nominalWithSystematics->Clone(systematic.c_str());
      
      TGraphAsymmErrors* errorNorm   = (TGraphAsymmErrors*)nominalNormWithSystematics->Clone(std::string(systematic+"_norm").c_str());
      
      for(unsigned int i=0;i<errList.size() && i<errNormList.size();++i){
         error->SetPointEYhigh(i,errList[i].up());
         error->SetPointEYlow(i,errList[i].down());
         
         errorNorm->SetPointEYhigh(i,errNormList[i].up());
         errorNorm->SetPointEYlow(i,errNormList[i].down());
         
      }
      
      outFile->cd();
      error->Write(error->GetName(),TObject::kOverwrite);
      errorNorm->Write(errorNorm->GetName(),TObject::kOverwrite);
      
      delete error; error = 0;
      delete errorNorm; errorNorm = 0;
      
   }
   
   outFile->Close();
   delete outFile;
   outFile = 0;
}

std::string GetInputFilename(const std::string& systematic,
                             const std::string& sampleName,
                             const std::string& channel,
                             const std::string& nToysStr,
                             const GeneratorLabel::Generator genLabelEnum,
                             const unsigned int regOrIter)
{
   // build input filename
   std::string filename = globals::outputFilesPath;
   
   // if this is generator systematic, the input file should come
   // from the opposite generator
   std::string tmp_systematic = systematic;
   if(systematic.find("mc_generator") != std::string::npos){
      
      // if current generator is MCATNLO, replace last part of path '/McAtNlo'
      // with '/Alpgen'
      if(genLabelEnum == GeneratorLabel::MCATNLO){
         std::string mcatnlo = "McAtNlo";
         std::string alpgen = "Alpgen";
         unsigned int index = filename.rfind(mcatnlo);
         if(index == std::string::npos){
            std::cerr << "[GetInputFilename] ERROR could not build filename for generator systematics\n";
            return std::string();
         }
         filename = filename.replace(index,mcatnlo.size(),alpgen);
      }
         // and if Alpgen is the generator, replace with McAtNlo
      else if(genLabelEnum == GeneratorLabel::ALPGEN){
         std::string mcatnlo = "McAtNlo";
         std::string alpgen = "Alpgen";
         unsigned int index = filename.rfind(alpgen);
         if(index == std::string::npos){
            std::cerr << "[GetInputFilename] ERROR could not build filename for generator systematics\n";
            return std::string();
         }
         filename = filename.replace(index,alpgen.size(),mcatnlo);
      }
      
      // in both cases the systematic name should be 'nominal'
      tmp_systematic = "nominal";
   }
   
   filename += "/";
   filename += tmp_systematic + "_" + sampleName + "_" + channel + "_toy" + nToysStr + "_";
   
   // make a temporary variable to hold the current unfolding method
   // being used.
   globals::unf_methods unfMethod = globals::globalUnfoldingMethod;
   
   // finish input filename
   if(unfMethod == globals::SVD){
      filename += "svd_reg";
      std::stringstream ss;
      ss << regOrIter;
      filename += ss.str();
   }
   else if(unfMethod == globals::BAYESIAN){
      filename += "bay_itr";
      std::stringstream ss;
      ss << regOrIter;
      filename += ss.str();
   }
   else if(unfMethod == globals::BINBYBIN){
      filename += "bin";
   }
   else if(unfMethod == globals::INVERSION){
      filename += "inv";
   }
   filename += ".root";
   
   
   return filename;
}
