#include "combineAll.h"

void combineAll(TString spectrum = "Pt", TString tag = "tagged", TString ljets = "mu"){  
     //change the paths!!!
  TString unfoldedPath = "../rootFiles/dataMinusBG.tag1.toy500.root";
  TString unfoldedHist="unfolding/toys/"+ljets+"/McNTag1_Perm0TopLepHad/reg2/toy500/H_"+ljets+"_McNTag1_Perm0TopLepHad_reg2_toy500_UnfoldedReco";
  TString efficiencyPath="../data/root_mc11c-20120223/nominal/Efficiency/efficiencies.root";
  TString efficiencyHist=tag+"_"+ljets;
  TString Ytitle;
  Float_t scalefactor=1.;
  if (spectrum=="Pt"){
    efficiencyHist+="_TopLepHadPt";
    Ytitle="d#sigma/dp_{T}^{top} [pb/GeV]";
    scalefactor=0.5;    //in one event there are two top quarks. Both leptonic and hadronic top Pt were filled.
  }
  if (spectrum=="Mtt"){
    efficiencyHist+="_SystemMass";
    Ytitle="d#sigma/dM^{t#bar{t}} [pb/GeV]";
  }
  if (spectrum=="Ptt"){
    efficiencyHist+="_SystemPt";
    Ytitle="d#sigma/dp_{T}^{t#bar{t}} [pb/GeV]";
  }
  if (spectrum=="leptonicPt"){
    efficiencyHist+="_Top1Pt";
    Ytitle="d#sigma/dp_{T}^{top} [pb/GeV]";
  } 
  if (spectrum=="hadronicPt"){
    efficiencyHist+="_Top2Pt";
    Ytitle="d#sigma/dp_{T}^{top} [pb/GeV]";
  } 
  TString outputDir="combineAll/";
  TString fileName = outputDir  + tag + "_" + ljets + "_" + spectrum; 
  Float_t lumi=4713.11;
  Float_t lumiError=0.035;  //3.5%
  
  gStyle->SetOptFit(1);
  gROOT->SetStyle("Plain");
  gStyle->SetFillColor(10);
  gStyle->SetPalette(1);
  gStyle->SetOptStat("");
  gSystem->mkdir("png/"+outputDir,1);
  gSystem->mkdir("eps/"+outputDir,1);
  gSystem->mkdir("pdf/"+outputDir,1);
  gSystem->mkdir("hist/"+outputDir,1);

  TH1F *unfolded,*efficiency;
  TCanvas *can=new TCanvas("can","Final spectrum",10,10,800,600);
  can->SetLeftMargin(0.13);
  can->SetBottomMargin(0.13);
  
  cout<<"getting unfolded root file"<<endl;
  TFile *unfoldedFile = new TFile(unfoldedPath.Data());
  if (!unfoldedFile || unfoldedFile -> IsZombie()) {
    cout << "ERROR opening the unfolded file!" << endl;
    return;
  }
  
  cout<<"getting efficiency root file"<<endl;
  TFile *efficiencyFile = new TFile(efficiencyPath.Data());
  if (!efficiencyFile || efficiencyFile -> IsZombie()) {
    cout << "ERROR opening the efficiency file!" << endl;
    return;
  }
  
 
  unfolded = (TH1F*)unfoldedFile->Get(unfoldedHist);
  efficiency = (TH1F*)efficiencyFile->Get(efficiencyHist);
   
  unfolded->Scale(1.);
//   unfolded->GetYaxis()->SetTitleOffset(1.2);
//   unfolded->GetXaxis()->SetTitleOffset(1.1);
//   unfolded->GetYaxis()->SetTitle("Events");
//   unfolded->SetMarkerSize(1.2);
//   unfolded->SetMarkerStyle(20);
//   unfolded->SetMarkerColor(4);
//   unfolded->SetLineColor(4);
 
  
  //drawing the original spectrum
  unfolded->Draw();
  SaveCanvas(can,fileName+"_beforeEffCorrection",true);
  //drawing the efficiecy
  efficiency->Draw();
  SaveCanvas(can,fileName+"_efficiency");
  
  //dividing by efficiency
  unfolded->Divide(efficiency);
  unfolded->Scale(100.);
  unfolded->Draw();
  SaveCanvas(can,fileName+"_afterEffCorrection",true);
  
  
  //scale by luminosity, the luminosity error is also added!!! 
  for (Int_t i=1;i<unfolded->GetNbinsX()+1;++i){
    unfolded->SetBinError(i,sqrt(unfolded->GetBinError(i)*unfolded->GetBinError(i)+unfolded->GetBinContent(i)*unfolded->GetBinContent(i)*lumiError*lumiError)/lumi);
    unfolded->SetBinContent(i,unfolded->GetBinContent(i)/lumi);
  }
  unfolded->Scale(scalefactor);
  Float_t SquareError=0;
  Float_t Integral=0;
  for (Int_t i=1;i<unfolded->GetNbinsX()+1;++i){
    SquareError+=unfolded->GetBinError(i)*unfolded->GetBinError(i);
  }  
  Integral=unfolded->Integral();
  
  //dividing by bin width 
  for (Int_t i=1;i<unfolded->GetNbinsX()+1;++i){
    unfolded->SetBinContent(i,unfolded->GetBinContent(i)/unfolded->GetBinWidth(i));
    unfolded->SetBinError(i,unfolded->GetBinError(i)/unfolded->GetBinWidth(i));
  }
  
  unfolded->Scale(1.);
  unfolded->GetYaxis()->SetTitle(Ytitle);  
  unfolded->Draw();
  
  //write the total cross section
 
  TString ToWrite;
  TLatex l2;
  l2.SetNDC();
  l2.SetTextFont(2);
  l2.SetTextSize(0.04); 
  l2.SetTextColor(1);
  TString number;
  number=Form("%2.1f",Integral);
  ToWrite="Total cross section: (" + number;
  number=Form("%2.1f",sqrt(SquareError));
  ToWrite+=" #pm " + number + ") pb";
  l2.DrawLatex(0.45,0.85,ToWrite.Data());
  ToWrite="theoretical: (73.4 #pm 5.0) pb";   //ttbar cross section for single lepton channel; the error is not exact
  l2.SetTextSize(0.033); 
  l2.DrawLatex(0.55,0.8,ToWrite.Data());
  
  
  SaveCanvas(can,fileName+"_DiffCrossSection", true);


//saving to root file
  TString option="RECREATE";
  TFile *MyFile=new TFile(("hist/"+fileName+".root").Data(),option.Data());
  unfolded->Write();
  MyFile->Close();
} 
