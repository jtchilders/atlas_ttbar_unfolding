#ifndef KINEMATICVARIABLE_H
#define KINEMATICVARIABLE_H

#include <string>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <errno.h> 

class KinematicVariable{
public:
   KinematicVariable(const std::string sampleName);
   ~KinematicVariable(void);
   
   void SetSampleName(const std::string sampleName){m_sampleName = sampleName;};
   std::string GetKinVarLabel();
   std::string GetUKinVarLabel();
   std::string GetDKinVarLabel();
   std::string GetNKinVarLabel();
   std::string GetLatexKinVarLabel();
   std::string GetLatexUKinVarLabel();
   std::string GetLatexDKinVarLabel();
   std::string GetLatexDKinVarLabelNoUnits();
   std::string GetLatexNKinVarLabel();
   std::string GetLatexTNKinVarLabel();
   std::string GetLatexTNKinVarNoUnitsLabel();
   std::string GetLatexTDKinVarLabel();
   std::string GetUnitDKinVarLabel();
   std::string GetUnitNKinVarLabel();
   std::string GetUnitKinVarLabel();
private:
   std::string m_label; //kinematic variable
   std::string m_ulabel; //kinematic variable with units
   std::string m_dlabel; //differential kinematic variable
   std::string m_nlabel; //normalized and differential kinematic variable  
   std::string m_latexlabel; //kinematic variable
   std::string m_latexulabel; //kinematic variable with units
   std::string m_latexdlabel; //differential kinematic variable
   std::string m_latexnlabel; //normalized and differential kinematic variable  
   std::string m_latextnlabel; //normalized special for tables
   std::string m_latextdlabel; //differential special for tables   
   std::string m_unitD;       //unit differential
   std::string m_unitN;       //unit normalized
      
   std::string m_sampleName;
};

#endif

