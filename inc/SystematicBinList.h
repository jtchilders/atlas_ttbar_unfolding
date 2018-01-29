#ifndef SYSTEMATICBINLIST_H
#define SYSTEMATICBINLIST_H

#include <vector>
#include <cmath>
#include <string>

#include "SystematicBin.h"

class SystematicBinList{
public:
   SystematicBinList(const std::string name = ""){clear();SetName(name);};
   SystematicBinList(const SystematicBinList& list){clear();Copy(list);};
   SystematicBinList(const unsigned int initialSize,const std::string name = ""){
      clear();m_list.resize(initialSize);SetName(name);
   };
   
   void clear(){m_list.clear();m_calcCombined=true;m_combined_up=0;m_combined_down=0;};
   
   SystematicBin& operator[] (const unsigned int entry){return m_list[entry];};
   const SystematicBin& operator[] (const unsigned int entry) const {return m_list[entry];};
   SystematicBin at(const unsigned int entry) {return m_list[entry];};
   
   void push_back(const SystematicBin& value){
      m_list.push_back(value);m_calcCombined=true;
   };
   
   unsigned int size() const {return m_list.size();};
   void resize(const unsigned int size){m_list.resize(size);};
   
   void GetCombinedSystematics(double& up, double& down){
      if(m_calcCombined) CalculateCombined();
      up = m_combined_up;
      down = m_combined_down;
   };
   
   SystematicBinList& operator= (const SystematicBinList& rhs){
      Copy(rhs);
      return *this;
   }
   
   std::vector<SystematicBin> GetList() const {return m_list;};
   
   std::vector<double> GetBinEdges() const {
      std::vector<double> output;
      for(unsigned int i=0;i<m_list.size();++i)
         output.push_back(m_list[i].low());
      output.push_back(m_list[m_list.size()-1].high());
      return output;
   }
   
   void Scale(const double scaler){
      for(unsigned int i=0;i<m_list.size();++i){
         m_list[i].Scale(scaler);
      }
   };
   
   double Integral(void) const{
      double integral = 0.;
      for(unsigned int i=0;i<m_list.size();++i){
         integral += m_list[i].content()*(m_list[i].high()-m_list[i].low());
      }
      return integral;
   }
   
   std::string GetName() const {return m_systematic;};
   void SetName(const std::string name){m_systematic = name;};
private:
   
   std::vector<SystematicBin> m_list;
   std::string m_systematic;
   
   double m_combined_up;
   double m_combined_down;
   bool m_calcCombined;
   
   void CalculateCombined(){
      double up2=0,down2=0;
      for(unsigned int i=0;i<size();++i){
         up2 += m_list[i].up2();
         down2 += m_list[i].down2();
      }
      m_combined_up = sqrt(up2);
      m_combined_down = sqrt(down2);
      m_calcCombined=false;
   };
   
   void Copy(const SystematicBinList& rhs){
      m_list.clear();
      m_list = rhs.GetList();
      m_calcCombined = true;
      m_systematic = rhs.GetName();
   };
};


#endif
