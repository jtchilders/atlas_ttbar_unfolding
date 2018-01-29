

//void temp(void)
{

   TFile in("data/peter/MC_el_T1_McAtNlo_Jimmy_nominal.root");
   
   TH1* tag0_partonTop1 = (TH1*)in.Get("Kinem/TopFit_NTag0/PartonTop1/PartonTop1PtSpectrum");
   tag0_partonTop1->SetMarkerStyle(21);
   tag0_partonTop1->SetMarkerColor(kGreen);
   tag0_partonTop1->SetLineColor(kGreen);
   tag0_partonTop1->SetTitle("");
   TH1* tag0_perm0Top1 = (TH1*)in.Get("Kinem/TopFit_NTag0/Perm0Top1/Perm0Top1PtSpectrum");
   tag0_perm0Top1->SetMarkerStyle(21);
   tag0_perm0Top1->SetMarkerColor(kGreen);
   tag0_perm0Top1->SetLineColor(kGreen);
   tag0_perm0Top1->SetTitle("");
   TH2* tag0_perm0Top1_matrix = (TH2*)in.Get("Kinem/TopFit_NTag0/Perm0Top1RecoVsPerm0Top1Truth/Perm0Top1RecoVsPerm0Top1TruthPt2VsPt1");
   
   TH1* tag1_partonTop1 = (TH1*)in.Get("Kinem/TopFit_NTag1/PartonTop1/PartonTop1PtSpectrum");
   tag1_partonTop1->SetMarkerStyle(22);
   tag1_partonTop1->SetMarkerColor(kBlue);
   tag1_partonTop1->SetLineColor(kBlue);
   tag1_partonTop1->SetTitle("");
   TH1* tag1_perm0Top1 = (TH1*)in.Get("Kinem/TopFit_NTag1/Perm0Top1/Perm0Top1PtSpectrum");
   tag1_perm0Top1->SetMarkerStyle(22);
   tag1_perm0Top1->SetMarkerColor(kBlue);
   tag1_perm0Top1->SetLineColor(kBlue);
   tag1_perm0Top1->SetTitle("");
   TH2* tag1_perm0Top1_matrix = (TH2*)in.Get("Kinem/TopFit_NTag1/Perm0Top1RecoVsPerm0Top1Truth/Perm0Top1RecoVsPerm0Top1TruthPt2VsPt1");
   
   TH1* tag2_partonTop1 = (TH1*)in.Get("Kinem/TopFit_NTag2/PartonTop1/PartonTop1PtSpectrum");
   tag2_partonTop1->SetMarkerStyle(23);
   tag2_partonTop1->SetMarkerColor(kBlack);
   tag2_partonTop1->SetLineColor(kBlack);
   tag2_partonTop1->SetTitle("");
   TH1* tag2_perm0Top1 = (TH1*)in.Get("Kinem/TopFit_NTag2/Perm0Top1/Perm0Top1PtSpectrum");
   tag2_perm0Top1->SetMarkerStyle(23);
   tag2_perm0Top1->SetMarkerColor(kBlack);
   tag2_perm0Top1->SetLineColor(kBlack);
   tag2_perm0Top1->SetTitle("");
   TH2* tag2_perm0Top1_matrix = (TH2*)in.Get("Kinem/TopFit_NTag2/Perm0Top1RecoVsPerm0Top1Truth/Perm0Top1RecoVsPerm0Top1TruthPt2VsPt1");
   
   TH1* tag1ex_partonTop1 = (TH1*)in.Get("Kinem/TopFit_NTag1Excl/PartonTop1/PartonTop1PtSpectrum");
   tag1ex_partonTop1->SetMarkerStyle(24);
   tag1ex_partonTop1->SetMarkerColor(kRed);
   tag1ex_partonTop1->SetLineColor(kRed);
   tag1ex_partonTop1->SetTitle("");
   TH1* tag1ex_perm0Top1 = (TH1*)in.Get("Kinem/TopFit_NTag1Excl/Perm0Top1/Perm0Top1PtSpectrum");
   tag1ex_perm0Top1->SetMarkerStyle(24);
   tag1ex_perm0Top1->SetMarkerColor(kRed);
   tag1ex_perm0Top1->SetLineColor(kRed);
   tag1ex_perm0Top1->SetTitle("");
   TH2* tag1ex_perm0Top1_matrix = (TH2*)in.Get("Kinem/TopFit_NTag1Excl/Perm0Top1RecoVsPerm0Top1Truth/Perm0Top1RecoVsPerm0Top1TruthPt2VsPt1");
   
   TH1* tag2ex_partonTop1 = (TH1*)in.Get("Kinem/TopFit_NTag2Excl/PartonTop1/PartonTop1PtSpectrum");
   tag2ex_partonTop1->SetMarkerStyle(25);
   tag2ex_partonTop1->SetMarkerColor(kCyan);
   tag2ex_partonTop1->SetLineColor(kCyan);
   tag2ex_partonTop1->SetTitle("");
   TH1* tag2ex_perm0Top1 = (TH1*)in.Get("Kinem/TopFit_NTag2Excl/Perm0Top1/Perm0Top1PtSpectrum");
   tag2ex_perm0Top1->SetMarkerStyle(25);
   tag2ex_perm0Top1->SetMarkerColor(kCyan);
   tag2ex_perm0Top1->SetLineColor(kCyan);
   tag2ex_perm0Top1->SetTitle("");
   TH2* tag2ex_perm0Top1_matrix = (TH2*)in.Get("Kinem/TopFit_NTag2Excl/Perm0Top1RecoVsPerm0Top1Truth/Perm0Top1RecoVsPerm0Top1TruthPt2VsPt1");
   
   TH1* tag1mc_partonTop1 = (TH1*)in.Get("Kinem/TopFit_NTag1/MiscHistos/TruthTopPt_goodMass");
   tag1mc_partonTop1->SetMarkerStyle(26);
   tag1mc_partonTop1->SetMarkerColor(kViolet);
   tag1mc_partonTop1->SetLineColor(kViolet);
   tag1mc_partonTop1->SetTitle("");
   TH1* tag1mc_perm0Top1 = (TH1*)in.Get("Kinem/TopFit_NTag1/MiscHistos/TopPt_goodMass");
   tag1mc_perm0Top1->SetMarkerStyle(26);
   tag1mc_perm0Top1->SetMarkerColor(kViolet);
   tag1mc_perm0Top1->SetLineColor(kViolet);
   tag1mc_perm0Top1->SetTitle("");
   TH2* tag1mc_perm0Top1_matrix = (TH2*)in.Get("Kinem/TopFit_NTag1/MiscHistos/MigrationPt_goodMass");
   
   TLegend leg(0.5,0.9,0.7,0.7);
   leg.SetBorderSize(0);
   leg.SetFillStyle(0);
   leg.AddEntry(tag0_partonTop1,"NTag0");
   leg.AddEntry(tag1_partonTop1,"NTag1");
   leg.AddEntry(tag2_partonTop1,"NTag2");
   leg.AddEntry(tag1ex_partonTop1,"NTag1Excl");
   leg.AddEntry(tag2ex_partonTop1,"NTag2Excl");
   leg.AddEntry(tag1mc_partonTop1,"NTag1_masscut");
   
   
   TCanvas can1("can1","can1",0,0,1200,600);
   can1.Divide(2,1);
   
   can1.cd(1);
   tag0_partonTop1->SetMaximum(5000);
   tag0_partonTop1->Draw();
   tag1_partonTop1->Draw("same");
   tag2_partonTop1->Draw("same");
   tag1ex_partonTop1->Draw("same");
   tag2ex_partonTop1->Draw("same");
   tag1mc_partonTop1->Draw("same");
   leg->SetHeader("PartonTop1 for:");
   leg->Draw("same");
   
   can1.cd(2);
   tag0_perm0Top1->SetMaximum(5000);
   tag0_perm0Top1->Draw();
   tag1_perm0Top1->Draw("same");
   tag2_perm0Top1->Draw("same");
   tag1ex_perm0Top1->Draw("same");
   tag2ex_perm0Top1->Draw("same");
   tag1mc_perm0Top1->Draw("same");
   leg->SetHeader("Perm0Top1 for:");
   leg->Draw("same");
   
   can1.SaveAs("compareTruthAndMcReco.png");
   
   TCanvas can2("can2","can2",0,0,1200,600);
   can2.SetMargin(0.,0.,0.,0.);
   can2.Divide(3,2);
   
   can2.cd(1);
   can2.cd(1)->SetLogz(true);
   can2.cd(1)->SetFrameBorderSize(0.);
   tag0_perm0Top1_matrix->SetTitle("Perm0Top1RecoVsPerm0Top1Truth for tag0");
   tag0_perm0Top1_matrix->Draw("colz");
   can2.cd(2);
   can2.cd(2)->SetLogz(true);
   tag1_perm0Top1_matrix->SetTitle("Perm0Top1RecoVsPerm0Top1Truth for tag1");
   tag1_perm0Top1_matrix->Draw("colz");
   can2.cd(3);
   can2.cd(3)->SetLogz(true);
   tag2_perm0Top1_matrix->SetTitle("Perm0Top1RecoVsPerm0Top1Truth for tag2");
   tag2_perm0Top1_matrix->Draw("colz");
   can2.cd(4);
   can2.cd(4)->SetLogz(true);
   tag1ex_perm0Top1_matrix->SetTitle("Perm0Top1RecoVsPerm0Top1Truth for tag1Excl");
   tag1ex_perm0Top1_matrix->Draw("colz");
   can2.cd(5);
   can2.cd(5)->SetLogz(true);
   tag2ex_perm0Top1_matrix->SetTitle("Perm0Top1RecoVsPerm0Top1Truth for tag2Excl");
   tag2ex_perm0Top1_matrix->Draw("colz");
   can2.cd(6);
   can2.cd(6)->SetLogz(true);
   tag1mc_perm0Top1_matrix->SetTitle("Perm0Top1RecoVsPerm0Top1Truth for tag1 with mass cut");
   tag1mc_perm0Top1_matrix->Draw("colz");
   
   can2.SaveAs("compareMigrationMatrix.png");
   
}
