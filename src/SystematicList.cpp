
#include "SystematicList.h"
#include "SystematicBinList.h"
#include "SystematicBin.h"
#include "MasterSystList.h"
#include "TGraphAsymmErrors.h"
#include "TFile.h"
#include "globals.h"
#include "TGraphAsymmErrors.h"
#include "TH1F.h"
#include "BLUECombination.h"

SystematicList::~SystematicList(){
   Delete();
}

SystematicBinList* SystematicList::GetCombinedBins(){
   
   if(begin() == end()){
      std::cerr << "[SystematicList::GetCombinedBins]: WARNING: returning empty bin list.\n";
      return (SystematicBinList*)0;
   }
   
   if(m_recalc_combined){
      // A bin list that will hold the errors to output.
      // Create from first entry in current systematic list
      // this copies all the bin information and data/uncertainties
      if(m_combined){
         Delete();
      }
      m_combined = new SystematicBinList(begin()->second.size());
      // Get combined jes components
      GetCombinedJesComponentBins();
      
      iterator itr = begin();
      
      // iterate over systematics in list
      for(;itr != end();++itr){
         std::string systematic = itr->first;
         SystematicBinList& binErrors = itr->second;
         
         // if running with JES components, need to do things differently
         if(MasterSystList::UseJesComponents()){
            // skip JES components and background as they have already been calculated
            if(MasterSystList::IsJesComponentSyst(systematic) ||
               MasterSystList::IsJesComponentBackgroundSyst(systematic)){
               continue;
            }
            else{
               // keep a running tally of the sum of squared errors for each ssytematic.
               for(unsigned int bin=0;bin<binErrors.size();++bin){
                  (*m_combined)[bin].up(  (*m_combined)[bin].up()  + binErrors[bin].up2());
                  (*m_combined)[bin].down((*m_combined)[bin].down()+ binErrors[bin].down2());
               }
            }
         }
         else{
            // keep a running tally of the sum of squared errors for each bin.
            for(unsigned int bin=0;bin<binErrors.size();++bin){
               (*m_combined)[bin].up(  (*m_combined)[bin].up()  +binErrors[bin].up2());
               (*m_combined)[bin].down((*m_combined)[bin].down()+binErrors[bin].down2());
            }
         }
      } // end for(systematics)
      
      // now add in the combined JES components if they are being used
      if(MasterSystList::UseJesComponents()){
         for(unsigned int bin=0;bin<begin()->second.size();++bin){
            (*m_combined)[bin].up(  (*m_combined)[bin].up()  +(*m_combined_jes)[bin].up2());
            (*m_combined)[bin].down((*m_combined)[bin].down()+(*m_combined_jes)[bin].down2());
         }
      }
      
      // now take sqrt of each sum of squares to get error
      for(unsigned int bin=0;bin<m_combined->size();++bin){
         (*m_combined)[bin].up(  sqrt((*m_combined)[bin].up()));
         (*m_combined)[bin].down(sqrt((*m_combined)[bin].down()));
         
         // copy the bin content, statistical uncertainty and bin information
         (*m_combined)[bin].content(begin()->second[bin].content());
         (*m_combined)[bin].stat(begin()->second[bin].stat());
         (*m_combined)[bin].low(begin()->second[bin].low());
         (*m_combined)[bin].high(begin()->second[bin].high());
      }
      
      m_recalc_combined = false;
   }
   return m_combined;
}

SystematicBinList* SystematicList::GetCombinedJesComponentBins(){
   
   // A bin list that will hold the errors to output.
   // Create from first entry in current systematic list
   // this copies all the bin information and data/uncertainties
   if(!m_combined_jes)
      m_combined_jes = new SystematicBinList(begin()->second.size());
   
   if(MasterSystList::UseJesComponents() && m_recalc_combined_jes){
      
      // container for combined components
      SystematicBinList jesi(begin()->second.size());
      // container for background
      SystematicBinList jesi_bkgd(begin()->second.size());
      
      iterator itr = begin();
   
      // iterate over systematics in list
      for(;itr != end();++itr){
         std::string systematic = itr->first;
         SystematicBinList& binErrors = itr->second;
         
         // add jes component systematics in quadrature separately
         if(MasterSystList::IsJesComponentSyst(systematic)){
            // keep a running tally of the sum of squared errors for each bin.
            for(unsigned int bin=0;bin<binErrors.size();++bin){
               jesi[bin].up(jesi[bin].up()+binErrors[bin].up2());
               jesi[bin].down(jesi[bin].down()+binErrors[bin].down2());
            }
         }
         // also kee the jes background separate for later manipulation
         else if(MasterSystList::IsJesComponentBackgroundSyst(systematic)){
            // there is only one background JES so we can just directly
            // assign the value
            for(unsigned int bin=0;bin<binErrors.size();++bin){
               jesi_bkgd[bin].up(binErrors[bin].up());
               jesi_bkgd[bin].down(binErrors[bin].down());
            }
         }
      }
      
      // loop over bins to calculate final combined error
      // now take sqrt of each sum of squares to get error
      for(unsigned int bin=0;bin<m_combined_jes->size();++bin){
         
         // first jes components are added linearly with the background
         double totalJES_up   = sqrt(jesi[bin].up())   + jesi_bkgd[bin].up();
         double totalJES_down = sqrt(jesi[bin].down()) + jesi_bkgd[bin].down();
         
         // then the total JES can be added in quadrature with the other systematics
         (*m_combined_jes)[bin].up(  (*m_combined_jes)[bin].up()   + totalJES_up   * totalJES_up);
         (*m_combined_jes)[bin].down((*m_combined_jes)[bin].down() + totalJES_down * totalJES_down);
         
         // now need to take the squareroot to get the total uncertainty
         (*m_combined_jes)[bin].up(  sqrt((*m_combined_jes)[bin].up()));
         (*m_combined_jes)[bin].down(sqrt((*m_combined_jes)[bin].down()));
         
         // copy the bin content, statistical uncertainty and bin information
         (*m_combined_jes)[bin].content(begin()->second[bin].content());
         (*m_combined_jes)[bin].stat(begin()->second[bin].stat());
         (*m_combined_jes)[bin].low(begin()->second[bin].low());
         (*m_combined_jes)[bin].high(begin()->second[bin].high());
      }
      m_recalc_combined_jes = false;
   }
   
   return m_combined_jes;
}

SystematicList* SystematicList::InstanceFromUnfoldOutput(const std::string filename, const bool normalized){
   const std::string error_function = "[SystematicList::InstanceFromUnfoldOutput]";
   
   // Get the master systematic list that is being used
   MasterSystList* masterSystList = new MasterSystList;
   const std::vector<std::string>& tmp = masterSystList->SystematicsToRunBase();
   std::vector<std::string> systematics(tmp);
   systematics.push_back("pdf");
   
   // open the input file
   TFile* inFile = globals::OpenFile(error_function,filename);
   
   // create output SystematicList
   SystematicList* list = new SystematicList;
   
   /// histo for stat uncertainties
   std::string histo_name = "diffxs";
   if(normalized) histo_name += "_norm";
   histo_name += "_stat";
   TH1F* diffxs_stat = globals::GetFromFile<TH1F>(error_function,inFile,histo_name);
   
   // find channel name
   std::string channel;
   if(filename.find("_el") != std::string::npos)
     channel = "el";
   else
     channel = "mu";
   
   for(std::vector<std::string>::const_iterator itr = systematics.begin();
       itr != systematics.end(); ++itr)
   {
      if(*itr=="nominal")continue;
      if(MasterSystList::skipSystematic(*itr,channel)) continue;
      
      // retrieve histograms for this systematic
      std::string histo_name = (*itr);
      if(normalized) histo_name += "_norm";
      TGraphAsymmErrors* nominalWithSingleSyst = globals::GetFromFile<TGraphAsymmErrors>(error_function,inFile,histo_name);
      
      histo_name = (*itr) + "_shape_up";
      if(normalized) histo_name += "_norm";
      TH1F* shape_up = (TH1F*)inFile->Get(histo_name.c_str());
      if(shape_up) shape_up->SetDirectory(0);
      
      histo_name = (*itr) + "_shape_down";
      if(normalized) histo_name += "_norm";
      TH1F* shape_down = (TH1F*)inFile->Get(histo_name.c_str());
      if(shape_down) shape_down->SetDirectory(0);
      
      histo_name = (*itr) + "_shape";
      if(normalized) histo_name += "_norm";
      TH1F* shape = (TH1F*)inFile->Get(histo_name.c_str());
      if(shape) shape->SetDirectory(0);
      
//       std::cerr << "systematic: " << (*itr) << std::endl;
      
      // create new SystematicBinList for this systematic
      (*list)[(*itr)] = SystematicBinList(nominalWithSingleSyst->GetN());
      SystematicBinList& binList = (*list)[(*itr)];
      
      // loop over bins and fill bin list
      for(unsigned int i=0;i<binList.size();++i){
         SystematicBin& bin = binList[i];
         
         bin.content(nominalWithSingleSyst->GetY()[i]);
         bin.up     (nominalWithSingleSyst->GetEYhigh()[i]);
         bin.down   (nominalWithSingleSyst->GetEYlow()[i]);
         bin.low    (nominalWithSingleSyst->GetX()[i]  - nominalWithSingleSyst->GetEXlow()[i]);
         bin.high   (nominalWithSingleSyst->GetX()[i]  + nominalWithSingleSyst->GetEXhigh()[i]);
         
         bin.stat(diffxs_stat->GetBinError(i+1));
         bin.syst((bin.up()+bin.down())/2.);
         
         // asymmetric uncertainties
         if(shape_up && shape_down && shape == 0){
            bin.shape_up(  shape_up  ->GetBinContent(i+1)-bin.content());
            bin.shape_down(shape_down->GetBinContent(i+1)-bin.content());
            bin.syst((bin.up()+bin.down())/2.);
         } // symmetric uncertainties
         else if(shape && shape_down == 0 && shape_up == 0){
            bin.shape_up(shape->GetBinContent(i+1)-bin.content());
            bin.shape_down(bin.content()-shape->GetBinContent(i+1));
            bin.syst((bin.up()+bin.down())/2.);
         }
         else{
            bin.shape_up(0);
            bin.shape_down(0);
            bin.syst(bin.up()); // up is equal to down since it's symmetric
         }
//          std::cerr << bin.str() << std::endl;
      } // end for(binList)
      
   }// end for(systematics)
   
   
   
   delete masterSystList;
   masterSystList = 0;
   
   return list;
}

