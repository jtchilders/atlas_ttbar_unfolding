#include "KinematicVariable.h"

#include <fstream>
#include <iostream>

KinematicVariable::KinematicVariable(const std::string sampleName){
   SetSampleName(sampleName);
}

KinematicVariable::~KinematicVariable(void){
}

std::string KinematicVariable::GetKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_label = "p^{t#bar{t}}_{T}";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_label = "M_{t#bar{t}}";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_label = "y_{t#bar{t}}";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_label = "p^{t}_{T}";
   }
   
   return m_label;
}

std::string KinematicVariable::GetLatexKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_latexlabel = "\\ptttbar";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_latexlabel = "\\mttbar";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_latexlabel = "\\yttbar";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_latexlabel = "\\ptt";
   }   
   
   return m_latexlabel;
}


std::string KinematicVariable::GetUKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_ulabel = "p^{t#bar{t}}_{T} [GeV]";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_ulabel = "M_{t#bar{t}} [GeV]";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_ulabel = "y_{t#bar{t}}";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_ulabel = "p^{t}_{T} [GeV]";
   }   
   
   return m_ulabel;
}

std::string KinematicVariable::GetLatexUKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_latexulabel = "\\ptttbar [\\rm GeV]$";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_latexulabel = "\\mttbar [\\rm GeV]$";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_latexulabel = "\\yttbar";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_latexulabel = "\\ptt [\\rm GeV]$";
   }   
   
   return m_latexulabel;
}


std::string KinematicVariable::GetDKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_dlabel = "#frac{d#sigma}{dp^{t#bar{t}}_{T}} #left[#frac{pb}{GeV}#right]";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_dlabel = "#frac{d#sigma}{dM_{t#bar{t}}} #left[#frac{pb}{GeV}#right]";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_dlabel = "#frac{d#sigma}{dy_{t#bar{t}}} #left[pb#right]";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_dlabel = "#frac{d#sigma}{dp_{T}^{t}} #left[#frac{pb}{GeV}#right]";
   }   
   
   return m_dlabel;
}

std::string KinematicVariable::GetLatexDKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_latexdlabel = "$\\frac{\\dsigma}{\\dptttbar} \\left[\\frac{\\rm pb}{\\rm GeV}\\right]$";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_latexdlabel = "$\\frac{\\dsigma}{\\mttbar} \\left[\\frac{\\rm pb}{\\rm GeV}\\right]$";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_latexdlabel = "$\\frac{\\dsigma}{\\yttbar} \\left[\\rm pb\\right]$";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_latexdlabel = "$\\frac{\\dsigma}{\\ptt} \\left[\\frac{\\rm pb}{\\rm GeV}\\right]$";
   }   
   
   return m_latexdlabel;
}


std::string KinematicVariable::GetLatexDKinVarLabelNoUnits(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_latexdlabel = "$\\frac{\\dsigma}{\\dptttbar}$";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_latexdlabel = "$\\frac{\\dsigma}{\\mttbar}$";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_latexdlabel = "$\\frac{\\dsigma}{\\yttbar}$";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_latexdlabel = "$\\frac{\\dsigma}{\\ptt}$";
   }   
   
   return m_latexdlabel;
}

std::string KinematicVariable::GetLatexTDKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_latextdlabel = "$\\frac{\\dsigma}{\\dptttbar} \\left[\\rm pb / \\rm GeV \\right]$";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_latextdlabel = "$\\frac{\\dsigma}{\\mttbar} \\left[\\rm pb / \\rm GeV \\right]$";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_latextdlabel = "$\\frac{\\dsigma}{\\yttbar} \\left[\\rm pb\\right]$";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_latextdlabel = "$\\frac{\\dsigma}{\\ptt} \\left[\\rm pb / \\rm GeV \\right]$";
   }   
   
   return m_latextdlabel;
}

std::string KinematicVariable::GetUnitDKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_unitD = "$\\left[\\rm pb / \\rm GeV \\right]$";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_unitD = "$\\left[\\rm pb / \\rm GeV \\right]$";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_unitD = "$\\left[\\rm pb\\right]$";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_unitD = "$\\left[\\rm pb / \\rm GeV \\right]$";
   }   
   
   return m_unitD;
}

std::string KinematicVariable::GetNKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_nlabel = "#frac{1}{#sigma} #frac{d#sigma}{dp^{t#bar{t}}_{T}} #left[GeV^{-1}#right]";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_nlabel = "#frac{1}{#sigma} #frac{d#sigma}{dM_{t#bar{t}}} #left[GeV^{-1}#right]";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_nlabel = "#frac{1}{#sigma} #frac{d#sigma}{dy_{t#bar{t}}}";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_nlabel = "#frac{1}{#sigma} #frac{d#sigma}{dp_{T}^{t}} #left[GeV^{-1}#right]";
   }   
   
   return m_nlabel;
}

std::string KinematicVariable::GetLatexNKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_latexnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dptttbar} \\left[\\rm GeV^{-1}\\right]$";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_latexnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dmttbar} \\left[\\rm GeV^{-1}\\right]$";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_latexnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dyttbar}$";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_latexnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dptt} \\left[\\rm GeV^{-1}\\right]$";
   }   
   
   return m_latexnlabel;
}

std::string KinematicVariable::GetLatexTNKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_latextnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dptttbar} \\left[\\rm \\ TeV^{-1}\\right]$";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_latextnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dmttbar} \\left[\\rm \\ TeV^{-1}\\right]$";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_latextnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dyttbar}$";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_latextnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dptt} \\left[\\rm \\ TeV^{-1}\\right]$";
   }   
   
   return m_latextnlabel;
}

std::string KinematicVariable::GetLatexTNKinVarNoUnitsLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_latextnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dptttbar}$";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_latextnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dmttbar} $";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_latextnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dyttbar}$";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_latextnlabel = "$\\frac{1}{\\sigma} \\frac{\\dsigma}{\\dptt}$";
   }   
   
   return m_latextnlabel;
}

std::string KinematicVariable::GetUnitNKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_unitN = "$\\rm TeV^{-1}$";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_unitN = "$\\rm TeV^{-1}$";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_unitN = "";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_unitN = "$\\rm TeV^{-1}$";
   }   
   
   return m_unitN;
}


std::string KinematicVariable::GetUnitKinVarLabel(){
   if(m_sampleName.find("SystemPt") != std::string::npos){
      m_unitN = "[GeV]";
   }
   else if(m_sampleName.find("SystemMass") != std::string::npos){
      m_unitN = "[GeV]";
   }   
   else if(m_sampleName.find("SystemRapidity") != std::string::npos){
      m_unitN = "";
   } 
   else if(m_sampleName.find("Top1") != std::string::npos || m_sampleName.find("Top2") != std::string::npos){
      m_unitN = "[GeV]";
   }   
   
   return m_unitN;
}
