#include <vector>
#include <cmath>
#include <math.h>
#include <ctime>
#include <cstdlib>
#include <map>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

#include "TDatime.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TTimeStamp.h"
#include "TMarker.h"
#include "Rtypes.h"
#include "TH2.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TLatex.h"
#include "TLine.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TGraphAsymmErrors.h"
#include "TBox.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TProfile.h"

#include "AtlasStyle.h"
#include "globals.h"
#include "MCFMHisto.h"
#include "KidonakisNNLO.h"
#include "YangPecjakNNLO.h"
#include "McAtNloBeforeCuts.h"
#include "AlpgenMC.h"
#include "PowHegMC.h"
#include "SystTable.h"
#include "SystematicBin.h"
#include "SystematicBinList.h"
#include "SystematicList.h"
#include "SystematicTextFile.h"
#include "AsymBLUECombination.h"
#include "KinematicVariable.h"
#include "Th1PlotAndRatio.h"
#include "GeneratorLabel.h"
#include "PValueTable.h"
#include "TopDecayChannel.h"
#include "ToyCovariance.h"
#include "MasterSystList.h"
#include "MatrixTable.h"
#include "SampleDef.h"


// theory background:
// http://www.sciencedirect.com/science/article/pii/S0168900203003292

void RunToys(const std::string& sampleName,
             std::vector<std::string>& correlatedSystematics,
             const unsigned int& nToys,
             const unsigned int& whichCovarianceMatrix
            );

void RunToyCovariance(BLUECombination& blue,
                                ToyCovariance& toy,
                                const SystematicList& list_el,
                                const SystematicList& list_mu,
                                const std::vector<std::string>& correlatedSystematics,
                                const unsigned int nToys,
                                const unsigned int whichCovarianceMatrix,
                                const unsigned int fillmode
                               );

void output_correlation_latex_table_file(TMatrixD const * const cor,
                                         const std::string& sampleName,
                                         const std::vector<double> binEdges,
                                         const unsigned int whichCovarianceMatrix
                                        );


void output_matrices_to_root_file(TMatrixD const * const cor,
                                  TMatrixD const * const cov,
                                  const std::string& sampleName,
                                  const unsigned int whichCovarianceMatrix
                                 );

std::vector<double> GetVectorFromHisto(TH1* histo);

int main(int argc,char** argv){
   
   SetAtlasStyle();
   const unsigned int nToys = 10;//50000;
   const unsigned int whichCovarianceMatrix = 1; // 1 means electron, 2 means muon, anything else means combination
   
   std::vector<std::string> correlatedSystematics;
//    if(!MasterSystList::UseJesComponents())
//       correlatedSystematics.push_back("jes");
   correlatedSystematics.push_back("btagsf");
   correlatedSystematics.push_back("ctagsf");
   correlatedSystematics.push_back("mistagsf");
   correlatedSystematics.push_back("cellout");
   correlatedSystematics.push_back("pileup");
   correlatedSystematics.push_back("jeff");
   correlatedSystematics.push_back("jer");
   correlatedSystematics.push_back("wjets_bb4");
   correlatedSystematics.push_back("wjets_bb5");
   correlatedSystematics.push_back("wjets_bbcc");
   correlatedSystematics.push_back("wjets_bbccc");
   correlatedSystematics.push_back("wjets_c4");
   correlatedSystematics.push_back("wjets_c5");
   correlatedSystematics.push_back("wjets_ca");
   correlatedSystematics.push_back("IFsr");
   correlatedSystematics.push_back("PowHeg");
   correlatedSystematics.push_back("bayesian");
   correlatedSystematics.push_back("bgstat");
   correlatedSystematics.push_back("PDF");
   correlatedSystematics.push_back("unfolding"); 
   correlatedSystematics.push_back("migrationToys");
   correlatedSystematics.push_back("generator");
   correlatedSystematics.push_back("hadronization");
   // add jesi if needed
   if(MasterSystList::UseJesComponents()){
      std::vector<std::string> jesi = MasterSystList::GetJesComponentBaseNames();
      correlatedSystematics.insert(correlatedSystematics.end(),jesi.begin(),jesi.end());
   }
   
   std::vector<std::string> sampleNames;
//    sampleNames.push_back("Tag1_Top1");
//    sampleNames.push_back("Tag1_Top2");
//    sampleNames.push_back("Tag1_Top1_lhood");
//    sampleNames.push_back("Tag1_Top2_lhood");
   sampleNames.push_back("Tag1_SystemMass");
//    sampleNames.push_back("Tag1_SystemPt");
//    sampleNames.push_back("Tag1_SystemRapidity");
   
   for(unsigned int type = 0;type<sampleNames.size();++type){
      std::cout << "\nsample: " << sampleNames[type] << "\n";
      std::string sampleName = sampleNames[type];
      
      /// for normalized spectra
      RunToys(sampleName,correlatedSystematics,nToys,whichCovarianceMatrix);
      
   } // for(sampleName)
   
   
   std::cout << "[createCorrelationMatrices] Done.\n";
}

void RunToys(const std::string& sampleName,
             std::vector<std::string>& correlatedSystematics,
             const unsigned int& nToys,
             const unsigned int& whichCovarianceMatrix
            )
{
   
   // filename for input file
   std::string samplePath = globals::GetOutputPath() + "/" + "unfoldedResult_"+sampleName;
   std::string efilename = samplePath + "_el.root";
   std::string mufilename = samplePath + "_mu.root";
   
   // get systematic data for spectra
   bool normalized = false;
   SystematicList* list_el = SystematicList::InstanceFromUnfoldOutput(efilename,normalized);
   SystematicList* list_mu = SystematicList::InstanceFromUnfoldOutput(mufilename,normalized);
   
   BLUECombination blue(list_el,list_mu,correlatedSystematics,BLUECombination::UNCORRELATED);
   blue.CalculateCovarianceMatrix();
   
   SystematicList::const_iterator it_el = list_el->begin();
   const SystematicBinList& firstBinList_el = it_el->second;
   
   double hmin = 0.;
   double hmax = 100.;
   int debug = 0;
   int nFineBins = 10;
   int NkinemBins = firstBinList_el.size();
   int mode = 1;
   int fillmode = 0;
   if (mode != 0)
      hmin = -hmax;
   ToyCovariance *toy = new ToyCovariance(NkinemBins, nFineBins, hmin, hmax, debug, mode, true);
   
   fillmode=1;
   RunToyCovariance(blue,*toy,*list_el,*list_mu,correlatedSystematics,nToys,whichCovarianceMatrix,fillmode);
   toy->PrintMeans();
   fillmode=2;
   RunToyCovariance(blue,*toy,*list_el,*list_mu,correlatedSystematics,nToys,whichCovarianceMatrix,fillmode);
   
   TMatrixD* cov = toy->GetCovariance();
   TMatrixD* cor = toy->GetCorrelation();
   
   cov->Print("all");
   cor->Print("all");
   
   output_correlation_latex_table_file(cor,sampleName,list_el->begin()->second.GetBinEdges(),whichCovarianceMatrix);
   
   output_matrices_to_root_file(cor,cov,sampleName,whichCovarianceMatrix);
   
   toy->Cleanup();
   
   delete list_el; list_el = 0;
   delete list_mu; list_mu = 0;
   delete toy; toy = 0;
}



void RunToyCovariance(BLUECombination& blue,
                                ToyCovariance& toy,
                                const SystematicList& list_el,
                                const SystematicList& list_mu,
                                const std::vector<std::string>& correlatedSystematics,
                                const unsigned int nToys,
                                const unsigned int whichCovarianceMatrix,
                                const unsigned int fillmode
                               )
{
   
   SystematicList::const_iterator it_el = list_el.begin();
   const SystematicBinList& firstBinList_el = it_el->second;
   SystematicList::const_iterator it_mu = list_mu.begin();
   const SystematicBinList& firstBinList_mu = it_mu->second;
   
   // create histograms from list_el and mu
   std::vector<double> binEdges = firstBinList_el.GetBinEdges();
   TH1D* nominal_el = new TH1D("nominal_el","",binEdges.size()-1,&binEdges[0]);
   TH1D* nominal_mu = new TH1D("nominal_mu","",binEdges.size()-1,&binEdges[0]);
   
//    std::cerr << " Nominal bin content (el/mu) << ";
   for(unsigned int bin=1;bin<=nominal_el->GetNbinsX();++bin){
      nominal_el->SetBinContent(bin,firstBinList_el[bin-1].content());
      nominal_mu->SetBinContent(bin,firstBinList_mu[bin-1].content());
//       std::cerr << " " << nominal_el->GetBinContent(bin) << "/" << nominal_mu->GetBinContent(bin);
   }
//    std::cerr << " >> end nominal\n";
   
   const double nom_el_integral = nominal_el->Integral("width");
   
   // still have to think about how to treat statistical errors
   TRandom3 r(1);
   double rNum;
   
   for(int toyNum=0; toyNum<nToys; toyNum++){
      std::cout << "toy: " << toyNum << "\n";
//       std::cout << "\r toy: " << toyNum;
      TH1D* el_toy = (TH1D*)nominal_el->Clone("el_toy");
      TH1D* mu_toy = (TH1D*)nominal_mu->Clone("mu_toy");
      std::vector<double> randomNums(correlatedSystematics.size());
      
      /// loop over electrons
      SystematicList::const_iterator errIter = list_el.begin();
      for(;errIter != list_el.end();++errIter){ // loop over electron systematics
         std::string systematic = errIter->first;
         if(systematic == "pdf" || systematic == BLUECombination::STATISTICS) continue;
         // get random number
         rNum = toyNum+10.;//r.Gaus(0,1);
         // if this is a correlated systematic, save the random number
         // so it can also be used in the muon case
         for(unsigned int i=0; i<correlatedSystematics.size(); i++){
            if(systematic==correlatedSystematics[i])
               randomNums[i]=rNum;
         }
         
         std::cerr << "   ";
         std::cerr.width(30);
         std::cerr << systematic << " | ";
         std::cerr.width(10);
         std::cerr << rNum << " |";
         
         // loop over systematic bin list
         const SystematicBinList& errList = errIter->second;
         for(unsigned int bin=1;bin<=errList.size();++bin){ // loop over bins
            
            // if the shape_up/down are set, then this is an asymmetric error
            if(fabs(errList[bin-1].shape_up()) > 1e-10 || fabs(errList[bin-1].shape_down()) > 1e-10){
               double shape = 0.;
               if(rNum>0){
                  shape = errList[bin-1].shape_up();
               }
               else{
                  shape = errList[bin-1].shape_down();
               }
               std::cerr << " ";
               std::cerr.width(10);
               std::cerr.precision(4);
               std::cerr << shape;
               el_toy->SetBinContent(bin,el_toy->GetBinContent(bin)+fabs(rNum)*shape);
            }
            else if(systematic == BLUECombination::STATISTICS){
               double shape = 0.;
               if(rNum>0){
                  shape = errList[bin-1].stat();
               }
               else{
                  shape = errList[bin-1].stat() * (-1.);
               }
               std::cerr << " ";
               std::cerr.width(10);
               std::cerr.precision(4);
               std::cerr << shape;
               el_toy->SetBinContent(bin,el_toy->GetBinContent(bin)+fabs(rNum)*shape);
            }
            else// otherwise it is a symmetric error 
               el_toy->SetBinContent(bin,el_toy->GetBinContent(bin)+rNum*errList[bin-1].syst());
            
            std::cerr << " ";
            std::cerr.width(10);
            std::cerr.precision(4);
            std::cerr << el_toy->GetBinContent(bin) << " .";
            
         } // end bins
         std::cerr << "\n";
      } // end electrons
      
      /// Loop over muons
      errIter = list_mu.begin();
      for(;errIter != list_mu.end();++errIter){ // loop over muon systematics
         std::string systematic = errIter->first;
         rNum=0;
         for(unsigned int i=0;i<correlatedSystematics.size(); i++){
            if(systematic==correlatedSystematics[i]){
               rNum = randomNums[i];
            }
         }
         if(rNum==0)
            rNum = r.Gaus(0,1);
         const SystematicBinList& errList = errIter->second;
         for(unsigned int bin=1;bin<=errList.size();++bin){ // loop over bins
            if(fabs(errList[bin].shape_up()) > 1e-10 || fabs(errList[bin].shape_down()) > 1e-10){
               double shape = 0.;
               if(rNum>0){
                  shape = errList[bin-1].shape_up();
               }
               else{
                  shape = errList[bin-1].shape_down();
               }
               mu_toy->SetBinContent(bin,mu_toy->GetBinContent(bin)+fabs(rNum)*shape);
            }
            else if(systematic == BLUECombination::STATISTICS){
               double shape = 0.;
               if(rNum>0){
                  shape = errList[bin-1].stat();
               }
               else{
                  shape = errList[bin-1].stat() * (-1.);
               }
               mu_toy->SetBinContent(bin,mu_toy->GetBinContent(bin)+fabs(rNum)*shape);
            }
            else
               mu_toy->SetBinContent(bin,mu_toy->GetBinContent(bin)+rNum*errList[bin-1].syst());
            
            
         }
         
      } // end muons
      
      std::vector<double> v_el_toy = GetVectorFromHisto(el_toy);
      std::vector<double> v_mu_toy = GetVectorFromHisto(mu_toy);
      
      SystematicBinList& combinedBinList=blue.Combine(v_el_toy,v_mu_toy);
      TH1D *binList_hist = 0;
      if(whichCovarianceMatrix == 1)
         binList_hist = el_toy;
      else if(whichCovarianceMatrix == 2)
         binList_hist = mu_toy;
      
      double integral=0;
      if(whichCovarianceMatrix == 1 || whichCovarianceMatrix == 2){
         integral = binList_hist->Integral("width"); // (*binList_hist).Integral("width");
         binList_hist->Scale(1./integral);
         std::cerr << "   Integral: " << integral << " nom: " << nom_el_integral << " " << ((integral-nom_el_integral)/nom_el_integral) << "\n";
//          binList_hist->Print("all");
         
      }
      else{
         integral = combinedBinList.Integral();
         combinedBinList.Scale(1./integral);
      }
      
      if(whichCovarianceMatrix == 1 || whichCovarianceMatrix == 2){
         toy.Fill(binList_hist, fillmode);
         //toy.PrintMeans();
      }
      else toy.Fill(combinedBinList, fillmode);
      
      if(el_toy){
         delete el_toy; el_toy = 0;
      }
      if(mu_toy){
         delete mu_toy; mu_toy = 0;
      }
      
//       std::cin.get();
      
   }// end toys
//    std::cout << std::endl;
   
   delete nominal_el; nominal_el = 0;
   delete nominal_mu; nominal_mu = 0;
}

void output_correlation_latex_table_file(TMatrixD const * const cor,
                                         const std::string& sampleName,
                                         const std::vector<double> binEdges,
                                         const unsigned int whichCovarianceMatrix
                                        )
{
   /// output matrices in latex to file
   std::string label = "tab:correlationMatrix_";
   label += sampleName;
   label += "_norm";
   MatrixTable correlationTable(cor,label,sampleName,binEdges);
      
   std::string texFilename = globals::GetLatexPath();
   texFilename += "/correlationMatrices_";
   texFilename += sampleName;
   if(whichCovarianceMatrix==1) texFilename += "_el_only";
   else if(whichCovarianceMatrix==2) texFilename += "_mu_only";
   texFilename += "_norm.tex";
   std::fstream outTexFile(texFilename.c_str(),std::fstream::out | std::fstream::trunc);
   if(!outTexFile){
      std::cerr << "[output_correlation_latex_table_file] ERROR opening file for tex table, " << texFilename << "\n";
      return;
   }
   std::string caption = "Bin-wise correlation matrix for the ";
   caption += "normalized ";
   SampleDef sample(sampleName);
   caption += sample.GetVariable() + " ";
   KinematicVariable var(sampleName);
   caption += var.GetLatexKinVarLabel() + ".";
   std::string table = correlationTable.GetTableText(caption);
   std::cerr << table;
   outTexFile << table;
   outTexFile.close();
   
}

void output_matrices_to_root_file(TMatrixD const * const cor,
                                  TMatrixD const * const cov,
                                  const std::string& sampleName,
                                  const unsigned int whichCovarianceMatrix
                                 )
{
   
   std::string filename = globals::GetOutputPath() + "/" + "unfoldedResult_"+sampleName+".root";
   
   // output matrices to file
   TFile outputMatrixFile(filename.c_str(),"UPDATE");
   if(!outputMatrixFile.IsOpen()){
      std::cerr << "[output_matrices_to_root_file] ERROR opening file for matrices to be saved, filename: " << filename << std::endl;
   }
   else{
      outputMatrixFile.cd();
      std::string name = "covarianceMatrix";
      if(whichCovarianceMatrix==1) name+="_el_only";
      else if(whichCovarianceMatrix==2) name+="_mu_only";
      name += "_norm";
      cov->Write(name.c_str(),TObject::kOverwrite);
      name = "correlationMatrix";
      name += "_norm";
      cor->Write(name.c_str(),TObject::kOverwrite);
      outputMatrixFile.Close();
   }
   
}

std::vector<double> GetVectorFromHisto(TH1* histo){
   std::vector<double> output;
   
   for(unsigned int i=0;i<histo->GetNbinsX();++i)
      output.push_back(histo->GetBinContent(i+1));
   
   return output;
}




