#include "Utils.h"

#include "TH1D.h"

TH1* Utils::Graph2Histo(TGraph const * const graph){
   
   if(graph == 0){
      std::cerr << "[Utils::GetShiftedUpHisto] ERROR Empty pointer.\n";
      return (TH1*)0;
   }
   
   if(graph->GetN() == 0){
      std::cerr << "[Utils::GetShiftedUpHisto] ERROR Empty graph: " << graph->GetName() << std::endl;
      return (TH1*)0;
   }
   
   // get bin edges
   std::vector<double> binEdges;
   for(unsigned int i=0;i<graph->GetN();++i)
      binEdges.push_back(graph->GetX()[i] - graph->GetEXlow()[i]);
   binEdges.push_back(graph->GetX()[graph->GetN()-1] + graph->GetEXhigh()[graph->GetN()-1]);
   
   std::string hname = graph->GetName();
   hname += "_shiftUp";
   TH1D* histo = new TH1D(hname.c_str(),graph->GetTitle(),binEdges.size()-1,&binEdges[0]);
   histo->SetDirectory(0);
   histo->GetXaxis()->SetTitle(graph->GetXaxis()->GetTitle());
   histo->GetYaxis()->SetTitle(graph->GetYaxis()->GetTitle());
   histo->SetLineColor(graph->GetLineColor());
   histo->SetLineWidth(graph->GetLineWidth());
   histo->SetLineStyle(graph->GetLineStyle());
   histo->SetMarkerColor(graph->GetMarkerColor());
   histo->SetMarkerStyle(graph->GetMarkerStyle());
   histo->SetFillColor(graph->GetFillColor());
   histo->SetFillStyle(graph->GetFillStyle());
   
   for(unsigned int i=0;i<graph->GetN();++i)
      histo->SetBinContent(i+1,graph->GetY()[i]);
   
   return histo;
}

TH1* Utils::GetShiftedUpHisto(TGraphAsymmErrors const * const graph){
   TH1* histo = Graph2Histo(graph);
   
   for(unsigned int i=0;i<graph->GetN();++i)
      histo->SetBinContent(i+1,graph->GetY()[i]+graph->GetEYhigh()[i]);
   
   return histo;
}

TH1* Utils::GetShiftedDownHisto(TGraphAsymmErrors const * const graph){
   TH1* histo = Graph2Histo(graph);
   
   for(unsigned int i=0;i<graph->GetN();++i)
      histo->SetBinContent(i+1,graph->GetY()[i]-graph->GetEYlow()[i]);
   
   return histo;
}

void Utils::ScaleGraph(TGraphAsymmErrors const * const graph,const double scaler){
   for(unsigned int i=0;i<graph->GetN();++i){
      graph->GetY()[i] *= scaler;
      graph->GetEYhigh()[i] *= scaler;
      graph->GetEYlow()[i] *= scaler;
   }
   
}