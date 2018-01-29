#ifndef PDFSYSTEMATIC_H
#define PDFSYSTEMATIC_H

#include <string>
#include <vector>

#include "TopDecayChannel.h"

class PdfSystematic{
public:
   PdfSystematic(const std::string sampleName,const TopDecayChannel& channel);
   
   double operator[](const unsigned int index) const {
      return m_array.size() > index ? m_array[index] : 0.;
   }
   
   unsigned int size(){return m_array.size();};
   
private:
   std::vector<double> m_array;
   
   static const double m_array_systemMass_el[];
   static const double m_array_systemMass_mu[];
   static const double m_array_systemRapidity_el[];
   static const double m_array_systemRapidity_mu[];
   static const double m_array_systemPt_el[];
   static const double m_array_systemPt_mu[];
   static const double m_array_top1_el[];
   static const double m_array_top1_mu[];
   static const double m_array_top2_el[];
   static const double m_array_top2_mu[];
   
   
};

#endif