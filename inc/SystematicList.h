#ifndef SYSTEMATICLIST_H
#define SYSTEMATICLIST_H

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "SystematicBinList.h"

class SystematicList{
public:
   typedef std::map<std::string,SystematicBinList>::iterator iterator;
   typedef std::map<std::string,SystematicBinList>::const_iterator const_iterator;
   
   SystematicList(){m_combined_jes=0;m_combined=0;clear();};
   ~SystematicList();
   
   void clear(){m_list.clear();Recalc();Delete();};
   
   unsigned int size() const {return m_list.size();};
   
   SystematicBinList& operator[] (const std::string systematicName)
      {Recalc();return m_list[systematicName];};
   
   SystematicList& operator= (const SystematicList& rhs)
      {m_list.clear();m_list = rhs.GetList();Recalc();return *this;};
   
   iterator begin() {return m_list.begin();};
   const_iterator begin() const {return m_list.begin();};
   iterator end() {return m_list.end();};
   const_iterator end() const {return m_list.end();};
   
   iterator find(const std::string systematic){return m_list.find(systematic);};
   const_iterator find(const std::string systematic) const {return m_list.find(systematic);};
   
   SystematicBinList* GetCombinedBins();
   SystematicBinList* GetCombinedJesComponentBins();
   
   std::map<std::string,SystematicBinList> GetList() const {return m_list;};
   
   static SystematicList* InstanceFromUnfoldOutput(const std::string filename, const bool normalized = true);
private:
   std::map<std::string,SystematicBinList> m_list;
   
   SystematicBinList* m_combined_jes;
   bool m_recalc_combined_jes;
   SystematicBinList* m_combined;
   bool m_recalc_combined;
   
   void Recalc(){m_recalc_combined=true;m_recalc_combined_jes=true;};
   void Delete(){
      if(m_combined_jes){
         delete m_combined_jes;m_combined_jes=0;
      }
      if(m_combined){
         delete m_combined;m_combined=0;
      }
   }
};

#endif
