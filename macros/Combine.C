

//void temp(void)
{
#include <vector>

   TFile in("analysis.root");
   
   const std::string path = "unfolding/toys/";
   const unsigned int nToys = 500;
   const unsigned int regValue = 3;
   
   std::vector<std::string> sampleList;
   sampleList.push_back("McNTag0_Perm0Top1");
   sampleList.push_back("McNTag0_Perm0Top2");
   sampleList.push_back("McNTag0_Perm0TopLepHad");
   
   sampleList.push_back("McNTag1_Perm0Top1");
   sampleList.push_back("McNTag1_Perm0Top2");
   sampleList.push_back("McNTag1_Perm0TopLepHad");
   
   sampleList.push_back("McNTag1Excl_Perm0Top1");
   sampleList.push_back("McNTag1Excl_Perm0Top2");
   sampleList.push_back("McNTag1Excl_Perm0TopLepHad");
   
   sampleList.push_back("McNTag2_Perm0Top1");
   sampleList.push_back("McNTag2_Perm0Top2");
   sampleList.push_back("McNTag2_Perm0TopLepHad");
   
   sampleList.push_back("McNTag2Excl_Perm0Top1");
   sampleList.push_back("McNTag2Excl_Perm0Top2");
   sampleList.push_back("McNTag2Excl_Perm0TopLepHad");
   
   sampleList.push_back("PeterTopMassCut");
   
   std::vector<TH1*> histoListTruth;
   histoListTruth.resize(sampleList.size(),0);
   std::vector<TH1*> histoListReco;
   histoListReco.resize(sampleList.size(),0);
   
   TCanvas canA("canA","canA",0,0,1200,600);
   canA.SetMargin(0,0,0,0);
   canA.Divide(2,1);
   
   TLegend legA1(0.6,0.92,0.9,0.45);
   legA1.SetBorderSize(0);
   legA1.SetFillStyle(0);
   legA1.SetHeader("mc_truth p_{T}");
   
   
   TLegend legA2(0.6,0.92,0.9,0.45);
   legA2.SetBorderSize(0);
   legA2.SetFillStyle(0);
   legA2.SetHeader("mc_reco p_{T}");
   
   for(unsigned int i=0;i<sampleList.size();++i){
      
      std::stringstream histoPath;
      histoPath << path << "H_" << sampleList[i] << "_toy" << nToys << "_reg" << regValue;
      
      std::string truthName = histoPath.str() + "_mc_truth";
      
      std::cout << "Retrieving: " << truthName << std::endl;
      
      unsigned int style = 20+i;
      if(style > 34) style = 6+i;
      unsigned int color = i+1;
      if(color > 9) color = i-8;
      
      histoListTruth[i] = (TH1*)in.Get(truthName.c_str());
      histoListTruth[i]->SetMarkerStyle(style);
      histoListTruth[i]->SetMarkerColor(color);
      histoListTruth[i]->SetLineColor(color);
      histoListTruth[i]->SetTitle("");
      
      if( i != 0 && histoListTruth[0]->GetMaximum() < histoListTruth[i]->GetMaximum())
         histoListTruth[0]->SetMaximum(histoListTruth[i]->GetMaximum()*1.15);
      
      canA.cd(1);
      if(i==0)
         histoListTruth[i]->Draw();
      else
         histoListTruth[i]->Draw("same");
      
      legA1->AddEntry(histoListTruth[i],sampleList[i].c_str());
      
      std::string recoName = histoPath.str() + "_mc_reco";
      
      histoListReco[i] = (TH1*)in.Get(recoName.c_str());
      histoListReco[i]->SetMarkerStyle(style);
      histoListReco[i]->SetMarkerColor(color);
      histoListReco[i]->SetLineColor(color);
      histoListReco[i]->SetTitle("");
      
      if( i != 0 && histoListReco[0]->GetMaximum() < histoListReco[i]->GetMaximum())
         histoListReco[0]->SetMaximum(histoListReco[i]->GetMaximum()*1.15);
      
      canA.cd(2);
      if(i==0)
         histoListReco[i]->Draw();
      else
         histoListReco[i]->Draw("same");
      
      legA2->AddEntry(histoListReco[i],sampleList[i].c_str());
      
      
   }
   
   canA.cd(1);
   legA1.Draw("same");
   
   canA.cd(2);
   legA2.Draw("same");
   
   
   canA.SaveAs("combineTruthAndReco.png");
   
   canA.SaveAs("combineTruthAndReco.ps(");
   
   
   ///////////////////////////
   /// Plot the Tag0
   //////////////////////////
   
   canA.Clear();
   canA.SetMargin(0,0,0,0);
   canA.Divide(2,1);
   
   legA1.Clear();
   legA1.SetBorderSize(0);
   legA1.SetFillStyle(0);
   legA1.SetHeader("mc_truth p_{T} Tag0 Only");
   
   
   legA2.Clear();
   legA2.SetBorderSize(0);
   legA2.SetFillStyle(0);
   legA2.SetHeader("mc_reco p_{T} Tag0 Only");
   
   TH1* firstTruth = 0;
   TH1* firstReco = 0;
   
   for(unsigned int i=0;i<sampleList.size();++i){
      
      // only want Tag0 plots
      if(sampleList[i].find("McNTag0_") == std::string::npos) continue;
      
      // set maximum of first plot to ensure all data points are visible
      if(firstTruth == 0)
         firstTruth = histoListTruth[i];
      else if( firstTruth->GetMaximum() < histoListTruth[i]->GetMaximum())
         firstTruth->SetMaximum(histoListTruth[i]->GetMaximum()*1.15);
      
      canA.cd(1);
      if(i==0)
         histoListTruth[i]->Draw();
      else
         histoListTruth[i]->Draw("same");
      
      legA1->AddEntry(histoListTruth[i],sampleList[i].c_str());
      
      if( firstReco==0 ) 
         firstReco = histoListReco[i];
      else if( firstReco->GetMaximum() < histoListReco[i]->GetMaximum())
         firstReco->SetMaximum(histoListReco[i]->GetMaximum()*1.15);
      
      canA.cd(2);
      if(i==0)
         histoListReco[i]->Draw();
      else
         histoListReco[i]->Draw("same");
      
      legA2->AddEntry(histoListReco[i],sampleList[i].c_str());
   }
   
   canA.cd(1);
   legA1.Draw("same");
   canA.cd(2);
   legA2.Draw("same");
   
   canA.SaveAs("combineTruthAndReco_McNTag0.png");
   canA.SaveAs("combineTruthAndReco.ps");
   
   ///////////////////////////
   /// Plot the Tag1
   //////////////////////////
   
   canA.Clear();
   canA.SetMargin(0,0,0,0);
   canA.Divide(2,1);
   
   legA1.Clear();
   legA1.SetBorderSize(0);
   legA1.SetFillStyle(0);
   legA1.SetHeader("mc_truth p_{T} Tag1 Only");
   
   
   legA2.Clear();
   legA2.SetBorderSize(0);
   legA2.SetFillStyle(0);
   legA2.SetHeader("mc_reco p_{T} Tag1 Only");
   
   firstTruth = 0;
   firstReco = 0;
   
   for(unsigned int i=0;i<sampleList.size();++i){
      
      // only want Tag0 plots
      if(sampleList[i].find("McNTag1_") == std::string::npos) continue;
      
      // set maximum of first plot to ensure all data points are visible
      if(firstTruth == 0)
         firstTruth = histoListTruth[i];
      else if( firstTruth->GetMaximum() < histoListTruth[i]->GetMaximum())
         firstTruth->SetMaximum(histoListTruth[i]->GetMaximum()*1.15);
      
      canA.cd(1);
      if(i==0)
         histoListTruth[i]->Draw();
      else
         histoListTruth[i]->Draw("same");
      
      legA1->AddEntry(histoListTruth[i],sampleList[i].c_str());
      
      if( firstReco==0 ) 
         firstReco = histoListReco[i];
      else if( firstReco->GetMaximum() < histoListReco[i]->GetMaximum())
         firstReco->SetMaximum(histoListReco[i]->GetMaximum()*1.15);
      
      canA.cd(2);
      if(i==0)
         histoListReco[i]->Draw();
      else
         histoListReco[i]->Draw("same");
      
      legA2->AddEntry(histoListReco[i],sampleList[i].c_str());
   }
   
   canA.cd(1);
   legA1.Draw("same");
   canA.cd(2);
   legA2.Draw("same");
   
   canA.SaveAs("combineTruthAndReco_McNTag1.png");
   canA.SaveAs("combineTruthAndReco.ps");
   
   
   
   ///////////////////////////
   /// Plot the Tag1Excl
   //////////////////////////
   
   canA.Clear();
   canA.SetMargin(0,0,0,0);
   canA.Divide(2,1);
   
   legA1.Clear();
   legA1.SetBorderSize(0);
   legA1.SetFillStyle(0);
   legA1.SetHeader("mc_truth p_{T} Tag1Excl Only");
   
   
   legA2.Clear();
   legA2.SetBorderSize(0);
   legA2.SetFillStyle(0);
   legA2.SetHeader("mc_reco p_{T} Tag1Excl Only");
   
   firstTruth = 0;
   firstReco = 0;
   
   for(unsigned int i=0;i<sampleList.size();++i){
      
      // only want Tag0 plots
      if(sampleList[i].find("McNTag1Excl_") == std::string::npos) continue;
      
      // set maximum of first plot to ensure all data points are visible
      if(firstTruth == 0)
         firstTruth = histoListTruth[i];
      else if( firstTruth->GetMaximum() < histoListTruth[i]->GetMaximum())
         firstTruth->SetMaximum(histoListTruth[i]->GetMaximum()*1.15);
      
      canA.cd(1);
      if(i==0)
         histoListTruth[i]->Draw();
      else
         histoListTruth[i]->Draw("same");
      
      legA1->AddEntry(histoListTruth[i],sampleList[i].c_str());
      
      if( firstReco==0 ) 
         firstReco = histoListReco[i];
      else if( firstReco->GetMaximum() < histoListReco[i]->GetMaximum())
         firstReco->SetMaximum(histoListReco[i]->GetMaximum()*1.15);
      
      canA.cd(2);
      if(i==0)
         histoListReco[i]->Draw();
      else
         histoListReco[i]->Draw("same");
      
      legA2->AddEntry(histoListReco[i],sampleList[i].c_str());
   }
   
   canA.cd(1);
   legA1.Draw("same");
   canA.cd(2);
   legA2.Draw("same");
   
   canA.SaveAs("combineTruthAndReco_McNTag1Excl.png");
   canA.SaveAs("combineTruthAndReco.ps");
   
   ///////////////////////////
   /// Plot the Tag2
   //////////////////////////
   
   canA.Clear();
   canA.SetMargin(0,0,0,0);
   canA.Divide(2,1);
   
   legA1.Clear();
   legA1.SetBorderSize(0);
   legA1.SetFillStyle(0);
   legA1.SetHeader("mc_truth p_{T} Tag2 Only");
   
   
   legA2.Clear();
   legA2.SetBorderSize(0);
   legA2.SetFillStyle(0);
   legA2.SetHeader("mc_reco p_{T} Tag2 Only");
   
   firstTruth = 0;
   firstReco = 0;
   
   for(unsigned int i=0;i<sampleList.size();++i){
      
      // only want Tag0 plots
      if(sampleList[i].find("McNTag2_") == std::string::npos) continue;
      
      // set maximum of first plot to ensure all data points are visible
      if(firstTruth == 0)
         firstTruth = histoListTruth[i];
      else if( firstTruth->GetMaximum() < histoListTruth[i]->GetMaximum())
         firstTruth->SetMaximum(histoListTruth[i]->GetMaximum()*1.15);
      
      canA.cd(1);
      if(i==0)
         histoListTruth[i]->Draw();
      else
         histoListTruth[i]->Draw("same");
      
      legA1->AddEntry(histoListTruth[i],sampleList[i].c_str());
      
      if( firstReco==0 ) 
         firstReco = histoListReco[i];
      else if( firstReco->GetMaximum() < histoListReco[i]->GetMaximum())
         firstReco->SetMaximum(histoListReco[i]->GetMaximum()*1.15);
      
      canA.cd(2);
      if(i==0)
         histoListReco[i]->Draw();
      else
         histoListReco[i]->Draw("same");
      
      legA2->AddEntry(histoListReco[i],sampleList[i].c_str());
   }
   
   canA.cd(1);
   legA1.Draw("same");
   canA.cd(2);
   legA2.Draw("same");
   
   canA.SaveAs("combineTruthAndReco_McNTag2.png");
   canA.SaveAs("combineTruthAndReco.ps");
   
   
   
   ///////////////////////////
   /// Plot the Tag2Excl
   //////////////////////////
   
   canA.Clear();
   canA.SetMargin(0,0,0,0);
   canA.Divide(2,1);
   
   legA1.Clear();
   legA1.SetBorderSize(0);
   legA1.SetFillStyle(0);
   legA1.SetHeader("mc_truth p_{T} Tag2Excl Only");
   
   
   legA2.Clear();
   legA2.SetBorderSize(0);
   legA2.SetFillStyle(0);
   legA2.SetHeader("mc_reco p_{T} Tag2Excl Only");
   
   firstTruth = 0;
   firstReco = 0;
   
   for(unsigned int i=0;i<sampleList.size();++i){
      
      // only want Tag0 plots
      if(sampleList[i].find("McNTag2Excl_") == std::string::npos) continue;
      
      // set maximum of first plot to ensure all data points are visible
      if(firstTruth == 0)
         firstTruth = histoListTruth[i];
      else if( firstTruth->GetMaximum() < histoListTruth[i]->GetMaximum())
         firstTruth->SetMaximum(histoListTruth[i]->GetMaximum()*1.15);
      
      canA.cd(1);
      if(i==0)
         histoListTruth[i]->Draw();
      else
         histoListTruth[i]->Draw("same");
      
      legA1->AddEntry(histoListTruth[i],sampleList[i].c_str());
      
      if( firstReco==0 ) 
         firstReco = histoListReco[i];
      else if( firstReco->GetMaximum() < histoListReco[i]->GetMaximum())
         firstReco->SetMaximum(histoListReco[i]->GetMaximum()*1.15);
      
      canA.cd(2);
      if(i==0)
         histoListReco[i]->Draw();
      else
         histoListReco[i]->Draw("same");
      
      legA2->AddEntry(histoListReco[i],sampleList[i].c_str());
   }
   
   canA.cd(1);
   legA1.Draw("same");
   canA.cd(2);
   legA2.Draw("same");
   
   canA.SaveAs("combineTruthAndReco_McNTag2Excl.png");
   canA.SaveAs("combineTruthAndReco.ps");
   
}
