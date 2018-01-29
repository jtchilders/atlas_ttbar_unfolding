#ifndef MEANVALUE_H
#define MEANVALUE_H

#include <cmath>

class MeanValue{
public:
   MeanValue(void){Clear();};
   ~MeanValue(void){};
   
   void Clear(void){m_sum=0.0;m_sum2=0.0;m_n=0;
      m_mean=0.0;m_sigma=0.0;
      m_withErrors=false;m_keepHistory=false;
      m_hist_mean.clear();m_hist_sigma.clear();
      m_updateMean=false;
   };
   
   void AddEntry(double entry){ 
      AddSum(entry);AddSum2(entry);Increment();m_updateMean=true;
      if(m_keepHistory){ CalculateMean();CalculateSigma();}
   };
   void AddEntry(double entry, double error){
      AddSum(entry,error);Increment();m_updateMean=true;
      if(m_keepHistory){ CalculateMean();CalculateSigma();}
   }
   
   int GetIntegerMean(void){
      if(m_updateMean) CalculateMean();
      if(m_mean > 0.){
         if(m_mean - floor(m_mean) > 0.5) return (int)m_mean + 1;
         else return (int)m_mean;
      }
      else{
         if(m_mean - floor(m_mean) > 0.5) return (int)m_mean;
         else return (int)m_mean + 1;
      }
   }
   
   int GetIntegerSigma(void){
      if(m_updateMean) CalculateSigma();
      if(m_sigma > 0.){
         if(m_sigma - floor(m_sigma) > 0.5) return (int)m_sigma + 1;
         else return (int)m_sigma;
      }
      else{
         if(m_sigma - floor(m_sigma) > 0.5) return (int)m_sigma;
         else return (int)m_sigma + 1;
      }
   }
   
   double GetMean(void){CalculateMean();return m_mean;};
   double GetSigma(void){CalculateSigma();return m_sigma;};
   
   void WithErrors(bool value){m_withErrors=value;};
   bool WithErrors(void){return m_withErrors;};
   
   void KeepHistory(bool value){m_keepHistory=value;};
   bool KeepHistory(void){return m_keepHistory;};
   
   std::vector<double>* GetMeanHistory(void){return &m_hist_mean;};
   std::vector<double>* GetSigmaHistory(void){return &m_hist_sigma;};
   
   double GetSum(void){return m_sum;};
   double GetSum2(void){return m_sum2;};
   unsigned int GetN(void){return m_n;};
   
private:
   double m_sum;
   double m_sum_error;
   double m_sum2;
   unsigned int m_n;
   
   bool m_withErrors;
   bool m_keepHistory;
   std::vector<double> m_hist_mean;
   std::vector<double> m_hist_sigma;
   
   bool m_updateMean;
   
   double m_mean;
   double m_sigma;
   
   void CalculateMean(void){
      m_updateMean = false;
      m_mean = (m_n == 0) ? 0 : m_sum/(double)m_n;
      if(m_keepHistory) m_hist_mean.push_back(m_mean);
   };
   
   void CalculateSigma(void){
      if(m_updateMean) CalculateMean();
      if(m_withErrors)
         m_sigma = (m_n == 0) ? 0 : m_sum_error/m_n;
      else
         m_sigma = (m_n <= 1)? 0 : sqrt((1.0/(double)m_n)*m_sum2 - m_mean*m_mean);
      
      if(m_keepHistory) m_hist_sigma.push_back(m_sigma);
   };
   
   void AddSum(double entry){m_sum+=entry;};
   void AddSum(double entry,double error){m_sum+=entry;m_sum_error+=error;};
   void AddSum2(double entry){m_sum2+=entry*entry;};
   void Increment(void){m_n++;};
   
   
   
};


#endif
