
#include <vector>
#include <sstream>
void dataMinusBG_nominalJesuJesd(void){


   TFile nominal("../rootFiles/dataMinusBG.tag1.toy500.root");
   TFile jesu("../rootFiles/dataMinusBG.tag1.toy500.jesu.root");
   TFile jesd("../rootFiles/dataMinusBG.tag1.toy500.jesd.root");
   TFile btu("../rootFiles/McTag1TopLepHad_DataTaggedTopLepHad_el.reg4.nToys500.btagsfup.root");
   TFile btd("../rootFiles/McTag1TopLepHad_DataTaggedTopLepHad_el.reg4.nToys500.btagsfdown.root");
   
   TCanvas can("can","can",0,0,800,600);
   
   std::vector<TH1*> meanDev,meanDevSc;
   std::vector<TH1*> rmsDev,rmsDevSc;
   std::vector<std::string> legendString;
   
   std::string basePath = "unfolding/toys/";
   const unsigned int reg = 4;
   const unsigned int nToys = 500;
   std::string elOrMu = "el";
   std::string sampleName = "McNTag1_Perm0TopLepHad";
   
   std::string outputFilename;
   std::stringstream outfile;
   outfile << "tag1.reg" << reg << ".toy" << nToys << ".compareNominalJesBtag.ps";
   std::string outputFilename = outfile.str();
   
   int nominal_marker_style = 20;
   int jesu_marker_style = 22;
   int jesd_marker_style = 23;
   int btu_marker_style = 24;
   int btd_marker_style = 25;
   
   int truth_color = kBlack;
   int reco_color = kRed;
   int measured_color = kBlue;
   int unfolded_color = kGreen;
   
   double maximum = 9000.;
   double maximum_meanDev = 0.4;
   double minimum_meanDev = -0.6;
   double maximum_rmsDev = 0.05;
   double minimum_rmsDev = 0.0;
   
   double leg_x1 = 0.5;
   double leg_y1 = 0.93;
   double leg_x2 = 0.95;
   double leg_y2 = 0.55;
   
   
   // draw el/top1
   can.Clear();
   can.Divide(2,2);
   legendString.push_back("ejets/lepHad");
   elOrMu = "el";
   sampleName = "McNTag1_Perm0TopLepHad";
   
   /// MC TRUTH
   can.cd(1);
   
   std::stringstream nameBase;
   nameBase << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys;
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
   truth_leg.AddEntry(truth_jesu,"JES shifted up (MC@NLO)");
   truth_leg.AddEntry(truth_jesd,"JES shifted down (MC@NLO)");
   truth_leg.AddEntry(truth_btu,"b-tag SF shifted up (MC@NLO)");
   truth_leg.AddEntry(truth_btd,"b-tag SF shifted down (MC@NLO)");
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
   reco_leg.AddEntry(reco_jesu,"JES shifted up (MC@NLO)");
   reco_leg.AddEntry(reco_jesd,"JES shifted down (MC@NLO)");
   reco_leg.AddEntry(reco_btu,"b-tag SF shifted up (MC@NLO)");
   reco_leg.AddEntry(reco_btd,"b-tag SF shifted down (MC@NLO)");
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
   measured_leg.AddEntry(measured_jesu,"JES shifted up (Data-BG)");
   measured_leg.AddEntry(measured_jesd,"JES shifted down (Data-BG)");
   measured_leg.AddEntry(measured_btu,"b-tag SF shifted up (Data-BG)");
   measured_leg.AddEntry(measured_btd,"b-tag SF shifted down (Data-BG)");
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
   unfolded_leg.AddEntry(unfolded_jesu,"JES shifted up (Data-BG)");
   unfolded_leg.AddEntry(unfolded_jesd,"JES shifted down (Data-BG)");
   unfolded_leg.AddEntry(unfolded_btu,"b-tag SF shifted up (Data-BG)");
   unfolded_leg.AddEntry(unfolded_btd,"b-tag SF shifted down (Data-BG)");
   unfolded_leg.Draw("same");
   
   can.SaveAs(std::string(outputFilename + "(").c_str());
   
   
   /// spectra again, but nominal all on one, etc.
   can.Clear();
   can.Divide(3,2);
   
   can.cd(1);
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
   
   can.cd(2);
   truth_jesd->Draw();
   reco_jesd->Draw("same");
   measured_jesd->Draw("same");
   unfolded_jesd->Draw("same");
   
   TLegend jesd_leg(leg_x1,leg_y1,leg_x2,leg_y2,"jesd, ejets, lepHad");
   jesd_leg.SetFillStyle(0);
   jesd_leg.SetBorderSize(0);
   jesd_leg.AddEntry(truth_jesd,"truth (MC@NLO)");
   jesd_leg.AddEntry(reco_jesd,"reco (MC@NLO)");
   jesd_leg.AddEntry(measured_jesd,"measured (Data-BG)");
   jesd_leg.AddEntry(unfolded_jesd,"unfolded (Data-BG)");
   jesd_leg.Draw("same");
   
   can.cd(5);
   truth_jesu->Draw();
   reco_jesu->Draw("same");
   measured_jesu->Draw("same");
   unfolded_jesu->Draw("same");
   
   TLegend jesu_leg(leg_x1,leg_y1,leg_x2,leg_y2,"jesu, ejets, lepHad");
   jesu_leg.SetFillStyle(0);
   jesu_leg.SetBorderSize(0);
   jesu_leg.AddEntry(truth_jesu,"truth (MC@NLO)");
   jesu_leg.AddEntry(reco_jesu,"reco (MC@NLO)");
   jesu_leg.AddEntry(measured_jesu,"measured (Data-BG)");
   jesu_leg.AddEntry(unfolded_jesu,"unfolded (Data-BG)");
   jesu_leg.Draw("same");
   
   
   can.cd(3);
   truth_btd->Draw();
   reco_btd->Draw("same");
   measured_btd->Draw("same");
   unfolded_btd->Draw("same");
   
   TLegend btd_leg(leg_x1,leg_y1,leg_x2,leg_y2,"btagsfd, ejets, lepHad");
   btd_leg.SetFillStyle(0);
   btd_leg.SetBorderSize(0);
   btd_leg.AddEntry(truth_btd,"truth (MC@NLO)");
   btd_leg.AddEntry(reco_btd,"reco (MC@NLO)");
   btd_leg.AddEntry(measured_btd,"measured (Data-BG)");
   btd_leg.AddEntry(unfolded_btd,"unfolded (Data-BG)");
   btd_leg.Draw("same");
   
   can.cd(6);
   truth_btu->Draw();
   reco_btu->Draw("same");
   measured_btu->Draw("same");
   unfolded_btu->Draw("same");
   
   TLegend btu_leg(leg_x1,leg_y1,leg_x2,leg_y2,"btagsfu, ejets, lepHad");
   btu_leg.SetFillStyle(0);
   btu_leg.SetBorderSize(0);
   btu_leg.AddEntry(truth_btu,"truth (MC@NLO)");
   btu_leg.AddEntry(reco_btu,"reco (MC@NLO)");
   btu_leg.AddEntry(measured_btu,"measured (Data-BG)");
   btu_leg.AddEntry(unfolded_btu,"unfolded (Data-BG)");
   btu_leg.Draw("same");
   
   
   can.SaveAs(outputFilename.c_str());
   
   
   /// MIGRATION MATRICES
   can.Clear();
   can.Divide(3,2);
   
   can.cd(1);
   can.cd(1)->SetLogz(1);
   
   name = nameBase.str() + "_mc_migration";
   nameB = nameBaseB.str() + "_mc_migration";
   TH1* truth_migration = nominal.Get(name.c_str());
   truth_migration->SetTitle("nominal");
   gStyle->SetPaintTextFormat(".2g");
   truth_migration->Draw("col");
   truth_migration->Draw("textsame");
   
   can.cd(2);
   can.cd(2)->SetLogz(1);
   
   TH1* jesd_migration = jesd.Get(name.c_str());
   jesd_migration->SetTitle("jesd");
   gStyle->SetPaintTextFormat(".2g");
   jesd_migration->Draw("col");
   jesd_migration->Draw("textsame");
   
   can.cd(5);
   can.cd(5)->SetLogz(1);
   TH1* jesu_migration = jesu.Get(name.c_str());
   jesu_migration->SetTitle("jesu");
   gStyle->SetPaintTextFormat(".2g");
   jesu_migration->Draw("col");
   jesu_migration->Draw("textsame");
   
   can.cd(3);
   can.cd(3)->SetLogz(1);
   
   TH1* btd_migration = btd.Get(nameB.c_str());
   btd_migration->SetTitle("btd");
   gStyle->SetPaintTextFormat(".2g");
   btd_migration->Draw("col");
   btd_migration->Draw("textsame");
   
   can.cd(6);
   can.cd(6)->SetLogz(1);
   TH1* btu_migration = btu.Get(nameB.c_str());
   btu_migration->SetTitle("btu");
   gStyle->SetPaintTextFormat(".2g");
   btu_migration->Draw("col");
   btu_migration->Draw("textsame");
   
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
   
   can.SaveAs(outputFilename.c_str());
   
   /// SCALED DEVIATION
   can.Clear();
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
   
   can.SaveAs(outputFilename.c_str());
   
   /// PLOT JESU/D - NOMINAL / NOMINAL
   
   TH1* jesuNominal = (TH1*)truth_nominal->Clone("jesuNominal");
   jesuNominal->Reset();
   jesuNominal->SetTitle(";p_{T} [GeV];(jes-shift - Nominal)/Nominal");
   
   TH1* jesdNominal = (TH1*)truth_nominal->Clone("jesdNominal");
   jesdNominal->Reset();
   jesdNominal->SetTitle(";p_{T} [GeV];(jes-shift - Nominal)/Nominal");
   
   TH1* btuNominal = (TH1*)truth_nominal->Clone("btuNominal");
   btuNominal->Reset();
   btuNominal->SetTitle(";p_{T} [GeV];(bt-shift - Nominal)/Nominal");
   
   TH1* btdNominal = (TH1*)truth_nominal->Clone("btdNominal");
   btdNominal->Reset();
   btdNominal->SetTitle(";p_{T} [GeV];(bt-shift - Nominal)/Nominal");
   
   for(unsigned int bin=1;bin<=truth_nominal->GetNbinsX();++bin){
      double nom = unfolded_nominal->GetBinContent(bin);
      double dnom = unfolded_nominal->GetBinError(bin);
      double ju = unfolded_jesu->GetBinContent(bin);
      double dju = unfolded_jesu->GetBinError(bin);
      double jd = unfolded_jesd->GetBinContent(bin);
      double djd = unfolded_jesd->GetBinError(bin);
      double bu = unfolded_btu->GetBinContent(bin);
      double dbu = unfolded_btu->GetBinError(bin);
      double bd = unfolded_btd->GetBinContent(bin);
      double dbd = unfolded_btd->GetBinError(bin);
      
      double junom = nom != 0 ? (ju-nom)/nom : 0;
      double jdnom = nom != 0 ? (jd-nom)/nom) : 0;
      double bunom = nom != 0 ? (bu-nom)/nom : 0;
      double bdnom = nom != 0 ? (bd-nom)/nom) : 0;
      
      double djunom = nom != 0 ? (1./nom)*sqrt( dju*dju + (dnom*dnom*ju*ju)/(nom*nom));
      double djdnom = nom != 0 ? (1./nom)*sqrt( djd*djd + (dnom*dnom*jd*jd)/(nom*nom));
      double dbunom = nom != 0 ? (1./nom)*sqrt( dbu*dbu + (dnom*dnom*bu*bu)/(nom*nom));
      double dbdnom = nom != 0 ? (1./nom)*sqrt( dbd*dbd + (dnom*dnom*bd*bd)/(nom*nom));
      
      jesuNominal->SetBinContent(bin,junom);
      jesuNominal->SetBinError(bin,djunom);
      
      jesdNominal->SetBinContent(bin,jdnom);
      jesdNominal->SetBinError(bin,djdnom);
      
      btuNominal->SetBinContent(bin,bunom);
      btuNominal->SetBinError(bin,dbunom);
      
      btdNominal->SetBinContent(bin,bdnom);
      btdNominal->SetBinError(bin,dbdnom);
      
   }
   
   can.Clear();
   
   jesuNominal->Draw();
   jesuNominal->SetMaximum(1.);
   jesuNominal->SetMinimum(-1.);
   jesuNominal->SetMarkerStyle(jesu_marker_style);
   jesdNominal->Draw("same");
   jesdNominal->SetMarkerStyle(jesd_marker_style);
   
   btuNominal->Draw("same");
   btuNominal->SetMarkerStyle(btu_marker_style);
   btdNominal->Draw("same");
   btdNominal->SetMarkerStyle(btd_marker_style);
   
   can.SaveAs(std::string(outputFilename+")").c_str());
   
   
   
   exit(0);
   return;
}

