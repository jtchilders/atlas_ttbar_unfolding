#include <vector>
#include <cmath>
#include <math.h>
#include <ctime>
#include <cstdlib>
#include <map>
#include "TDatime.h"
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

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

#include "AtlasStyle.h"
#include "globals.h"
#include "MCFMHisto.h"
#include "MCFMWithPDF.h"
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
#include "MasterSystList.h"
#include "LatexDoc.h"
#include "TexTable.h"
#include "SystematicSample.h"


// theory background:
// http://www.sciencedirect.com/science/article/pii/S0168900203003292

void output_latex_table(const SystematicBinList& combination,
                        const SystematicBinList& combinationNorm,
                        const std::string sampleName,
                        PValueList& pValues,
                        PValueList& pValuesNorm);


void plot_XS(TGraphAsymmErrors nominalSyst,TH1* syst,TH1* nominal,
             std::string filename,std::string outputPath,
             std::string legendLabel,std::string topSample,
             LatexDoc* const latexDoc,std::string sampleName,
             PValueList& pValues,
             const bool norm,const bool log,
             TMatrixD const * const Cov);

void combinePlot(double value, double statUnc, double sysUp,
                 double sysDown,std::string outputPath,
                 std::string filename,std::string topSample,
                 LatexDoc* const latexDoc,std::string sampleName,
                 std::string generator);

double GetIntegral(TGraphAsymmErrors* graph);

int main(int argc,char** argv){
   
   SetAtlasStyle();
   
   
   std::vector<std::string> correlatedSystematics;
   if(!MasterSystList::UseJesComponents())
      correlatedSystematics.push_back("jes");
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
   sampleNames.push_back("Tag1_Top1_lhood");
   sampleNames.push_back("Tag1_Top2_lhood");
   sampleNames.push_back("Tag1_SystemMass");
   sampleNames.push_back("Tag1_SystemPt");
   sampleNames.push_back("Tag1_SystemRapidity");
   
   std::string basePath = globals::GetCombinationTextFilePath();
   
   
   // variables to hold pvalues and chi2's
   PValueList pValues,pValuesNorm;
   
   for(unsigned int type = 0;type<sampleNames.size();++type){
      std::cout << "sample: " << sampleNames[type] << "\n";
      std::string sampleName = sampleNames[type];
      
      std::string samplePath = basePath + "/" + sampleName;
      
      std::string efilename = samplePath + "_el.tex";
      std::string efilenameUp = samplePath + "_el_up.tex";
      std::string efilenameDown = samplePath + "_el_down.tex";
      std::string mufilename = samplePath + "_mu.tex";
      std::string mufilenameUp = samplePath + "_mu_up.tex";
      std::string mufilenameDown = samplePath + "_mu_down.tex";
      
      std::cerr << "efile "<<efilename<<" mufile "<<mufilename<<"\n";
      
      AsymBLUECombination blue(efilenameUp,efilenameDown,mufilenameUp,mufilenameDown,correlatedSystematics,BLUECombination::UNCORRELATED);
      const SystematicBinList& blueList = blue.Combine();
      
      /// now the normalized plots
      
      efilename = samplePath + "_el_norm.tex";
      efilenameUp = samplePath + "_el_norm_up.tex";
      efilenameDown = samplePath + "_el_norm_down.tex";
      mufilename = samplePath + "_mu_norm.tex";
      mufilenameUp = samplePath + "_mu_norm_up.tex";
      mufilenameDown = samplePath + "_mu_norm_down.tex";
      
      std::cerr << "normalized efile "<<efilename<<" mufile "<<mufilename<<"\n";
      
      AsymBLUECombination blueNorm(efilenameUp,efilenameDown,mufilenameUp,mufilenameDown,correlatedSystematics,BLUECombination::UNCORRELATED);
      const SystematicBinList& blueListNorm = blueNorm.Combine();
      
      output_latex_table(blueList,blueListNorm,sampleName,pValues,pValuesNorm);
      
      std::cout << "[combineChannels] Done for loop.\n";
   }
   
   // output pvalue tables
   std::string pvalue_label   = "tab:pvalues_combined";
   std::string pvalue_caption = "Probability that the $\\chi^{2}$ between the prediction and data is less than that which was calculated. The first column lists the kinematic variable for which the values are calculated, the second column lists the top pair decay channel for with the values are calculated, and the remaining columns list the calculated probablities for the data and the listed prediction. These values are for the absolute spectra.";
   PValueTable pvaluesTable(pValues,pvalue_label,pvalue_caption);
   std::string pvalue_filename = globals::GetLatexPath();
   pvalue_filename += "/pvaluesTableCombined.tex";
   pvaluesTable.WriteTableToFile(pvalue_filename);
   
   
   std::string pvalueNorm_label   = "tab:pvalues_combined_norm";
   std::string pvalueNorm_caption = "Probability that the $\\chi^{2}$ between the prediction and data is less than that which was calculated. The first column lists the kinematic variable for which the values are calculated, the second column lists the top pair decay channel for with the values are calculated, and the remaining columns list the calculated probablities for the data and the listed prediction. These values are for the normalized spectra.";
   PValueTable pvaluesNormTable(pValuesNorm,pvalueNorm_label,pvalueNorm_caption);
   std::string pvalueNorm_filename = globals::GetLatexPath();
   pvalueNorm_filename += "/pvaluesTableCombinedNorm.tex";
   pvaluesNormTable.WriteTableToFile(pvalueNorm_filename);
   
   
   
   // compile latex
   std::string makeCmd = "make -C ";
   makeCmd += globals::GetLatexPath();
   makeCmd += " -f Makefile_combined";
   
   system(makeCmd.c_str());
   makeCmd += "_lhood";
   system(makeCmd.c_str());
   
   std::cout << "[combineChannels] Done.\n";
}





void output_latex_table(const SystematicBinList& binList,
                        const SystematicBinList& binListNorm,
                        const std::string sampleName,
                        PValueList& pValues,
                        PValueList& pValuesNorm){
   
   // build bin boundaries
   std::vector<double> binBoundaries;
   for (unsigned int i=0; i<binList.size(); ++i){
      binBoundaries.push_back(binList[i].low());
   }
   binBoundaries.push_back(binList[binList.size()-1].high());
   
   // get right labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string UkinVarLabel = kinVar->GetUKinVarLabel();
   std::string DkinVarLabel = kinVar->GetDKinVarLabel();
   std::string LatexKinVarLabel = kinVar->GetLatexKinVarLabel();
   std::string LatexUkinVarLabel = kinVar->GetLatexUKinVarLabel();
   std::string LatexNkinVarLabel = kinVar->GetLatexNKinVarLabel();
   std::string LatexDkinVarLabel = kinVar->GetLatexDKinVarLabel();
   std::string LatexTDkinVarLabel = kinVar->GetLatexTDKinVarLabel();
   std::string LatexDkinVarLabelNoUnits = kinVar->GetLatexDKinVarLabelNoUnits();
   std::string LatexTDkinVarLabelNoUnits = kinVar->GetLatexTNKinVarNoUnitsLabel();
   std::string UnitDkinVarLabel = kinVar->GetUnitDKinVarLabel();
   std::string UnitNKinVarLabel = kinVar->GetUnitNKinVarLabel();
   std::string LatexTNKinVarLabel = kinVar->GetLatexTNKinVarLabel();
   std::string KinVarUnitLabel = kinVar->GetUnitKinVarLabel();
   
   /// determine which generator we are using ALPGEN or MCATNLO
   GeneratorLabel* genLabel = new GeneratorLabel(globals::GetOutputPath());
   std::string generator = genLabel->GetLabel();
   
   // open an output file to save the results to disk for later manipulation
   std::string out_filename = globals::GetOutputPath() + "/unfoldedResult_" + sampleName + ".root";
   TFile* outFile = new TFile(out_filename.c_str(),"UPDATE");
   if(!outFile->IsOpen()){
      std::cerr << "[output_latex_table] ERROR opening file for output: " << out_filename << "\n";
   }
   // retrieve Covariance Matrix from file
   std::string covMatrixName = "covarianceMatrix_norm";
   TMatrixD* covMatrix     = 0;
   TMatrixD* covMatrixNorm = globals::GetFromFile<TMatrixD>("output_latex_table",outFile,covMatrixName);
   
   // creating histograms for absolute
   TH1F* nominalWithStat = new TH1F(std::string(sampleName+"lep_top_pt").c_str(), "", binBoundaries.size()-1,&binBoundaries.front());
   for (unsigned int bin=1; bin<=nominalWithStat->GetNbinsX(); bin++){
      nominalWithStat->SetBinContent(bin,binList[bin-1].content());
   }
   
   TH1F* nominalWithStatUp = new TH1F(std::string(sampleName+"lep_top_pt_up").c_str(), "",binBoundaries.size()-1,&binBoundaries.front());
   for (unsigned int bin=1; bin<=nominalWithStatUp->GetNbinsX(); bin++){
      nominalWithStatUp->SetBinContent(bin,binList[bin-1].content() + binList[bin-1].up());
   }
   
   TH1F* nominalWithStatDown = new TH1F(std::string(sampleName+"lep_top_pt_down").c_str(), "",binBoundaries.size()-1,&binBoundaries.front());
   for (unsigned int bin=1; bin<=nominalWithStatDown->GetNbinsX(); bin++){
      nominalWithStatDown->SetBinContent(bin,binList[bin-1].content() - binList[bin-1].down());
   }
   
   std::string name = nominalWithStat->GetName();
   name += "WithSystematics";
   TH1* h_nominalWithSystematics = (TH1*)nominalWithStat->Clone();
   TGraphAsymmErrors nominalWithSystematics(h_nominalWithSystematics);
   nominalWithSystematics.SetName(name.c_str());
   
   
   // creating histograms for normalized
   TH1F* nominalNormWithStat = new TH1F(std::string(sampleName+"lep_top_pt_norm").c_str(), "", binBoundaries.size()-1,&binBoundaries.front());
   for (unsigned int bin=1; bin<=nominalNormWithStat->GetNbinsX(); bin++){
      nominalNormWithStat->SetBinContent(bin,binListNorm[bin-1].content());
   }
   
   TH1F* nominalNormWithStatUp = new TH1F(std::string(sampleName+"lep_top_pt_norm_up").c_str(), "",binBoundaries.size()-1,&binBoundaries.front());
   for (unsigned int bin=1; bin<=nominalNormWithStatUp->GetNbinsX(); bin++){
      nominalNormWithStatUp->SetBinContent(bin,binListNorm[bin-1].content() + binListNorm[bin-1].up());
   }
   
   TH1F* nominalNormWithStatDown = new TH1F(std::string(sampleName+"lep_top_pt_norm_down").c_str(), "",binBoundaries.size()-1,&binBoundaries.front());
   for (unsigned int bin=1; bin<=nominalNormWithStatDown->GetNbinsX(); bin++){
      nominalNormWithStatDown->SetBinContent(bin,binListNorm[bin-1].content() - binListNorm[bin-1].down());
   }
   
   name = nominalNormWithStat->GetName();
   name += "_withSyst";
   TH1* h_nominalNormWithSystematics = (TH1*)nominalNormWithStat->Clone();
   TGraphAsymmErrors nominalNormWithSystematics(h_nominalNormWithSystematics);
   nominalNormWithSystematics.SetName(name.c_str());
   
   
   
   // setup pretty text for sample name
   std::string topSample,topSampleRoot;
   if(sampleName.find("Top1") != std::string::npos){
      topSample = "leptonic top \\ptt";
      topSampleRoot = "leptonic top";
   }
   else if(sampleName.find("Top2") != std::string::npos){
      topSample = "hadronic top \\ptt";
      topSampleRoot = "hadronic top";
   }
   else if(sampleName.find("SystemMass") != std::string::npos){
      topSample = "\\ttbar mass";
      topSampleRoot = "t#bar{t} mass";
   }
   else if(sampleName.find("SystemPt") != std::string::npos){
      topSample = "\\ttbar \\pt";
      topSampleRoot = "t#bar{t} p_{T}";
   }
   
   std::string outputPath = globals::GetLatexPath();
   std::string texFilename = outputPath + "/" + sampleName + ".tex";
   std::string texCaptionAbs = "Combined statistical and systematic absolute errors are shown in each bin for the " + topSample + " spectrum. Signal model: \\" + generator + ".";
   std::string texCaptionAbsNorm = "Combined statistical and systematic absolute errors are shown in each bin for the normalized " + topSample + " spectrum. Signal model: \\" + generator + ".";
   std::string texCaptionRel = "Combined statistical and systematic percent errors are shown in each bin for the " + topSample + " spectrum. Signal model: \\" + generator + ".";
   std::string texCaptionRelNorm = "Combined statistical and systematic percent errors are shown in each bin for the normalized " + topSample + " spectrum. Signal model: \\" + generator + "."; 
   
   

   //Save the information in latex file
   std::cout << "Writing tex file: " << texFilename << "\n";
   LatexDoc* latexDoc = LatexDoc::GetTopDiffInstance();
   
   
   //////////////////////////////////////////////////
   /// Table with absolute errors
   ////////////////////
   double stat2 = 0,intNom = 0,intStat = 0,downUnc = 0,upUnc = 0;
   {
      TexTable table("ht!","r@{  to }rccc");
      table.hline();
      std::stringstream header;
      header<< "\\multicolumn{2}{l}{$" << LatexKinVarLabel << "$ " << KinVarUnitLabel << "} & "<< LatexTDkinVarLabel << " & stat. "<< UnitDkinVarLabel <<" & syst. "<< UnitDkinVarLabel;
      table.AddRow(header.str());
      table.hline();
      
      for (unsigned int bin = 1; bin<=binList.size(); bin++) {
         
         /// Filling nominal graphs
         double binContents = binList[bin-1].content();
         double binLowEdge = binList[bin-1].low();
         double binHighEdge = binList[bin-1].high();
         
         double binSystUp = binList[bin-1].up();
         double binSystDown = binList[bin-1].down();
         double binStat = binList[bin-1].stat();
         
         nominalWithSystematics.SetPointEYhigh(bin-1,(binStat+binSystUp));
         nominalWithSystematics.SetPointEYlow(bin-1,(binStat+binSystDown));
         h_nominalWithSystematics->SetBinError(bin,(binStat+binSystUp + binStat+binSystDown)/2.);
         nominalWithStat->SetBinError(bin,binStat);
         
         double binWidth = binList[bin-1].high() - binList[bin-1].low();
         stat2 += binStat*binStat*binWidth*binWidth;
         
         /// Filling table
         // no decimals on the bin numbers, but do add them to other numbers
         std::stringstream row;
         // bin edges
         row.setf(std::ios::fixed);
         row.precision(0);
         row.width(4);
         row << binLowEdge << " & ";
         row.precision(0);
         row.width(4);
         row << binHighEdge << " & ";
         // bin contents
         row.precision(4);
         row.width(10);
         row << binContents;
         // statistics
         row << " & $\\pm";
         row.precision(4);
         row.width(6);
         row << binStat << "$";
         // systematics down
         row << " & $-";
         row.precision(4);
         row.width(6);
         row << binSystDown << "$/$+";
         // systematics up
         row.precision(4);
         row.width(6);
         row << binSystUp << "$";
         
         table.AddRow(row.str());
      }
      table.hline();
      
      /// calculate total XS
      double intUp = nominalWithStatUp->Integral("width");
      double intDown = nominalWithStatDown->Integral("width");
      intNom = nominalWithStat->Integral("width");
    
      upUnc = (intUp - intNom);
      downUnc = (intNom - intDown);
   
      intStat = sqrt(stat2);
      
      std::stringstream totalXS;
      totalXS.precision(3);
      totalXS << "\\multicolumn{2}{l}{$\\sigma^{inc}_{t\\bar{t}}$ [pb]} & " << intNom;
      totalXS.precision(5);
      totalXS << " & $\\pm$" << intStat;
      totalXS.precision(4);
      totalXS << " & $-" << downUnc << "$/$+" << upUnc << "$";
      
      table.AddRow(totalXS.str());
      table.hline();
      std::string label = "table:topPt_";
      label += sampleName;
      //latexDoc->AddBodyText(table.GetTableText(texCaptionAbs,label));
       
   }
   
   outFile->cd();
   nominalWithSystematics.Write("diffxs_syst_stat",TObject::kOverwrite);
   h_nominalWithSystematics->Write("diffxs_syst_stat_symmetrical",TObject::kOverwrite);
   nominalWithStat->Write("diffxs_stat",TObject::kOverwrite);
   
   
   
   
   
   /////////////////////////////////
   /// Table with Relative Errors
   ////////////////////
   {
      TexTable table("ht!","r@{  to }rccc");
      table.hline();
      std::stringstream header;
      header<< "\\multicolumn{2}{l}{$" << LatexKinVarLabel << "$ " << KinVarUnitLabel << "} & "<< LatexTDkinVarLabel << " & stat.[\\%] & syst.[\\%]";
      table.AddRow(header.str());
      table.hline();
      
      for (unsigned int bin = 1; bin<=binList.size(); bin++) {
         
         double binContents = binList[bin-1].content();
         
         double binSystUp = binList[bin-1].up();
         double binSystDown = binList[bin-1].down();
         double binStat = binList[bin-1].stat();
         double binLowEdge = binList[bin-1].low();
         double binHighEdge = binList[bin-1].high();
         
         // no decimals on the bin numbers, but do add them to other numbers
         std::stringstream row;
         row.setf(std::ios::fixed);
         row.precision(0);
         row.width(4);
         row << binLowEdge << " & ";
         row.precision(0);
         row.width(4);
         row << binHighEdge << " & ";
         row.precision(5);
         row.precision(4);
         row.width(10);
         row << binContents << " & $\\pm";
         row.precision(1);
         row.width(4);
         row << (binStat/binContents*100.) << "$ & $-";
         row.precision(1);
         row.width(4);
         row << (binSystDown/binContents*100.) << "$/$+";
         row.precision(1);
         row.width(4);
         row << (binSystUp/binContents*100.) << "$";
         
         table.AddRow(row.str());
      }
      
      table.hline();
      std::stringstream totalXS;
      totalXS.setf(std::ios::fixed);
      totalXS << "\\multicolumn{2}{c}{$\\sigma^{inc}_{t\\bar{t}}$ [pb]} & ";
      totalXS.precision(2);
      totalXS.width(7);
      totalXS<< intNom << " & $\\pm";
      totalXS.precision(1);
      totalXS.width(4);
      totalXS << (intStat/intNom*100.) << "$ & $-";
      totalXS.precision(1);
      totalXS.width(4);
      totalXS << (downUnc/intNom*100.) << "$/$+";
      totalXS.precision(1);
      totalXS.width(4);
      totalXS << (upUnc/intNom*100.) << "$";
      
      table.AddRow(totalXS.str());
      table.hline();
      
      std::string label = "table:topPt_";
      label += sampleName + "_withRelativeErrors";
      latexDoc->AddBodyText(table.GetTableText(texCaptionRel,label));
   }
   
   ///////////////////////////////////////////////////////
   /// Table with relative contribution of each uncertainty to the combined uncertainty
   ///////////////////////////
   {
      std::string columnStructure = "l";
      std::stringstream header;
      header.setf(std::ios::fixed);
      header.width(25);
      header << "systematic[\\%]";
      for(unsigned int i=0;i<binList.size();++i){
         columnStructure += " r";
         const SystematicBin& bin = binList[i];
         header << " & ";
         header.precision(0);
         header.width(4);
         header << bin.low() << " to ";
         header.precision(0);
         header.width(4);
         header << bin.high();
      }
      TexTable table("!ht",columnStructure);
      table.hline();
      table.AddRow(header.str());
      table.hline();
      table.hline();
      
      // build the rows all at the same time
      std::map<std::string,std::string> rows;
      for(unsigned int i = 0;i<binList.size();++i){
         const SystematicBin& bin = binList[i];
         std::map<std::string,double> percentContribution = bin.percentContribution();
         std::map<std::string,double>::const_iterator itr = percentContribution.begin();
         double jes_sum = 0.;
         for(;itr != percentContribution.end();++itr){
            const std::string& systematic = itr->first;
            const std::string systPretty = SystematicSample::GetSamplePrettyBaseString(systematic);
            const double& fraction = itr->second;
            
            if(MasterSystList::IsJesComponentSyst(systematic) ||
               MasterSystList::IsJesComponentBackgroundSyst(systematic))
            {
               jes_sum += fraction;
            }
            else{
               std::stringstream row;
               row.setf(std::ios::fixed);
               if(rows.find(systematic) == rows.end()){
                  row.width(25);
                  row << systPretty;
                  rows[systematic] = "";
               }
               row << " & ";
               row.width(7);
               row.precision(1);
               row << (fraction*100.);
               rows[systematic] += row.str();
            }
         }
         
         // fill JES Bin
         std::stringstream row;
         row.setf(std::ios::fixed);
         if(rows.find("jes") == rows.end()){
            row.width(25);
            row << "Jet Energy Scale";
            rows["jes"] = "";
         }
         row << " & ";
         row.width(7);
         row.precision(1);
         row << (jes_sum*100.);
         rows["jes"] += row.str();
      }
      
      
      std::map<std::string,std::string>::const_iterator itr = rows.begin();
      for(;itr != rows.end();++itr){
         table.AddRow(itr->second);
         table.hline();
      }
      table.hline();
      std::stringstream caption,label;
      caption << "Percent contribution of each uncertainty to the total combined uncertainty  calculated using the BLUE method with symmetrized uncertainties for $" << LatexKinVarLabel << "$. These symmetrized uncertainties are calculated by averaging the asymmetric up and down components of each uncertainty. This is an approximation, however, as the asymmetry in most uncertainties is not extreme, it is not an unreasonable approximation. This is for the absolute spectrum.";
      label << "tab:relativeUncertaintiesInCombination_" << sampleName;
      latexDoc->AddBodyText(table.GetTableText(caption.str(),label.str()));
   }
   
   //////////////////////
   /// Plots of XS
   //////////
   
   plot_XS(nominalWithSystematics,h_nominalWithSystematics,nominalWithStat,sampleName,outputPath,topSampleRoot,topSample,latexDoc,sampleName,pValues,false,true,covMatrix);
   
   
   /////////////////////////////////////////////////
   /// Normalized Table with absolute errors
   ////////////////////
   const double norm_shift = 1000.;
   {
      TexTable table("ht!","r@{  to }rccc");
      table.hline();
      std::stringstream header;
      header<< "\\multicolumn{2}{l}{$" << LatexKinVarLabel << "$ " << KinVarUnitLabel << "} & "<< LatexTDkinVarLabel << " & stat. "<< UnitDkinVarLabel <<" & syst. "<< UnitDkinVarLabel;
      table.AddRow(header.str());
      table.hline();
      
      for (unsigned int bin = 1; bin<=binList.size(); bin++) {
         
         double binSystUp=0;
         double binSystDown=0;
         double binStat=0;
         
         double binContents = binListNorm[bin-1].content();
         double binLowEdge = binListNorm[bin-1].low();
         double binHighEdge = binListNorm[bin-1].high();
      
         binSystUp = binListNorm[bin-1].up();
         binSystDown = binListNorm[bin-1].down();
         binStat = binListNorm[bin-1].stat();
      
         nominalNormWithSystematics.SetPointEYhigh(bin-1,(binStat+binSystUp));
         nominalNormWithSystematics.SetPointEYlow(bin-1,(binStat+binSystDown));
         h_nominalNormWithSystematics->SetBinError(bin,(binStat+binSystUp + binStat+binSystDown)/2.);
         nominalNormWithStat->SetBinError(bin,binStat);
         
         /// Filling table
         // no decimals on the bin numbers, but do add them to other numbers
         std::stringstream row;
         // bin edges
         row.setf(std::ios::fixed);
         row.precision(0);
         row.width(4);
         row << binLowEdge << " & ";
         row.precision(0);
         row.width(4);
         row << binHighEdge << " & ";
         // bin contents
         row.precision(4);
         row.width(10);
         row << binContents*norm_shift;
         // statistics
         row << " & $\\pm";
         row.precision(4);
         row.width(6);
         row << binStat*norm_shift << "$";
         // systematics down
         row << " & $-";
         row.precision(4);
         row.width(6);
         row << binSystDown*norm_shift << "$/$+";
         // systematics up
         row.precision(4);
         row.width(6);
         row << binSystUp*norm_shift << "$";
         
         table.AddRow(row.str());
         
      }
      table.hline();
      std::string label = "table:topPt_";
      label += sampleName + "_norm";
      //latexDoc->AddBodyText(table.GetTableText(texCaptionAbsNorm,label));
      
   }
   
   outFile->cd();
   nominalNormWithSystematics.Write("diffxs_norm_syst_stat",TObject::kOverwrite);
   h_nominalNormWithSystematics->Write("diffxs_norm_syst_stat_symmetrical",TObject::kOverwrite);
   nominalNormWithStat->Write("diffxs_norm_stat",TObject::kOverwrite);
   
   
   ////////////////////
   /// Normalized Table with Relative Errors
   ////////////////////
   {
      TexTable table("ht!","r@{  to }rccc");
      table.hline();
      std::stringstream header;
      header<< "\\multicolumn{2}{l}{$" << LatexKinVarLabel << "$ " << KinVarUnitLabel << "} & "<< LatexTNKinVarLabel << " & stat. [\\%] & syst. [\\%]";
      table.AddRow(header.str());
      table.hline();
      
      for (unsigned int bin = 1; bin<=binList.size(); bin++) {
         
         double binSystUp=0;
         double binSystDown=0;
         double binStat=0;
         
         double binContents = binListNorm[bin-1].content();
         double binLowEdge = binListNorm[bin-1].low();
         double binHighEdge = binListNorm[bin-1].high();
         
         binSystUp = binListNorm[bin-1].up();
         binSystDown = binListNorm[bin-1].down();
         binStat = binListNorm[bin-1].stat();
         
         
         /// Filling table
         // no decimals on the bin numbers, but do add them to other numbers
         std::stringstream row;
         // bin edges
         row.setf(std::ios::fixed);
         row.precision(0);
         row.width(4);
         row << binLowEdge << " & ";
         row.precision(0);
         row.width(4);
         row << binHighEdge << " & ";
         // bin contents
         row.precision(4);
         row.width(10);
         row << binContents*norm_shift;
         // statistics
         row << " & $\\pm";
         row.precision(1);
         row.width(4);
         row << (binStat/binContents*100.) << "$";
         // systematics down
         row << " & $-";
         row.precision(1);
         row.width(4);
         row << (binSystDown/binContents*100.) << "$/$+";
         // systematics up
         row.precision(1);
         row.width(4);
         row << (binSystUp/binContents*100.) << "$";
         
         table.AddRow(row.str());
      }
      
      table.hline();
      std::string label = "table:topPt_";
      label += sampleName + "_norm_withRelativeErrors";
      latexDoc->AddBodyText(table.GetTableText(texCaptionRelNorm,label));
      
   }
   
   ///////////////////////////////////////////////////////
   /// Table with relative contribution of each uncertainty to the combined uncertainty
   ///////////////////////////
   {
      std::string columnStructure = "l";
      std::stringstream header;
      header.setf(std::ios::fixed);
      header.width(25);
      header << "systematic[\\%]";
      for(unsigned int i=0;i<binListNorm.size();++i){
         columnStructure += " r";
         const SystematicBin& bin = binListNorm[i];
         header << " & ";
         header.precision(0);
         header.width(4);
         header << bin.low() << " to ";
         header.precision(0);
         header.width(4);
         header << bin.high();
      }
      TexTable table("!ht",columnStructure);
      table.hline();
      table.AddRow(header.str());
      table.hline();
      table.hline();
      
      // build the rows all at the same time
      std::map<std::string,std::string> rows;
      for(unsigned int i = 0;i<binListNorm.size();++i){
         const SystematicBin& bin = binListNorm[i];
         std::map<std::string,double> percentContribution = bin.percentContribution();
         std::map<std::string,double>::const_iterator itr = percentContribution.begin();
         double jes_sum = 0.;
         for(;itr != percentContribution.end();++itr){
            const std::string& systematic = itr->first;
            const std::string systPretty = SystematicSample::GetSamplePrettyBaseString(systematic);
            const double& fraction = itr->second;
            
            if(MasterSystList::IsJesComponentSyst(systematic) ||
               MasterSystList::IsJesComponentBackgroundSyst(systematic))
            {
               jes_sum += fraction;
            }
            else{
               std::stringstream row;
               row.setf(std::ios::fixed);
               if(rows.find(systematic) == rows.end()){
                  row.width(25);
                  row << systPretty;
                  rows[systematic] = "";
               }
               row << " & ";
               row.width(7);
               row.precision(1);
               row << (fraction*100.);
               rows[systematic] += row.str();
            }
         }
         
         // fill JES Bin
         std::stringstream row;
         row.setf(std::ios::fixed);
         if(rows.find("jes") == rows.end()){
            row.width(25);
            row << "Jet Energy Scale";
            rows["jes"] = "";
         }
         row << " & ";
         row.width(7);
         row.precision(1);
         row << (jes_sum*100.);
         rows["jes"] += row.str();
      }
      
      
      std::map<std::string,std::string>::const_iterator itr = rows.begin();
      for(;itr != rows.end();++itr){
         table.AddRow(itr->second);
         table.hline();
      }
      table.hline();
      std::stringstream caption,label;
      caption << "Percent contribution of each uncertainty to the total combined uncertainty  calculated using the BLUE method with symmetrized uncertainties for $" << LatexKinVarLabel << "$. These symmetrized uncertainties are calculated by averaging the asymmetric up and down components of each uncertainty. This is an approximation, however, as the asymmetry in most uncertainties is not extreme, it is not an unreasonable approximation. This is for the normalized spectrum.";
      label << "tab:relativeUncertaintiesInCombination_" << sampleName << "_norm";
      latexDoc->AddBodyText(table.GetTableText(caption.str(),label.str()));
   }
   
   
   /// plot normalized specrta
   plot_XS(nominalNormWithSystematics,h_nominalNormWithSystematics,nominalNormWithStat,sampleName,outputPath,topSampleRoot,topSample,latexDoc,sampleName,pValuesNorm,true,true,covMatrixNorm);
   
   
   
   // plot combined cross sections from e/mu/combined/theory
   double sysUp = upUnc;
   double sysDown = downUnc;
   double value = intNom; 
   double statUnc = sqrt(stat2);
   
   combinePlot(value,statUnc,sysUp,sysDown,outputPath,sampleName,topSample,latexDoc,sampleName,generator);
   
   
   latexDoc->WriteToFile(texFilename);
   globals::set_chmod(texFilename);
   
   
   
   delete h_nominalWithSystematics;
   h_nominalWithSystematics = 0;
   
   delete nominalWithStat;
   nominalWithStat = 0;
   
   delete h_nominalNormWithSystematics;
   h_nominalNormWithSystematics = 0;
   
   delete nominalNormWithStat;
   nominalNormWithStat = 0;
   
   outFile->Close();
   
   std::cout << "[output_latex_table] Done.\n";
}

//function to plot the differential and normalized spectrum with and without log scale 
void plot_XS(TGraphAsymmErrors nominalSyst,TH1* h_nominalSyst,
             TH1* nominalStat,std::string filename,
             std::string outputPath,std::string legendLabel,
             std::string topSample,LatexDoc* const latexDoc,
             std::string sampleName,
             PValueList& pValues,
             const bool norm,const bool log,
             TMatrixD const * const Cov)
{
   
   if(norm) filename += "_norm";
   
   std::string pdfFilename       = globals::GetLatexPdfPath() + "/" + filename + ".pdf";
   std::string epsFilename       = globals::GetLatexEpsPath() + "/" + filename + ".eps";
   std::string pdfFilename_log   = globals::GetLatexPdfPath() + "/" + filename + "_log.pdf";
   std::string epsFilename_log   = globals::GetLatexEpsPath() + "/" + filename + "_log.eps";
   std::string labelname = "figure:XS_"+filename;
   
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
   
   // get generator
   GeneratorLabel genLabel(globals::outputFilesPath);
   std::string generator = genLabel.GetLabel();
   
   
   // Get MC@NLO
   McAtNloBeforeCuts MCatNLO(sampleName);
   if(genLabel.GetGenerator() == GeneratorLabel::MCATNLO){
      MCatNLO.SetMainGenerator(true);
   }
   
   TH1* mcAtNlo= MCatNLO.GetHisto();
   if(norm) mcAtNlo = MCatNLO.GetHistoNorm();
   
   
   // Get ALPGEN
   AlpgenMC alpgen(sampleName);
   if(genLabel.GetGenerator() == GeneratorLabel::ALPGEN){
      alpgen.SetMainGenerator(true);
   }
   
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
   Th1PlotAndRatio::RATIO_ORDER ratioOrder = Th1PlotAndRatio::DATA_AS_DENOMINATOR;
   if(globals::DATA_ON_TOP)
      ratioOrder = Th1PlotAndRatio::DATA_AS_NUMERATOR;
   Th1PlotAndRatio plotAndRatio(legendLabel,ratioOrder);
   if(globals::DATA_ON_TOP)
      plotAndRatio.SetCombineGeneratorRatios(false);
   else
      plotAndRatio.SetCombineGeneratorRatios(true);
   
   if(sampleName.find("SystemRapidity") != std::string::npos){
      plotAndRatio.SetAtlasLabelPosition(0.25,0.85);
      plotAndRatio.SetAtlasLumiPosition(0.25,0.75);
      if(norm){
         plotAndRatio.SetYAxisMax(1.);
         plotAndRatio.SetYAxisMin(0.);
      }
      else{
         plotAndRatio.SetYAxisMax(150);
      }
   }
   
   
   // set axis labels
   if(norm)
      plotAndRatio.SetYAxisTitleTop(latexNkinVarLabel);
   else
      plotAndRatio.SetYAxisTitleTop(DkinVarLabel);
   
   plotAndRatio.SetXAxisTitle(UkinVarLabel);
   plotAndRatio.SetYAxisTitleRatio("Data/MC");
   if(!globals::DATA_ON_TOP)
      plotAndRatio.SetYAxisTitleRatio("MC/Data");
   
   
   // set color/style for main data
   nominalSyst.SetMarkerSize(1.);
   nominalSyst.SetMarkerColor(kBlack);
   nominalSyst.SetLineColor(kBlack);
   nominalSyst.SetMarkerStyle(20);
   
   //needed to enlarge the error bars 
   gStyle->SetEndErrorSize(3);
   
   // add nominal data as reference plot
   std::stringstream label;
   label << "Data";
   if(!norm) label << " #int=" << GetIntegral(&nominalSyst) << "pb";
   plotAndRatio.SetData(&nominalSyst,label.str().c_str());
   
   // add remaining plots
   TopDecayChannel channel(TopDecayChannel::COMBINED);
   if(hAlpgen){
      label.str("");
      label << "Alpgen"; 
      if(!norm) label << " #int=" << hAlpgen->Integral("width") << "pb";
      std::string ratio_y_axis_title = "Alpgen/Data";
      if(!globals::DATA_ON_TOP)
         ratio_y_axis_title = "Data/Alpgen";
      plotAndRatio.AddGenerator(hAlpgen,label.str().c_str(),ratio_y_axis_title,true,"l");
      
      if(Cov)
         pValues.CalculateAndFillPValueList(sampleName,channel,"Alpgen",&nominalSyst,hAlpgen,norm,Cov);
      else
         pValues.CalculateAndFillPValueList(sampleName,channel,"Alpgen",&nominalSyst,hAlpgen,norm);
   }
   if(mcAtNlo){
      label.str("");
      label << "MC@NLO";
      if(!norm) label << " #int=" << mcAtNlo->Integral("width") << "pb";
      std::string ratio_y_axis_title = "MC@NLO/Data";
      if(!globals::DATA_ON_TOP)
         ratio_y_axis_title = "Data/MC@NLO";
      plotAndRatio.AddGenerator(mcAtNlo,label.str().c_str(),ratio_y_axis_title,true,"l");
      
      if(Cov)
         pValues.CalculateAndFillPValueList(sampleName,channel,"McAtNlo",&nominalSyst,mcAtNlo,norm,Cov);
      else
         pValues.CalculateAndFillPValueList(sampleName,channel,"McAtNlo",&nominalSyst,mcAtNlo,norm);
   }
   if(hPowheg){
      label.str("");
      label.precision(4);
      label << "PowHeg"; 
      if(!norm) label << " #int=" << hPowheg->Integral("width") << "pb";
      std::string ratio_y_axis_title = "PowHeg/Data";
      if(!globals::DATA_ON_TOP)
         ratio_y_axis_title = "Data/PowHeg";
      plotAndRatio.AddGenerator(hPowheg,label.str().c_str(),ratio_y_axis_title,true,"l");
      
      if(Cov)
         pValues.CalculateAndFillPValueList(sampleName,channel,"PowHeg",&nominalSyst,hPowheg,norm,Cov);
      else
         pValues.CalculateAndFillPValueList(sampleName,channel,"PowHeg",&nominalSyst,hPowheg,norm);
   }
   if(NNLO && hNNLO){
      label.str("");
      label.precision(4);
      label << "Approx. NNLO";
      if(!norm) label << " #int=" << hNNLO->Integral("width") << "pb";
      std::string ratio_y_axis_title = "NNLO/Data";
      if(!globals::DATA_ON_TOP)
         ratio_y_axis_title = "Data/NNLO";
      plotAndRatio.AddTheory(NNLO,label.str().c_str(),ratio_y_axis_title,hNNLO,true,"f");
      
      if(Cov)
         pValues.CalculateAndFillPValueList(sampleName,channel,"NNLO",&nominalSyst,NNLO,norm,Cov);
      else
         pValues.CalculateAndFillPValueList(sampleName,channel,"NNLO",&nominalSyst,NNLO,norm);
   }
   if(gMcfm && hMcfm){
      label.str("");
      label.precision(4);
      label << "MCFM (NLO)";
      if(!norm) label << " #int=" << hMcfm->Integral("width") << "pb";
      std::string ratio_y_axis_title = "MCFM/Data";
      if(!globals::DATA_ON_TOP)
         ratio_y_axis_title = "Data/MCFM";
      plotAndRatio.AddTheory(gMcfm,label.str(),ratio_y_axis_title,hMcfm,true,"f");
      
      if(Cov)
         pValues.CalculateAndFillPValueList(sampleName,channel,"MCFM",&nominalSyst,gMcfm,norm,Cov);
      else
         pValues.CalculateAndFillPValueList(sampleName,channel,"MCFM",&nominalSyst,gMcfm,norm);
   }
   // save plots to files
   
   plotAndRatio.SaveAs(pdfFilename);
   plotAndRatio.SaveAs(epsFilename);
   
   std::string texCaption = topSample + ". The error bars on data points denote the total combined statistical and systematic uncertainty while the bands in the ratio plots denote theory scale variations. \\" + generator + " was used to unfold the data.";
   std::string texCaptionLog = topSample + " in log scale. The error bars on data points denote the total combined statistical and systematic uncertainty while the bands in the ratio plots denote theory scale variations. \\" + generator + " was used to unfold the data.";
   
   std::stringstream mylatexF;
   mylatexF<<"\\begin{figure}[th!] \n";
   mylatexF<<"\\centering \n";
   mylatexF<<"\\includegraphics[width=0.8\\textwidth]{" << filename << "} \n";
   if(norm)
      mylatexF<<"\\caption{Normalized top differential cross-section versus "<< texCaption << "} \n";
   else
      mylatexF<<"\\caption{Top differential cross-section versus "<< texCaption << "} \n";
   mylatexF<<"\\label{"<<labelname<<"} \n";
   mylatexF<<"\\end{figure} \n";
   mylatexF<<"\n";
   
   // if log plots are requested, add them too
   
   // save plots to files
   if(log){
      if(sampleName.find("SystemRapidity") != std::string::npos){
         plotAndRatio.SetAtlasLabelPosition(0.25,0.85);
         plotAndRatio.SetAtlasLumiPosition(0.25,0.75);
         if(norm){
            plotAndRatio.SetYAxisMax(4);
            plotAndRatio.SetYAxisMin(0.05);
         }
         else{
            plotAndRatio.SetYAxisMax(500);
            plotAndRatio.SetYAxisMin(9);
         }
      }
      plotAndRatio.SetMainPadLogy(true);
      plotAndRatio.SaveAs(pdfFilename_log);
      plotAndRatio.SaveAs(epsFilename_log);
      
      mylatexF<<"\\begin{figure} \n";
      mylatexF<<"\\centering \n";
      mylatexF<<"\\includegraphics[width=0.8\\textwidth]{" << filename << "_log} \n";
      if(norm)
         mylatexF<<"\\caption{Normalized top differential cross-section versus "<< texCaptionLog << "} \n";
      else
         mylatexF<<"\\caption{Top differential cross-section versus " << texCaptionLog << "} \n";
      mylatexF<<"\\label{"<<labelname<<"_log} \n";
      mylatexF<<"\\end{figure} \n";
      mylatexF<<"\n";
   }
   latexDoc->AddBodyText(mylatexF.str());
   
   if(NNLO){
      delete NNLO;
      NNLO = 0;
   }
   
   delete kinVar;
   kinVar = 0;
   
}



void combinePlot(double value, double statUnc, double sysUp, double sysDown, 
                 std::string outputPath,std::string filename,
                 std::string topSample,LatexDoc* const latexDoc,
                 std::string sampleName,std::string generator){
   
   std::string texCaption = "Summary of all integrated top pair production cross-sections calculated from the " + topSample + " spectrum. The theory corresponds to the MCFM (NLO) prediction. Signal model: \\" + generator + ".";
   
   std::string pdfFilename   = globals::GetLatexPdfPath() + "/Combine_" + filename + ".pdf";
   std::string epsFilename   = globals::GetLatexEpsPath() + "/Combine_" + filename + ".eps";
   std::string labelname = "figure:combine_"+filename;
   
   //define labels
   KinematicVariable* kinVar = new KinematicVariable(sampleName);
   std::string kinVarLabel  = kinVar->GetKinVarLabel();
   std::string latexkinVarLabel  = kinVar->GetLatexKinVarLabel();
   std::string latexNkinVarLabel  = kinVar->GetNKinVarLabel();
   std::stringstream ntitle;
   
   size_t vsize = 4;
   std::vector<double> values(vsize,0.);
   std::vector<double> bins(vsize,0.);
   std::vector<double> up(vsize,0.);
   std::vector<double> down(vsize,0.);
   std::vector<double> fake(vsize,0.);
   
   for (unsigned int i = 0; i<vsize; ++i){
      bins[i] = 4-i;
      fake[i] = 0.0001;
   }
   
   values[0]=value;
   up[0]=statUnc+sysUp;
   down[0]=statUnc+sysDown;
   
   std::string mufilename = globals::GetCombinationTextFilePath() + "/InclusiveXS_" + filename + "_mu.tex";
   std::ifstream mufile(mufilename.c_str());
   if(mufile.fail()){
      std::cerr << "[combinePlot] ERROR opening " << mufilename << "\n";
      return;
   }
   while( !mufile.eof() ) {
      double mean, estat, edown, eup;
      mufile>>mean>>estat>>edown>>eup;
      values[1]=mean;
      up[1]=eup+estat;
      down[1]=edown+estat;
   }  
   mufile.close();
   
   std::string efilename = globals::GetCombinationTextFilePath() + "/InclusiveXS_" + filename + "_el.tex";
   std::ifstream efile(efilename.c_str());
   if(efile.fail()){
      std::cerr << "[combinePlot] ERROR opening " << efilename << "\n";
      return;
   }
   while( !efile.eof() ) {
      double mean, estat, edown, eup;
      efile>>mean>>estat>>edown>>eup;
      values[2]=mean;
      up[2]=eup+estat;
      down[2]=edown+estat;
   }  
   efile.close(); 
   
   std::string tfilename = globals::GetCombinationTextFilePath() + "/InclusiveXS_";
   if(sampleName.find("Top") != std::string::npos){
      tfilename += "MCFMTag1_Top1_lhood.tex";
   }
   else{
      tfilename += "MCFMTag1_SystemMass.tex";
   }
   
   std::ifstream tfile(tfilename.c_str());
   if(tfile.fail()){
      std::cerr << "[combinePlot] ERROR opening " << tfilename << "\n";
      return;
   }
   while( !tfile.eof() ) {
      double mean, tdown, tup;
      tfile>>mean>>tdown>>tup;
      values[3]=mean;
      up[3]=tup;
      down[3]=tdown;
   }  
   tfile.close(); 
   
   TCanvas c("xs log","diffxsPlot log",0,0,800,600);
   c.cd();
   
   TH1F *h1 = new TH1F("h1", "",300, 50,350);
   h1->SetDirectory(false);
   h1->GetYaxis()->SetDecimals(kTRUE);
   h1->SetMinimum(0);
   h1->SetMaximum(8);
   h1->SetStats(kFALSE);
   h1->SetTitle("");
   h1->GetXaxis()->SetTitle("#sigma^{inc}_{t #bar{t}} [pb]");
   h1->GetYaxis()->SetTitle("");  
   h1->GetYaxis()->SetLabelOffset(3);
   h1->GetXaxis()->SetTickLength(0.05);
   h1->Draw("AXIS");
   
   TBox unc;
   unc.SetFillStyle(1001);
   unc.SetFillColor(kGray);
   unc.DrawBox(values[3]-down[3], 0.05, values[3]+up[3], 5.);
   
   TLine l;
   l.SetLineColor(kBlack);
   l.SetLineStyle(kDashed);
   l.SetLineWidth(2);
   l.DrawLine(values[3],0.05,values[3],5.);
   
   TGraphAsymmErrors* gXS = new TGraphAsymmErrors(vsize-1,&values[0],&bins[0],&down[0],&up[0],&fake[0],&fake[0]);
   gXS->SetMarkerSize(1.);
   gXS->SetMarkerColor(kBlue);
   gXS->SetLineColor(kBlue);
   gXS->SetLineWidth(2);
   gXS->SetMarkerStyle(20);
   gXS->Draw("P");
   
   TLatex l_atlas1;;
   l_atlas1.SetNDC();
   l_atlas1.SetTextFont(72);
   l_atlas1.SetTextColor(1);
   l_atlas1.DrawLatex(0.19,0.86,"ATLAS");

   TLatex l_other1;
   l_other1.SetNDC();
   l_other1.SetTextColor(1);
   l_other1.DrawLatex(0.31,0.86,"internal");
   
   TLatex lumi;
   lumi.SetNDC();
   lumi.SetTextColor(1);
   lumi.DrawLatex(0.48,0.86,"#int Ldt = 4.7 fb^{-1}");  
    
   TLatex l_energy;
   l_energy.SetNDC();
   l_energy.SetTextColor(1);
   l_energy.DrawLatex(0.75,0.86,"#sqrt{s} = 7 TeV"); 
    
   TLatex l_names;
   l_names.SetNDC();
   l_names.SetTextColor(1);
   l_names.DrawLatex(0.2,0.54,"Combined");
   l_names.DrawLatex(0.2,0.44,"#mu+jets");
   l_names.DrawLatex(0.2,0.34,"e+jets"); 
   l_names.DrawLatex(0.2,0.24,"Theory"); 
   
   TLatex l_values;
   l_values.SetNDC();
   l_values.SetTextColor(1);
   
   std::vector<TString> numbers(vsize);
   for (unsigned int i = 0; i<vsize; ++i){
      numbers[i] = TString(Form("%4.1f",values[i])) + " ^{+" + TString(Form("%4.1f",up[i])) + "}_{-" + TString(Form("%4.1f",down[i])) +"} pb";
      l_values.DrawLatex(0.68,0.54-0.1*i,numbers[i]);
   }
       
   TH1F *hTheory =new TH1F( "hTheory", "Theory", 1, 0, 1 );
   hTheory->SetDirectory(false);
   hTheory->SetFillColor(kGray);
   hTheory->SetLineWidth(2);
   hTheory->SetLineStyle(kDashed);
       
   TLegend leg(0.30,0.81,0.9,0.68,"");
   leg.SetFillStyle(0);
   leg.SetBorderSize(0);
   //leg.AddEntry(hTheory,"Theory (NNLO) for m_{t}=172.5 GeV","lf");
   leg.AddEntry(hTheory,"MCFM (NLO)","lf");
   leg.AddEntry(gXS,"Total Uncertainty","lpe");   
   leg.Draw("same");   
   
   c.SaveAs(pdfFilename.c_str());
   c.SaveAs(epsFilename.c_str());
   
   std::stringstream mylatexF;
   mylatexF<<"\\begin{figure} \n";
   mylatexF<<"\\centering \n";
   mylatexF<<"\\includegraphics[width=0.8\\textwidth]{""Combine_"<< filename << ".eps} \n";
   mylatexF<<"\\caption{"<<texCaption<<"} \n";
   mylatexF<<"\\label{"<<labelname<<"} \n";
   mylatexF<<"\\end{figure} \n";
   mylatexF<<"\\clearpage    \n";
   latexDoc->AddBodyText(mylatexF.str());
   
   delete hTheory;
   hTheory = 0;
   
//    delete numbers;
//    numbers = 0;
   
   delete gXS;
   gXS = 0;
   
   delete h1;
   h1 = 0;
   
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


