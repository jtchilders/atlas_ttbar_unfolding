#ifndef PVALUELIST_H
#define PVALUELIST_H

#include <string>
#include <map>
#include <iostream>

#include "TMath.h"
#include "TGraphAsymmErrors.h"
#include "TH1.h"
#include "TMatrixD.h"

#include "TopDecayChannel.h"

class PValueList{
public:
   
   class PValue{
   public:
      PValue(const double pvalue,const double chi2,const double ndf){m_pvalue = pvalue;m_chi2=chi2;m_ndf=ndf;};
      PValue(){m_pvalue=0.;m_chi2=0.;};
      double pvalue() const {return m_pvalue;};
      void pvalue(const double value){m_pvalue = value;};
      double chi2() const {return m_chi2;};
      void chi2(const double value){m_chi2 = value;};
      PValue& operator=(const PValue& rhs){m_pvalue=rhs.pvalue();m_chi2=rhs.chi2();m_ndf=rhs.ndf();return *this;};
      double ndf() const {return m_ndf;};
      void ndf(const double value){m_ndf = value;};
   private:
      double m_pvalue;
      double m_chi2;
      double m_ndf;
   };
   
   PValueList(){};
   ~PValueList(){};
   
   void SetPValue(const std::string sampleName,
                  const TopDecayChannel& channel,
                  const std::string predictionName,
                  const PValue& pvalue)
   {
      m_pvalue[sampleName][channel.GetChannel()][predictionName] = pvalue;
   }
   void SetPValue(const std::string sampleName,
                  const TopDecayChannel channel,
                  const std::string predictionName,
                  const double pvalue,
                  const double chi2,
                  const double ndf)
   {
      PValue local_pvalue(pvalue,chi2,ndf);
      m_pvalue[sampleName][channel.GetChannel()][predictionName] = local_pvalue;
   }
   PValue GetPValue(const std::string sampleName,
                  const TopDecayChannel& channel,
                  const std::string predictionName) const;
   
   void CalculateAndFillPValueList(const std::string sampleName,
                                   const TopDecayChannel channel,
                                   const std::string predictionName,
                                   TGraphAsymmErrors const * const data,
                                   TH1 const * const theory,
                                   const bool normalized,
                                   TMatrixD const * const Cov = 0)
   {
      double pvalue = 0,chi2=0;
      double ndf=data->GetN();
      if(normalized) ndf = data->GetN()-1;
      
      GetPValue(data,theory,normalized,pvalue,chi2,Cov);
      SetPValue(sampleName,channel,predictionName,pvalue,chi2,ndf);
   }
   
   void CalculateAndFillPValueList(const std::string sampleName,
                                   const TopDecayChannel channel,
                                   const std::string predictionName,
                                   TGraphAsymmErrors const * const data,
                                   TGraphAsymmErrors const * const theory,
                                   const bool normalized,
                                   TMatrixD const * const Cov = 0)
   {
      double pvalue = 0,chi2=0;
      double ndf=data->GetN();
      if(normalized) ndf = data->GetN()-1;
      GetPValue(data,theory,normalized,pvalue,chi2,Cov);
      SetPValue(sampleName,channel,predictionName,pvalue,chi2,ndf);
   }
   
   static void GetPValue(TGraphAsymmErrors const * const data,
                         TH1 const * const theory,
                         const bool& normalized,
                         double& pvalue,double& chi2,
                         TMatrixD const * const Cov = 0);
   
   static void GetPValue(TGraphAsymmErrors const * const data,
                         TGraphAsymmErrors const * const theory,
                         const bool& normalized,
                         double& pvalue,
                         double& chi2,
                         TMatrixD const * const Cov = 0);
   
   typedef std::map<std::string,std::map<TopDecayChannel::Channel,std::map<std::string,PValue> > > list;
   typedef list::const_iterator list_const_iterator;
   typedef std::map<TopDecayChannel::Channel,std::map<std::string,PValue> > sublist;
   typedef sublist::const_iterator sublist_const_iterator;
   typedef std::map<std::string,PValue> subsublist;
   typedef subsublist::const_iterator subsublist_const_iterator;
   
   
   const list& pvalue_list() const {return m_pvalue;};
   
   
   
private:
   std::map<std::string,std::map<TopDecayChannel::Channel,std::map<std::string,PValue> > > m_pvalue;
   
   
};



#endif
