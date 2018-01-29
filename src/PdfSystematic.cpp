#include "PdfSystematic.h"

const double PdfSystematic::m_array_systemMass_el[]
      = {.017,.018,.020,.02, .037};
const double PdfSystematic::m_array_systemMass_mu[]
      = {.013,.015,.016,.020,.035};

const double PdfSystematic::m_array_systemPt_el[]
      = {.0197,.0178,.0205,.0294};
const double PdfSystematic::m_array_systemPt_mu[]
      = {.0163,.0139,.0172,.0291};

const double PdfSystematic::m_array_systemRapidity_el[]
      = {.0226,.0085,.0009,.0002,.0092,.0236};
const double PdfSystematic::m_array_systemRapidity_mu[]
      = {.0213,.0079,.001,.0008,.008,.0213};

const double PdfSystematic::m_array_top1_el[]
      = {.021,.021,.020,.018,.016, .012,.012};
const double PdfSystematic::m_array_top1_mu[]
      = {.019,.018,.016,.013,.010,.0052,.0063};

const double PdfSystematic::m_array_top2_el[]
      = {.021,.021,.020,.018,.014,.011,.011};
const double PdfSystematic::m_array_top2_mu[]
      = {.021,.021,.020,.018,.016,.012,.012};


PdfSystematic::PdfSystematic(const std::string sampleName,const TopDecayChannel& channel){
   
   if(sampleName.find("Top1") != std::string::npos){
      if(channel == TopDecayChannel::EJETS)
         m_array.assign(m_array_top1_el,m_array_top1_el + (sizeof(m_array_top1_el)/sizeof(double)));
      else if(channel == TopDecayChannel::MUJETS)
         m_array.assign(m_array_top1_el,m_array_top1_mu + (sizeof(m_array_top1_mu)/sizeof(double)));
   }
   else if(sampleName.find("Top2") != std::string::npos){
      if(channel == TopDecayChannel::EJETS)
         m_array.assign(m_array_top2_el,m_array_top2_el + (sizeof(m_array_top2_el)/sizeof(double)));
      else if(channel == TopDecayChannel::MUJETS)
         m_array.assign(m_array_top2_mu,m_array_top2_mu + (sizeof(m_array_top2_mu)/sizeof(double)));
   }
   else if(sampleName.find("SystemMass") != std::string::npos){
      if(channel == TopDecayChannel::EJETS)
         m_array.assign(m_array_systemMass_el,m_array_systemMass_el + (sizeof(m_array_systemMass_el)/sizeof(double)));
      else if(channel == TopDecayChannel::MUJETS)
         m_array.assign(m_array_systemMass_mu,m_array_systemMass_mu + (sizeof(m_array_systemMass_mu)/sizeof(double)));
   }
   else if(sampleName.find("SystemPt") != std::string::npos){
      if(channel == TopDecayChannel::EJETS)
         m_array.assign(m_array_systemPt_el,m_array_systemPt_el + (sizeof(m_array_systemPt_el)/sizeof(double)));
      else if(channel == TopDecayChannel::MUJETS)
         m_array.assign(m_array_systemPt_mu,m_array_systemPt_mu + (sizeof(m_array_systemPt_mu)/sizeof(double)));
   }
   else if(sampleName.find("SystemRapidity") != std::string::npos){
      if(channel == TopDecayChannel::EJETS)
         m_array.assign(m_array_systemRapidity_el,m_array_systemRapidity_el + (sizeof(m_array_systemRapidity_el)/sizeof(double)));
      else if(channel == TopDecayChannel::MUJETS)
         m_array.assign(m_array_systemRapidity_mu,m_array_systemRapidity_mu + (sizeof(m_array_systemRapidity_mu)/sizeof(double)));
   }
   
   
}
