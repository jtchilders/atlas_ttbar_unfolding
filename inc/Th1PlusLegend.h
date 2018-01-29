#ifndef TH1PLUSLEGEND_H
#define TH1PLUSLEGEND_H

#include <string>

#include "TH1.h"
#include "TLegend.h"
#include "THStack.h"

class Th1PlusLegend{
public:
   Th1PlusLegend(const double x1,const double y1,const double x2,const double y2,const std::string label);
   ~Th1PlusLegend(void);
   
   void AddHisto(TH1* const histo,const std::string legendText = "");
   
   void SetHeader(const std::string label){m_leg->SetHeader(label.c_str());};
   void SetTitle(const std::string title){m_stack->SetTitle(title.c_str());};
   void SetTitle(const std::string xaxis,const std::string yaxis, const std::string title = "");
   
   void Draw(const std::string option = "") const;
   
   double GetMinimum(void) const {return m_stack->GetMinimum();};
   double GetMaximum(void) const {return m_stack->GetMaximum();};
   void SetMinimum(double min){m_stack->SetMinimum(min);};
   void SetMaximum(double max){m_stack->SetMaximum(max);};
   
   THStack* GetStack(void) const {return m_stack;};
   TLegend* GetLegend(void) const {return m_leg;};
   
private:
   
   TLegend*          m_leg;
   THStack*          m_stack;
   bool              m_first;
   
   bool LocateLegend(void);
};




#endif
