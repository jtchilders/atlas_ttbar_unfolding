#include "PValueList.h"
#include "globals.h"

PValueList::PValue PValueList::GetPValue(const std::string sampleName,
                  const TopDecayChannel& channel,
                  const std::string predictionName) const
{
   list_const_iterator sampleItr = m_pvalue.find(sampleName);
   if(sampleItr == m_pvalue.end()){
      std::cerr << "[PValueList::GetPValue] ERROR no map entry for sample name: " << sampleName << "\n";
      return PValue();
   }
   sublist_const_iterator channelItr = sampleItr->second.find(channel.GetChannel());
   if(channelItr == sampleItr->second.end()){
      std::cerr << "[PValueList::GetPValue] ERROR no map entry for channel name: " << channel.GetChannelString() << "\n";
      return PValue();
   }
   subsublist_const_iterator predictionItr = channelItr->second.find(predictionName);
   if(predictionItr == channelItr->second.end()){
      std::cerr << "[PValueList::GetPValue] ERROR no map entry for prediction name: " << predictionName << "\n";
      return PValue();
   }
   return predictionItr->second;
}

void PValueList::GetPValue(TGraphAsymmErrors const * const data,
                           TH1 const * const theory,
                           const bool& normalized,
                           double& pvalue,double& chi2,
                           TMatrixD const * const Cov)
{
   chi2 = 0;
   
   if(Cov == 0){
      for(unsigned int i=0;i<data->GetN();++i){
         // get data entry
         double d_i = data->GetY()[i];
         // get data error (average up and down)
         double d_sigma_i = (data->GetEYlow()[i]+data->GetEYhigh()[i])/2.;
         // get theory entry
         double t_i = theory->GetBinContent(i+1);
         // get theory error (average up and down)
         double t_sigma_i = theory->GetBinError(i+1);
         
         // get combined error squared
         double sigma_i_2 = d_sigma_i*d_sigma_i + t_sigma_i*t_sigma_i;
         
         // calculate chisquare
         double diff = d_i-t_i;
         chi2 += (diff*diff)/sigma_i_2;
      }
   }
   else
      chi2 = globals::GetChisquared(data,theory,Cov,true);
   
   pvalue = 0;
   if(normalized)
      pvalue = TMath::Prob(chi2,data->GetN()-1);
   else
      pvalue = TMath::Prob(chi2,data->GetN());
}

void PValueList::GetPValue(TGraphAsymmErrors const * const data,
                           TGraphAsymmErrors const * const theory,
                           const bool& normalized,
                           double& pvalue,
                           double& chi2,
                           TMatrixD const * const Cov)
{
   chi2 = 0;
   if(Cov == 0){
      for(unsigned int i=0;i<data->GetN();++i){
         // get data entry
         double d_i = data->GetY()[i];
         // get data error (average up and down)
         double d_sigma_i = (data->GetEYlow()[i]+data->GetEYhigh()[i])/2.;
         // get theory entry
         double t_i = theory->GetY()[i];
         // get theory error (average up and down)
         double t_sigma_i = (theory->GetEYlow()[i]+theory->GetEYhigh()[i])/2.;
         
         // get combined error squared
         double sigma_i_2 = d_sigma_i*d_sigma_i + t_sigma_i*t_sigma_i;
         
         // calculate chisquare
         double diff = d_i-t_i;
         chi2 += (diff*diff)/sigma_i_2;
      }
   }
   else
      chi2 = globals::GetChisquared(data,theory,Cov,true);
   
   pvalue = 0;
   if(normalized)
      pvalue = TMath::Prob(chi2,data->GetN()-1);
   else
      pvalue = TMath::Prob(chi2,data->GetN());
}

