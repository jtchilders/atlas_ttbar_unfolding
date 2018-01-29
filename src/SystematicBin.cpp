#include "SystematicBin.h"
#include "BLUECombination.h"

#include <cmath>
#include <iostream>

bool SystematicBin::extractStatistical(){
   
   if(m_percentContribution.find(BLUECombination::STATISTICS) == m_percentContribution.end()){
      std::cerr << "[SystematicBin::extractStatistical] ERROR No statistics included in percentContribution.\n";
      return false;
   }
   
   std::map<std::string,double>::iterator itr = m_percentContribution.find(BLUECombination::STATISTICS);
   
   // fractional constribution of the statistical uncertainty to the full systematic
   double statFraction = itr->second;
   
   // calculate absolute statistic/systematic
   // remember that the fractions were calculated on the squares of the uncertainties
   // therefore I square the uncertainty, then take the squareroot
   double absStat = sqrt(statFraction*syst()*syst());
   double absSyst = sqrt((1-statFraction)*syst()*syst());
   
   // set new stat/syst values
   syst(absSyst);
   stat(absStat);
   
   // remove statistics from the map
   m_percentContribution.erase(BLUECombination::STATISTICS);
   
   // now rescale the remaining fractional contributions
   // so they add up to one again
   itr = m_percentContribution.begin();
   for(;itr != m_percentContribution.end();++itr){
      itr->second = itr->second/(1-statFraction);
   }
   
   
}


bool SystematicBin::extractStatisticalUp(){
   
   if(m_up_percentContribution.find(BLUECombination::STATISTICS) == m_up_percentContribution.end()){
      std::cerr << "[SystematicBin::extractStatisticalUp] ERROR No statistics included in percentContribution.\n";
      return false;
   }
   
   std::map<std::string,double>::iterator itr = m_up_percentContribution.find(BLUECombination::STATISTICS);
   
   // fractional constribution of the statistical uncertainty to the full systematic
   double statFraction = itr->second;
   
   // calculate absolute statistic/systematic
   // remember that the fractions were calculated on the squares of the uncertainties
   // therefore I square the uncertainty, then take the squareroot
   double absStat = sqrt(statFraction*up()*up());
   double absSyst = sqrt((1-statFraction)*up()*up());
   
   // set new stat/syst values
   up(absSyst);
   stat(absStat);
   
   // remove statistics from the map
   m_up_percentContribution.erase(BLUECombination::STATISTICS);
   
   // now rescale the remaining fractional contributions
   // so they add up to one again
   itr = m_up_percentContribution.begin();
   for(;itr != m_up_percentContribution.end();++itr){
      itr->second = itr->second/(1-statFraction);
   }
   
   
}

bool SystematicBin::extractStatisticalDown(){
   
   if(m_down_percentContribution.find(BLUECombination::STATISTICS) == m_down_percentContribution.end()){
      std::cerr << "[SystematicBin::extractStatisticalUp] ERROR No statistics included in percentContribution.\n";
      return false;
   }
   
   std::map<std::string,double>::iterator itr = m_down_percentContribution.find(BLUECombination::STATISTICS);
   
   // fractional constribution of the statistical uncertainty to the full systematic
   double statFraction = itr->second;
   
   // calculate absolute statistic/systematic
   // remember that the fractions were calculated on the squares of the uncertainties
   // therefore I square the uncertainty, then take the squareroot
   double absStat = sqrt(statFraction*down()*down());
   double absSyst = sqrt((1-statFraction)*down()*down());
   
   // set new stat/syst values
   down(absSyst);
   stat(absStat);
   
   // remove statistics from the map
   m_down_percentContribution.erase(BLUECombination::STATISTICS);
   
   // now rescale the remaining fractional contributions
   // so they add up to one again
   itr = m_down_percentContribution.begin();
   for(;itr != m_down_percentContribution.end();++itr){
      itr->second = itr->second/(1-statFraction);
   }
   
   
}

void SystematicBin::Scale(const double scaler){
   m_syst         = scaler * m_syst;
   m_up           = scaler * m_up;
   m_down         = scaler * m_down;
   m_shape_up     = scaler * m_shape_up;
   m_shape_down   = scaler * m_shape_down;
   m_stat         = scaler * m_stat;
   m_content      = scaler * m_content;
}
