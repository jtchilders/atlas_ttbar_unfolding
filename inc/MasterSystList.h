#ifndef MASTERSYSTLIST_H
#define MASTERSYSTLIST_H

#include <vector>
#include <string>
#include <cstring>
#include <iostream>

class MasterSystList{
public:
   MasterSystList();
   ~MasterSystList(){};
   
   static bool IsJesComponentSyst(const std::string syst);
   static bool IsJesComponentBackgroundSyst(const std::string syst);
   
   static bool IsJesComponentSystBase(const std::string syst);
   
   const std::vector<std::string>& SystematicsToRun(){return m_systematicsToRun;};
   const std::vector<std::string>& SystematicsToRunBase(){return m_systematicsToRunBase;};
   
   static bool UseJesComponents() {return m_use_jesi;};
   
   static std::vector<std::string> GetJesComponentBaseNames();
   
   static bool skipSystematic(const std::string& systematic,const std::string& channel);
private:
   MasterSystList(const MasterSystList& list){};
   
   std::vector<std::string> m_systematicsToRun;
   std::vector<std::string> m_systematicsToRunBase;
   
   std::vector<std::string> m_standardSystematics;
   
   static const char *m_jes_component_systematics[];
   static const unsigned int m_num_jes_components;
   
   static const char *m_jes_component_bkgd_systematics[];
   static const unsigned int m_num_jes_components_bkgd;
   
   static const bool m_use_jesi;
   
};

#endif