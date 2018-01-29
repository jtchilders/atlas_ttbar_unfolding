#ifndef PLOTLABEL_H
#define PLOTLABEL_H

#include <string>

#include "TLatex.h"

class PlotLabel{
public:
   
   
   static TLatex* DrawAtlasInternal(double x,double y){
      TLatex atlas;
      atlas.SetNDC();
      atlas.SetTextFont(72);
      return atlas.DrawLatex(x,y,m_atlas_internal_label.c_str());
   }
   
   static TLatex* DrawLumi(double x,double y){
      TLatex lumi;
      lumi.SetNDC();
      lumi.SetTextFont(62);
      return lumi.DrawLatex(x,y,m_lumi_label.c_str());
   }
   
   static TLatex* DrawEnergy(double x,double y){
      TLatex energy;
      energy.SetNDC();
      energy.SetTextFont(62);
      return energy.DrawLatex(x,y,m_energy_label.c_str());
   }
   
   
   
private:
   PlotLabel(){};
   PlotLabel(const PlotLabel& label){};
   
   static const std::string m_atlas_internal_label;
   static const std::string m_lumi_label;
   static const std::string m_energy_label;
   
};



#endif

