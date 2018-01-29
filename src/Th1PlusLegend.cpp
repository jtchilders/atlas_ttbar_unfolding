#include "Th1PlusLegend.h"
#include <iostream>

Th1PlusLegend::Th1PlusLegend(const double x1,const double y1,const double x2,const double y2,const std::string label){
   
   m_leg = new TLegend(x1,y1,x2,y2,label.c_str());
   m_leg->SetFillStyle(0);
   m_leg->SetBorderSize(0);
   
   m_stack = new THStack("stack","");
   
   m_first = true;
   
}

Th1PlusLegend::~Th1PlusLegend(void){
   if(m_leg){
      delete m_leg;
      m_leg = 0;
   }
   if(m_stack){
      delete m_stack;
      m_stack = 0;
   }
   
}

void Th1PlusLegend::AddHisto(TH1* const histo,const std::string legendText){
   
   m_leg->AddEntry(histo,legendText.c_str());
   
   if(m_first){
      m_stack->Add(histo);
      
      SetTitle(histo->GetXaxis()->GetTitle(),histo->GetYaxis()->GetTitle());
      m_first = false;
   }
   else
      m_stack->Add(histo);
   
}

void Th1PlusLegend::Draw(const std::string option) const{
   
   std::string opts = "nostack E1";
   if(option.size() > 0){
      opts += " ";
      opts += option;
   }
   
   m_stack->Draw(opts.c_str());
   m_stack->GetHistogram()->Draw("0");
   m_stack->Draw(std::string(opts + " same").c_str());
   
   
   m_leg->Draw("same");
   
}

void Th1PlusLegend::SetTitle(const std::string xaxis,const std::string yaxis, const std::string title){
   std::string text = title + ";";
   text += xaxis + ";";
   text += yaxis;
   
   SetTitle(text);
}


// bool Th1PlusLegend::LocateLegend(void){
//    
//    if(m_histo.size() == 0){
//       std::cerr << "[Th1PlusLegend::LocateLegend] ERROR, cannot run with no histograms.\n";
//       return false;
//    }
//    
//    // set minimum area needed for legend in NDC coords
//    double width = 0.25;
//    double height = 0.05 * m_histo.size();
//    
//    
//    // the first histogram determines the plot axis so use it for reference
//    TH1* h = m_histo[0];
//    
//    // get total width, height
//    const double width = h->GetXaxis()->GetXmax() - h->GetXaxis()->GetXmin();
//    const double height = h->GetYaxis()->GetXmax() - h->GetYaxis()->GetXmin();
//    
//    std::vector<double> x (h->GetNbinsX(),0.);
//    std::vector<double> dx(h->GetNbinsX(),0.);
//    std::vector<double> y (h->GetNbinsX(),0.);
//    std::vector<double> dy(h->GetNbinsX(),0.);
//    for(unsigned int bin = 1; bin <= h->GetNbinsX(); ++bin){
//       x [bin-1] = h->GetBinCenter(bin);
//       dx[bin-1] = h->GetBinWidth(bin);
//       y [bin-1] = h->GetBinContent(bin);
//       dy[bin-1] = h->GetBinError(bin)*2;
//    }
//    
//    // so now I have the filled areas and I just need to find a region big enough
//    // for the legend
//    
//    for(unsigned int bin = 1; bin <= h->GetNbinsX(); ++bin){
//       // calculate the space above the data point
//       double dy_above = height - (y[bin] + dy[bin]);
//       double dy_below = height - dy_above - dy[bin];
//    }
//    
//    
// }