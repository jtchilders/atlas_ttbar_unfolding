
{
#include <vector>
#include <sstream>

   TFile in("../rootFiles/dataMinusBG.tag1.toy500.root");
   
   TCanvas can("can","can",0,0,800,600);
   
   std::vector<TH1*> meanDev,meanDevSc;
   std::vector<TH1*> rmsDev,rmsDevSc;
   std::vector<std::string> legendString;
   
   std::string basePath = "unfolding/toys/";
   const unsigned int reg = 4;
   const unsigned int nToys = 500;
   std::string elOrMu = "el";
   std::string sampleName = "McNTag1_Perm0Top1";
   
   
   // draw el/top1
   legendString.push_back("ejets/leptonic");
   elOrMu = "el";
   sampleName = "McNTag1_Perm0Top1";
   
   std::stringstream name;
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_truth";
   std::cout << "retreiving: " << name.str() << "\n";
   TH1* truth = in.Get(name.str().c_str());
   truth->SetMarkerColor(kBlack);
   truth->SetLineColor(kBlack);
   truth->SetTitle("");
   truth->SetMaximum(4000);
   truth->Draw();
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_reco";
   std::cout << "retreiving: " << name.str() << "\n";
   TH1* reco = in.Get(name.str().c_str());
   reco->SetMarkerColor(kRed);
   reco->SetLineColor(kRed);
   reco->SetTitle("");
   reco->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_data_reco";
   std::cout << "retreiving: " << name.str() << "\n";
   TH1* measured = in.Get(name.str().c_str());
   measured->SetMarkerColor(kBlue);
   measured->SetLineColor(kBlue);
   measured->SetTitle("");
   measured->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_UnfoldedReco";
   std::cout << "retreiving: " << name.str() << "\n";
   TH1* unfolded = in.Get(name.str().c_str());
   unfolded->SetMarkerColor(kGreen);
   unfolded->SetLineColor(kGreen);
   unfolded->SetTitle("");
   unfolded->Draw("same");
   
   TLegend leg(0.6,0.9,0.85,0.5,"ejets, leptonic top");
   leg.SetFillColor(kWhite);
   leg.AddEntry(truth,"truth (MC@NLO)");
   leg.AddEntry(reco,"reco (MC@NLO)");
   leg.AddEntry(measured,"Data-BG (Peter)");
   leg.AddEntry(unfolded,"unfolded (SVD)");
   leg.Draw("same");
   
   can.SaveAs("plots.ps(");
   
   can.Clear();
   can.SetLogz(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_migration";
   TH1* migration = in.Get(name.str().c_str());
   migration->SetTitle("");
   
   gStyle->SetPaintTextFormat(".2g");
   migration->Draw("col");
   migration->Draw("textsame");
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviation";
   TH1* meanDeviation = in.Get(name.str().c_str());
   meanDeviation->SetTitle("");
   meanDeviation->Draw();
   meanDev.push_back(meanDeviation);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviation";
   TH1* rmsDeviation = in.Get(name.str().c_str());
   rmsDeviation->SetTitle("");
   rmsDeviation->Draw();
   rmsDev.push_back(rmsDeviation);
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviationScaled";
   TH1* meanDeviationScaled = in.Get(name.str().c_str());
   meanDeviationScaled->SetTitle("");
   meanDeviationScaled->Draw();
   meanDevSc.push_back(meanDeviationScaled);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviationScaled";
   TH1* rmsDeviationSacled = in.Get(name.str().c_str());
   rmsDeviationSacled->SetTitle("");
   rmsDeviationSacled->Draw();
   rmsDevSc.push_back(rmsDeviationSacled);
   
   can.SaveAs("plots.ps");
   
   // draw mu/top1
   can.Clear();
   legendString.push_back("mujets/leptonic");
   elOrMu = "mu";
   sampleName = "McNTag1_Perm0Top1";
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_truth";
   truth = (TH1*)in.Get(name.str().c_str());
   truth->SetMarkerColor(kBlack);
   truth->SetLineColor(kBlack);
   truth->SetTitle("");
   truth->SetMaximum(8000);
   truth->Draw();
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_reco";
   reco = (TH1*)in.Get(name.str().c_str());
   reco->SetMarkerColor(kRed);
   reco->SetLineColor(kRed);
   reco->SetTitle("");
   reco->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_data_reco";
   measured = (TH1*)in.Get(name.str().c_str());
   measured->SetMarkerColor(kBlue);
   measured->SetLineColor(kBlue);
   measured->SetTitle("");
   measured->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_UnfoldedReco";
   unfolded = (TH1*)in.Get(name.str().c_str());
   unfolded->SetMarkerColor(kGreen);
   unfolded->SetLineColor(kGreen);
   unfolded->SetTitle("");
   unfolded->Draw("same");
   
   TLegend leg(0.6,0.9,0.85,0.5,"mujets, leptonic top");
   leg.SetFillColor(kWhite);
   leg.AddEntry(truth,"truth (MC@NLO)");
   leg.AddEntry(reco,"reco (MC@NLO)");
   leg.AddEntry(measured,"Data-BG (Peter)");
   leg.AddEntry(unfolded,"unfolded (SVD)");
   leg.Draw("same");
   
   can.SaveAs("plots.ps(");
   
   can.Clear();
   can.SetLogz(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_migration";
   migration = (TH1*)in.Get(name.str().c_str());
   migration->SetTitle("");
   
   gStyle->SetPaintTextFormat(".2g");
   migration->Draw("col");
   migration->Draw("textsame");
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviation";
   meanDeviation = (TH1*)in.Get(name.str().c_str());
   meanDeviation->SetTitle("");
   meanDeviation->Draw();
   meanDev.push_back(meanDeviation);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviation";
   rmsDeviation = (TH1*)in.Get(name.str().c_str());
   rmsDeviation->SetTitle("");
   rmsDeviation->Draw();
   rmsDev.push_back(rmsDeviation);
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviationScaled";
   meanDeviationScaled = (TH1*)in.Get(name.str().c_str());
   meanDeviationScaled->SetTitle("");
   meanDeviationScaled->Draw();
   meanDevSc.push_back(meanDeviationScaled);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviationScaled";
   rmsDeviationSacled = (TH1*)in.Get(name.str().c_str());
   rmsDeviationSacled->SetTitle("");
   rmsDeviationSacled->Draw();
   rmsDevSc.push_back(rmsDeviationSacled);
   
   can.SaveAs("plots.ps");
   
   
   // draw el/top2
   can.Clear();
   legendString.push_back("ejets/hadronic");
   elOrMu = "el";
   sampleName = "McNTag1_Perm0Top2";
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_truth";
   TH1* truth = (TH1*)in.Get(name.str().c_str());
   truth->SetMarkerColor(kBlack);
   truth->SetLineColor(kBlack);
   truth->SetTitle("");
   truth->SetMaximum(4000);
   truth->Draw();
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_reco";
   TH1* reco = (TH1*)in.Get(name.str().c_str());
   reco->SetMarkerColor(kRed);
   reco->SetLineColor(kRed);
   reco->SetTitle("");
   reco->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_data_reco";
   TH1* measured = (TH1*)in.Get(name.str().c_str());
   measured->SetMarkerColor(kBlue);
   measured->SetLineColor(kBlue);
   measured->SetTitle("");
   measured->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_UnfoldedReco";
   TH1* unfolded = (TH1*)in.Get(name.str().c_str());
   unfolded->SetMarkerColor(kGreen);
   unfolded->SetLineColor(kGreen);
   unfolded->SetTitle("");
   unfolded->Draw("same");
   
   TLegend leg(0.6,0.9,0.85,0.5,"ejets, hadronic top");
   leg.SetFillColor(kWhite);
   leg.AddEntry(truth,"truth (MC@NLO)");
   leg.AddEntry(reco,"reco (MC@NLO)");
   leg.AddEntry(measured,"Data-BG (Peter)");
   leg.AddEntry(unfolded,"unfolded (SVD)");
   leg.Draw("same");
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.SetLogz(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_migration";
   TH1* migration = (TH1*)in.Get(name.str().c_str());
   migration->SetTitle("");
   
   gStyle->SetPaintTextFormat(".2g");
   migration->Draw("col");
   migration->Draw("textsame");
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviation";
   TH1* meanDeviation = (TH1*)in.Get(name.str().c_str());
   meanDeviation->SetTitle("");
   meanDeviation->Draw();
   meanDev.push_back(meanDeviation);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviation";
   TH1* rmsDeviation = (TH1*)in.Get(name.str().c_str());
   rmsDeviation->SetTitle("");
   rmsDeviation->Draw();
   rmsDev.push_back(rmsDeviation);
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviationScaled";
   TH1* meanDeviationScaled = (TH1*)in.Get(name.str().c_str());
   meanDeviationScaled->SetTitle("");
   meanDeviationScaled->Draw();
   meanDevSc.push_back(meanDeviationScaled);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviationScaled";
   TH1* rmsDeviationSacled = (TH1*)in.Get(name.str().c_str());
   rmsDeviationSacled->SetTitle("");
   rmsDeviationSacled->Draw();
   rmsDevSc.push_back(rmsDeviationSacled);
   
   can.SaveAs("plots.ps");
   
   // draw mu/top2
   can.Clear();
   legendString.push_back("mujets/hadronic");
   elOrMu = "mu";
   sampleName = "McNTag1_Perm0Top2";
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_truth";
   truth = (TH1*)in.Get(name.str().c_str());
   truth->SetMarkerColor(kBlack);
   truth->SetLineColor(kBlack);
   truth->SetTitle("");
   truth->SetMaximum(8000);
   truth->Draw();
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_reco";
   reco = (TH1*)in.Get(name.str().c_str());
   reco->SetMarkerColor(kRed);
   reco->SetLineColor(kRed);
   reco->SetTitle("");
   reco->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_data_reco";
   measured = (TH1*)in.Get(name.str().c_str());
   measured->SetMarkerColor(kBlue);
   measured->SetLineColor(kBlue);
   measured->SetTitle("");
   measured->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_UnfoldedReco";
   unfolded = (TH1*)in.Get(name.str().c_str());
   unfolded->SetMarkerColor(kGreen);
   unfolded->SetLineColor(kGreen);
   unfolded->SetTitle("");
   unfolded->Draw("same");
   
   TLegend leg(0.6,0.9,0.85,0.5,"mujets, hadronic top");
   leg.SetFillColor(kWhite);
   leg.AddEntry(truth,"truth (MC@NLO)");
   leg.AddEntry(reco,"reco (MC@NLO)");
   leg.AddEntry(measured,"Data-BG (Peter)");
   leg.AddEntry(unfolded,"unfolded (SVD)");
   leg.Draw("same");
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.SetLogz(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_migration";
   migration = (TH1*)in.Get(name.str().c_str());
   migration->SetTitle("");
   
   gStyle->SetPaintTextFormat(".2g");
   migration->Draw("col");
   migration->Draw("textsame");
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviation";
   meanDeviation = (TH1*)in.Get(name.str().c_str());
   meanDeviation->SetTitle("");
   meanDeviation->Draw();
   meanDev.push_back(meanDeviation);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviation";
   rmsDeviation = (TH1*)in.Get(name.str().c_str());
   rmsDeviation->SetTitle("");
   rmsDeviation->Draw();
   rmsDev.push_back(rmsDeviation);
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviationScaled";
   meanDeviationScaled = (TH1*)in.Get(name.str().c_str());
   meanDeviationScaled->SetTitle("");
   meanDeviationScaled->Draw();
   meanDevSc.push_back(meanDeviationScaled);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviationScaled";
   rmsDeviationSacled = (TH1*)in.Get(name.str().c_str());
   rmsDeviationSacled->SetTitle("");
   rmsDeviationSacled->Draw();
   rmsDevSc.push_back(rmsDeviationSacled);
   
   can.SaveAs("plots.ps");
   
   
   // draw el/topLepHad
   can.Clear();
   legendString.push_back("ejets/lep+had");
   elOrMu = "el";
   sampleName = "McNTag1_Perm0TopLepHad";
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_truth";
   TH1* truth = (TH1*)in.Get(name.str().c_str());
   truth->SetMarkerColor(kBlack);
   truth->SetLineColor(kBlack);
   truth->SetTitle("");
   truth->SetMaximum(8000);
   truth->Draw();
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_reco";
   TH1* reco = (TH1*)in.Get(name.str().c_str());
   reco->SetMarkerColor(kRed);
   reco->SetLineColor(kRed);
   reco->SetTitle("");
   reco->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_data_reco";
   TH1* measured = (TH1*)in.Get(name.str().c_str());
   measured->SetMarkerColor(kBlue);
   measured->SetLineColor(kBlue);
   measured->SetTitle("");
   measured->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_UnfoldedReco";
   TH1* unfolded = (TH1*)in.Get(name.str().c_str());
   unfolded->SetMarkerColor(kGreen);
   unfolded->SetLineColor(kGreen);
   unfolded->SetTitle("");
   unfolded->Draw("same");
   
   TLegend leg(0.6,0.9,0.85,0.5,"ejets, lep+had top");
   leg.SetFillColor(kWhite);
   leg.AddEntry(truth,"truth (MC@NLO)");
   leg.AddEntry(reco,"reco (MC@NLO)");
   leg.AddEntry(measured,"Data-BG (Peter)");
   leg.AddEntry(unfolded,"unfolded (SVD)");
   leg.Draw("same");
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.SetLogz(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_migration";
   TH1* migration = (TH1*)in.Get(name.str().c_str());
   migration->SetTitle("");
   
   gStyle->SetPaintTextFormat(".2g");
   migration->Draw("col");
   migration->Draw("textsame");
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviation";
   TH1* meanDeviation = (TH1*)in.Get(name.str().c_str());
   meanDeviation->SetTitle("");
   meanDeviation->Draw();
   meanDev.push_back(meanDeviation);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviation";
   TH1* rmsDeviation = (TH1*)in.Get(name.str().c_str());
   rmsDeviation->SetTitle("");
   rmsDeviation->Draw();
   rmsDev.push_back(rmsDeviation);
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviationScaled";
   TH1* meanDeviationScaled = (TH1*)in.Get(name.str().c_str());
   meanDeviationScaled->SetTitle("");
   meanDeviationScaled->Draw();
   meanDevSc.push_back(meanDeviationScaled);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviationScaled";
   TH1* rmsDeviationSacled = (TH1*)in.Get(name.str().c_str());
   rmsDeviationSacled->SetTitle("");
   rmsDeviationSacled->Draw();
   rmsDevSc.push_back(rmsDeviationSacled);
   
   can.SaveAs("plots.ps");
   
   // draw mu/topLepHad
   can.Clear();
   legendString.push_back("mujets/lep+had");
   elOrMu = "mu";
   sampleName = "McNTag1_Perm0TopLepHad";
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_truth";
   truth = (TH1*)in.Get(name.str().c_str());
   truth->SetMarkerColor(kBlack);
   truth->SetLineColor(kBlack);
   truth->SetTitle("");
   truth->SetMaximum(16000);
   truth->Draw();
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_reco";
   reco = (TH1*)in.Get(name.str().c_str());
   reco->SetMarkerColor(kRed);
   reco->SetLineColor(kRed);
   reco->SetTitle("");
   reco->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_data_reco";
   measured = (TH1*)in.Get(name.str().c_str());
   measured->SetMarkerColor(kBlue);
   measured->SetLineColor(kBlue);
   measured->SetTitle("");
   measured->Draw("same");
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_UnfoldedReco";
   unfolded = (TH1*)in.Get(name.str().c_str());
   unfolded->SetMarkerColor(kGreen);
   unfolded->SetLineColor(kGreen);
   unfolded->SetTitle("");
   unfolded->Draw("same");
   
   TLegend leg(0.6,0.9,0.85,0.5,"mujets, lep+had top");
   leg.SetFillColor(kWhite);
   leg.AddEntry(truth,"truth (MC@NLO)");
   leg.AddEntry(reco,"reco (MC@NLO)");
   leg.AddEntry(measured,"Data-BG (Peter)");
   leg.AddEntry(unfolded,"unfolded (SVD)");
   leg.Draw("same");
   
   can.SaveAs("plots.ps(");
   
   can.Clear();
   can.SetLogz(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_mc_migration";
   migration = (TH1*)in.Get(name.str().c_str());
   migration->SetTitle("");
   
   gStyle->SetPaintTextFormat(".2g");
   migration->Draw("col");
   migration->Draw("textsame");
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviation";
   meanDeviation = (TH1*)in.Get(name.str().c_str());
   meanDeviation->SetTitle("");
   meanDeviation->Draw();
   meanDev.push_back(meanDeviation);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviation";
   rmsDeviation = (TH1*)in.Get(name.str().c_str());
   rmsDeviation->SetTitle("");
   rmsDeviation->Draw();
   rmsDev.push_back(rmsDeviation);
   
   can.SaveAs("plots.ps");
   
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_meanDeviationScaled";
   meanDeviationScaled = (TH1*)in.Get(name.str().c_str());
   meanDeviationScaled->SetTitle("");
   meanDeviationScaled->Draw();
   meanDevSc.push_back(meanDeviationScaled);
   can.cd(2);
   
   name.str("");
   name << basePath << elOrMu << "/" << sampleName << "/reg" << reg << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << reg << "_toy" << nToys << "_rmsDeviationScaled";
   rmsDeviationSacled = (TH1*)in.Get(name.str().c_str());
   rmsDeviationSacled->SetTitle("");
   rmsDeviationSacled->Draw();
   rmsDevSc.push_back(rmsDeviationSacled);
   
   can.SaveAs("plots.ps");
   
   
   can.Clear();
   can.Divide(1,2);
   
   TLegend legA(0.8,0.93,0.9,0.66,"mean deviation");
   legA->SetFillStyle(0);
   legA->SetBorderSize(0);
   TLegend legB(0.25,0.9,0.5,0.55,"rms deviation");
   legB->SetFillStyle(0);
   legB->SetBorderSize(0);
   TLegend legC(0.2,0.65,0.35,0.2,"mean deviation scaled");
   legC->SetFillStyle(0);
   legC->SetBorderSize(0);
   TLegend legD(0.25,0.9,0.5,0.55,"rms deviation scaled");
   legD->SetFillStyle(0);
   legD->SetBorderSize(0);
   
   
   TCanvas canB("canB","canB",0,0,800,600);
   canB.Divide(1,2);
   
   for(unsigned int i=0;i<meanDev.size();++i){
      unsigned int color = i+1;
      unsigned int style = 20+i;
      
      
      meanDev[i]->SetMarkerColor(color);
      meanDev[i]->SetLineColor(color);
      meanDev[i]->SetMarkerStyle(style);
      meanDev[i]->SetMaximum(0.5);
      meanDev[i]->SetMinimum(-0.5);
      
      rmsDev[i]->SetMarkerColor(color);
      rmsDev[i]->SetLineColor(color);
      rmsDev[i]->SetMarkerStyle(style);
      rmsDev[i]->SetMaximum(0.06);
      rmsDev[i]->SetMinimum(0.0);
      
      
      meanDevSc[i]->SetMarkerColor(color);
      meanDevSc[i]->SetLineColor(color);
      meanDevSc[i]->SetMarkerStyle(style);
      meanDevSc[i]->SetMaximum(0.3);
      meanDevSc[i]->SetMinimum(-0.5);
      
      rmsDevSc[i]->SetMarkerColor(color);
      rmsDevSc[i]->SetLineColor(color);
      rmsDevSc[i]->SetMarkerStyle(style);
      rmsDevSc[i]->SetMaximum(0.06);
      rmsDevSc[i]->SetMinimum(0.);
      
      std::string option;
      if(i==0) option = "";
      else option = "same";
      
      
      can.cd(1);
      meanDev[i]->Draw(option.c_str());
      can.cd(2);
      rmsDev[i]->Draw(option.c_str());
      canB.cd(1);
      meanDevSc[i]->Draw(option.c_str());
      canB.cd(2);
      rmsDevSc[i]->Draw(option.c_str());
      
      
      legA.AddEntry(meanDev[i],legendString[i].c_str());
      legB.AddEntry(rmsDev[i],legendString[i].c_str());
      legC.AddEntry(meanDevSc[i],legendString[i].c_str());
      legD.AddEntry(rmsDevSc[i],legendString[i].c_str());
      
   }
   
   can.cd(1);
   legA.Draw("same");
   can.cd(2);
   legB.Draw("same");
   
   can.SaveAs("plots.ps");
   
   canB.cd(1);
   legC.Draw("same");
   canB.cd(2);
   legD.Draw("same");
   
   canB.SaveAs("plots.ps)");
   
   exit(0);
}

