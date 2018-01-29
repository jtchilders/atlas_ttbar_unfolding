
#include "BLUECombination.h"

#include "SystematicList.h"
#include "SystematicTextFile.h"

#include "TVectorD.h"
#include "TH1D.h"
#include "TH1.h"

const std::string BLUECombination::STATISTICS = "statistics";

BLUECombination::BLUECombination(const std::string filenameA,const std::string filenameB,
                std::vector<std::string>& correlatedSystematics,
                const StatTreatment statTreatment):
      m_correlatedSystematics(correlatedSystematics)
{
   
   Init();
   m_statTreatment = statTreatment;
   
   SystematicTextFile fileA(filenameA);
   fileA.DeleteList(false);
   SystematicTextFile fileB(filenameB);
   fileB.DeleteList(false);
   
   m_listA = fileA.list();
   m_listB = fileB.list();
   m_delete_lists = true;
   
   
   
   // if statistical errors should be treated via BLUE
   // add them to the Systematic Lists
   if(m_statTreatment == CORRELATED || m_statTreatment == UNCORRELATED)
      AddStatsToLists();
}

BLUECombination::BLUECombination(SystematicList* listA,SystematicList* listB,
                                 std::vector<std::string>& correlatedSystematics,
                                 const StatTreatment statTreatment):
      m_correlatedSystematics(correlatedSystematics)
{
   Init();
   m_statTreatment = statTreatment;
   
   m_listA = listA;
   m_listB = listB;
   
   // if statistical errors should be treated via BLUE
   // add them to the Systematic Lists
   if(m_statTreatment == CORRELATED || m_statTreatment == UNCORRELATED)
      AddStatsToLists();
   
}

BLUECombination::~BLUECombination(){
   if(m_histoWithSyst){
      delete m_histoWithSyst; m_histoWithSyst = 0;
   }
   if(m_histoWithStat){
      delete m_histoWithStat; m_histoWithStat = 0;
   }
   if(m_listA && m_delete_lists){
      delete m_listA; m_listA = 0;
   }
   if(m_listB && m_delete_lists){
      delete m_listB; m_listB = 0;
   }
}

void BLUECombination::Init(){
   m_histoWithSyst = 0;
   m_histoWithStat = 0;
   m_delete_lists = false;
}

void BLUECombination::CalculateCovarianceMatrix()
{

   // loop over list A first
   // store names of systematics so we can skip them
   // when we loop over list B
   std::vector<std::string> listOfSystematics;
   
   SystematicList::const_iterator itrA = m_listA->begin();
   
   
   for(;itrA!=m_listA->end();++itrA){
      
      const std::string systematic = itrA->first;
      listOfSystematics.push_back(systematic);
      
      // is this a correlated systematic?
      bool correlated = false;
      if(SystematicCorrelated(m_correlatedSystematics,systematic))
         correlated = true;
      
      const SystematicBinList& binListA = itrA->second;
      
      // does listB have this systematic?
      SystematicList::const_iterator itrB = m_listB->find(systematic);
      if(itrB != m_listB->end()){
         const SystematicBinList& binListB = itrB->second;
         LoopBinsAndFillCovMat(&binListA,&binListB,correlated);
         
      }
      else{
         LoopBinsAndFillCovMat(&binListA,(SystematicBinList*)0,correlated);
      }
      
      
   }
   
   // loop over the systematics that are associated with B, but not A
   SystematicList::const_iterator itrB = m_listB->begin();
   for(;itrB!=m_listB->end();++itrB){
      const std::string systematic = itrB->first;
      
      // skip systematics that were included in the loop over A
      if(SystematicRepeated(listOfSystematics,systematic))
         continue;
      
      listOfSystematics.push_back(systematic);
      
      // is this a correlated systematic? Shouldn't be (since it's only
      // affecting one of the two)
      bool correlated = false;
      if(SystematicCorrelated(m_correlatedSystematics,systematic))
         correlated = true;
      
      const SystematicBinList& binListB = itrB->second;
      
      // does listA have this systematic? I don't see how
      // you could reach this code, but just in case
      SystematicList::const_iterator itrA = m_listA->find(systematic);
      if(itrA != m_listA->end()){
         const SystematicBinList& binListA = itrA->second;
         LoopBinsAndFillCovMat(&binListA,&binListB,correlated);
      }
      else{ // only have B
         LoopBinsAndFillCovMat((SystematicBinList*)0,&binListB,correlated);
      }
      
   }
   
}

void BLUECombination::LoopBinsAndFillCovMat(
            SystematicBinList const * const listA,
            SystematicBinList const * const listB,
            const bool correlated)
{
   
   if(listA && listB){
      if(listA->size() != listB->size()){
         std::cerr << "[BLUECombination::LoopBinsAndFillCovMat] ERROR lists have different number of bins\n";
         return;
      }
   }
   
   unsigned int nbins = 0;
   if(listA) nbins = listA->size();
   else if(listB) nbins = listB->size();
   
   // initialize vector if not done already
   if(m_combinedCovarianceMatrices.size() == 0)
      m_combinedCovarianceMatrices.resize(nbins,(TMatrixD*)0);
   
   // initialize map of vector if not done already
   std::string systematic;
   if(listA) systematic = listA->GetName();
   else if(listB) systematic = listB->GetName();
   if(m_covarianceMatrices.find(systematic) == m_covarianceMatrices.end())
      m_covarianceMatrices[systematic] = std::vector<TMatrixD*>::vector<TMatrixD*>(nbins,(TMatrixD*)0);
   
   // loop over bins
   for(unsigned int i=0;i<nbins;++i){
      
      // create matrix if not already done
      TMatrixD* combinedBinMatrix = 0;
      if(!m_combinedCovarianceMatrices[i]){
         combinedBinMatrix = new TMatrixD(2,2);
         m_combinedCovarianceMatrices[i] = combinedBinMatrix;
      }
      else
         combinedBinMatrix = m_combinedCovarianceMatrices[i];
      
      TMatrixD* systBinMatrix = 0;
      if(!m_covarianceMatrices[systematic][i]){
         systBinMatrix = new TMatrixD(2,2);
         m_covarianceMatrices[systematic][i] = systBinMatrix;
      }
      else
         systBinMatrix = m_covarianceMatrices[systematic][i];
      
      // now create the covariance matrix for A & B, if one list is missing,
      // just create it for the one that is present
      
      // if both A and B exist combine them
      if( listA && listB ){
         *systBinMatrix = CovarianceMatrix((*listA)[i].syst(),(*listB)[i].syst(),correlated);
         *combinedBinMatrix += *systBinMatrix;
      }
      // if only A exists, this is not a shared systematic so use zero
      // for B in covariance matrix
      else if( listA ){
         *systBinMatrix = CovarianceMatrix((*listA)[i].syst(),0.,correlated);
         *combinedBinMatrix += *systBinMatrix;
      }
      // if only B exists, this is not a shared systematic so use zero
      // for A in covariance matrix
      else if( listB ){
         *systBinMatrix = CovarianceMatrix(0.,(*listB)[i].syst(),correlated);
         *combinedBinMatrix += *systBinMatrix;
      }
      
      
   }
   
}

TMatrixD BLUECombination::CovarianceMatrix(const double systA,
                                           const double systB,
                                           const bool correlated)
{
   
    // let's create a covariance matrix
   TMatrixD Cov(2,2);
   
   Cov[0][0] = systA * systA;
   Cov[1][1] = systB * systB;
   
   if(!correlated){
      Cov[0][1] = 0.;
      Cov[1][0] = 0.;
   }
   else{
      Cov[0][1] = systA * systB;
      Cov[1][0] = systA * systB;
   }
   
//    Cov.Print();
    
   return Cov;
}

SystematicBinList& BLUECombination::Combine(
      const std::vector<double>& dataA,
      const std::vector<double>& dataB)
{
   
   unsigned int nbins = m_listA->begin()->second.size();
   bool useNewData = false;
   if(dataA.size() > 0 && dataB.size() > 0){ 
      
      if(dataA.size() != m_listA->begin()->second.size() || 
         dataB.size() != m_listB->begin()->second.size()){
         std::cerr << "[BLUECombination::Combine] ERROR new data passed, but bin numbers do not match with those passed to constructor. dataA: " << dataA.size() << " dataB: " << dataB.size() << " listA(first entry): " << m_listA->begin()->second.size() << " listB(first entry): " << m_listB->begin()->second.size() << "\n";
         return m_combined;
      }
      useNewData = true;
   }
   
   if(m_listA->size() == 0 || m_listB->size() == 0)
      std::cerr << "[BLUECombination::Combine] ERROR SystematicList objects have no entries: " << m_listA->size() << " " << m_listB->size() << "\n";
   
   SystematicList::const_iterator listA_itr = m_listA->begin();
   if(listA_itr == m_listA->end())
      std::cerr << "[BLUECombination::Combine] ERROR SystematicList (listA) is empty.\n";
   const SystematicBinList& binListA = listA_itr->second;
//    SystematicBinList const * const binListA = &(listA_itr->second);
   SystematicList::const_iterator listB_itr = m_listB->begin();
   if(listB_itr == m_listB->end())
         std::cerr << "[BLUECombination::Combine] ERROR SystematicList (listB) is empty.\n";
   const SystematicBinList& binListB = listB_itr->second;
//    SystematicBinList const * const binListB = &(m_listB->begin()->second);
   
   m_combined.resize(binListA.size());
   
   for(unsigned int i=0;i<nbins;++i){
      
      const SystematicBin& binA = binListA[i];
      const SystematicBin& binB = binListB[i];
//       SystematicBin const * const binA = &(binListA[i]);
//       SystematicBin const * const binB = &(binListB[i]);
      
      
      TMatrixD& Cov = *m_combinedCovarianceMatrices[i];
      
      // let's invert it for combination purposes:
      TMatrixD InvCov = Cov;
      InvCov.Invert();
      
      
      // let's prepare the Unitary matrix, which basically tells both values are the measurement of the same observable
      // it's rectangular shape is also needed for the matrix multiplicatoin below to work properly
      TMatrixD Umatrix(2,1);
      Umatrix[0][0] = 1.;
      Umatrix[1][0] = 1.;
   
      TMatrixD UmatrixT(1,2);
      UmatrixT[0][0] = 1.;
      UmatrixT[0][1] = 1.;
   
   
      // full BLUE formula, using the matrix multiplication definition of lambda:
   
      // this is realy the meat of the method, using the already computed inverted covariance
      // and the lambda (linear coefficients for the combinations) computed right here:
   
   
      // --- compute lambda:
      TMatrixD lambda(1,2);
      lambda = (UmatrixT*InvCov*Umatrix).Invert() * (UmatrixT*InvCov);
      
      TMatrixD lambdaT(2,1);
      lambdaT[0][0] = lambda[0][0];
      lambdaT[1][0] = lambda[0][1];
      
      // --- combined mean:
      
      TVectorD data(2);
      if(useNewData){
         data[0] = dataA[i];
         data[1] = dataB[i];
      }
      else{
         data[0] = binA.content();
         data[1] = binB.content();
      }
      
      TVectorD combMeanVec(1);
      combMeanVec = (lambda*data);
      
      // we have to extract the value from 1x1 'vector':
      double combMean = combMeanVec[0];
   
      // double combMean = getWeightedMean(Inv,x,y);
   
      // --- and the error:
      TVectorD combErrVec(1);
      combErrVec = lambda*(Cov*lambdaT);
      // we have to extract the value from 1x1 'vector':
      double combErr = sqrt(combErrVec[0]);
      
      SystematicBin& bin = m_combined[i];
      bin.content(combMean);
      bin.syst(combErr);
      double combinedStat =  0.;
      if(m_statTreatment == INDEPENDENT)
         combinedStat = sqrt(binA.stat()*binA.stat() + binB.stat()*binB.stat());
      bin.stat(combinedStat);
      
      // copy other information to bin
      bin.low(binA.low());
      bin.high(binA.high());
      bin.stat(binA.stat());
      bin.up(0.);
      bin.down(0.);
      bin.shape_down(0.);
      bin.shape_up(0.);
      
      
      FillSystematicContributions(lambda,lambdaT,combErr,i,bin);
      
   }
   
   return m_combined;
}

bool BLUECombination::SystematicRepeated(const std::vector<std::string>& list,const std::string& syst){
   for(unsigned int i=0;i<list.size();++i){
      if(list[i].find(syst) != std::string::npos){
         return true;
      }
   }
   return false;
}

bool BLUECombination::SystematicCorrelated(
      const std::vector<std::string>& list,
      const std::string& syst)
{
   for(unsigned int i=0;i<list.size();++i){
      if(list[i].find(syst) != std::string::npos){
         return true;
      }
   }
   return false;
}

TH1D* BLUECombination::GetHistoWithSyst(){
   
   if(!m_histoWithSyst){
      // build custom bin vector
      std::vector<double> binBoundaries;
      for(unsigned int i=0;i<m_combined.size();++i){
         binBoundaries.push_back(m_combined[i].low()); // push_back the low bin edge
      }
      // last include the high bin edge
      binBoundaries.push_back(m_combined[m_combined.size()-1].high());
      
      m_histoWithSyst = new TH1D("BlueCombinationWithSyst","",binBoundaries.size()-1,&binBoundaries.front());
      m_histoWithSyst->SetDirectory(false);
      
      for(unsigned int bin=1;bin<=m_histoWithSyst->GetNbinsX();++bin){
         m_histoWithSyst->SetBinContent(bin,m_combined[bin-1].content());
         m_histoWithSyst->SetBinError(bin,m_combined[bin-1].syst());
      }
      
      
   }
   
   return m_histoWithSyst;
}


TH1D* BLUECombination::GetHistoWithStat(){
   
   if(!m_histoWithStat){
      // build custom bin vector
      std::vector<double> binBoundaries;
      for(unsigned int i=0;i<m_combined.size();++i){
         binBoundaries.push_back(m_combined[i].low()); // push_back the low bin edge
      }
      // last include the high bin edge
      binBoundaries.push_back(m_combined[m_combined.size()-1].high());
      
      m_histoWithStat = new TH1D("BlueCombinationWithStat","",binBoundaries.size()-1,&binBoundaries.front());
      m_histoWithStat->SetDirectory(false);
      
      for(unsigned int bin=1;bin<=m_histoWithSyst->GetNbinsX();++bin){
         m_histoWithStat->SetBinContent(bin,m_combined[bin-1].content());
         m_histoWithStat->SetBinError(bin,m_combined[bin-1].stat());
      }
      
      
   }
   
   return m_histoWithStat;
}

void BLUECombination::AddStatsToLists()
{
   
   // the statistical uncertainties should be the same for all
   // systematics, so just grab the first one in the list.
   SystematicList::const_iterator itrA = m_listA->begin();
   if(itrA == m_listA->end()){
      std::cerr << "[BLUECombination::AddStatsToLists] ERROR ListA is empty.\n";
      return;
   }
   const SystematicBinList& binListA = itrA->second;
   
   // now for ListB
   SystematicList::const_iterator itrB = m_listB->begin();
   if(itrB == m_listB->end()){
      std::cerr << "[BLUECombination::AddStatsToLists] ERROR ListB is empty.\n";
      return;
   }
   const SystematicBinList& binListB = itrB->second;
   
   // loop over the bins and fill a new SystematicBinList with stats
   (*m_listA)[STATISTICS] = SystematicBinList();
   SystematicBinList& statsA = m_listA->find(STATISTICS)->second;
   statsA.SetName(STATISTICS);
   (*m_listB)[STATISTICS] = SystematicBinList();
   SystematicBinList& statsB = m_listB->find(STATISTICS)->second;
   statsB.SetName(STATISTICS);
   
   for(unsigned int i=0;i<binListA.size();++i)
   {
      // new bins
      statsA.push_back(SystematicBin());
      // overwrite systematic with statistical uncertainty
      statsA[i].stat(binListA[i].stat());
      statsA[i].syst(binListA[i].stat());
      statsA[i].low(binListA[i].low());
      statsA[i].high(binListA[i].high());
      statsA[i].content(binListA[i].content());
      statsA[i].up(0.);
      statsA[i].down(0.);
      statsA[i].shape_down(0.);
      statsA[i].shape_up(0.);
      // and for list B
      statsB.push_back(SystematicBin());
      statsB[i].stat(binListB[i].stat());
      statsB[i].syst(binListB[i].stat());
      statsB[i].low(binListB[i].low());
      statsB[i].high(binListB[i].high());
      statsB[i].content(binListB[i].content());
      statsB[i].up(0.);
      statsB[i].down(0.);
      statsB[i].shape_down(0.);
      statsB[i].shape_up(0.);
      
   }
   
   
   // add statistics to the correlated list
   if(m_statTreatment == CORRELATED)
      m_correlatedSystematics.push_back(STATISTICS);
}

void BLUECombination::FillSystematicContributions(const TMatrixD& lambda,
      const TMatrixD& lambdaT,
      const double& combinedUncertainty,
      const unsigned int& binNumber,
      SystematicBin& systBin)
{
   TMatrixD combined(2,2);
   double sumRatio = 0.;
   std::map<std::string,std::vector<TMatrixD*> >::iterator itr = m_covarianceMatrices.begin();
   for(;itr != m_covarianceMatrices.end();++itr){
      std::string systematic = itr->first;
      const std::vector<TMatrixD*>& binMatrices = itr->second;
      
      TMatrixD* Cov = binMatrices[binNumber];
      
      combined += *Cov;
      
      TVectorD combErrVec(1);
      combErrVec = lambda*(*Cov*lambdaT);
      // we have to extract the value from 1x1 'vector':
      double err2 = combErrVec[0];
      
      // combinedUncertainty was calculated by taking the sqrt, need to square it
      // to get a consistent comparison
      double ratio = err2/(combinedUncertainty*combinedUncertainty);
      
      sumRatio += ratio;
      systBin.percentContribution(systematic,ratio);
      
   }
   
//    if( fabs(sumRatio-1) > 0.001 )
//       std::cerr << "[BLUECombination::FillSystematicContributions] WARNING fractional uncertainties do not add to 1, but " << sumRatio << "\n";
   
   // if the statistical uncertainty were included in the BLUE method
   // extract it and re-normalize the percent contributions
   if(m_statTreatment == UNCORRELATED || m_statTreatment == CORRELATED){
      systBin.extractStatistical();
   }
}

