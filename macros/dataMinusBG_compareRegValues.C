
#include <vector>
#include <sstream>
#include "combineAll.C"

void dataMinusBG_compareRegValues(void){
   
   
   TFile nominal("../rootFiles/dataMinusBG.tag1.toy500.root");
   
   TCanvas can("can","can",0,0,800,600);
   
    const unsigned int nToys = 500;
   std::string elOrMu = "el";
   std::string sampleName = "McNTag1_Perm0Top1";
   
   std::string outputFilename;
   std::stringstream outfile;
   outfile << "tag1.toy" << nToys << ".nominal.compareRegValues.ps";
   std::string outputFilename = outfile.str();
   
   drawPlots(nominal,nToys,"el","McNTag1_Perm0Top1",outputFilename,true,false);
   drawPlots(nominal,nToys,"mu","McNTag1_Perm0Top1",outputFilename,false,false);
   drawPlots(nominal,nToys,"el","McNTag1_Perm0Top2",outputFilename,false,false);
   drawPlots(nominal,nToys,"mu","McNTag1_Perm0Top2",outputFilename,false,false);
   drawPlots(nominal,nToys,"el","McNTag1_Perm0TopLepHad",outputFilename,false,false);
   drawPlots(nominal,nToys,"mu","McNTag1_Perm0TopLepHad",outputFilename,false,true);
   
   exit(0);
}

void drawPlots(TFile& nominal,const unsigned int nToys, const std::string elOrMu, const std::string sampleName, const std::string outputFilename, const bool first, const bool last){
   
   TCanvas can("can","can",0,0,800,600);
   
   int reg3_marker_style = 20;
   int reg4_marker_style = 22;
   int reg2_marker_style = 23;
   
   int reg3_color = kBlack;
   int reg4_color = kRed;
   int reg2_color = kBlue;
   
   double maximum = 4000.;
   double maximum_meanDev = 0.4;
   double minimum_meanDev = -0.6;
   double maximum_rmsDev = 0.1;
   double minimum_rmsDev = 0.0;
   
   
   std::stringstream leg_name;
   leg_name << "unfolded (Data-BG), " << elOrMu << ", ";
   if(sampleName.find("Top1") != std::string::npos){
      leg_name << "leptonic top";
      
      if(elOrMu.find("el") != std::string::npos){
         maximum = 4000.;
         maximum_meanDev = 0.4;
         minimum_meanDev = -0.6;
         maximum_rmsDev = 0.1;
         minimum_rmsDev = 0.0;
      }
      else{
         maximum = 8000.;
         maximum_meanDev = 0.7;
         minimum_meanDev = -0.3;
         maximum_rmsDev = 0.1;
         minimum_rmsDev = 0.0;
      }
      
   }
   else if( sampleName.find("Top2") != std::string::npos){
      leg_name << "hadronic top";
      
      if(elOrMu.find("el") != std::string::npos){
         maximum = 4000.;
         maximum_meanDev = 0.4;
         minimum_meanDev = -0.6;
         maximum_rmsDev = 0.1;
         minimum_rmsDev = 0.0;
      }
      else{
         maximum = 8000.;
         maximum_meanDev = 0.7;
         minimum_meanDev = -0.3;
         maximum_rmsDev = 0.1;
         minimum_rmsDev = 0.0;
      }
      
   }
   else if( sampleName.find("TopLepHad") != std::string::npos){
      leg_name << "lep+had top";
      
      if(elOrMu.find("el") != std::string::npos){
         maximum = 8000.;
         maximum_meanDev = 0.4;
         minimum_meanDev = -0.6;
         maximum_rmsDev = 0.1;
         minimum_rmsDev = 0.0;
      }
      else{
         maximum = 16000.;
         maximum_meanDev = 0.7;
         minimum_meanDev = -0.3;
         maximum_rmsDev = 0.1;
         minimum_rmsDev = 0.0;
      }
      
   }
   
   
   double leg_x1 = 0.5;
   double leg_y1 = 0.93;
   double leg_x2 = 0.95;
   double leg_y2 = 0.55;
   
   std::string basePath = "unfolding/toys/";
  
   std::stringstream nameBase_reg3;
   nameBase_reg3 << basePath << elOrMu << "/" << sampleName << "/reg" << 3 << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << 3 << "_toy" << nToys;
   
   std::stringstream nameBase_reg2;
   nameBase_reg2 << basePath << elOrMu << "/" << sampleName << "/reg" << 2 << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << 2 << "_toy" << nToys;
   
   std::stringstream nameBase_reg4;
   nameBase_reg4 << basePath << elOrMu << "/" << sampleName << "/reg" << 4 << "/toy" << nToys << "/H_" << elOrMu << "_" << sampleName << "_reg" << 4 << "_toy" << nToys;
   
   
   can.Clear();
   
   /// Draw all Unfolded results
   
   
   std::string name = nameBase_reg2.str() + "_UnfoldedReco";
   std::cout << "retreiving: " << name << "\n";
   TH1* unfolded_reg2 = nominal.Get(name.c_str());
   unfolded_reg2->SetMarkerColor(reg2_color);
   unfolded_reg2->SetLineColor(reg2_color);
   unfolded_reg2->SetMarkerStyle(reg2_marker_style);
   unfolded_reg2->SetTitle("");
   unfolded_reg2->SetMaximum(maximum);
   unfolded_reg2->Draw();
   
   name = nameBase_reg3.str() + "_UnfoldedReco";
   std::cout << "retreiving: " << name << "\n";
   TH1* unfolded_reg3 = nominal.Get(name.c_str());
   unfolded_reg3->SetMarkerColor(reg3_color);
   unfolded_reg3->SetLineColor(reg3_color);
   unfolded_reg3->SetMarkerStyle(reg3_marker_style);
   unfolded_reg3->SetTitle("");
   unfolded_reg3->SetMaximum(maximum);
   unfolded_reg3->Draw("same");
   
   name = nameBase_reg4.str() + "_UnfoldedReco";
   std::cout << "retreiving: " << name << "\n";
   TH1* unfolded_reg4 = nominal.Get(name.c_str());
   unfolded_reg4->SetMarkerColor(reg4_color);
   unfolded_reg4->SetLineColor(reg4_color);
   unfolded_reg4->SetMarkerStyle(reg4_marker_style);
   unfolded_reg4->SetTitle("");
   unfolded_reg4->SetMaximum(maximum);
   unfolded_reg4->Draw("same");
   
   TLegend unfolded_leg(leg_x1,leg_y1,leg_x2,leg_y2,leg_name.str().c_str());
   unfolded_leg.SetFillStyle(0);
   unfolded_leg.SetBorderSize(0);
   unfolded_leg.AddEntry(unfolded_reg2,"k = 2");
   unfolded_leg.AddEntry(unfolded_reg3,"k = 3");
   unfolded_leg.AddEntry(unfolded_reg4,"k = 4");
   unfolded_leg.Draw("same");
   
   if(first)
      can.SaveAs(std::string(outputFilename + "(").c_str());
   else
      can.SaveAs(outputFilename.c_str());
   
   /// DEVIATION
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name = nameBase_reg2.str() + "_meanDeviation";
   TH1* meanDeviation_reg2 = nominal.Get(name.c_str());
   meanDeviation_reg2->SetMarkerColor(reg2_color);
   meanDeviation_reg2->SetLineColor(reg2_color);
   meanDeviation_reg2->SetMarkerStyle(reg2_marker_style);
   meanDeviation_reg2->SetTitle("");
   meanDeviation_reg2->SetMaximum(maximum_meanDev);
   meanDeviation_reg2->SetMinimum(minimum_meanDev);
   meanDeviation_reg2->Draw();
   
   name = nameBase_reg3.str() + "_meanDeviation";
   TH1* meanDeviation_reg3 = nominal.Get(name.c_str());
   meanDeviation_reg3->SetMarkerColor(reg3_color);
   meanDeviation_reg3->SetLineColor(reg3_color);
   meanDeviation_reg3->SetMarkerStyle(reg3_marker_style);
   meanDeviation_reg3->SetTitle("");
   meanDeviation_reg3->SetMaximum(maximum_meanDev);
   meanDeviation_reg3->SetMinimum(minimum_meanDev);
   meanDeviation_reg3->Draw("same");
   
   name = nameBase_reg4.str() + "_meanDeviation";
   TH1* meanDeviation_reg4 = nominal.Get(name.c_str());
   meanDeviation_reg4->SetMarkerColor(reg4_color);
   meanDeviation_reg4->SetLineColor(reg4_color);
   meanDeviation_reg4->SetMarkerStyle(reg4_marker_style);
   meanDeviation_reg4->SetTitle("");
   meanDeviation_reg4->SetMaximum(maximum_meanDev);
   meanDeviation_reg4->SetMinimum(minimum_meanDev);
   meanDeviation_reg4->Draw("same");
   
   can.cd(2);
   
   name = nameBase_reg2.str() + "_rmsDeviation";
   TH1* rmsDeviation_reg2 = nominal.Get(name.c_str());
   rmsDeviation_reg2->SetMarkerColor(reg2_color);
   rmsDeviation_reg2->SetLineColor(reg2_color);
   rmsDeviation_reg2->SetMarkerStyle(reg2_marker_style);
   rmsDeviation_reg2->SetTitle("");
   rmsDeviation_reg2->SetMaximum(maximum_rmsDev);
   rmsDeviation_reg2->SetMinimum(minimum_rmsDev);
   rmsDeviation_reg2->Draw();
   
   name = nameBase_reg3.str() + "_rmsDeviation";
   TH1* rmsDeviation_reg3 = nominal.Get(name.c_str());
   rmsDeviation_reg3->SetMarkerColor(reg3_color);
   rmsDeviation_reg3->SetLineColor(reg3_color);
   rmsDeviation_reg3->SetMarkerStyle(reg3_marker_style);
   rmsDeviation_reg3->SetTitle("");
   rmsDeviation_reg3->SetMaximum(maximum_rmsDev);
   rmsDeviation_reg3->SetMinimum(minimum_rmsDev);
   rmsDeviation_reg3->Draw("same");
   
   name = nameBase_reg4.str() + "_rmsDeviation";
   TH1* rmsDeviation_reg4 = nominal.Get(name.c_str());
   rmsDeviation_reg4->SetMarkerColor(reg4_color);
   rmsDeviation_reg4->SetLineColor(reg4_color);
   rmsDeviation_reg4->SetMarkerStyle(reg4_marker_style);
   rmsDeviation_reg4->SetTitle("");
   rmsDeviation_reg4->SetMaximum(maximum_rmsDev);
   rmsDeviation_reg4->SetMinimum(minimum_rmsDev);
   rmsDeviation_reg4->Draw("same");
   
   can.SaveAs(outputFilename.c_str());
   
   /// SCALED DEVIATION
   can.Clear();
   can.Divide(1,2);
   can.cd(1);
   
   name = nameBase_reg2.str() + "_meanDeviationScaled";
   TH1* meanDeviationScaled_reg2 = nominal.Get(name.c_str());
   meanDeviationScaled_reg2->SetMarkerColor(reg2_color);
   meanDeviationScaled_reg2->SetLineColor(reg2_color);
   meanDeviationScaled_reg2->SetMarkerStyle(reg2_marker_style);
   meanDeviationScaled_reg2->SetTitle("");
   meanDeviationScaled_reg2->SetMaximum(maximum_meanDev);
   meanDeviationScaled_reg2->SetMinimum(minimum_meanDev);
   meanDeviationScaled_reg2->Draw();
   
   name = nameBase_reg3.str() + "_meanDeviationScaled";
   TH1* meanDeviationScaled_reg3 = nominal.Get(name.c_str());
   meanDeviationScaled_reg3->SetMarkerColor(reg3_color);
   meanDeviationScaled_reg3->SetLineColor(reg3_color);
   meanDeviationScaled_reg3->SetMarkerStyle(reg3_marker_style);
   meanDeviationScaled_reg3->SetTitle("");
   meanDeviationScaled_reg3->SetMaximum(maximum_meanDev);
   meanDeviationScaled_reg3->SetMinimum(minimum_meanDev);
   meanDeviationScaled_reg3->Draw("same");
   
   name = nameBase_reg4.str() + "_meanDeviationScaled";
   TH1* meanDeviationScaled_reg4 = nominal.Get(name.c_str());
   meanDeviationScaled_reg4->SetMarkerColor(reg4_color);
   meanDeviationScaled_reg4->SetLineColor(reg4_color);
   meanDeviationScaled_reg4->SetMarkerStyle(reg4_marker_style);
   meanDeviationScaled_reg4->SetTitle("");
   meanDeviationScaled_reg4->SetMaximum(maximum_meanDev);
   meanDeviationScaled_reg4->SetMinimum(minimum_meanDev);
   meanDeviationScaled_reg4->Draw("same");
   
   can.cd(2);
   
   name = nameBase_reg2.str() + "_rmsDeviationScaled";
   TH1* rmsDeviationScaled_reg2 = nominal.Get(name.c_str());
   rmsDeviationScaled_reg2->SetMarkerColor(reg2_color);
   rmsDeviationScaled_reg2->SetLineColor(reg2_color);
   rmsDeviationScaled_reg2->SetMarkerStyle(reg2_marker_style);
   rmsDeviationScaled_reg2->SetTitle("");
   rmsDeviationScaled_reg2->SetMaximum(maximum_rmsDev);
   rmsDeviationScaled_reg2->SetMinimum(minimum_rmsDev);
   rmsDeviationScaled_reg2->Draw();
   
   name = nameBase_reg3.str() + "_rmsDeviationScaled";
   TH1* rmsDeviationScaled_reg3 = nominal.Get(name.c_str());
   rmsDeviationScaled_reg3->SetMarkerColor(reg3_color);
   rmsDeviationScaled_reg3->SetLineColor(reg3_color);
   rmsDeviationScaled_reg3->SetMarkerStyle(reg3_marker_style);
   rmsDeviationScaled_reg3->SetTitle("");
   rmsDeviationScaled_reg3->SetMaximum(maximum_rmsDev);
   rmsDeviationScaled_reg3->SetMinimum(minimum_rmsDev);
   rmsDeviationScaled_reg3->Draw("same");
   
   name = nameBase_reg4.str() + "_rmsDeviationScaled";
   TH1* rmsDeviationScaled_reg4 = nominal.Get(name.c_str());
   rmsDeviationScaled_reg4->SetMarkerColor(reg4_color);
   rmsDeviationScaled_reg4->SetLineColor(reg4_color);
   rmsDeviationScaled_reg4->SetMarkerStyle(reg4_marker_style);
   rmsDeviationScaled_reg4->SetTitle("");
   rmsDeviationScaled_reg4->SetMaximum(maximum_rmsDev);
   rmsDeviationScaled_reg4->SetMinimum(minimum_rmsDev);
   rmsDeviationScaled_reg4->Draw("same");
   
   if(last)
      can.SaveAs(std::string(outputFilename+")").c_str());
   else
      can.SaveAs(outputFilename.c_str());
   
   return;
}
