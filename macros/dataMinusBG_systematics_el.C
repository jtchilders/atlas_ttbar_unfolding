#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "TDatime.h"
#include <time.h>
#include <map>
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
#include "TFile.h"
#include "TGraphErrors.h"
#include "TLatex.h"
#include "TLine.h"

void dump(std::string name, size_t vsize, double* down, double* up, double* x, double* y, double* ey, double* downy, double* upy, double* JESu, double* JESd, double* BTAGSFu, double* BTAGSFd, double* EERu, double* EERd, double* JER);

std::string get_histo_name_base(const std::string basePath,
                                const std::string channel,
                                const std::string sampleName,
                                const unsigned int reg,
                                const unsigned int nToys
                               );

std::string get_truth_histo_name(const std::string basePath,
                                 const std::string channel,
                                 const std::string sampleName,
                                 const unsigned int reg,
                                 const unsigned int nToys
                                );

std::string get_reco_histo_name(const std::string basePath,
                                const std::string channel,
                                const std::string sampleName,
                                const unsigned int reg,
                                const unsigned int nToys
                               );


std::string get_measured_histo_name(const std::string basePath,
                                    const std::string channel,
                                    const std::string sampleName,
                                    const unsigned int reg,
                                    const unsigned int nToys
                                   );

std::string get_unfolded_histo_name(const std::string basePath,
                                    const std::string channel,
                                    const std::string sampleName,
                                    const unsigned int reg,
                                    const unsigned int nToys
                                   );

struct histoGroup{
   TH1* truth;
   TH1* reco;
   TH1* measured;
   TH1* unfolded;
   TH1* diffxs;
};

void dataMinusBG_systematics_el(void){


   TFile inputFile("../rootFiles/dataMinusBG.tag1.toy500.nominal.root");
   if(!inputFile.IsOpen()){
      std::cerr << " ERROR Opening file.\n";
      return;
   }
   
   std::vector<std::string> systematics;
   systematics.push_back("nominal");
   systematics.push_back("jesu");
   systematics.push_back("jesd");
   systematics.push_back("btagsfup");
   systematics.push_back("btagsfdown"); 
   systematics.push_back("eeru");
   systematics.push_back("eerd");
   systematics.push_back("jeff");
   systematics.push_back("jer");
   systematics.push_back("muidu");
   systematics.push_back("muidd");
   systematics.push_back("musc");
   systematics.push_back("celloutu");
   systematics.push_back("celloutd");
   systematics.push_back("pileupu");
   systematics.push_back("pileupd");
   
   
   
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
   
   std::vector<std::string> sampleNames;
   sampleNames.push_back("McNTag1_Perm0Top1");
   sampleNames.push_back("McNTag1_Perm0Top2");
   sampleNames.push_back("McNTag1_Perm0TopLepHad");
   
   std::vector<std::string> channels;
   channels.push_back("el");
   channels.push_back("mu");
   
   std::map<std::string,std::map<std::string,std::map<std::string,histoGroup> histoList;
   
   std::string input_data_path = "../data/unfolded";
   std::string baseHistoPath = "unfolding/toys";
   const unsigned int reg = 4;
   const unsigned int nToys = 1000;
   
   // build output filename
   std::string outputFilename;
   std::stringstream outfile;
   outfile << "tag1.reg" << reg << ".toy" << nToys << ".compareNominalJesBtag.ps";
   std::string outputFilename = outfile.str();
   
   
   TCanvas can("can","can",0,0,800,600);
   
   std::vector<TH1*> meanDev,meanDevSc;
   std::vector<TH1*> rmsDev,rmsDevSc;
   std::vector<std::string> legendString;
   
   int truth_color = kBlack;
   int reco_color = kRed;
   int measured_color = kBlue;
   int unfolded_color = kGreen;
   int diffxs_color = kOrange;
   
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
      
      for(unsigned int channelNum = 0;channelNum<channels.size();++channelNum){
         std::string channel = channels[channelNum];
         
         for(unsigned int sysNum = 0;sysNum<systematics.size();++sysNum){
            std::string systematic = systematics[sysNum];
            
            histoGroup hg;
            
            std::string name = get_truth_histo_name(baseHistoPath,channel,sampleName,reg,nToys);
            hg.truth = (TH1*)inputFile->Get(name.c_str());
            hg.truth->SetMarkerColor(truth_color);
            hg.truth->SetLineColor(truth_color);
            hg.truth->SetMarkerStyle(styles[systematic]);
            hg.truth->SetTitle("");
            
            name = get_reco_histo_name(baseHistoPath,channel,sampleName,reg,nToys);
            hg.reco = (TH1*)inputFile->Get(name.c_str());
            hg.reco->SetMarkerColor(reco_color);
            hg.reco->SetLineColor(reco_color);
            hg.reco->SetMarkerStyle(styles[systematic]);
            hg.reco->SetTitle("");
            
            name = get_reco_histo_name(baseHistoPath,channel,sampleName,reg,nToys);
            hg.reco = (TH1*)inputFile->Get(name.c_str());
            hg.reco->SetMarkerColor(reco_color);
            hg.reco->SetLineColor(reco_color);
            hg.reco->SetMarkerStyle(styles[systematic]);
            hg.reco->SetTitle("");
            
            
            
            histoList[sampleName][channel][systematic]
            
            
            
         } // for(systematics)
         
      }// for(channels)
   }// for(sampleNames)
   
   
   
   
   // draw el/top1
   can.Clear();
   can.Divide(2,2);
   legendString.push_back("ejets/lepHad");
   elOrMu = "el";
   sampleName = "McNTag1_Perm0TopLepHad";
   
   /// MC TRUTH
   can.cd(1);
   std::stringstream nameBase;
   nameBase << basePath << elOrMu << "/" << sampleName << "/SVD/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_SVD_reg" << reg << "_toy" << nToys;
   std::string name = nameBase.str() + "_mc_truth";
   std::cout << "retreiving: " << name << "\n";
   
   std::stringstream nameBaseB;
   nameBaseB << basePath << elOrMu << "/" << sampleName << "/SVD/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_SVD_reg" << reg << "_toy" << nToys;
   std::string nameB = nameBaseB.str() + "_mc_truth";
   std::cout << "retreiving: " << nameB << "\n";
   
   TH1* truth_nominal = nominal.Get(name.c_str());
   truth_nominal->SetMarkerColor(truth_color);
   truth_nominal->SetLineColor(truth_color);
   truth_nominal->SetMarkerStyle(nominal_marker_style);
   truth_nominal->SetTitle("");
   truth_nominal->SetMaximum(maximum);
   truth_nominal->Draw();
   
   TH1* truth_jesu = jesu.Get(name.c_str());
   truth_jesu->SetMarkerColor(truth_color);
   truth_jesu->SetLineColor(truth_color);
   truth_jesu->SetMarkerStyle(jesu_marker_style);
   truth_jesu->SetTitle("");
   truth_jesu->SetMaximum(maximum);
   truth_jesu->Draw("same");
   
   TH1* truth_jesd = jesd.Get(name.c_str());
   truth_jesd->SetMarkerColor(truth_color);
   truth_jesd->SetLineColor(truth_color);
   truth_jesd->SetMarkerStyle(jesd_marker_style);
   truth_jesd->SetTitle("");
   truth_jesd->SetMaximum(maximum);
   truth_jesd->Draw("same");
      
   TLegend truth_leg(leg_x1,leg_y1,leg_x2,leg_y2,"truth, ejets, lepHad");
   truth_leg.SetFillStyle(0);
   truth_leg.SetBorderSize(0);
   truth_leg.AddEntry(truth_nominal,"nominal (MC@NLO)");
   truth_leg.AddEntry(truth_jesu,"JES up (MC@NLO)");
   truth_leg.AddEntry(truth_jesd,"JES down (MC@NLO)");
   truth_leg.Draw("same");
   
   
   /// MC RECO
   can.cd(2);
   
   name = nameBase.str() + "_mc_reco";
   nameB = nameBaseB.str() + "_mc_reco";
   std::cout << "retreiving: " << name << "\n";
   TH1* reco_nominal = nominal.Get(name.c_str());
   reco_nominal->SetMarkerColor(reco_color);
   reco_nominal->SetLineColor(reco_color);
   reco_nominal->SetMarkerStyle(nominal_marker_style);
   reco_nominal->SetTitle("");
   reco_nominal->SetMaximum(maximum);
   reco_nominal->Draw();
   
   TH1* reco_jesu = jesu.Get(name.c_str());
   reco_jesu->SetMarkerColor(reco_color);
   reco_jesu->SetLineColor(reco_color);
   reco_jesu->SetMarkerStyle(jesu_marker_style);
   reco_jesu->SetTitle("");
   reco_jesu->SetMaximum(maximum);
   reco_jesu->Draw("same");
   
   TH1* reco_jesd = jesd.Get(name.c_str());
   reco_jesd->SetMarkerColor(reco_color);
   reco_jesd->SetLineColor(reco_color);
   reco_jesd->SetMarkerStyle(jesd_marker_style);
   reco_jesd->SetTitle("");
   reco_jesd->SetMaximum(maximum);
   reco_jesd->Draw("same");
   
   TLegend reco_leg(leg_x1,leg_y1,leg_x2,leg_y2,"reco, ejets, lepHad");
   reco_leg.SetFillStyle(0);
   reco_leg.SetBorderSize(0);
   reco_leg.AddEntry(reco_nominal,"nominal (MC@NLO)");
   reco_leg.AddEntry(reco_jesu,"JES up (MC@NLO)");
   reco_leg.AddEntry(reco_jesd,"JES down (MC@NLO)");
   reco_leg.Draw("same");
   
   /// DATA - BG
   can.cd(4);
   
   name = nameBase.str() + "_data_reco";
   nameB = nameBaseB.str() + "_data_reco";
   std::cout << "retreiving: " << name << "\n";
   TH1* measured_nominal = nominal.Get(name.c_str());
   measured_nominal->SetMarkerColor(measured_color);
   measured_nominal->SetLineColor(measured_color);
   measured_nominal->SetMarkerStyle(nominal_marker_style);
   measured_nominal->SetTitle("");
   measured_nominal->SetMaximum(maximum);
   measured_nominal->Draw();
   
   TH1* measured_jesu = jesu.Get(name.c_str());
   measured_jesu->SetMarkerColor(measured_color);
   measured_jesu->SetLineColor(measured_color);
   measured_jesu->SetMarkerStyle(jesu_marker_style);
   measured_jesu->SetTitle("");
   measured_jesu->SetMaximum(maximum);
   measured_jesu->Draw("same");
   
   TH1* measured_jesd = jesd.Get(name.c_str());
   measured_jesd->SetMarkerColor(measured_color);
   measured_jesd->SetLineColor(measured_color);
   measured_jesd->SetMarkerStyle(jesd_marker_style);
   measured_jesd->SetTitle("");
   measured_jesd->SetMaximum(maximum);
   measured_jesd->Draw("same");

   TLegend measured_leg(leg_x1,leg_y1,leg_x2,leg_y2,"measured, ejets, lepHad");
   measured_leg.SetFillStyle(0);
   measured_leg.SetBorderSize(0);
   measured_leg.AddEntry(measured_nominal,"nominal (Data-BG)");
   measured_leg.AddEntry(measured_jesu,"JES up (Data-BG)");
   measured_leg.AddEntry(measured_jesd,"JES down (Data-BG)");
   measured_leg.Draw("same");
   
   /// UNFOLDED DATA - BG
   can.cd(3);
   
   name = nameBase.str() + "_UnfoldedReco";
   nameB = nameBaseB.str() + "_UnfoldedReco";
   std::cout << "retreiving: " << name << "\n";
   TH1* unfolded_nominal = nominal.Get(name.c_str());
   unfolded_nominal->SetMarkerColor(unfolded_color);
   unfolded_nominal->SetLineColor(unfolded_color);
   unfolded_nominal->SetMarkerStyle(nominal_marker_style);
   unfolded_nominal->SetTitle("");
   unfolded_nominal->SetMaximum(maximum);
   unfolded_nominal->Draw();
  
   TH1* unfolded_jesu = jesu.Get(name.c_str());
   unfolded_jesu->SetMarkerColor(unfolded_color);
   unfolded_jesu->SetLineColor(unfolded_color);
   unfolded_jesu->SetMarkerStyle(jesu_marker_style);
   unfolded_jesu->SetTitle("");
   unfolded_jesu->SetMaximum(maximum);
   unfolded_jesu->Draw("same");
   
   TH1* unfolded_jesd = jesd.Get(name.c_str());
   unfolded_jesd->SetMarkerColor(unfolded_color);
   unfolded_jesd->SetLineColor(unfolded_color);
   unfolded_jesd->SetMarkerStyle(jesd_marker_style);
   unfolded_jesd->SetTitle("");
   unfolded_jesd->SetMaximum(maximum);
   unfolded_jesd->Draw("same");
   
   TLegend unfolded_leg(leg_x1,leg_y1,leg_x2,leg_y2,"unfolded, ejets, lepHad");
   unfolded_leg.SetFillStyle(0);
   unfolded_leg.SetBorderSize(0);
   unfolded_leg.AddEntry(unfolded_nominal,"nominal (Data-BG)");
   unfolded_leg.AddEntry(unfolded_jesu,"JES up (Data-BG)");
   unfolded_leg.AddEntry(unfolded_jesd,"JES down (Data-BG)");
   unfolded_leg.Draw("same");
   
   can.SaveAs(std::string(outputFilename + "(").c_str());

   //b-tag SF systematic
   can.Clear();
   can.Divide(2,2);
   
   /// MC TRUTH b-tag
   can.cd(1);
   
   std::stringstream nameBase;
   nameBase << basePath << elOrMu << "/" << sampleName << "/SVD/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_SVD_reg" << reg << "_toy" << nToys;
   std::string name = nameBase.str() + "_mc_truth";
   std::cout << "retreiving: " << name << "\n";
   
   std::stringstream nameBaseB;
   nameBaseB << basePath << elOrMu << "/" << sampleName << "/SVD/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_SVD_reg" << reg << "_toy" << nToys;
   std::string nameB = nameBaseB.str() + "_mc_truth";
   std::cout << "retreiving: " << nameB << "\n";
   
   truth_nominal->SetMarkerColor(truth_color);
   truth_nominal->SetLineColor(truth_color);
   truth_nominal->SetMarkerStyle(nominal_marker_style);
   truth_nominal->SetTitle("");
   truth_nominal->SetMaximum(maximum);
   truth_nominal->Draw();
   
   TH1* truth_btu = btu.Get(nameB.c_str());
   truth_btu->SetMarkerColor(truth_color);
   truth_btu->SetLineColor(truth_color);
   truth_btu->SetMarkerStyle(btu_marker_style);
   truth_btu->SetTitle("");
   truth_btu->SetMaximum(maximum);
   truth_btu->Draw("same");
     
   TH1* truth_btd = btd.Get(nameB.c_str());
   truth_btd->SetMarkerColor(truth_color);
   truth_btd->SetLineColor(truth_color);
   truth_btd->SetMarkerStyle(btd_marker_style);
   truth_btd->SetTitle("");
   truth_btd->SetMaximum(maximum);
   truth_btd->Draw("same");
   
   TLegend truth_leg(leg_x1,leg_y1,leg_x2,leg_y2,"truth, ejets, lepHad");
   truth_leg.SetFillStyle(0);
   truth_leg.SetBorderSize(0);
   truth_leg.AddEntry(truth_nominal,"nominal (MC@NLO)");
   truth_leg.AddEntry(truth_btu,"BTAGSF up (MC@NLO)");
   truth_leg.AddEntry(truth_btd,"BTAGSF down (MC@NLO)");
   truth_leg.Draw("same");
   
   
   /// MC RECO
   can.cd(2);
   
   name = nameBase.str() + "_mc_reco";
   nameB = nameBaseB.str() + "_mc_reco";
   std::cout << "retreiving: " << name << "\n";

   reco_nominal->SetMarkerColor(reco_color);
   reco_nominal->SetLineColor(reco_color);
   reco_nominal->SetMarkerStyle(nominal_marker_style);
   reco_nominal->SetTitle("");
   reco_nominal->SetMaximum(maximum);
   reco_nominal->Draw();
      
   TH1* reco_btu = btu.Get(nameB.c_str());
   reco_btu->SetMarkerColor(reco_color);
   reco_btu->SetLineColor(reco_color);
   reco_btu->SetMarkerStyle(btu_marker_style);
   reco_btu->SetTitle("");
   reco_btu->SetMaximum(maximum);
   reco_btu->Draw("same");
   
   TH1* reco_btd = btd.Get(nameB.c_str());
   reco_btd->SetMarkerColor(reco_color);
   reco_btd->SetLineColor(reco_color);
   reco_btd->SetMarkerStyle(btd_marker_style);
   reco_btd->SetTitle("");
   reco_btd->SetMaximum(maximum);
   reco_btd->Draw("same");
   
   TLegend reco_leg(leg_x1,leg_y1,leg_x2,leg_y2,"reco, ejets, lepHad");
   reco_leg.SetFillStyle(0);
   reco_leg.SetBorderSize(0);
   reco_leg.AddEntry(reco_nominal,"nominal (MC@NLO)");
   reco_leg.AddEntry(reco_btu,"BTAGSF up (MC@NLO)");
   reco_leg.AddEntry(reco_btd,"BTAGSF down (MC@NLO)");
   reco_leg.Draw("same");
   
   /// DATA - BG
   can.cd(4);
   
   name = nameBase.str() + "_data_reco";
   nameB = nameBaseB.str() + "_data_reco";
   std::cout << "retreiving: " << name << "\n";
   
   measured_nominal->SetMarkerColor(measured_color);
   measured_nominal->SetLineColor(measured_color);
   measured_nominal->SetMarkerStyle(nominal_marker_style);
   measured_nominal->SetTitle("");
   measured_nominal->SetMaximum(maximum);
   measured_nominal->Draw();
   
   TH1* measured_btu = btu.Get(nameB.c_str());
   measured_btu->SetMarkerColor(measured_color);
   measured_btu->SetLineColor(measured_color);
   measured_btu->SetMarkerStyle(btu_marker_style);
   measured_btu->SetTitle("");
   measured_btu->SetMaximum(maximum);
   measured_btu->Draw("same");
   
   TH1* measured_btd = btd.Get(nameB.c_str());
   measured_btd->SetMarkerColor(measured_color);
   measured_btd->SetLineColor(measured_color);
   measured_btd->SetMarkerStyle(btd_marker_style);
   measured_btd->SetTitle("");
   measured_btd->SetMaximum(maximum);
   measured_btd->Draw("same");
   
   TLegend measured_leg(leg_x1,leg_y1,leg_x2,leg_y2,"measured, ejets, lepHad");
   measured_leg.SetFillStyle(0);
   measured_leg.SetBorderSize(0);
   measured_leg.AddEntry(measured_nominal,"nominal (Data-BG)");
   measured_leg.AddEntry(measured_btu,"BTAGSF up (Data-BG)");
   measured_leg.AddEntry(measured_btd,"BTAGSF down (Data-BG)");
   measured_leg.Draw("same");
   
   /// UNFOLDED DATA - BG
   can.cd(3);
   
   name = nameBase.str() + "_UnfoldedReco";
   nameB = nameBaseB.str() + "_UnfoldedReco";
   std::cout << "retreiving: " << name << "\n";
   
   unfolded_nominal->SetMarkerColor(unfolded_color);
   unfolded_nominal->SetLineColor(unfolded_color);
   unfolded_nominal->SetMarkerStyle(nominal_marker_style);
   unfolded_nominal->SetTitle("");
   unfolded_nominal->SetMaximum(maximum);
   unfolded_nominal->Draw();
   
   TH1* unfolded_btu = btu.Get(nameB.c_str());
   unfolded_btu->SetMarkerColor(unfolded_color);
   unfolded_btu->SetLineColor(unfolded_color);
   unfolded_btu->SetMarkerStyle(btu_marker_style);
   unfolded_btu->SetTitle("");
   unfolded_btu->SetMaximum(maximum);
   unfolded_btu->Draw("same");
   
   TH1* unfolded_btd = btd.Get(nameB.c_str());
   unfolded_btd->SetMarkerColor(unfolded_color);
   unfolded_btd->SetLineColor(unfolded_color);
   unfolded_btd->SetMarkerStyle(btd_marker_style);
   unfolded_btd->SetTitle("");
   unfolded_btd->SetMaximum(maximum);
   unfolded_btd->Draw("same");
   
   TLegend unfolded_leg(leg_x1,leg_y1,leg_x2,leg_y2,"unfolded, ejets, lepHad");
   unfolded_leg.SetFillStyle(0);
   unfolded_leg.SetBorderSize(0);
   unfolded_leg.AddEntry(unfolded_nominal,"nominal (Data-BG)");
   unfolded_leg.AddEntry(unfolded_btu,"BTAGSF up (Data-BG)");
   unfolded_leg.AddEntry(unfolded_btd,"BTAGSF down (Data-BG)");
   unfolded_leg.Draw("same");
   
   can.SaveAs(std::string(outputFilename + "(").c_str());
   
 
   can.Clear();
   can.Divide(2,2);
   
   /// MC TRUTH
   can.cd(1);
   
   std::stringstream nameBase;
   nameBase << basePath << elOrMu << "/" << sampleName << "/SVD/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_SVD_reg" << reg << "_toy" << nToys;
   std::string name = nameBase.str() + "_mc_truth";
   std::cout << "retreiving: " << name << "\n";
   
   std::stringstream nameBaseB;
   nameBaseB << basePath << elOrMu << "/" << sampleName << "/SVD/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_SVD_reg" << reg << "_toy" << nToys;
   std::string nameB = nameBaseB.str() + "_mc_truth";
   std::cout << "retreiving: " << nameB << "\n";
   
   truth_nominal->SetMarkerColor(truth_color);
   truth_nominal->SetLineColor(truth_color);
   truth_nominal->SetMarkerStyle(nominal_marker_style);
   truth_nominal->SetTitle("");
   truth_nominal->SetMaximum(maximum);
   truth_nominal->Draw();
   
   TH1* truth_eeru = eeru.Get(nameB.c_str());
   truth_eeru->SetMarkerColor(truth_color);
   truth_eeru->SetLineColor(truth_color);
   truth_eeru->SetMarkerStyle(eeru_marker_style);
   truth_eeru->SetTitle("");
   truth_eeru->SetMaximum(maximum);
   truth_eeru->Draw("same");
   
   TH1* truth_eerd = eerd.Get(nameB.c_str());
   truth_eerd->SetMarkerColor(truth_color);
   truth_eerd->SetLineColor(truth_color);
   truth_eerd->SetMarkerStyle(eerd_marker_style);
   truth_eerd->SetTitle("");
   truth_eerd->SetMaximum(maximum);
   truth_eerd->Draw("same");
   
   TLegend truth_leg(leg_x1,leg_y1,leg_x2,leg_y2,"truth, ejets, lepHad");
   truth_leg.SetFillStyle(0);
   truth_leg.SetBorderSize(0);
   truth_leg.AddEntry(truth_nominal,"nominal (MC@NLO)");
   truth_leg.AddEntry(truth_eeru,"EER up (MC@NLO)");
   truth_leg.AddEntry(truth_eerd,"EER down (MC@NLO)");
   truth_leg.Draw("same");
   
   
   /// MC RECO
   can.cd(2);
   
   name = nameBase.str() + "_mc_reco";
   nameB = nameBaseB.str() + "_mc_reco";
   std::cout << "retreiving: " << name << "\n";

   reco_nominal->SetMarkerColor(reco_color);
   reco_nominal->SetLineColor(reco_color);
   reco_nominal->SetMarkerStyle(nominal_marker_style);
   reco_nominal->SetTitle("");
   reco_nominal->SetMaximum(maximum);
   reco_nominal->Draw();

   TH1* reco_eeru = eeru.Get(nameB.c_str());
   reco_eeru->SetMarkerColor(reco_color);
   reco_eeru->SetLineColor(reco_color);
   reco_eeru->SetMarkerStyle(eeru_marker_style);
   reco_eeru->SetTitle("");
   reco_eeru->SetMaximum(maximum);
   reco_eeru->Draw("same");
   
   TH1* reco_eerd = eerd.Get(nameB.c_str());
   reco_eerd->SetMarkerColor(reco_color);
   reco_eerd->SetLineColor(reco_color);
   reco_eerd->SetMarkerStyle(eerd_marker_style);
   reco_eerd->SetTitle("");
   reco_eerd->SetMaximum(maximum);
   reco_eerd->Draw("same");
   
   TLegend reco_leg(leg_x1,leg_y1,leg_x2,leg_y2,"reco, ejets, lepHad");
   reco_leg.SetFillStyle(0);
   reco_leg.SetBorderSize(0);
   reco_leg.AddEntry(reco_nominal,"nominal (MC@NLO)");
   reco_leg.AddEntry(reco_eeru,"EER (MC@NLO)");
   reco_leg.AddEntry(reco_eerd,"EER (MC@NLO)");
   reco_leg.Draw("same");
   
   /// DATA - BG
   can.cd(4);
   
   name = nameBase.str() + "_data_reco";
   nameB = nameBaseB.str() + "_data_reco";
   std::cout << "retreiving: " << name << "\n";
   
   measured_nominal->SetMarkerColor(measured_color);
   measured_nominal->SetLineColor(measured_color);
   measured_nominal->SetMarkerStyle(nominal_marker_style);
   measured_nominal->SetTitle("");
   measured_nominal->SetMaximum(maximum);
   measured_nominal->Draw();
   
   TH1* measured_eeru = eeru.Get(nameB.c_str());
   measured_eeru->SetMarkerColor(measured_color);
   measured_eeru->SetLineColor(measured_color);
   measured_eeru->SetMarkerStyle(eeru_marker_style);
   measured_eeru->SetTitle("");
   measured_eeru->SetMaximum(maximum);
   measured_eeru->Draw("same");
   
   TH1* measured_eerd = eerd.Get(nameB.c_str());
   measured_eerd->SetMarkerColor(measured_color);
   measured_eerd->SetLineColor(measured_color);
   measured_eerd->SetMarkerStyle(eerd_marker_style);
   measured_eerd->SetTitle("");
   measured_eerd->SetMaximum(maximum);
   measured_eerd->Draw("same");
   
   TLegend measured_leg(leg_x1,leg_y1,leg_x2,leg_y2,"measured, ejets, lepHad");
   measured_leg.SetFillStyle(0);
   measured_leg.SetBorderSize(0);
   measured_leg.AddEntry(measured_nominal,"nominal (Data-BG)");
   measured_leg.AddEntry(measured_eeru,"EER up (Data-BG)");
   measured_leg.AddEntry(measured_eerd,"EER (Data-BG)");
   measured_leg.Draw("same");
   
   /// UNFOLDED DATA - BG
   can.cd(3);
   
   name = nameBase.str() + "_UnfoldedReco";
   nameB = nameBaseB.str() + "_UnfoldedReco";
   std::cout << "retreiving: " << name << "\n";
   
   unfolded_nominal->SetMarkerColor(unfolded_color);
   unfolded_nominal->SetLineColor(unfolded_color);
   unfolded_nominal->SetMarkerStyle(nominal_marker_style);
   unfolded_nominal->SetTitle("");
   unfolded_nominal->SetMaximum(maximum);
   unfolded_nominal->Draw();
   
   TH1* unfolded_eeru = eeru.Get(nameB.c_str());
   unfolded_eeru->SetMarkerColor(unfolded_color);
   unfolded_eeru->SetLineColor(unfolded_color);
   unfolded_eeru->SetMarkerStyle(eeru_marker_style);
   unfolded_eeru->SetTitle("");
   unfolded_eeru->SetMaximum(maximum);
   unfolded_eeru->Draw("same");
   
   TH1* unfolded_eerd = eerd.Get(nameB.c_str());
   unfolded_eerd->SetMarkerColor(unfolded_color);
   unfolded_eerd->SetLineColor(unfolded_color);
   unfolded_eerd->SetMarkerStyle(eerd_marker_style);
   unfolded_eerd->SetTitle("");
   unfolded_eerd->SetMaximum(maximum);
   unfolded_eerd->Draw("same");
   
   TLegend unfolded_leg(leg_x1,leg_y1,leg_x2,leg_y2,"unfolded, ejets, lepHad");
   unfolded_leg.SetFillStyle(0);
   unfolded_leg.SetBorderSize(0);
   unfolded_leg.AddEntry(unfolded_nominal,"nominal (Data-BG)");
   unfolded_leg.AddEntry(unfolded_eeru,"EER up (Data-BG)");
   unfolded_leg.AddEntry(unfolded_eerd,"EER down (Data-BG)");   
   unfolded_leg.Draw("same");
   
   can.SaveAs(std::string(outputFilename + "(").c_str());
   
   
   can.Clear();
   can.Divide(2,2);
   
   /// MC TRUTH
   can.cd(1);
   
   std::stringstream nameBase;
   nameBase << basePath << elOrMu << "/" << sampleName << "/SVD/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_SVD_reg" << reg << "_toy" << nToys;
   std::string name = nameBase.str() + "_mc_truth";
   std::cout << "retreiving: " << name << "\n";
   
   std::stringstream nameBaseB;
   nameBaseB << basePath << elOrMu << "/" << sampleName << "/SVD/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_SVD_reg" << reg << "_toy" << nToys;
   std::string nameB = nameBaseB.str() + "_mc_truth";
   std::cout << "retreiving: " << nameB << "\n";
   
   truth_nominal->SetMarkerColor(truth_color);
   truth_nominal->SetLineColor(truth_color);
   truth_nominal->SetMarkerStyle(nominal_marker_style);
   truth_nominal->SetTitle("");
   truth_nominal->SetMaximum(maximum);
   truth_nominal->Draw();
   
   TH1* truth_jer = jer.Get(nameB.c_str());
   truth_jer->SetMarkerColor(truth_color);
   truth_jer->SetLineColor(truth_color);
   truth_jer->SetMarkerStyle(jer_marker_style);
   truth_jer->SetTitle("");
   truth_jer->SetMaximum(maximum);
   truth_jer->Draw("same");
   
   TLegend truth_leg(leg_x1,leg_y1,leg_x2,leg_y2,"truth, ejets, lepHad");
   truth_leg.SetFillStyle(0);
   truth_leg.SetBorderSize(0);
   truth_leg.AddEntry(truth_nominal,"nominal (MC@NLO)");
   truth_leg.AddEntry(truth_jer,"JER (MC@NLO)");
   truth_leg.Draw("same");
   
   
   /// MC RECO
   can.cd(2);
   
   name = nameBase.str() + "_mc_reco";
   nameB = nameBaseB.str() + "_mc_reco";
   std::cout << "retreiving: " << name << "\n";

   reco_nominal->SetMarkerColor(reco_color);
   reco_nominal->SetLineColor(reco_color);
   reco_nominal->SetMarkerStyle(nominal_marker_style);
   reco_nominal->SetTitle("");
   reco_nominal->SetMaximum(maximum);
   reco_nominal->Draw();
   
   TH1* reco_jer = jer.Get(nameB.c_str());
   reco_jer->SetMarkerColor(reco_color);
   reco_jer->SetLineColor(reco_color);
   reco_jer->SetMarkerStyle(jer_marker_style);
   reco_jer->SetTitle("");
   reco_jer->SetMaximum(maximum);
   reco_jer->Draw("same");
   
   TLegend reco_leg(leg_x1,leg_y1,leg_x2,leg_y2,"reco, ejets, lepHad");
   reco_leg.SetFillStyle(0);
   reco_leg.SetBorderSize(0);
   reco_leg.AddEntry(reco_nominal,"nominal (MC@NLO)");
   reco_leg.AddEntry(reco_jer,"JER (MC@NLO)");
   reco_leg.Draw("same");
   
   /// DATA - BG
   can.cd(4);
   
   name = nameBase.str() + "_data_reco";
   nameB = nameBaseB.str() + "_data_reco";
   std::cout << "retreiving: " << name << "\n";
   
   measured_nominal->SetMarkerColor(measured_color);
   measured_nominal->SetLineColor(measured_color);
   measured_nominal->SetMarkerStyle(nominal_marker_style);
   measured_nominal->SetTitle("");
   measured_nominal->SetMaximum(maximum);
   measured_nominal->Draw();
   
   TH1* measured_jer = jer.Get(nameB.c_str());
   measured_jer->SetMarkerColor(measured_color);
   measured_jer->SetLineColor(measured_color);
   measured_jer->SetMarkerStyle(jer_marker_style);
   measured_jer->SetTitle("");
   measured_jer->SetMaximum(maximum);
   measured_jer->Draw("same");
   
   TLegend measured_leg(leg_x1,leg_y1,leg_x2,leg_y2,"measured, ejets, lepHad");
   measured_leg.SetFillStyle(0);
   measured_leg.SetBorderSize(0);
   measured_leg.AddEntry(measured_nominal,"nominal (Data-BG)");
   measured_leg.AddEntry(measured_jer,"JER (Data-BG)");
   measured_leg.Draw("same");
   
   /// UNFOLDED DATA - BG
   can.cd(3);
   
   name = nameBase.str() + "_UnfoldedReco";
   nameB = nameBaseB.str() + "_UnfoldedReco";
   std::cout << "retreiving: " << name << "\n";
   
   unfolded_nominal->SetMarkerColor(unfolded_color);
   unfolded_nominal->SetLineColor(unfolded_color);
   unfolded_nominal->SetMarkerStyle(nominal_marker_style);
   unfolded_nominal->SetTitle("");
   unfolded_nominal->SetMaximum(maximum);
   unfolded_nominal->Draw();
   
   TH1* unfolded_jer = jer.Get(nameB.c_str());
   unfolded_jer->SetMarkerColor(unfolded_color);
   unfolded_jer->SetLineColor(unfolded_color);
   unfolded_jer->SetMarkerStyle(jer_marker_style);
   unfolded_jer->SetTitle("");
   unfolded_jer->SetMaximum(maximum);
   unfolded_jer->Draw("same");
   
   TLegend unfolded_leg(leg_x1,leg_y1,leg_x2,leg_y2,"unfolded, ejets, lepHad");
   unfolded_leg.SetFillStyle(0);
   unfolded_leg.SetBorderSize(0);
   unfolded_leg.AddEntry(unfolded_nominal,"nominal (Data-BG)");
   unfolded_leg.AddEntry(unfolded_jer,"JER (Data-BG)");
   unfolded_leg.Draw("same");
   
   can.SaveAs(std::string(outputFilename + "(").c_str());
   

   /// spectra again, but nominal all on one, etc.
   can.Clear();
   
   truth_nominal->Draw();
   reco_nominal->Draw("same");
   measured_nominal->Draw("same");
   unfolded_nominal->Draw("same");
   
   TLegend nominal_leg(leg_x1,leg_y1,leg_x2,leg_y2,"nominal, ejets, lepHad");
   nominal_leg.SetFillStyle(0);
   nominal_leg.SetBorderSize(0);
   nominal_leg.AddEntry(truth_nominal,"truth (MC@NLO)");
   nominal_leg.AddEntry(reco_nominal,"reco (MC@NLO)");
   nominal_leg.AddEntry(measured_nominal,"measured (Data-BG)");
   nominal_leg.AddEntry(unfolded_nominal,"unfolded (Data-BG)");
   nominal_leg.Draw("same");
   

   can.SaveAs(outputFilename.c_str());
      
   can.Clear();
   can.Divide(2,1);
         
   can.cd(1);
   truth_jesd->Draw();
   reco_jesd->Draw("same");
   measured_jesd->Draw("same");
   unfolded_jesd->Draw("same");
   
   TLegend jesd_leg(leg_x1,leg_y1,leg_x2,leg_y2,"JES down, ejets, lepHad");
   jesd_leg.SetFillStyle(0);
   jesd_leg.SetBorderSize(0);
   jesd_leg.AddEntry(truth_jesd,"truth (MC@NLO)");
   jesd_leg.AddEntry(reco_jesd,"reco (MC@NLO)");
   jesd_leg.AddEntry(measured_jesd,"measured (Data-BG)");
   jesd_leg.AddEntry(unfolded_jesd,"unfolded (Data-BG)");
   jesd_leg.Draw("same");
   
   can.cd(2);
   truth_jesu->Draw();
   reco_jesu->Draw("same");
   measured_jesu->Draw("same");
   unfolded_jesu->Draw("same");
   
   TLegend jesu_leg(leg_x1,leg_y1,leg_x2,leg_y2,"JES up, ejets, lepHad");
   jesu_leg.SetFillStyle(0);
   jesu_leg.SetBorderSize(0);
   jesu_leg.AddEntry(truth_jesu,"truth (MC@NLO)");
   jesu_leg.AddEntry(reco_jesu,"reco (MC@NLO)");
   jesu_leg.AddEntry(measured_jesu,"measured (Data-BG)");
   jesu_leg.AddEntry(unfolded_jesu,"unfolded (Data-BG)");
   jesu_leg.Draw("same");
   

   can.SaveAs(outputFilename.c_str());
   
   can.Clear();
   can.Divide(2,1);
           
   can.cd(1);
   truth_btd->Draw();
   reco_btd->Draw("same");
   measured_btd->Draw("same");
   unfolded_btd->Draw("same");
   
   TLegend btd_leg(leg_x1,leg_y1,leg_x2,leg_y2,"BTAGSF down, ejets, lepHad");
   btd_leg.SetFillStyle(0);
   btd_leg.SetBorderSize(0);
   btd_leg.AddEntry(truth_btd,"truth (MC@NLO)");
   btd_leg.AddEntry(reco_btd,"reco (MC@NLO)");
   btd_leg.AddEntry(measured_btd,"measured (Data-BG)");
   btd_leg.AddEntry(unfolded_btd,"unfolded (Data-BG)");
   btd_leg.Draw("same");
   
   can.cd(2);
   truth_btu->Draw();
   reco_btu->Draw("same");
   measured_btu->Draw("same");
   unfolded_btu->Draw("same");
   
   TLegend btu_leg(leg_x1,leg_y1,leg_x2,leg_y2,"BTAGSF up, ejets, lepHad");
   btu_leg.SetFillStyle(0);
   btu_leg.SetBorderSize(0);
   btu_leg.AddEntry(truth_btu,"truth (MC@NLO)");
   btu_leg.AddEntry(reco_btu,"reco (MC@NLO)");
   btu_leg.AddEntry(measured_btu,"measured (Data-BG)");
   btu_leg.AddEntry(unfolded_btu,"unfolded (Data-BG)");
   btu_leg.Draw("same");
   
   
   can.SaveAs(outputFilename.c_str());
   
   can.Clear();
   can.Divide(2,1);
           
   can.cd(1);
   truth_eerd->Draw();
   reco_eerd->Draw("same");
   measured_eerd->Draw("same");
   unfolded_eerd->Draw("same");
   
   TLegend eerd_leg(leg_x1,leg_y1,leg_x2,leg_y2,"EER down, ejets, lepHad");
   eerd_leg.SetFillStyle(0);
   eerd_leg.SetBorderSize(0);
   eerd_leg.AddEntry(truth_eerd,"truth (MC@NLO)");
   eerd_leg.AddEntry(reco_eerd,"reco (MC@NLO)");
   eerd_leg.AddEntry(measured_eerd,"measured (Data-BG)");
   eerd_leg.AddEntry(unfolded_eerd,"unfolded (Data-BG)");
   eerd_leg.Draw("same");
   
   can.cd(2);
   truth_eeru->Draw();
   reco_eeru->Draw("same");
   measured_eeru->Draw("same");
   unfolded_eeru->Draw("same");
   
   TLegend eeru_leg(leg_x1,leg_y1,leg_x2,leg_y2,"EER up, ejets, lepHad");
   eeru_leg.SetFillStyle(0);
   eeru_leg.SetBorderSize(0);
   eeru_leg.AddEntry(truth_eeru,"truth (MC@NLO)");
   eeru_leg.AddEntry(reco_eeru,"reco (MC@NLO)");
   eeru_leg.AddEntry(measured_eeru,"measured (Data-BG)");
   eeru_leg.AddEntry(unfolded_eeru,"unfolded (Data-BG)");
   eeru_leg.Draw("same");
   
   
   can.SaveAs(outputFilename.c_str());
   
  
   can.Clear();
   can.Divide(2,1);
           
   can.cd(1);
   truth_jer->Draw();
   reco_jer->Draw("same");
   measured_jer->Draw("same");
   unfolded_jer->Draw("same");
   
   TLegend jer_leg(leg_x1,leg_y1,leg_x2,leg_y2,"JER, ejets, lepHad");
   jer_leg.SetFillStyle(0);
   jer_leg.SetBorderSize(0);
   jer_leg.AddEntry(truth_jer,"truth (MC@NLO)");
   jer_leg.AddEntry(reco_jer,"reco (MC@NLO)");
   jer_leg.AddEntry(measured_jer,"measured (Data-BG)");
   jer_leg.AddEntry(unfolded_jer,"unfolded (Data-BG)");
   jer_leg.Draw("same");
   
   can.cd(2);
   
   
   can.SaveAs(outputFilename.c_str());
   
   /// MIGRATION MATRICES
   can.Clear();
   can.cd(1)->SetLogz(1);
   gStyle->SetPaintTextFormat("1.1f");
   
   name = nameBase.str() + "_mc_migration";
   nameB = nameBaseB.str() + "_mc_migration";
   TH1* truth_migration = nominal.Get(name.c_str());
   truth_migration->SetTitle("nominal");
   gStyle->SetPaintTextFormat(".2g");
   truth_migration->Draw("col");
   truth_migration->Draw("textsame");
   
   can.SaveAs(outputFilename.c_str());
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   can.cd(1)->SetLogz(1);
   gStyle->SetPaintTextFormat("1.1f");
      
   TH1* jesd_migration = jesd.Get(name.c_str());
   jesd_migration->SetTitle("JES down");
   gStyle->SetPaintTextFormat(".2g");
   jesd_migration->Draw("col");
   jesd_migration->Draw("textsame");
   
   can.cd(2);
   can.cd(2)->SetLogz(1);
   gStyle->SetPaintTextFormat("1.1f");
   
   TH1* jesu_migration = jesu.Get(name.c_str());
   jesu_migration->SetTitle("JES up");
   gStyle->SetPaintTextFormat(".2g");
   jesu_migration->Draw("col");
   jesu_migration->Draw("textsame");
   
   can.SaveAs(outputFilename.c_str());
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   can.cd(1)->SetLogz(1);
   gStyle->SetPaintTextFormat("1.1f");
      
   TH1* btd_migration = btd.Get(nameB.c_str());
   btd_migration->SetTitle("BTAGSF down");
   gStyle->SetPaintTextFormat(".2g");
   btd_migration->Draw("col");
   btd_migration->Draw("textsame");
   
   can.cd(2);
   can.cd(2)->SetLogz(1);
   gStyle->SetPaintTextFormat("1.1f");
   
   TH1* btu_migration = btu.Get(nameB.c_str());
   btu_migration->SetTitle("BTAGSF up");
   gStyle->SetPaintTextFormat(".2g");
   btu_migration->Draw("col");
   btu_migration->Draw("textsame");
   
   can.SaveAs(outputFilename.c_str());
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   can.cd(1)->SetLogz(1);
   gStyle->SetPaintTextFormat("1.1f");
      
   TH1* eerd_migration = eerd.Get(nameB.c_str());
   eerd_migration->SetTitle("EER down");
   gStyle->SetPaintTextFormat(".2g");
   eerd_migration->Draw("col");
   eerd_migration->Draw("textsame");
   
   can.cd(2);
   can.cd(2)->SetLogz(1);
   gStyle->SetPaintTextFormat("1.1f");
   
   TH1* eeru_migration = eeru.Get(nameB.c_str());
   eeru_migration->SetTitle("EER up");
   gStyle->SetPaintTextFormat(".2g");
   eeru_migration->Draw("col");
   eeru_migration->Draw("textsame");
   
   can.SaveAs(outputFilename.c_str());
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   can.cd(1)->SetLogz(1);
      
   TH1* jer_migration = jer.Get(nameB.c_str());
   jer_migration->SetTitle("JER");
   gStyle->SetPaintTextFormat(".2g");
   jer_migration->Draw("col");
   jer_migration->Draw("textsame");
   
   can.cd(2);
   
   can.SaveAs(outputFilename.c_str());
     
   /// DEVIATION
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name = nameBase.str() + "_meanDeviation";
   nameB = nameBaseB.str() + "_meanDeviation";
   TH1* nominal_meanDeviation = nominal.Get(name.c_str());
   nominal_meanDeviation->SetMarkerColor(truth_color);
   nominal_meanDeviation->SetLineColor(truth_color);
   nominal_meanDeviation->SetMarkerStyle(nominal_marker_style);
   nominal_meanDeviation->SetTitle("");
   nominal_meanDeviation->SetMaximum(maximum_meanDev);
   nominal_meanDeviation->SetMinimum(minimum_meanDev);
   nominal_meanDeviation->Draw();
   
   TH1* jesu_meanDeviation = jesu.Get(name.c_str());
   jesu_meanDeviation->SetMarkerColor(truth_color);
   jesu_meanDeviation->SetLineColor(truth_color);
   jesu_meanDeviation->SetMarkerStyle(jesu_marker_style);
   jesu_meanDeviation->SetTitle("");
   jesu_meanDeviation->Draw("same");
   
   TH1* jesd_meanDeviation = jesd.Get(name.c_str());
   jesd_meanDeviation->SetMarkerColor(truth_color);
   jesd_meanDeviation->SetLineColor(truth_color);
   jesd_meanDeviation->SetMarkerStyle(jesd_marker_style);
   jesd_meanDeviation->SetTitle("");
   jesd_meanDeviation->Draw("same");
   
   TLegend dev_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"Mean Deviation, ejets, lepHad");
   dev_leg.SetFillStyle(0);
   dev_leg.SetBorderSize(0);
   dev_leg.AddEntry(truth_nominal,"nominal");
   dev_leg.AddEntry(truth_jesu,"JES up");
   dev_leg.AddEntry(truth_jesd,"JES down");
   dev_leg.Draw("same");
   
   can.cd(2);
   
   name = nameBase.str() + "_rmsDeviation";
   nameB = nameBaseB.str() + "_rmsDeviation";
   TH1* nominal_rmsDeviation = nominal.Get(name.c_str());
   nominal_rmsDeviation->SetMarkerColor(truth_color);
   nominal_rmsDeviation->SetLineColor(truth_color);
   nominal_rmsDeviation->SetMarkerStyle(nominal_marker_style);
   nominal_rmsDeviation->SetTitle("");
   nominal_rmsDeviation->SetMaximum(maximum_rmsDev);
   nominal_rmsDeviation->SetMinimum(minimum_rmsDev);
   nominal_rmsDeviation->Draw();
   
   TH1* jesu_rmsDeviation = jesu.Get(name.c_str());
   jesu_rmsDeviation->SetMarkerColor(truth_color);
   jesu_rmsDeviation->SetLineColor(truth_color);
   jesu_rmsDeviation->SetMarkerStyle(jesu_marker_style);
   jesu_rmsDeviation->SetTitle("");
   jesu_rmsDeviation->Draw("same");
   
   TH1* jesd_rmsDeviation = jesd.Get(name.c_str());
   jesd_rmsDeviation->SetMarkerColor(truth_color);
   jesd_rmsDeviation->SetLineColor(truth_color);
   jesd_rmsDeviation->SetMarkerStyle(jesd_marker_style);
   jesd_rmsDeviation->SetTitle("");
   jesd_rmsDeviation->Draw("same");
   
   TLegend rms_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"RMS Deviation, ejets, lepHad");
   rms_leg.SetFillStyle(0);
   rms_leg.SetBorderSize(0);
   rms_leg.AddEntry(truth_nominal,"nominal");
   rms_leg.AddEntry(truth_jesu,"JES up");
   rms_leg.AddEntry(truth_jesd,"JES down");
   rms_leg.Draw("same");
   
   can.SaveAs(outputFilename.c_str());

   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name = nameBase.str() + "_meanDeviation";
   nameB = nameBaseB.str() + "_meanDeviation";

   nominal_meanDeviation->SetMarkerColor(truth_color);
   nominal_meanDeviation->SetLineColor(truth_color);
   nominal_meanDeviation->SetMarkerStyle(nominal_marker_style);
   nominal_meanDeviation->SetTitle("");
   nominal_meanDeviation->SetMaximum(maximum_meanDev);
   nominal_meanDeviation->SetMinimum(minimum_meanDev);
   nominal_meanDeviation->Draw();

   TH1* btu_meanDeviation = btu.Get(nameB.c_str());
   btu_meanDeviation->SetMarkerColor(truth_color);
   btu_meanDeviation->SetLineColor(truth_color);
   btu_meanDeviation->SetMarkerStyle(btu_marker_style);
   btu_meanDeviation->SetTitle("");
   btu_meanDeviation->Draw("same");
   
   TH1* btd_meanDeviation = btd.Get(nameB.c_str());
   btd_meanDeviation->SetMarkerColor(truth_color);
   btd_meanDeviation->SetLineColor(truth_color);
   btd_meanDeviation->SetMarkerStyle(btd_marker_style);
   btd_meanDeviation->SetTitle("");
   btd_meanDeviation->Draw("same");

   TLegend dev1_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"Mean Deviation, ejets, lepHad");
   dev1_leg.SetFillStyle(0);
   dev1_leg.SetBorderSize(0);
   dev1_leg.AddEntry(truth_nominal,"nominal");
   dev1_leg.AddEntry(truth_btu,"BTAGSF up");
   dev1_leg.AddEntry(truth_btd,"BTAGSF down");
   dev1_leg.Draw("same");
   
   can.cd(2);
   
   name = nameBase.str() + "_rmsDeviation";
   nameB = nameBaseB.str() + "_rmsDeviation";
   nominal_rmsDeviation->SetMarkerColor(truth_color);
   nominal_rmsDeviation->SetLineColor(truth_color);
   nominal_rmsDeviation->SetMarkerStyle(nominal_marker_style);
   nominal_rmsDeviation->SetTitle("");
   nominal_rmsDeviation->SetMaximum(maximum_rmsDev);
   nominal_rmsDeviation->SetMinimum(minimum_rmsDev);
   nominal_rmsDeviation->Draw();
   
   TH1* btu_rmsDeviation = btu.Get(nameB.c_str());
   btu_rmsDeviation->SetMarkerColor(truth_color);
   btu_rmsDeviation->SetLineColor(truth_color);
   btu_rmsDeviation->SetMarkerStyle(btu_marker_style);
   btu_rmsDeviation->SetTitle("");
   btu_rmsDeviation->Draw("same");
   
   TH1* btd_rmsDeviation = btd.Get(nameB.c_str());
   btd_rmsDeviation->SetMarkerColor(truth_color);
   btd_rmsDeviation->SetLineColor(truth_color);
   btd_rmsDeviation->SetMarkerStyle(btd_marker_style);
   btd_rmsDeviation->SetTitle("");
   btd_rmsDeviation->Draw("same");
   
   TLegend rms1_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"RMS Deviation, ejets, lepHad");
   rms1_leg.SetFillStyle(0);
   rms1_leg.SetBorderSize(0);
   rms1_leg.AddEntry(truth_nominal,"nominal");
   rms1_leg.AddEntry(truth_btu,"BTAGSF up");
   rms1_leg.AddEntry(truth_btd,"BTAGSF down");
   rms1_leg.Draw("same");
   
   can.SaveAs(outputFilename.c_str());
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name = nameBase.str() + "_meanDeviation";
   nameB = nameBaseB.str() + "_meanDeviation";

   nominal_meanDeviation->SetMarkerColor(truth_color);
   nominal_meanDeviation->SetLineColor(truth_color);
   nominal_meanDeviation->SetMarkerStyle(nominal_marker_style);
   nominal_meanDeviation->SetTitle("");
   nominal_meanDeviation->SetMaximum(maximum_meanDev);
   nominal_meanDeviation->SetMinimum(minimum_meanDev);
   nominal_meanDeviation->Draw();
   
   TH1* eeru_meanDeviation = eeru.Get(nameB.c_str());
   eeru_meanDeviation->SetMarkerColor(truth_color);
   eeru_meanDeviation->SetLineColor(truth_color);
   eeru_meanDeviation->SetMarkerStyle(eeru_marker_style);
   eeru_meanDeviation->SetTitle("");
   eeru_meanDeviation->Draw("same");
   
   TH1* eerd_meanDeviation = eerd.Get(nameB.c_str());
   eerd_meanDeviation->SetMarkerColor(truth_color);
   eerd_meanDeviation->SetLineColor(truth_color);
   eerd_meanDeviation->SetMarkerStyle(eerd_marker_style);
   eerd_meanDeviation->SetTitle("");
   eerd_meanDeviation->Draw("same");
   
   TLegend dev2_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"Mean Deviation, ejets, lepHad");
   dev2_leg.SetFillStyle(0);
   dev2_leg.SetBorderSize(0);
   dev2_leg.AddEntry(truth_nominal,"nominal");
   dev2_leg.AddEntry(truth_eeru,"EER up");
   dev2_leg.AddEntry(truth_eerd,"EER down");
   dev2_leg.Draw("same");
   
   can.cd(2);
   
   name = nameBase.str() + "_rmsDeviation";
   nameB = nameBaseB.str() + "_rmsDeviation";
   nominal_rmsDeviation->SetMarkerColor(truth_color);
   nominal_rmsDeviation->SetLineColor(truth_color);
   nominal_rmsDeviation->SetMarkerStyle(nominal_marker_style);
   nominal_rmsDeviation->SetTitle("");
   nominal_rmsDeviation->SetMaximum(maximum_rmsDev);
   nominal_rmsDeviation->SetMinimum(minimum_rmsDev);
   nominal_rmsDeviation->Draw();
   
   TH1* eeru_rmsDeviation = eeru.Get(nameB.c_str());
   eeru_rmsDeviation->SetMarkerColor(truth_color);
   eeru_rmsDeviation->SetLineColor(truth_color);
   eeru_rmsDeviation->SetMarkerStyle(eeru_marker_style);
   eeru_rmsDeviation->SetTitle("");
   eeru_rmsDeviation->Draw("same");
   
   TH1* eerd_rmsDeviation = eerd.Get(nameB.c_str());
   eerd_rmsDeviation->SetMarkerColor(truth_color);
   eerd_rmsDeviation->SetLineColor(truth_color);
   eerd_rmsDeviation->SetMarkerStyle(eerd_marker_style);
   eerd_rmsDeviation->SetTitle("");
   eerd_rmsDeviation->Draw("same");
   
   TLegend rms2_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"RMS Deviation, ejets, lepHad");
   rms2_leg.SetFillStyle(0);
   rms2_leg.SetBorderSize(0);
   rms2_leg.AddEntry(truth_nominal,"nominal");
   rms2_leg.AddEntry(truth_eeru,"EER up");
   rms2_leg.AddEntry(truth_eerd,"EER down");
   rms2_leg.Draw("same");
   
   can.SaveAs(outputFilename.c_str());

   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name = nameBase.str() + "_meanDeviation";
   nameB = nameBaseB.str() + "_meanDeviation";

   nominal_meanDeviation->SetMarkerColor(truth_color);
   nominal_meanDeviation->SetLineColor(truth_color);
   nominal_meanDeviation->SetMarkerStyle(nominal_marker_style);
   nominal_meanDeviation->SetTitle("");
   nominal_meanDeviation->SetMaximum(maximum_meanDev);
   nominal_meanDeviation->SetMinimum(minimum_meanDev);
   nominal_meanDeviation->Draw();
   
   TH1* jer_meanDeviation = jer.Get(nameB.c_str());
   jer_meanDeviation->SetMarkerColor(truth_color);
   jer_meanDeviation->SetLineColor(truth_color);
   jer_meanDeviation->SetMarkerStyle(jer_marker_style);
   jer_meanDeviation->SetTitle("");
   jer_meanDeviation->Draw("same");
   
   TLegend dev2_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"Mean Deviation, ejets, lepHad");
   dev2_leg.SetFillStyle(0);
   dev2_leg.SetBorderSize(0);
   dev2_leg.AddEntry(truth_nominal,"nominal");
   dev2_leg.AddEntry(truth_eeru,"JER");
   dev2_leg.Draw("same");
   
   can.cd(2);
   
   name = nameBase.str() + "_rmsDeviation";
   nameB = nameBaseB.str() + "_rmsDeviation";
   nominal_rmsDeviation->SetMarkerColor(truth_color);
   nominal_rmsDeviation->SetLineColor(truth_color);
   nominal_rmsDeviation->SetMarkerStyle(nominal_marker_style);
   nominal_rmsDeviation->SetTitle("");
   nominal_rmsDeviation->SetMaximum(maximum_rmsDev);
   nominal_rmsDeviation->SetMinimum(minimum_rmsDev);
   nominal_rmsDeviation->Draw();
   
   TH1* jer_rmsDeviation = jer.Get(nameB.c_str());
   jer_rmsDeviation->SetMarkerColor(truth_color);
   jer_rmsDeviation->SetLineColor(truth_color);
   jer_rmsDeviation->SetMarkerStyle(jer_marker_style);
   jer_rmsDeviation->SetTitle("");
   jer_rmsDeviation->Draw("same");
      
   TLegend rms2_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"RMS Deviation, ejets, lepHad");
   rms2_leg.SetFillStyle(0);
   rms2_leg.SetBorderSize(0);
   rms2_leg.AddEntry(truth_nominal,"nominal");
   rms2_leg.AddEntry(truth_eeru,"JER"); 
   rms2_leg.Draw("same");
   
   can.SaveAs(outputFilename.c_str());
   
   /// SCALED DEVIATION
   /*can.Clear();
     can.Divide(1,2);
     can.cd(1);
   
     name = nameBase.str() + "_meanDeviationScaled";
     nameB = nameBaseB.str() + "_meanDeviationScaled";
     TH1* nominal_meanDeviationScaled = nominal.Get(name.c_str());
     nominal_meanDeviationScaled->SetMarkerColor(truth_color);
     nominal_meanDeviationScaled->SetLineColor(truth_color);
     nominal_meanDeviationScaled->SetMarkerStyle(nominal_marker_style);
     nominal_meanDeviationScaled->SetTitle("");
     nominal_meanDeviationScaled->SetMaximum(maximum_meanDev);
     nominal_meanDeviationScaled->SetMinimum(minimum_meanDev);
     nominal_meanDeviationScaled->Draw();
   
     TH1* jesu_meanDeviationScaled = jesu.Get(name.c_str());
     jesu_meanDeviationScaled->SetMarkerColor(truth_color);
     jesu_meanDeviationScaled->SetLineColor(truth_color);
     jesu_meanDeviationScaled->SetMarkerStyle(jesu_marker_style);
     jesu_meanDeviationScaled->SetTitle("");
     jesu_meanDeviationScaled->Draw("same");
   
     TH1* jesd_meanDeviationScaled = jesd.Get(name.c_str());
     jesd_meanDeviationScaled->SetMarkerColor(truth_color);
     jesd_meanDeviationScaled->SetLineColor(truth_color);
     jesd_meanDeviationScaled->SetMarkerStyle(jesd_marker_style);
     jesd_meanDeviationScaled->SetTitle("");
     jesd_meanDeviationScaled->Draw("same");
   
     TH1* btu_meanDeviationScaled = btu.Get(nameB.c_str());
     btu_meanDeviationScaled->SetMarkerColor(truth_color);
     btu_meanDeviationScaled->SetLineColor(truth_color);
     btu_meanDeviationScaled->SetMarkerStyle(btu_marker_style);
     btu_meanDeviationScaled->SetTitle("");
     btu_meanDeviationScaled->Draw("same");
   
     TH1* btd_meanDeviationScaled = btd.Get(nameB.c_str());
     btd_meanDeviationScaled->SetMarkerColor(truth_color);
     btd_meanDeviationScaled->SetLineColor(truth_color);
     btd_meanDeviationScaled->SetMarkerStyle(btd_marker_style);
     btd_meanDeviationScaled->SetTitle("");
     btd_meanDeviationScaled->Draw("same");
   
     can.cd(2);
   
     name = nameBase.str() + "_rmsDeviationScaled";
     nameB = nameBaseB.str() + "_rmsDeviationScaled";
     TH1* nominal_rmsDeviationScaled = nominal.Get(name.c_str());
     nominal_rmsDeviationScaled->SetMarkerColor(truth_color);
     nominal_rmsDeviationScaled->SetLineColor(truth_color);
     nominal_rmsDeviationScaled->SetMarkerStyle(nominal_marker_style);
     nominal_rmsDeviationScaled->SetTitle("");
     nominal_rmsDeviationScaled->SetMaximum(maximum_rmsDev);
     nominal_rmsDeviationScaled->SetMinimum(minimum_rmsDev);
     nominal_rmsDeviationScaled->Draw();
   
     TH1* jesu_rmsDeviationScaled = jesu.Get(name.c_str());
     jesu_rmsDeviationScaled->SetMarkerColor(truth_color);
     jesu_rmsDeviationScaled->SetLineColor(truth_color);
     jesu_rmsDeviationScaled->SetMarkerStyle(jesu_marker_style);
     jesu_rmsDeviationScaled->SetTitle("");
     jesu_rmsDeviationScaled->Draw("same");
   
     TH1* jesd_rmsDeviationScaled = jesd.Get(name.c_str());
     jesd_rmsDeviationScaled->SetMarkerColor(truth_color);
     jesd_rmsDeviationScaled->SetLineColor(truth_color);
     jesd_rmsDeviationScaled->SetMarkerStyle(jesd_marker_style);
     jesd_rmsDeviationScaled->SetTitle("");
     jesd_rmsDeviationScaled->Draw("same");
   
     TH1* btu_rmsDeviationScaled = btu.Get(nameB.c_str());
     btu_rmsDeviationScaled->SetMarkerColor(truth_color);
     btu_rmsDeviationScaled->SetLineColor(truth_color);
     btu_rmsDeviationScaled->SetMarkerStyle(btu_marker_style);
     btu_rmsDeviationScaled->SetTitle("");
     btu_rmsDeviationScaled->Draw("same");
   
     TH1* btd_rmsDeviationScaled = btd.Get(nameB.c_str());
     btd_rmsDeviationScaled->SetMarkerColor(truth_color);
     btd_rmsDeviationScaled->SetLineColor(truth_color);
     btd_rmsDeviationScaled->SetMarkerStyle(btd_marker_style);
     btd_rmsDeviationScaled->SetTitle("");
     btd_rmsDeviationScaled->Draw("same");
   
     can.SaveAs(outputFilename.c_str());*/
  
   
   /// CROSS SECTION jes up/down
   can.Clear();
   
   name = nameBase.str() + "_diffxs";
   nameB = nameBaseB.str() + "_diffxs";
   std::cout << "retreiving: " << name << "\n";
   TH1* diffxs_nominal = nominal.Get(name.c_str());
   diffxs_nominal->SetMarkerColor(diffxs_color);
   diffxs_nominal->SetLineColor(diffxs_color);
   diffxs_nominal->SetMarkerStyle(nominal_marker_style);
   diffxs_nominal->SetTitle("");
   diffxs_nominal->SetMaximum(maximum_diffxs);
   diffxs_nominal->Draw();
  
   TH1* diffxs_jesu = jesu.Get(name.c_str());
   diffxs_jesu->SetMarkerColor(diffxs_color);
   diffxs_jesu->SetLineColor(diffxs_color);
   diffxs_jesu->SetMarkerStyle(jesu_marker_style);
   diffxs_jesu->SetTitle("");
   diffxs_jesu->SetMaximum(maximum);
   diffxs_jesu->Draw("same");
   
   TH1* diffxs_jesd = jesd.Get(name.c_str());
   diffxs_jesd->SetMarkerColor(diffxs_color);
   diffxs_jesd->SetLineColor(diffxs_color);
   diffxs_jesd->SetMarkerStyle(jesd_marker_style);
   diffxs_jesd->SetTitle("");
   diffxs_jesd->SetMaximum(maximum);
   diffxs_jesd->Draw("same");
  
   TLegend diffxs_leg(leg_x1,leg_y1,leg_x2,leg_y2,"d#sigma/dp_{T}, ejets, lepHad");
   diffxs_leg.SetFillStyle(0);
   diffxs_leg.SetBorderSize(0);
   diffxs_leg.AddEntry(diffxs_nominal,"nominal");
   diffxs_leg.AddEntry(diffxs_jesu,"JES up");
   diffxs_leg.AddEntry(diffxs_jesd,"JES down");
   diffxs_leg.Draw("same");
   
   can.SaveAs(std::string(outputFilename + "(").c_str());
   
   can.Clear();
   
   diffxs_nominal->SetMarkerColor(diffxs_color);
   diffxs_nominal->SetLineColor(diffxs_color);
   diffxs_nominal->SetMarkerStyle(nominal_marker_style);
   diffxs_nominal->SetTitle("");
   diffxs_nominal->SetMaximum(maximum_diffxs);
   diffxs_nominal->Draw();
   
   TH1* diffxs_btu = btu.Get(nameB.c_str());
   diffxs_btu->SetMarkerColor(diffxs_color);
   diffxs_btu->SetLineColor(diffxs_color);
   diffxs_btu->SetMarkerStyle(btu_marker_style);
   diffxs_btu->SetTitle("");
   diffxs_btu->SetMaximum(maximum);
   diffxs_btu->Draw("same");
   
   TH1* diffxs_btd = btd.Get(nameB.c_str());
   diffxs_btd->SetMarkerColor(diffxs_color);
   diffxs_btd->SetLineColor(diffxs_color);
   diffxs_btd->SetMarkerStyle(btd_marker_style);
   diffxs_btd->SetTitle("");
   diffxs_btd->SetMaximum(maximum);
   diffxs_btd->Draw("same");
   
   TLegend diffxs_leg1(leg_x1,leg_y1,leg_x2,leg_y2,"d#sigma/dp_{T}, ejets, lepHad");
   diffxs_leg1.SetFillStyle(0);
   diffxs_leg1.SetBorderSize(0);
   diffxs_leg1.AddEntry(diffxs_nominal,"nominal");
   diffxs_leg1.AddEntry(diffxs_btu,"BTAGSF up");
   diffxs_leg1.AddEntry(diffxs_btd,"BTAGSF down");
   diffxs_leg1.Draw("same");
      
   can.SaveAs(std::string(outputFilename + "(").c_str());
   
   can.Clear();
   
   diffxs_nominal->SetMarkerColor(diffxs_color);
   diffxs_nominal->SetLineColor(diffxs_color);
   diffxs_nominal->SetMarkerStyle(nominal_marker_style);
   diffxs_nominal->SetTitle("");
   diffxs_nominal->SetMaximum(maximum_diffxs);
   diffxs_nominal->Draw();
   
   TH1* diffxs_eeru = eeru.Get(nameB.c_str());
   diffxs_eeru->SetMarkerColor(diffxs_color);
   diffxs_eeru->SetLineColor(diffxs_color);
   diffxs_eeru->SetMarkerStyle(eeru_marker_style);
   diffxs_eeru->SetTitle("");
   diffxs_eeru->SetMaximum(maximum);
   diffxs_eeru->Draw("same");
   
   TH1* diffxs_eerd = eerd.Get(nameB.c_str());
   diffxs_eerd->SetMarkerColor(diffxs_color);
   diffxs_eerd->SetLineColor(diffxs_color);
   diffxs_eerd->SetMarkerStyle(eerd_marker_style);
   diffxs_eerd->SetTitle("");
   diffxs_eerd->SetMaximum(maximum);
   diffxs_eerd->Draw("same");

   TLegend diffxs_leg2(leg_x1,leg_y1,leg_x2,leg_y2,"d#sigma/dp_{T}, ejets, lepHad");
   diffxs_leg2.SetFillStyle(0);
   diffxs_leg2.SetBorderSize(0);
   diffxs_leg2.AddEntry(diffxs_nominal,"nominal");
   diffxs_leg2.AddEntry(diffxs_eeru,"EER up");
   diffxs_leg2.AddEntry(diffxs_eerd,"EER down");   
   diffxs_leg2.Draw("same");
   
   can.SaveAs(std::string(outputFilename + "(").c_str());
   
   can.Clear();
   
   diffxs_nominal->SetMarkerColor(diffxs_color);
   diffxs_nominal->SetLineColor(diffxs_color);
   diffxs_nominal->SetMarkerStyle(nominal_marker_style);
   diffxs_nominal->SetTitle("");
   diffxs_nominal->SetMaximum(maximum_diffxs);
   diffxs_nominal->Draw();
   
   TH1* diffxs_jer = jer.Get(nameB.c_str());
   diffxs_jer->SetMarkerColor(diffxs_color);
   diffxs_jer->SetLineColor(diffxs_color);
   diffxs_jer->SetMarkerStyle(jer_marker_style);
   diffxs_jer->SetTitle("");
   diffxs_jer->SetMaximum(maximum);
   diffxs_jer->Draw("same");

   TLegend diffxs_leg2(leg_x1,leg_y1,leg_x2,leg_y2,"d#sigma/dp_{T}, ejets, lepHad");
   diffxs_leg2.SetFillStyle(0);
   diffxs_leg2.SetBorderSize(0);
   diffxs_leg2.AddEntry(diffxs_nominal,"nominal");
   diffxs_leg2.AddEntry(diffxs_eeru,"JER");
   diffxs_leg2.Draw("same");
   
   can.SaveAs(std::string(outputFilename + "(").c_str());

   /// PLOT JESU/D - NOMINAL / NOMINAL
   
   TH1* jesuNominal = (TH1*)truth_nominal->Clone("jesuNominal");
   jesuNominal->Reset();
   jesuNominal->SetTitle(";p_{T} [GeV];(JES - Nominal)/Nominal");
   
   TH1* jesdNominal = (TH1*)truth_nominal->Clone("jesdNominal");
   jesdNominal->Reset();
   jesdNominal->SetTitle(";p_{T} [GeV];(JES - Nominal)/Nominal");
   
   TH1* btuNominal = (TH1*)truth_nominal->Clone("btuNominal");
   btuNominal->Reset();
   btuNominal->SetTitle(";p_{T} [GeV];(BTAGSF - Nominal)/Nominal");
   
   TH1* btdNominal = (TH1*)truth_nominal->Clone("btdNominal");
   btdNominal->Reset();
   btdNominal->SetTitle(";p_{T} [GeV];(BTAGSF - Nominal)/Nominal");
      
   TH1* eruNominal = (TH1*)truth_nominal->Clone("eruNominal");
   eruNominal->Reset();
   eruNominal->SetTitle(";p_{T} [GeV];(EER - Nominal)/Nominal");
   
   TH1* erdNominal = (TH1*)truth_nominal->Clone("erdNominal");
   erdNominal->Reset();
   erdNominal->SetTitle(";p_{T} [GeV];(EER - Nominal)/Nominal");
   
   TH1* jerNominal = (TH1*)truth_nominal->Clone("jerNominal");
   jerNominal->Reset();
   jerNominal->SetTitle(";p_{T} [GeV];(JER - Nominal)/Nominal");

   size_t vsize = truth_nominal->GetNbinsX();
   double* xnom = new double[vsize];
   double* ynom = new double[vsize];
   double* exupnom = new double[vsize];
   double* exdownnom = new double[vsize];
   double* exnom = new double[vsize];   
   double* eynom = new double[vsize];
   double* esysup = new double[vsize];
   double* esysdown = new double[vsize];
   double* sysJESu = new double[vsize];
   double* sysJESd = new double[vsize];
   double* sysBTAGSFu = new double[vsize];
   double* sysBTAGSFd = new double[vsize];
   double* sysEERu = new double[vsize];
   double* sysEERd = new double[vsize];
   double* sysJER = new double[vsize];
   double* etotUp = new double[vsize];
   double* etotDown = new double[vsize];
   
   for(unsigned int bin=1;bin<=truth_nominal->GetNbinsX();++bin){
      double nom = diffxs_nominal->GetBinContent(bin); 
      double dnom = diffxs_nominal->GetBinError(bin);
      xnom[bin-1] = diffxs_nominal->GetXaxis()->GetBinCenter(bin);
      double xnom_width = diffxs_nominal->GetXaxis()->GetBinWidth(bin);
      exupnom[bin-1] =  xnom_width/2.;
      exdownnom[bin-1] = xnom_width/2.;
      exnom[bin-1] = xnom_width/2.;
      ynom[bin-1] = nom;
      eynom[bin-1] = nominal_rmsDeviation->GetBinContent(bin);
      double ju = diffxs_jesu->GetBinContent(bin);
      double dju = diffxs_jesu->GetBinError(bin);
      double jd = diffxs_jesd->GetBinContent(bin);
      double djd = diffxs_jesd->GetBinError(bin);
      double bu = diffxs_btu->GetBinContent(bin);
      double dbu = diffxs_btu->GetBinError(bin);
      double bd = diffxs_btd->GetBinContent(bin);
      double dbd = diffxs_btd->GetBinError(bin);
      double eru = diffxs_eeru->GetBinContent(bin);
      double deru = diffxs_eeru->GetBinError(bin);      
      double erd = diffxs_eerd->GetBinContent(bin);
      double derd = diffxs_eerd->GetBinError(bin); 
      double je = diffxs_jer->GetBinContent(bin);
      double dje = diffxs_jer->GetBinError(bin); 
       
      double junom = nom != 0 ? (ju-nom)/nom : 0;
      double jdnom = nom != 0 ? (jd-nom)/nom) : 0;
      double bunom = nom != 0 ? (bu-nom)/nom : 0;
      double bdnom = nom != 0 ? (bd-nom)/nom) : 0;
      double erunom = nom != 0 ? (eru-nom)/nom : 0;
      double erdnom = nom != 0 ? (erd-nom)/nom) : 0;
      double jenom = nom != 0 ? (je-nom)/nom) : 0;
      
      double djunom = nom != 0 ? (1./nom)*sqrt( dju*dju + (dnom*dnom*ju*ju)/(nom*nom));
      double djdnom = nom != 0 ? (1./nom)*sqrt( djd*djd + (dnom*dnom*jd*jd)/(nom*nom));
      double dbunom = nom != 0 ? (1./nom)*sqrt( dbu*dbu + (dnom*dnom*bu*bu)/(nom*nom));
      double dbdnom = nom != 0 ? (1./nom)*sqrt( dbd*dbd + (dnom*dnom*bd*bd)/(nom*nom));
      double derunom = nom != 0 ? (1./nom)*sqrt( deru*deru + (dnom*dnom*eru*eru)/(nom*nom));
      double derdnom = nom != 0 ? (1./nom)*sqrt( derd*derd + (dnom*dnom*erd*erd)/(nom*nom));
      double djenom = nom != 0 ? (1./nom)*sqrt( dje*dje + (dnom*dnom*je*je)/(nom*nom));

      sysJESu[bin-1] = ju - nom;
      sysJESd[bin-1] = jd - nom;
      sysBTAGSFu[bin-1] = bu - nom;  
      sysBTAGSFd[bin-1] = bd - nom;
      sysEERu[bin-1] = eru - nom; 
      sysEERd[bin-1] = erd - nom;
      sysJER[bin-1] = je - nom;

      double sysup2 = 0;
      double sysdown2 =0;
      
      if (junom>0) sysup2 = sysup2 + ((ju - nom) * (ju - nom));
      else sysdown2 = sysdown2 + ((ju - nom) * (ju - nom));
      
      if (jdnom>0) sysup2 = sysup2 + ((jd - nom) * (jd - nom));
      else sysdown2 = sysdown2 + ((jd - nom) * (jd - nom));      
      
      if (bunom>0) sysup2 = sysup2 + ((bu - nom) * (bu - nom));
      else sysdown2 = sysdown2 + ((bu - nom) * (bu - nom)); 
      
      if (bdnom>0) sysup2 = sysup2 + ((bd - nom) * (bd - nom));
      else sysdown2 = sysdown2 + ((bd - nom) * (bd - nom)); 

      if (erunom>0) sysup2 = sysup2 + ((eru - nom) * (eru - nom));
      else sysdown2 = sysdown2 + ((eru - nom) * (eru - nom)); 
      
      if (erdnom>0) sysup2 = sysup2 + ((erd - nom) * (erd - nom));
      else sysdown2 = sysdown2 + ((erd - nom) * (erd - nom)); 
     
      sysup2 = sysup2 + ((je - nom) * (je - nom));
      sysdown2 = sysdown2 + ((je - nom) * (je - nom)); 

      esysup[bin-1] = sqrt (sysup2);
      esysdown[bin-1] = sqrt (sysdown2);
      
      etotUp[bin-1] = esysup[bin-1] + eynom[bin-1];
      etotDown[bin-1] = esysdown[bin-1] + eynom[bin-1];
      
      cout<<etotUp[bin-1]<<"  "<<esysup[bin-1]<<endl;
      
      jesuNominal->SetBinContent(bin,junom);
      jesuNominal->SetBinError(bin,djunom);
      
      jesdNominal->SetBinContent(bin,jdnom);
      jesdNominal->SetBinError(bin,djdnom);
      
      btuNominal->SetBinContent(bin,bunom);
      btuNominal->SetBinError(bin,dbunom);
      
      btdNominal->SetBinContent(bin,bdnom);
      btdNominal->SetBinError(bin,dbdnom);
      
      eruNominal->SetBinContent(bin,erunom);
      eruNominal->SetBinError(bin,derunom);
      
      erdNominal->SetBinContent(bin,erdnom);
      erdNominal->SetBinError(bin,erdnom);

      jerNominal->SetBinContent(bin,jenom);
      jerNominal->SetBinError(bin,jenom);
   }

   dump("jesu", vsize, exdownnom, exupnom, xnom, ynom, eynom, esysdown, esysup, sysJESu, sysJESd, sysBTAGSFu, sysBTAGSFd, sysEERu, sysEERd, sysJER);

   can.Clear();
   can.Divide(1,2);
   
   can.cd(1);
      
   jesuNominal->Draw();
   jesuNominal->SetMaximum(0.65);
   jesuNominal->SetMinimum(-0.65);
   jesuNominal->SetMarkerStyle(jesu_marker_style);
   jesdNominal->Draw("same");
   jesdNominal->SetMarkerStyle(jesd_marker_style);
   
TLegend diffNom_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"ejets, lepHad");
diffNom_leg.SetFillStyle(0);
diffNom_leg.SetBorderSize(0);
diffNom_leg.AddEntry(jesuNominal,"JES up");
diffNom_leg.AddEntry(jesdNominal,"JES down");
diffNom_leg.Draw("same");
   
can.cd(2);
   
btuNominal->Draw();
btuNominal->SetMaximum(0.3);
btuNominal->SetMinimum(-0.3);
btuNominal->SetMarkerStyle(btu_marker_style);
btdNominal->Draw("same");
btdNominal->SetMarkerStyle(btd_marker_style);
   
TLegend diffNom1_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"ejets, lepHad");
diffNom1_leg.SetFillStyle(0);
diffNom1_leg.SetBorderSize(0);
diffNom1_leg.AddEntry(btuNominal,"BTAGSF up");
diffNom1_leg.AddEntry(btdNominal,"BTAGSF down");
diffNom1_leg.Draw("same");
   
can.SaveAs(outputFilename.c_str());
   
can.Clear();
can.Divide(1,2);
can.cd(1);
   
eruNominal->Draw();
eruNominal->SetMaximum(0.1);
eruNominal->SetMinimum(-0.1);
eruNominal->SetMarkerStyle(eeru_marker_style);
erdNominal->Draw("same");
erdNominal->SetMarkerStyle(eerd_marker_style);
   
TLegend diffNom2_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"ejets, lepHad");
diffNom2_leg.SetFillStyle(0);
diffNom2_leg.SetBorderSize(0);
diffNom2_leg.AddEntry(eruNominal,"EER up");
diffNom2_leg.AddEntry(erdNominal,"EER down");
diffNom2_leg.Draw("same");
   
can.cd(2);
   
jerNominal->Draw();
jerNominal->SetMaximum(0.1);
jerNominal->SetMinimum(-0.1);
jerNominal->SetMarkerStyle(jer_marker_style);
jerNominal->Draw("same");
jerNominal->SetMarkerStyle(jer_marker_style);
   
TLegend diffNom2_leg(leg2_x1,leg2_y1,leg2_x2,leg2_y2,"ejets, lepHad");
diffNom2_leg.SetFillStyle(0);
diffNom2_leg.SetBorderSize(0);
diffNom2_leg.AddEntry(jerNominal,"JER");
diffNom2_leg.Draw("same");
   
can.SaveAs(outputFilename.c_str());
   
can.Clear();

TMultiGraph *mg = new TMultiGraph();
mg->SetTitle("");

systDiffxs = new TGraphAsymmErrors(vsize,xnom,ynom,exdownnom,exupnom,etotDown,etotUp);
   
systDiffxs->SetMarkerColor(diffxs_color);
systDiffxs->SetLineColor(3);
systDiffxs->SetLineWidth(2);
systDiffxs->SetMarkerStyle(nominal_marker_style);
systDiffxs->SetTitle("");
systDiffxs->SetMaximum(maximum_diffxs);
systDiffxs->GetXaxis()->SetRangeUser(0,570);
systDiffxs->GetYaxis()->SetTitle("d#sigma/dp_{T} [pb/GeV]"); 
systDiffxs->GetXaxis()->SetTitle("p_{T} [GeV]");    


statDiffxs = new TGraphErrors(vsize,xnom,ynom,exnom,eynom);
   
statDiffxs->SetMarkerColor(diffxs_color);
statDiffxs->SetLineColor(4);
statDiffxs->SetLineWidth(2);
statDiffxs->SetMarkerStyle(nominal_marker_style);
statDiffxs->SetTitle("");
statDiffxs->SetMaximum(maximum_diffxs);
statDiffxs->GetXaxis()->SetRangeUser(0,570);
statDiffxs->GetYaxis()->SetTitle("d#sigma/dp_{T} [pb/GeV]"); 
statDiffxs->GetXaxis()->SetTitle("p_{T} [GeV]");    
//statDiffxs->Draw("p");

mg->Add(systDiffxs);
mg->Add(statDiffxs);
mg->Draw("ape");

mg->GetYaxis()->SetTitle("d#sigma/dp_{T} [pb/GeV]");
mg->GetXaxis()->SetTitle("p_{T} [GeV]");



can.SaveAs(outputFilename.c_str());
can.Clear();
can.Divide(2,1);
   
can.cd(1);
   
statDiffxs = new TGraphErrors(vsize,xnom,ynom,exnom,eynom);
   
statDiffxs->SetMarkerColor(diffxs_color);
statDiffxs->SetLineColor(4);
statDiffxs->SetLineWidth(2);
statDiffxs->SetMarkerStyle(nominal_marker_style);
statDiffxs->SetTitle("Statistic Errors");
statDiffxs->SetMaximum(maximum_diffxs);
statDiffxs->GetXaxis()->SetRangeUser(0,570);
statDiffxs->GetYaxis()->SetTitle("d#sigma/dp_{T} [pb/GeV]"); 
statDiffxs->GetXaxis()->SetTitle("p_{T} [GeV]");    
statDiffxs->Draw("AP");
   
can.cd(2);

systDiffxs = new TGraphAsymmErrors(vsize,xnom,ynom,exdownnom,exupnom,esysdown,esysup);
   
systDiffxs->SetMarkerColor(diffxs_color);
systDiffxs->SetLineColor(1);
systDiffxs->SetLineWidth(2);
systDiffxs->SetMarkerStyle(nominal_marker_style);
systDiffxs->SetTitle("Systematic Errors");
systDiffxs->SetMaximum(maximum_diffxs);
systDiffxs->GetXaxis()->SetRangeUser(0,570);
systDiffxs->GetYaxis()->SetTitle("d#sigma/dp_{T} [pb/GeV]"); 
systDiffxs->GetXaxis()->SetTitle("p_{T} [GeV]");    
systDiffxs->Draw("AP");
      
can.SaveAs(std::string(outputFilename+")").c_str());
exit(0);
return;
}

void dump(std::string name, size_t vsize, double* down, double* up, double* x, double* y, double* ey, double* downy, double* upy,  double* JESu, double* JESd, double* BTAGSFu, double* BTAGSFd, double* EERu, double* EERd, double* JER) {
   //Save the information in latex file
   ofstream mylatexF;
   std::string lname = name + ".tex"; 
   mylatexF.open (lname.c_str());

   mylatexF<<"\\documentclass[english,american,a4paper,12pt]{article}\n";
   mylatexF<<"\\pagestyle{empty}\n";

   mylatexF<<"\\usepackage{graphics}\n";
   mylatexF<<"\\usepackage{epsfig}\n";
   mylatexF<<"\\usepackage[pagewise,switch]{lineno}\n";
   mylatexF<<"\\graphicspath{{figures/}}\n";
   mylatexF<<"\\usepackage{subfigure}\n";
   mylatexF<<"\\usepackage{multirow}\n";
   mylatexF<<"\\usepackage{xspace}\n";
   mylatexF<<"\\RequirePackage{ifpdf}\n";
   mylatexF<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
   mylatexF<<"% Hyperlinks                         % \n";
   mylatexF<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% \n";
   mylatexF<<"\\RequirePackage{url} \n";
   mylatexF<<"\\ifpdf \n";
   mylatexF<<"\\RequirePackage[pdftex,colorlinks]{hyperref} \n";
   mylatexF<<"\\else \n";
   mylatexF<<"\\RequirePackage[colorlinks]{hyperref} \n";
   mylatexF<<"\\fi \n";
   mylatexF<<"    \n";
   mylatexF<<"\\RequirePackage{geometry} \n";
   mylatexF<<"\\geometry{ \n";
   mylatexF<<"a4paper,% \n";
   mylatexF<<"textwidth=16cm,% \n"; 
   mylatexF<<"textheight=23.2cm,% \n";
   mylatexF<<"marginparsep=7pt,% \n"; 
   mylatexF<<"marginparwidth=2.5cm% \n";
   mylatexF<<"} \n";
   mylatexF<<"    \n";
   mylatexF<<"\\begin{document} \n";
   mylatexF<<"    \n";
   mylatexF<<"\\begin{table} [htbp] \n";
   mylatexF<<"\\begin{center} \n";
   mylatexF<<"\\begin{tabular}{llll} \n";
   mylatexF<<"\\hline \\hline \n";
   mylatexF<<"$p_{t}$ [GeV]   &$d\sigma/dp_{t}$ [fb/GeV]   & stat.[fb/GeV]   & syst. [fb/GeV]  \\\\ \n";
   mylatexF<<"\\hline \n";

   for (unsigned int i = 0; i<vsize; ++i) {
      char helper[1000];
      double xup = x[i] + up[i];
      double xdown = x[i]-down[i];
      double axisY = y[i] * 1000.;
      double statY = ey[i] * 1000.;
      double sysD = downy[i] * 1000.;
      double sysU = upy[i] * 1000.;
      
      sprintf(helper, "\\textcolor{blue}{%5.2f} - \\textcolor{blue}{%5.2f}   & \\textcolor{blue}{%5.3f}   & \\textcolor{blue}{$\\pm %5.3f$}   & \\textcolor{blue}{$-%5.3f$ $/$ $+%5.3f$}  \\\\ \n", xdown, xup, axisY,statY, sysD, sysU);
      mylatexF << helper;
   }

   mylatexF<<"\\hline \n";
   mylatexF<<"\\end{tabular} \n"; 
   mylatexF<<"\\caption{} \n"; 
   mylatexF<<"\\label{table:nominal} \n";
   mylatexF<<"\\end{center} \n";
   mylatexF<<"\\end{table} \n";
   mylatexF<<"  \n";


   mylatexF<<"    \n";
   mylatexF<<"\\begin{table} [htbp] \n";
   mylatexF<<"\\begin{center} \n";
   mylatexF<<"\\begin{tabular}{llllllll} \n";
   mylatexF<<"\\hline \\hline \n";
   mylatexF<<"$p_{t}$ [GeV]  & JESu & JESd &  BTAGSFu & BTAGSFd & EERu & EERd  &  JER   \\\\ \n";
   mylatexF<<"\\hline \n";

   for (unsigned int i = 0; i<vsize; ++i) {
      char helper[1000];
      double xup = x[i] + up[i];
      double xdown = x[i]-down[i];
      double sysJESu = JESu[i] * 1000.;
      double sysJESd = JESd[i] * 1000.;
      double sysBTAGSFu = BTAGSFu[i] * 1000.;
      double sysBTAGSFd = BTAGSFd[i] * 1000.;
      double sysEERu = EERu[i] * 1000.;
      double sysEERd = EERd[i] * 1000.;
      double sysJER = JER[i] * 1000.;
      
      sprintf(helper, "\\textcolor{blue}{%5.2f} - \\textcolor{blue}{%5.2f}   &  \\textcolor{blue}{%5.3f}  & \\textcolor{blue}{%5.3f}  & \\textcolor{blue}{%5.3f}  & \\textcolor{blue}{%5.3f}  & \\textcolor{blue}{%5.3f}  & \\textcolor{blue}{%5.3f}  & \\textcolor{blue}{%5.3f}   \\\\ \n", xdown, xup, sysJESu, sysJESd, sysBTAGSFu, sysBTAGSFd, sysEERu, sysEERd, sysJER);
      mylatexF << helper;
   }

   mylatexF<<"\\hline \n";
   mylatexF<<"\\end{tabular} \n"; 
   mylatexF<<"\\caption{} \n"; 
   mylatexF<<"\\label{table:syst} \n";
   mylatexF<<"\\end{center} \n";
   mylatexF<<"\\end{table} \n";
   mylatexF<<"  \n";
   mylatexF<<"\\end{document} \n";
   mylatexF.close();
}

std::string get_histo_name_base(const std::string basePath,
                                const std::string channel,
                                const std::string sampleName,
                                const unsigned int reg,
                                const unsigned int nToys
                               )
{
   
   std::stringstream nameBase;
   nameBase << basePath << "/" << channel << "/" << sampleName << "/SVD/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_SVD_reg" << reg << "_toy" << nToys;
   
   return nameBase.str();
}

std::string get_truth_histo_name(const std::string basePath,
                                 const std::string channel,
                                 const std::string sampleName,
                                 const unsigned int reg,
                                 const unsigned int nToys
                                )
{
   return get_histo_name_base(basePath,channel,sampleName,reg,nToys) + "_mc_truth";
}

std::string get_reco_histo_name(const std::string basePath,
                                 const std::string channel,
                                 const std::string sampleName,
                                 const unsigned int reg,
                                 const unsigned int nToys
                                )
{
   return get_histo_name_base(basePath,channel,sampleName,reg,nToys) + "_mc_reco";
}

std::string get_measured_histo_name(const std::string basePath,
                                const std::string channel,
                                const std::string sampleName,
                                const unsigned int reg,
                                const unsigned int nToys
                               )
{
   return get_histo_name_base(basePath,channel,sampleName,reg,nToys) + "_data_reco";
}

std::string get_unfolded_histo_name(const std::string basePath,
                                const std::string channel,
                                const std::string sampleName,
                                const unsigned int reg,
                                const unsigned int nToys
                               )
{
   return get_histo_name_base(basePath,channel,sampleName,reg,nToys) + "_UnfoldedReco";
}

