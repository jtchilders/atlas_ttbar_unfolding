#ifndef SYSTEMATICHISTOGROUP_H
#define SYSTEMATICHISTOGROUP_H

#include "SystematicSample.h"

class SystematicHistoGroup{
public:
   SystematicHistoGroup(void){
      truth    = 0;
      truth_diffxs = 0;
      truthNorm_diffxs = 0;
      reco     = 0;
      measured = 0;
      unfolded = 0;
      unfoldedNorm = 0;
      diffxs   = 0;
      diffxsNorm = 0;
      meanDeviation = 0;
      rmsDeviation = 0;
      
   };
   
   ~SystematicHistoGroup(){
      if(truth){
         delete truth;truth = 0;
      }
      if(truth_diffxs){
         delete truth_diffxs;truth_diffxs = 0;
      }
      if(truthNorm_diffxs){
         delete truthNorm_diffxs;truthNorm_diffxs = 0;
      }
      if(reco){
         delete reco;reco = 0;
      }
      if(measured){
         delete measured;measured = 0;
      }
      if(unfolded){
         delete unfolded;unfolded = 0;
      }
      if(unfoldedNorm){
         delete unfoldedNorm;unfoldedNorm = 0;
      }
      if(diffxs){
         delete diffxs;diffxs = 0;
      }
      if(diffxsNorm){
         delete diffxsNorm;diffxsNorm = 0;
      }
      if(meanDeviation){
         delete meanDeviation;meanDeviation = 0;
      }
      if(rmsDeviation){
         delete rmsDeviation;rmsDeviation = 0;
      }
   }
   
   TH1* truth;
   TH1* truth_diffxs;
   TH1* truthNorm_diffxs;
   TH1* reco;
   TH1* measured;
   TH1* unfolded;
   TH1* unfoldedNorm;
   TH1* diffxs;
   TH1* diffxsNorm;
   TH1* meanDeviation;
   TH1* rmsDeviation;
   SystematicSample sample;
   
private:
   
};


#endif

