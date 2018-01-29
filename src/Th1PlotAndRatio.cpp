#include "Th1PlotAndRatio.h"
#include "PlotLabel.h"

#include <cmath>
#include <fstream>
#include <algorithm>

const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_YAXIS_TITLE_OFFSET = 0.2;
const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_YAXIS_TITLE_SIZE   = 0.15;
const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_YAXIS_TICK_LENGTH  = 0.01;
const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_YAXIS_LABEL_SIZE   = 0.17;

const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_XAXIS_TITLE_OFFSET = 0.9;
const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_XAXIS_TITLE_SIZE   = 0.2;
const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_XAXIS_TICK_LENGTH  = 0.05;
const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_XAXIS_LABEL_SIZE   = 0.14;

const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_Y_MAX_A = 1.6;
const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_Y_MIN_A = 0.4;
const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_Y_MAX_B = 1.8;
const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_Y_MIN_B = 0.6;
const double Th1PlotAndRatio::RatioPlot::RATIOPLOT_X_AXIS_SIZE = 0.4;

const double Th1PlotAndRatio::PAD_MARGIN_RIGHT = 0.15;
const double Th1PlotAndRatio::PAD_MARGIN_LEFT  = 0.05;

const double Th1PlotAndRatio::Y_AXIS_MAX_NULL = -999.;
const double Th1PlotAndRatio::Y_AXIS_MIN_NULL = 999.;

Th1PlotAndRatio::Th1PlotAndRatio(const std::string legendLabel,Th1PlotAndRatio::RATIO_ORDER ratio_order)
{
   m_leg_label             = legendLabel;
   m_ratio_order           = ratio_order;
   m_combine_generator_ratios = false;
   
   m_dataPlot              = 0;
   
   m_ratio_pad_gridy       = false;
   m_main_pad_logy         = false;
   
   m_y_axis_title_ratio    = "Data/MC";
   m_y_axis_max            = -999.;
   m_y_axis_min            = 999.;
   
   m_canvas                = 0;
   m_updateCanvas          = true;
   m_redrawCanvas          = true;
   m_pad_top               = 0;
   m_legend                = 0;
   m_horizontalLine        = 0;
   
   m_leg_x1                = 0.55;
   m_leg_y1                = 0.92;
   m_leg_x2                = 0.9;
   m_leg_y2                = 0.5;
   
   m_atlas_label_x         = 0.57;
   m_atlas_label_y         = 0.45;
   m_atlas_lumi_x          = 0.57;
   m_atlas_lumi_y          = 0.35;
   
}

Th1PlotAndRatio::~Th1PlotAndRatio(void){
   Clear();
   
   if(m_dataPlot){
      delete m_dataPlot;m_dataPlot = 0;
   }
   
   for(std::vector<PredictionPlot*>::iterator itr = m_generatorPlots.end();
       m_generatorPlots.size() > 0;)
   {
      --itr;
      if(*itr){
         delete *itr;*itr = 0;
         m_generatorPlots.erase(itr);
      }
   }
   
   for(std::vector<PredictionPlot*>::iterator itr = m_theoryPlots.end();
       m_generatorPlots.size() > 0;)
   {
      --itr;
      if(*itr){
         delete *itr;*itr = 0;
         m_theoryPlots.erase(itr);
      }
   }
   
}

void Th1PlotAndRatio::Clear(){
   
   
   if(m_canvas){
      delete m_canvas; m_canvas = 0;
      // in ROOT, a canvas delete automatically deletes child TPad objects
      // so we can just set the pad pointers to zero without deleting them
      if(m_pad_top) m_pad_top = 0;
      for(unsigned int i=m_pad_ratios.size();i>0;--i){
         if(m_pad_ratios[i-1]){
            m_pad_ratios[i-1] = 0;
            m_pad_ratios.pop_back();
         }
      }
   }
   
   if(m_pad_top){
      m_pad_top->Print();
      delete m_pad_top; m_pad_top = 0;
   }
   for(unsigned int i=m_pad_ratios.size();i>0;--i){
      if(m_pad_ratios[i-1]){
         delete m_pad_ratios[i-1];
         m_pad_ratios.pop_back();
      }
   }
   if(m_legend){
      delete m_legend; m_legend = 0;
   }
   if(m_horizontalLine){
      delete m_horizontalLine; m_horizontalLine = 0;
   }
   
}


void Th1PlotAndRatio::CreateCanvas()
{
   
   if(!m_dataPlot){
      std::cerr << "[Th1PlotAndRatio::SaveAs] ERROR: no data plot has been set.\n";
      return;
   }
   
   
   // clean up from any previous plots
   Clear();
   
   // configure the canvas with the top pad and ratio pads
   const double numRatioPads = GetNumRatioPads();
   const double canvas_height = MAINPLOT_HEIGHT+RATIOPLOT_HEIGHT*numRatioPads;
   SetupCanvas(canvas_height,numRatioPads);
   
   // Get X/Y Axis limits based on the data given
   double main_y_max = 0.,main_y_min = 0.,main_x_max = 0.,main_x_min = 0.;
   GetAxisLimits(main_x_min,main_x_max,main_y_min,main_y_max);
   
   // create legend
   m_legend = new TLegend(m_leg_x1,m_leg_y1,m_leg_x2,m_leg_y2,m_leg_label.c_str());
   m_legend->SetFillStyle(0);
   m_legend->SetBorderSize(0);
   
   // Draw data plot
   {
      TGraphAsymmErrors* data = m_dataPlot->graph();
      if(m_y_axis_max != Y_AXIS_MAX_NULL)
         data->SetMaximum(m_y_axis_max);
      else
         data->SetMaximum(main_y_max*1.2);
      if(m_y_axis_min != Y_AXIS_MIN_NULL)
         data->SetMinimum(m_y_axis_min);
      else
         data->SetMinimum(main_y_min);
      data->GetXaxis()->SetLimits( (main_x_min > 0 ? 0 : main_x_min), main_x_max);
      
      data->GetYaxis()->SetTitle(m_y_axis_title_top.c_str());
      // use decimals even for numbers like 1.0
      data->GetYaxis()->SetDecimals(kTRUE);
      data->GetYaxis()->SetTitleOffset(1.3);
      data->GetYaxis()->SetTitleSize(0.05);
      
      data->Draw("ap");
      
      TH1* legend_histo = m_dataPlot->legend_histo();
      // add data to legend
      if(legend_histo){
         m_legend->AddEntry(legend_histo,m_dataPlot->legend_label().c_str(),"lpe");
      }
      else{
         m_legend->AddEntry(data,m_dataPlot->legend_label().c_str(),"lpe");
      }
   }
   
   // calculate the height of the last ratio pad which has to be slightly larger because
   // it will contain the x-axis labels. In order for the plot area to be the same as 
   // all ratios, it has to be slightly larger.
   const double heightOfAllRatioPads = canvas_height - MAINPLOT_HEIGHT;
   const double totalFractionOfRatioPads = heightOfAllRatioPads/canvas_height;
   const double last_ratio_pad_height = canvas_height > 0 ? totalFractionOfRatioPads/((numRatioPads-1)*(1-RatioPlot::RATIOPLOT_X_AXIS_SIZE)+1) : 0.;
   
   // used to scale the bottom most plot
   // if only one bottom plot exists, fix to a value
   const double mid_ratio_pad_height = (numRatioPads == 1) ?
         0. :
         (1-RatioPlot::RATIOPLOT_X_AXIS_SIZE)*last_ratio_pad_height;
   
   
   ///////////////
   // First plot in main pad
   /////////////////////////////////////
   
   // change focus to main pad
   m_pad_top->cd();
   
   // first loop for generator plots
   for(std::vector<PredictionPlot*>::iterator itr = m_generatorPlots.begin();
       itr != m_generatorPlots.end();++itr)
   {
      
      // draw graph on main pad
      (*itr)->histo()->SetFillStyle(0);
      (*itr)->histo()->Draw("HIST same");
      // add to legend
      if((*itr)->legend_histo()){
         m_legend->AddEntry((*itr)->legend_histo(),(*itr)->legend_label().c_str(),(*itr)->legend_draw_options().c_str());
      }
      else{
         m_legend->AddEntry((*itr)->graph(),(*itr)->legend_label().c_str(),(*itr)->legend_draw_options().c_str());
      }
   }
   
   // second loop for theory plots
   for(std::vector<PredictionPlot*>::iterator itr = m_theoryPlots.begin();
       itr != m_theoryPlots.end();++itr)
   {
      
      // draw graph on main pad
      (*itr)->histo()->Draw("HIST same");
      // add to legend
      if((*itr)->legend_histo()){
         m_legend->AddEntry((*itr)->legend_histo(),(*itr)->legend_label().c_str(),(*itr)->legend_draw_options().c_str());
      }
      else{
         m_legend->AddEntry((*itr)->graph(),(*itr)->legend_label().c_str(),(*itr)->legend_draw_options().c_str());
      }
   }
   
   
   ///////////////
   // Now plot ratio pads
   ///////////////////
   unsigned int numRatioPadsThatHaveBeenPlotted = 0;
   
   /// For Generators, if we are combining the ratios into one pad
   /// create the pad now.
   if(m_combine_generator_ratios && m_generatorPlots.size() > 0){
      // change focus to canvas where pad will be drawn
      m_canvas->cd();
      
      double pad_xlow = 0.,pad_xhigh = 1.;
      double pad_ylow = 0.,pad_yhigh = 0.;
      // if this is the last ratio pad we need to set the correct height
      // since this is the first pad to plot, we just need to check if there
      // is only one pad to produce
      if(numRatioPads == 1){
         pad_ylow  = 0.;
         pad_yhigh = last_ratio_pad_height;
      }
      else{
         const unsigned int reverseCount = numRatioPads - numRatioPadsThatHaveBeenPlotted - 1;
         pad_ylow  = last_ratio_pad_height + (reverseCount-1)*mid_ratio_pad_height;
         pad_yhigh = last_ratio_pad_height + reverseCount*mid_ratio_pad_height;
      }
      
      std::stringstream pad_name;
      pad_name << "ratiopad_" << m_pad_ratios.size();
      TPad* pad = new TPad(pad_name.str().c_str(),
                           pad_name.str().c_str(),pad_xlow,
                           pad_ylow,pad_xhigh,pad_yhigh
                          );
      m_pad_ratios.push_back(pad);
      
      // again if this is the one and only pad, use the correct margins
      if(numRatioPads == 1)
         pad->SetMargin(PAD_MARGIN_RIGHT,PAD_MARGIN_LEFT,RatioPlot::RATIOPLOT_X_AXIS_SIZE,0.);
      else
         pad->SetMargin(PAD_MARGIN_RIGHT,PAD_MARGIN_LEFT,0.,0.);
      
      // draw pad on canvas
      pad->Draw();
      
      // change focus to pad for plot
      pad->cd();
      pad->SetGridy(m_ratio_pad_gridy);
      
      numRatioPadsThatHaveBeenPlotted++;
   }
   
   
   // first loop for generator plots
   for(std::vector<PredictionPlot*>::iterator itr = m_generatorPlots.begin();
       itr != m_generatorPlots.end();++itr)
   {
      // if the plot should be made into a ratio
      if(!(*itr)->isPlotted()) continue;
      
      /////////////////
      // if we are not combining the generator ratios
      // create ratio pad for the ratio of reference over this histogram
      
      if(!m_combine_generator_ratios){
         // change focus to canvas where pad will be drawn
         m_canvas->cd();
         
         double pad_xlow = 0.,pad_xhigh = 1.;
         double pad_ylow = 0.,pad_yhigh = 0.;
         // if this is the last ratio pad we need to set the correct height
         if(numRatioPadsThatHaveBeenPlotted == numRatioPads - 1){
            pad_ylow  = 0.;
            pad_yhigh = last_ratio_pad_height;
         }
         else{
            const unsigned int reverseCount = numRatioPads - numRatioPadsThatHaveBeenPlotted - 1;
            pad_ylow  = last_ratio_pad_height + (reverseCount-1)*mid_ratio_pad_height;
            pad_yhigh = last_ratio_pad_height + reverseCount*mid_ratio_pad_height;
         }
         
         std::stringstream pad_name;
         pad_name << "ratiopad_" << m_pad_ratios.size();
         TPad* pad = new TPad(pad_name.str().c_str(),
                              pad_name.str().c_str(),pad_xlow,
                              pad_ylow,pad_xhigh,pad_yhigh
                           );
         m_pad_ratios.push_back(pad);
         
         if(numRatioPadsThatHaveBeenPlotted == numRatioPads - 1)
            pad->SetMargin(PAD_MARGIN_RIGHT,PAD_MARGIN_LEFT,RatioPlot::RATIOPLOT_X_AXIS_SIZE,0.);
         else
            pad->SetMargin(PAD_MARGIN_RIGHT,PAD_MARGIN_LEFT,0.,0.);
         
         // draw pad on canvas
         pad->Draw();
         
         // change focus to pad for plot
         pad->cd();
         pad->SetGridy(m_ratio_pad_gridy);
         
         numRatioPadsThatHaveBeenPlotted++;
      }
      
      //////////////////////////////
      // plot ratio
      /////////////////////
      
      TGraphAsymmErrors* const ratio      = (*itr)->ratio()->graph();
      TH1* const               ratioHisto = (*itr)->ratio()->histo();
      TGraphAsymmErrors* const selfRatio = (m_ratio_order == DATA_AS_NUMERATOR) ? (*itr)->selfRatio()->graph() : m_dataPlot->selfRatio()->graph();
      
      // set x axis since the prediciton ratio is drawn first and sets the axies
      if(numRatioPadsThatHaveBeenPlotted == numRatioPads){
         ratio->GetXaxis()->SetTitle(m_x_axis_title.c_str());
         ratio->GetXaxis()->SetTitleOffset(RatioPlot::RATIOPLOT_XAXIS_TITLE_OFFSET);
         ratio->GetXaxis()->SetTitleSize(RatioPlot::RATIOPLOT_XAXIS_TITLE_SIZE);
         ratio->GetXaxis()->SetLabelSize(RatioPlot::RATIOPLOT_XAXIS_LABEL_SIZE);
         ratio->GetXaxis()->SetTickLength(RatioPlot::RATIOPLOT_XAXIS_TICK_LENGTH);
         ratioHisto->GetXaxis()->SetTitle(m_x_axis_title.c_str());
         ratioHisto->GetXaxis()->SetTitleOffset(RatioPlot::RATIOPLOT_XAXIS_TITLE_OFFSET);
         ratioHisto->GetXaxis()->SetTitleSize(RatioPlot::RATIOPLOT_XAXIS_TITLE_SIZE);
         ratioHisto->GetXaxis()->SetLabelSize(RatioPlot::RATIOPLOT_XAXIS_LABEL_SIZE);
         ratioHisto->GetXaxis()->SetTickLength(RatioPlot::RATIOPLOT_XAXIS_TICK_LENGTH);
      }
      else{
         ratio->GetXaxis()->SetTitle("");
         ratioHisto->GetXaxis()->SetTitle("");
      }
      
      ratio->GetXaxis()->SetLimits(main_x_min,main_x_max);
      ratioHisto->GetXaxis()->SetLimits(main_x_min,main_x_max);
      
      // set y axis
      const double ratio_scaleFactor = (numRatioPads > 1) ? mid_ratio_pad_height/last_ratio_pad_height : 0.85;
      if(numRatioPadsThatHaveBeenPlotted == numRatioPads){
         ratio->GetYaxis()->SetTitleOffset(RatioPlot::RATIOPLOT_YAXIS_TITLE_OFFSET/ratio_scaleFactor);
         ratio->GetYaxis()->SetTitleSize(RatioPlot::RATIOPLOT_YAXIS_TITLE_SIZE*ratio_scaleFactor);
         ratio->GetYaxis()->SetLabelSize(RatioPlot::RATIOPLOT_YAXIS_LABEL_SIZE*ratio_scaleFactor);
         ratio->GetYaxis()->SetTickLength(RatioPlot::RATIOPLOT_YAXIS_TICK_LENGTH/ratio_scaleFactor);
         ratioHisto->GetYaxis()->SetTitleOffset(RatioPlot::RATIOPLOT_YAXIS_TITLE_OFFSET/ratio_scaleFactor);
         ratioHisto->GetYaxis()->SetTitleSize(RatioPlot::RATIOPLOT_YAXIS_TITLE_SIZE*ratio_scaleFactor);
         ratioHisto->GetYaxis()->SetLabelSize(RatioPlot::RATIOPLOT_YAXIS_LABEL_SIZE*ratio_scaleFactor);
         ratioHisto->GetYaxis()->SetTickLength(RatioPlot::RATIOPLOT_YAXIS_TICK_LENGTH/ratio_scaleFactor);
      }
      else{
         ratio->GetYaxis()->SetTitleOffset(RatioPlot::RATIOPLOT_YAXIS_TITLE_OFFSET);
         ratio->GetYaxis()->SetTitleSize(RatioPlot::RATIOPLOT_YAXIS_TITLE_SIZE);
         ratio->GetYaxis()->SetLabelSize(RatioPlot::RATIOPLOT_YAXIS_LABEL_SIZE);
         ratio->GetYaxis()->SetTickLength(RatioPlot::RATIOPLOT_YAXIS_TICK_LENGTH);
         
         ratioHisto->GetYaxis()->SetTitleOffset(RatioPlot::RATIOPLOT_YAXIS_TITLE_OFFSET);
         ratioHisto->GetYaxis()->SetTitleSize(RatioPlot::RATIOPLOT_YAXIS_TITLE_SIZE);
         ratioHisto->GetYaxis()->SetLabelSize(RatioPlot::RATIOPLOT_YAXIS_LABEL_SIZE);
         ratioHisto->GetYaxis()->SetTickLength(RatioPlot::RATIOPLOT_YAXIS_TICK_LENGTH);
      }
      
      ratio->GetYaxis()->SetNdivisions(503);
      ratio->GetYaxis()->CenterTitle();
      ratio->GetYaxis()->SetTitle((*itr)->y_axis_label().c_str());
      if(m_ratio_order == DATA_AS_DENOMINATOR){
         ratio->SetMaximum(RatioPlot::RATIOPLOT_Y_MAX_B);
         ratio->SetMinimum(RatioPlot::RATIOPLOT_Y_MIN_B);
         ratioHisto->SetMaximum(RatioPlot::RATIOPLOT_Y_MAX_B);
         ratioHisto->SetMinimum(RatioPlot::RATIOPLOT_Y_MIN_B);
      }
      else if(m_ratio_order == DATA_AS_NUMERATOR){
         ratio->SetMaximum(RatioPlot::RATIOPLOT_Y_MAX_A);
         ratio->SetMinimum(RatioPlot::RATIOPLOT_Y_MIN_A);
         ratioHisto->SetMaximum(RatioPlot::RATIOPLOT_Y_MAX_A);
         ratioHisto->SetMinimum(RatioPlot::RATIOPLOT_Y_MIN_A);
      }
      
      ratioHisto->GetYaxis()->SetNdivisions(503);
      ratioHisto->GetYaxis()->CenterTitle();
      ratioHisto->GetYaxis()->SetTitle((*itr)->y_axis_label().c_str());
      
      // draw
      if(m_combine_generator_ratios){
         
         // only need to create axis once
         // only need to plot selfRatio once since it is shared
         // only need to plot the line once
         if(itr == m_generatorPlots.begin()){
            ratioHisto->GetYaxis()->SetTitle("MC/Data");
            ratioHisto->Draw("HIST");
            selfRatio->Draw("2");
            GetLine()->Draw("same");
         }
         ratioHisto->SetLineColor(ratio->GetLineColor());
         ratioHisto->SetLineStyle(ratio->GetLineStyle());
         ratioHisto->Draw("HIST same");
      }
      else{
         ratio->Draw("ap");
         selfRatio->Draw("2");
         // plot horizontal line at y=1
         GetLine()->Draw("same");
         
         ratio->Draw("p");
      }
      
   } // end for(generatorPlots)
   
   // loop for theory plots
   for(std::vector<PredictionPlot*>::iterator itr = m_theoryPlots.begin();
       itr != m_theoryPlots.end();++itr)
   {
      // if the plot should be made into a ratio
      if(!(*itr)->isPlotted()) continue;
      
      
      /////////////////
      // create ratio pad for the ratio of reference over this histogram
      
      // change focus to canvas where pad will be drawn
      m_canvas->cd();
      
      double pad_xlow = 0.,pad_xhigh = 1.;
      double pad_ylow = 0.,pad_yhigh = 0.;
      // if this is the last ratio pad we need to set the correct height
      if(numRatioPadsThatHaveBeenPlotted == numRatioPads - 1){
         pad_ylow  = 0.;
         pad_yhigh = last_ratio_pad_height;
      }
      else{
         const unsigned int reverseCount = numRatioPads - numRatioPadsThatHaveBeenPlotted - 1;
         pad_ylow  = last_ratio_pad_height + (reverseCount-1)*mid_ratio_pad_height;
         pad_yhigh = last_ratio_pad_height + reverseCount*mid_ratio_pad_height;
      }
      
      std::stringstream pad_name;
      pad_name << "ratiopad_" << m_pad_ratios.size();
      TPad* pad = new TPad(pad_name.str().c_str(),
                           pad_name.str().c_str(),pad_xlow,
                           pad_ylow,pad_xhigh,pad_yhigh
                          );
      m_pad_ratios.push_back(pad);
      
      if(numRatioPadsThatHaveBeenPlotted == numRatioPads - 1)
         pad->SetMargin(PAD_MARGIN_RIGHT,PAD_MARGIN_LEFT,RatioPlot::RATIOPLOT_X_AXIS_SIZE,0.);
      else
         pad->SetMargin(PAD_MARGIN_RIGHT,PAD_MARGIN_LEFT,0.,0.);
      
      // draw pad on canvas
      pad->Draw();
      
      // change focus to pad for plot
      pad->cd();
      pad->SetGridy(m_ratio_pad_gridy);
      
      numRatioPadsThatHaveBeenPlotted++;
      
      //////////////////////////////
      // plot ratio
      /////////////////////
      
      TGraphAsymmErrors* const ratio = (*itr)->ratio()->graph();
      TGraphAsymmErrors* const selfRatio = (m_ratio_order == DATA_AS_NUMERATOR) ? (*itr)->selfRatio()->graph() : m_dataPlot->selfRatio()->graph();
      
      // set x axis since the prediciton ratio is drawn first and sets the axies
      if(numRatioPadsThatHaveBeenPlotted == numRatioPads){
         ratio->GetXaxis()->SetTitle(m_x_axis_title.c_str());
         ratio->GetXaxis()->SetTitleOffset(RatioPlot::RATIOPLOT_XAXIS_TITLE_OFFSET);
         ratio->GetXaxis()->SetTitleSize(RatioPlot::RATIOPLOT_XAXIS_TITLE_SIZE);
         ratio->GetXaxis()->SetLabelSize(RatioPlot::RATIOPLOT_XAXIS_LABEL_SIZE);
         ratio->GetXaxis()->SetTickLength(RatioPlot::RATIOPLOT_XAXIS_TICK_LENGTH);
      }
      else ratio->GetXaxis()->SetTitle("");
      
      ratio->GetXaxis()->SetLimits(main_x_min,main_x_max);
      
      // set y axis
      const double ratio_scaleFactor = (numRatioPads > 1) ? mid_ratio_pad_height/last_ratio_pad_height : 0.85;
      if(numRatioPadsThatHaveBeenPlotted == numRatioPads){
         ratio->GetYaxis()->SetTitleOffset(RatioPlot::RATIOPLOT_YAXIS_TITLE_OFFSET/ratio_scaleFactor);
         ratio->GetYaxis()->SetTitleSize(RatioPlot::RATIOPLOT_YAXIS_TITLE_SIZE*ratio_scaleFactor);
         ratio->GetYaxis()->SetLabelSize(RatioPlot::RATIOPLOT_YAXIS_LABEL_SIZE*ratio_scaleFactor);
         ratio->GetYaxis()->SetTickLength(RatioPlot::RATIOPLOT_YAXIS_TICK_LENGTH/ratio_scaleFactor);
      }
      else{
         ratio->GetYaxis()->SetTitleOffset(RatioPlot::RATIOPLOT_YAXIS_TITLE_OFFSET);
         ratio->GetYaxis()->SetTitleSize(RatioPlot::RATIOPLOT_YAXIS_TITLE_SIZE);
         ratio->GetYaxis()->SetLabelSize(RatioPlot::RATIOPLOT_YAXIS_LABEL_SIZE);
         ratio->GetYaxis()->SetTickLength(RatioPlot::RATIOPLOT_YAXIS_TICK_LENGTH);
      }
      
      ratio->GetYaxis()->SetNdivisions(503);
      ratio->GetYaxis()->CenterTitle();
      ratio->GetYaxis()->SetTitle((*itr)->y_axis_label().c_str());
      if(m_ratio_order == DATA_AS_DENOMINATOR){
         ratio->SetMaximum(RatioPlot::RATIOPLOT_Y_MAX_B);
         ratio->SetMinimum(RatioPlot::RATIOPLOT_Y_MIN_B);
      }
      else if(m_ratio_order == DATA_AS_NUMERATOR){
         ratio->SetMaximum(RatioPlot::RATIOPLOT_Y_MAX_A);
         ratio->SetMinimum(RatioPlot::RATIOPLOT_Y_MIN_A);
      }
      
      
      // draw
      ratio->Draw("ap");
      selfRatio->Draw("2");
      // plot horizontal line at y=1
      GetLine()->Draw("same");
      
      ratio->Draw("p");
      
   } // end for(theory)
   
   
   
   // change focus to main pad
   m_pad_top->cd();
   // draw legend
   m_dataPlot->graph()->Draw("p same");
   m_legend->Draw("same");
   
   // Add ATLAS Text
   PlotLabel::DrawAtlasInternal(m_atlas_label_x,m_atlas_label_y);
   PlotLabel::DrawLumi(m_atlas_lumi_x,m_atlas_lumi_y);
   
   m_updateCanvas = false;
   m_redrawCanvas = false;
}

void Th1PlotAndRatio::SaveAs(const std::string filename){
   
   // make sure canvas has been created
   if(m_updateCanvas)
      CreateCanvas();
   
   if(m_redrawCanvas){
      RedrawCanvas();
      m_redrawCanvas = false;
   }
   
   // Save canvas
   m_canvas->SaveAs(filename.c_str());
   
}

void Th1PlotAndRatio::RedrawCanvas(){
   
   
   if(m_canvas){
      m_canvas->cd();
      
      // Draw top pad first
      m_pad_top->cd();
      m_pad_top->SetLogy(m_main_pad_logy);
      
      // Get X/Y Axis limits based on the data given
      double main_y_max = 0.,main_y_min = 0.,main_x_max = 0.,main_x_min = 0.;
      GetAxisLimits(main_x_min,main_x_max,main_y_min,main_y_max);
      // Draw data plot
      if(m_y_axis_max != -999.)
         m_dataPlot->graph()->SetMaximum(m_y_axis_max);
      else
         m_dataPlot->graph()->SetMaximum(main_y_max*1.2);
      if(m_y_axis_min != 999.)
         m_dataPlot->graph()->SetMinimum(m_y_axis_min);
      else
         m_dataPlot->graph()->SetMinimum(main_y_min);
      m_dataPlot->graph()->GetXaxis()->SetLimits( (main_x_min > 0 ? 0 : main_x_min), main_x_max);
      
      m_dataPlot->graph()->Draw("ap");
      
      // first loop for generator plots
      for(std::vector<PredictionPlot*>::iterator itr = m_generatorPlots.begin();
          itr != m_generatorPlots.end();++itr)
      {
         (*itr)->histo()->Draw("same HIST");
      }
      // then loop for theory plots
      for(std::vector<PredictionPlot*>::iterator itr = m_theoryPlots.begin();
          itr != m_theoryPlots.end();++itr)
      {
         (*itr)->histo()->Draw("same HIST");
      }
      
      m_dataPlot->graph()->Draw("p same");
      m_legend->Draw("same");
      
      // Add ATLAS Text
      PlotLabel::DrawAtlasInternal(m_atlas_label_x,m_atlas_label_y);
      PlotLabel::DrawLumi(m_atlas_lumi_x,m_atlas_lumi_y);
      
      // Draw ratio pads
      std::vector<PredictionPlot*>::iterator gen_itr = m_generatorPlots.begin();
      std::vector<PredictionPlot*>::iterator theory_itr = m_theoryPlots.begin();
      for(unsigned int i=0;i<m_pad_ratios.size();++i){
         
         m_pad_ratios[i]->cd();
         
         // first loop for generator plots
         bool continueLoop = false;
         for(;gen_itr != m_generatorPlots.end();)
         {
            if(!(*gen_itr)->isPlotted()){
               ++gen_itr;
               continue;
            }
            
            TGraphAsymmErrors* const ratio      = (*gen_itr)->ratio()->graph();
            TH1* const               ratioHisto = (*gen_itr)->ratio()->histo();
            TGraphAsymmErrors* const selfRatio = (m_ratio_order == DATA_AS_NUMERATOR) ? (*gen_itr)->selfRatio()->graph() : m_dataPlot->selfRatio()->graph();
            
            if(m_combine_generator_ratios){
               // only need to create axis once
               // only need to plot selfRatio once since it is shared
               // only need to plot the line once
               if(gen_itr == m_generatorPlots.begin()){
                  ratioHisto->Draw("HIST");
                  selfRatio->Draw("2");
                  GetLine()->Draw("same");
               }
               ratioHisto->Draw("HIST same");
               ++gen_itr;
               continueLoop = true;
               continue;
            }
            else{
               ratio->Draw("ap");
               selfRatio->Draw("2");
               GetLine()->Draw("same");
               ratio->Draw("p");
               ++gen_itr;
               continueLoop = true;
               break;
            }
         }
         if(continueLoop) continue;
         // loop for theory plots
         for(;theory_itr != m_theoryPlots.end();)
         {
            if(!(*theory_itr)->isPlotted()){ 
               ++theory_itr;
               continue;
            }
            (*theory_itr)->ratio()->graph()->Draw("ap");
            if(m_ratio_order == DATA_AS_NUMERATOR)
               (*theory_itr)->selfRatio()->graph()->Draw("2");
            else if(m_ratio_order == DATA_AS_DENOMINATOR)
               m_dataPlot->selfRatio()->graph()->Draw("2");
            GetLine()->Draw("same");
            (*theory_itr)->ratio()->graph()->Draw("p");
            ++theory_itr;
            break;
         }
      }
      
   }
   
}

void Th1PlotAndRatio::GetAxisLimits(double& x_min,double& x_max,double& y_min,double& y_max){
   
   x_min = 1e14;
   x_max = 1e-14;
   y_min = 1e14;
   y_max = 1e-14;
   double lowest_dataPoint = 1e14;
   
   GetAxisLimits(m_dataPlot->graph(),x_min,x_max,y_min,y_max,lowest_dataPoint);
   
   for(std::vector<PredictionPlot*>::iterator itr = m_generatorPlots.begin();
       itr != m_generatorPlots.end();++itr)
   {
      if((*itr)->isPlotted())
         GetAxisLimits((TGraph*)(*itr)->graph(),x_min,x_max,y_min,y_max,lowest_dataPoint);
   }
   
   for(std::vector<PredictionPlot*>::iterator itr = m_theoryPlots.begin();
       itr != m_theoryPlots.end();++itr)
   {
      if((*itr)->isPlotted())
         GetAxisLimits((TGraph*)(*itr)->graph(),x_min,x_max,y_min,y_max,lowest_dataPoint);
   }
   
   
   if(y_min < 0) y_min = lowest_dataPoint*0.6; // if error bar goes below zero
   else{
      if(m_main_pad_logy){
         y_min *=0.8; // add a little space below error bar
         y_max *=1.2; // add a little space above error bar
      }
      else
         y_min = 0; // set minimum to zero for non-log plots
   }
}

void Th1PlotAndRatio::GetAxisLimits(TGraph const * const graph,double& x_min,double& x_max,double& y_min,double& y_max,double& lowest_dataPoint) const{
   
   for(unsigned int point=0;point<graph->GetN();++point){
      double x_high = graph->GetX()[point] + graph->GetEXhigh()[point];
      double x_low  = graph->GetX()[point] - graph->GetEXlow()[point];
      if(x_high > x_max)
         x_max = x_high;
      if(x_low < x_min)
         x_min = x_low;
      double y_high = graph->GetY()[point] + graph->GetEYhigh()[point];
      double y_low  = graph->GetY()[point] - graph->GetEYlow()[point];
      if(y_high > y_max)
         y_max = y_high;
      if(y_low < y_min)
         y_min = y_low;
      
      if(graph->GetY()[point] < lowest_dataPoint)
         lowest_dataPoint = graph->GetY()[point];
   }
   
}

unsigned int Th1PlotAndRatio::GetNumRatioPlots(){
   unsigned int num_plots = 0;
   
   for(std::vector<PredictionPlot*>::iterator itr = m_generatorPlots.begin();
       itr != m_generatorPlots.end();++itr)
   {
      if((*itr)->isPlotted()){
         num_plots++;
      }
   }
   
   for(std::vector<PredictionPlot*>::iterator itr = m_theoryPlots.begin();
       itr != m_theoryPlots.end();++itr)
   {
      if((*itr)->isPlotted()){
         num_plots++;
      }
   }
   
   return num_plots;
}

unsigned int Th1PlotAndRatio::GetNumRatioPads(){
   unsigned int num_pads = 0;
   
   for(std::vector<PredictionPlot*>::iterator itr = m_generatorPlots.begin();
       itr != m_generatorPlots.end();++itr)
   {
      if((*itr)->isPlotted()){
         num_pads++;
      }
   }
   // if combining ratios for generators, then only need one pad for them
   if(num_pads > 0 && m_combine_generator_ratios)
      num_pads = 1;
   
   for(std::vector<PredictionPlot*>::iterator itr = m_theoryPlots.begin();
       itr != m_theoryPlots.end();++itr)
   {
      if((*itr)->isPlotted()){
         num_pads++;
      }
   }
   
   return num_pads;
}


void Th1PlotAndRatio::SetupCanvas(const double canvas_height,const double numRatioPads){
   
   // The canvas should be setup to contain a main pad on top
   // which is MAINPLOT_WIDTH by MAINPLOT_HEIGHT, then followed by
   // some number of ratio pads, each size MAINPLOT_WIDTH by
   // RATIOPLOT_HEIGHT
   
   // if canvas exists delete it
   if(m_canvas){
      delete m_canvas; m_canvas = 0;
   }
   
   // create canvas
   // canvas height is calculated to include the main pad and all ratio pads
   m_canvas = new TCanvas("canvas","canvas",0,0,MAINPLOT_WIDTH,MAINPLOT_HEIGHT+RATIOPLOT_HEIGHT*numRatioPads);
   m_canvas->SetMargin(0.,0.,0.,0.);
   m_canvas->cd();
   
   // calculate main pad diimensions
   const double mainPad_ylow  = (canvas_height - MAINPLOT_HEIGHT)/canvas_height;
   const double mainPad_yhigh = 1.;
   
   // pad margins
   const double pad_margin_bottom = numRatioPads > 0 ? 0. : 0.2;
   
   // if pad exists delete it
   if(m_pad_top){
      delete m_pad_top; m_pad_top = 0;
   }
   
   m_pad_top = new TPad("main","main",0.,mainPad_ylow,1.,mainPad_yhigh);
   m_pad_top->SetMargin(PAD_MARGIN_RIGHT,PAD_MARGIN_LEFT,0.,0.05);
   m_pad_top->Draw();
   m_pad_top->cd();
   m_pad_top->SetLogy(m_main_pad_logy);
   
}

TF1* Th1PlotAndRatio::GetLine(){
   
   if(!m_horizontalLine){
      m_horizontalLine = new TF1("horizontal","pol1",0,10000);
      m_horizontalLine->SetParameter(0,1.);
      m_horizontalLine->SetParameter(1,0.);
      m_horizontalLine->SetLineColor(kBlack);
      m_horizontalLine->SetLineStyle(2);
      m_horizontalLine->SetLineWidth(1);
   }
   
   return m_horizontalLine;
}

// uses relative errors only
TGraphAsymmErrors* Th1PlotAndRatio::DivideAsymmErrors(TGraphAsymmErrors const * const numerator,
                                               TGraphAsymmErrors const * const denominator)
{
   
   if(numerator->GetN() != denominator->GetN()){
      std::cerr << "[Th1PlotAndRatio::DivideAsymmErrors] plots do not have the same number of data points, " << numerator->GetName() << " has " << numerator->GetN() << " != " << numerator->GetName() << " has " << denominator->GetN() << ".\n";
      return (TGraphAsymmErrors*)0;
   }
   
   TGraphAsymmErrors* output = new TGraphAsymmErrors;
   std::string gname = "ratio_";
   gname += numerator->GetName();
   gname += "_";
   gname += denominator->GetName();
   output->SetName(gname.c_str());
   
   for(unsigned int i=0;i<numerator->GetN();++i){
      double num     = numerator->GetY()[i];
      double numUp   = numerator->GetEYhigh()[i];
      double numDown = numerator->GetEYlow()[i];
      
      double den     = denominator->GetY()[i];
      double denUp   = denominator->GetEYhigh()[i];
      double denDown = denominator->GetEYlow()[i];
      
      double ratio   = (den == 0) ? 0 : ( num     / den );
      double up      = (den == 0) ? 0 : ( numDown / den );
      double down    = (den == 0) ? 0 : ( numUp   / den );
      
      output->SetPoint(i,numerator->GetX()[i],ratio);
      output->SetPointError(i,numerator->GetEXlow()[i],numerator->GetEXhigh()[i],down,up);
      
   }
   
   return output;
}

/////////////////////
/// PREDICTION PLOT
/////////////////////

Th1PlotAndRatio::PredictionPlot::PredictionPlot(TH1* const histo,
      const std::string legendLabel,
      DataPlot const * const dataPlot,
      const RATIO_ORDER ratio_order,
      const std::string yaxisLabel,
      const bool includeRatio,
      const std::string entryOption)
{
   m_graph        = new TGraphAsymmErrors(histo);
   m_graph->SetLineStyle(histo->GetLineStyle());
   m_graph->SetLineColor(histo->GetLineColor());
   m_graph->SetMarkerStyle(histo->GetMarkerStyle());
   m_graph->SetMarkerColor(histo->GetMarkerColor());
   m_graph->SetFillStyle(histo->GetFillStyle());
   m_graph->SetFillColor(histo->GetFillColor());
   
   m_legend_label = legendLabel;
   m_legend_histo = 0;
   m_y_axis_label = yaxisLabel;
   m_plot_ratio   = includeRatio;
   m_legend_draw_options = entryOption;
   
   std::string hname = histo->GetName();
   hname += "_clone";
   m_histo        = (TH1*)histo->Clone(hname.c_str());
   m_histo->AddDirectory(0);
   
   if(m_plot_ratio){
      if(ratio_order == Th1PlotAndRatio::DATA_AS_NUMERATOR){
         m_ratio = new RatioPlot(dataPlot->graph(),m_graph);
      }
      else if(ratio_order == Th1PlotAndRatio::DATA_AS_DENOMINATOR){
         m_ratio = new RatioPlot(m_graph,dataPlot->graph());
      }
      
      m_ratio->graph()->SetLineStyle(0);
      m_ratio->graph()->SetLineColor(kBlack);
      m_ratio->graph()->SetMarkerStyle(histo->GetMarkerStyle());
      m_ratio->graph()->SetMarkerColor(kBlack);
      m_ratio->graph()->SetFillStyle(histo->GetFillStyle());
      m_ratio->graph()->SetFillColor(histo->GetFillColor());
      
      if(ratio_order == Th1PlotAndRatio::DATA_AS_DENOMINATOR){
         m_ratio->graph()->SetLineColor(histo->GetLineColor());
         m_ratio->graph()->SetMarkerColor(histo->GetMarkerColor());
         m_ratio->graph()->SetLineStyle(histo->GetLineStyle());
      }
      
      m_selfRatio = new RatioPlot(m_graph,m_graph);
      m_selfRatio->graph()->SetLineStyle(histo->GetLineStyle());
      m_selfRatio->graph()->SetLineColor(histo->GetLineColor());
      m_selfRatio->graph()->SetMarkerStyle(histo->GetMarkerStyle());
      m_selfRatio->graph()->SetMarkerColor(histo->GetMarkerColor());
      m_selfRatio->graph()->SetFillStyle(histo->GetFillStyle());
      m_selfRatio->graph()->SetFillColor(histo->GetFillColor());
   }
}

Th1PlotAndRatio::PredictionPlot::PredictionPlot(TGraphAsymmErrors* const graph,
      const std::string legendLabel,
      DataPlot const * const dataPlot,
      const RATIO_ORDER ratio_order,
      TH1* const leg_hist,
      const std::string yaxisLabel,
      const bool includeRatio,
      const std::string entryOption)
{
   std::string gname = graph->GetName();
   gname += "_clone";
   m_graph        = (TGraphAsymmErrors*)graph->Clone(gname.c_str());
   m_legend_label = legendLabel;
   m_legend_histo = leg_hist;
   m_y_axis_label = yaxisLabel;
   m_plot_ratio   = includeRatio;
   m_legend_draw_options = entryOption;
   
   m_histo        = GetHistoForPlotting(m_graph);
   
   if(m_plot_ratio){
      if(ratio_order == Th1PlotAndRatio::DATA_AS_NUMERATOR)
         m_ratio = new RatioPlot(dataPlot->graph(),m_graph);
      else if(ratio_order == Th1PlotAndRatio::DATA_AS_DENOMINATOR)
         m_ratio = new RatioPlot(m_graph,dataPlot->graph());
      
      m_ratio->graph()->SetLineStyle(0);
      m_ratio->graph()->SetLineColor(kBlack);
      m_ratio->graph()->SetMarkerStyle(m_graph->GetMarkerStyle());
      m_ratio->graph()->SetMarkerColor(kBlack);
      m_ratio->graph()->SetFillStyle(m_graph->GetFillStyle());
      m_ratio->graph()->SetFillColor(m_graph->GetFillColor());
      
      if(ratio_order == Th1PlotAndRatio::DATA_AS_DENOMINATOR){
         m_ratio->graph()->SetLineColor(m_graph->GetLineColor());
         m_ratio->graph()->SetMarkerColor(m_graph->GetMarkerColor());
         m_ratio->graph()->SetLineStyle(m_graph->GetLineStyle());
      }
      
      m_selfRatio = new RatioPlot(m_graph,m_graph);
      m_selfRatio->graph()->SetLineStyle(m_graph->GetLineStyle());
      m_selfRatio->graph()->SetLineColor(m_graph->GetLineColor());
      m_selfRatio->graph()->SetMarkerStyle(m_graph->GetMarkerStyle());
      m_selfRatio->graph()->SetMarkerColor(m_graph->GetMarkerColor());
      m_selfRatio->graph()->SetFillStyle(m_graph->GetFillStyle());
      m_selfRatio->graph()->SetFillColor(m_graph->GetFillColor());
   }
}


Th1PlotAndRatio::PredictionPlot::~PredictionPlot(){
   
   if(m_graph){
      delete m_graph; m_graph = 0;
   }
   
   if(m_histo){
      delete m_histo; m_histo = 0;
   }
   
   if(m_ratio){
      delete m_ratio; m_ratio = 0;
   }
   
   if(m_selfRatio){
      delete m_selfRatio; m_selfRatio = 0;
   }
   
}


TH1D* Th1PlotAndRatio::PredictionPlot::GetHistoForPlotting(TGraphAsymmErrors const * const graph){
   std::string name = graph->GetName();
   name = "GetHistoForPlotting_" + name;
   
   std::vector<double> x_low_edge;
   for(unsigned int i = 0;i<graph->GetN();++i){
      x_low_edge.push_back(graph->GetX()[i]-graph->GetEXlow()[i]);
      if(i == graph->GetN()-1)
         x_low_edge.push_back(graph->GetX()[i]+graph->GetEXhigh()[i]);
   }
   
   TH1D* output = 0;
   if(x_low_edge.size() > 0){
      output = new TH1D(name.c_str(),"",graph->GetN(),&x_low_edge[0]);
      
      for(unsigned int bin = 1;bin<=output->GetNbinsX();++bin)
         output->SetBinContent(bin,graph->GetY()[bin-1]);
   }
   
   output->SetMarkerStyle(graph->GetMarkerStyle());
   output->SetMarkerColor(graph->GetMarkerColor());
   output->SetLineStyle(graph->GetLineStyle());
   output->SetLineWidth(graph->GetLineWidth());
   output->SetLineColor(graph->GetLineColor());
   output->SetFillColor(graph->GetFillColor());
   output->SetFillStyle(graph->GetFillStyle());
   
   output->SetFillStyle(0);
   output->SetDirectory(0);
   
   return output;
}

void Th1PlotAndRatio::PredictionPlot::SetColors(const bool combined){
   if(combined){
      
   }
   else{
      
   }
}

/////////////////////
/// DATA PLOT
/////////////////////


Th1PlotAndRatio::DataPlot::DataPlot(TH1* const histo,const std::string legendLabel){
   m_graph        = new TGraphAsymmErrors(histo);
   m_legend_label = legendLabel;
   m_legend_histo = histo;
   m_histo        = Th1PlotAndRatio::PredictionPlot::GetHistoForPlotting(m_graph);
   m_selfRatio    = new RatioPlot(m_graph,m_graph);
   m_selfRatio->graph()->SetLineStyle(histo->GetLineStyle());
   m_selfRatio->graph()->SetLineColor(histo->GetLineColor());
   m_selfRatio->graph()->SetMarkerStyle(histo->GetMarkerStyle());
   m_selfRatio->graph()->SetMarkerColor(histo->GetMarkerColor());
   m_selfRatio->graph()->SetFillStyle(histo->GetFillStyle());
   m_selfRatio->graph()->SetFillColor(histo->GetFillColor());
}

Th1PlotAndRatio::DataPlot::DataPlot(TGraphAsymmErrors* const graph,
                                    const std::string legendLabel,
                                    TH1* const leg_hist)
{
   std::string gname = graph->GetName();
   gname += "_clone";
   m_graph        = (TGraphAsymmErrors*)graph->Clone(gname.c_str());
   m_legend_label = legendLabel;
   m_legend_histo = leg_hist;
   m_histo        = Th1PlotAndRatio::PredictionPlot::GetHistoForPlotting(m_graph);
   m_selfRatio    = new RatioPlot(m_graph,m_graph);
   m_selfRatio->graph()->SetLineStyle(graph->GetLineStyle());
   m_selfRatio->graph()->SetLineColor(graph->GetLineColor());
   m_selfRatio->graph()->SetMarkerStyle(graph->GetMarkerStyle());
   m_selfRatio->graph()->SetMarkerColor(graph->GetMarkerColor());
   m_selfRatio->graph()->SetFillStyle(graph->GetFillStyle());
   m_selfRatio->graph()->SetFillColor(graph->GetFillColor());
}

Th1PlotAndRatio::DataPlot::~DataPlot(){
   if(m_graph){
      delete m_graph;m_graph = 0;
   }
   if(m_selfRatio){
      delete m_selfRatio;m_selfRatio = 0;
   }
   if(m_histo){
      delete m_histo; m_histo = 0;
   }
}


/////////////////////
/// RATIO PLOT
/////////////////////

Th1PlotAndRatio::RatioPlot::RatioPlot(TGraphAsymmErrors const * const numerator,
                                      TGraphAsymmErrors const * const denominator
                                     )
{
   m_graph = Th1PlotAndRatio::DivideAsymmErrors(numerator,denominator);
   m_histo = Th1PlotAndRatio::PredictionPlot::GetHistoForPlotting(m_graph);
}

Th1PlotAndRatio::RatioPlot::~RatioPlot(){
   if(m_graph){
      delete m_graph;m_graph = 0;
   }
   if(m_histo){
      delete m_histo;m_histo = 0;
   }
}
