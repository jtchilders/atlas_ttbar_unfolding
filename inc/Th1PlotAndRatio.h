#ifndef TH1PLOTANDRATIO_H
#define TH1PLOTANDRATIO_H

#include <iostream>
#include <vector>
#include <sstream>


#include "TH1.h"
#include "TH1D.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TF1.h"

class Th1PlotAndRatio{
public:
   enum RATIO_ORDER{DATA_AS_NUMERATOR,DATA_AS_DENOMINATOR,NUM_RATIO_ORDER};
   /// constructor
   // legendLabel is the text passed to the TLegend constructor
   Th1PlotAndRatio(const std::string legendLabel = "",RATIO_ORDER ratio_order = DATA_AS_NUMERATOR);
   ~Th1PlotAndRatio(void);
   // delete canvas and pads and ratio plots
   // the only thing left is what the user provided
   void Clear(); 
   
   class DataPlot;
   class GeneratorPlot;
   class TheoryPlot;
   class RatioPlot;
   class PredictionPlot;
   
   // Data plot is what will be used in all the ratio plots 
   // You can add the data as a TH1 or a TGraphAsymmErrors, but not both.
   // histo is the histogram
   // legendLabel is the text which will appear in the legend for this histogram
   void SetData(TH1* const histo,const std::string legendLabel){
      m_dataPlot = new DataPlot(histo,legendLabel);
   }
   
   // graph is the graph to be plotted
   // legendLabel is the text which will appear in the legend for this graph
   // leg_hist is a histogram whose style is that which you would like to use in the legend instead of the graph
   //          if this is not included (pointer is 0) then the graph if used to set the legend style
   void SetData(TGraphAsymmErrors* const graph,const std::string legendLabel,TH1* const leg_hist = 0){
      m_dataPlot = new DataPlot(graph,legendLabel,leg_hist);
   }
   
   
   // Generator plots appear on the main plot with the data and the ratio
   // of the data and generator will also be plotted below the main plot.
   // You can add the data as a TH1 or a TGraphAsymmErrors, but not both.
   // histo is the histogram
   // legendLabel is the text which will appear in the legend for this histogram
   // yaxisLabel is the y-axis label to use, if none "Data/MC" is used
   // includeRatio: if set to true the ratio (reference/graph) is plotted, if false, this graph
   //               only shows up in the top plot
   // entryOption: this is the option passed to TLegend::AddEntry which determines
   //              what is graphically presented, i.e. "lpe" is line/point/error
   void AddGenerator(TH1* const histo,const std::string legendLabel,
                         const std::string yaxisLabel = "", const bool includeRatio = true,
                         const std::string entryOption = "lpe")
   {
      m_generatorPlots.push_back(new PredictionPlot(histo,legendLabel,
                     m_dataPlot,m_ratio_order,
                     yaxisLabel,includeRatio,entryOption)
                                );
   }
   
   // graph is the graph to be plotted
   // legendLabel is the text which will appear in the legend for this graph
   // leg_hist is a histogram whose style is that which you would like to use in the legend instead of the graph
   //          if this is not included (pointer is 0) then the graph if used to set the legend style
   // yaxisLabel is the y-axis label to use, if none "Data/MC" is used
   // includeRatio: if set to true the ratio (reference/graph) is plotted, if false, this graph
   //               only shows up in the top plot
   // entryOption: this is the option passed to TLegend::AddEntry which determines
   //              what is graphically presented, i.e. "lpe" is line/point/error
   void AddGenerator(TGraphAsymmErrors* const graph,const std::string legendLabel,
                     const std::string yaxisLabel = "",TH1* const leg_hist = 0,
                     const bool includeRatio = true,
                     const std::string entryOption = "lpe")
   {
      m_generatorPlots.push_back(new PredictionPlot(graph,legendLabel,
                                 m_dataPlot,m_ratio_order,
                                 leg_hist,yaxisLabel,includeRatio,entryOption)
                                );
   }
   
   // Theory plots appear on the main plot with the data and the ratio
   // of the data and theory will also be plotted below the main plot.
   // You can add the data as a TH1 or a TGraphAsymmErrors, but not both.
   // histo is the histogram
   // legendLabel is the text which will appear in the legend for this histogram
   // yaxisLabel is the y-axis label to use, if none "Data/MC" is used
   // includeRatio: if set to true the ratio (reference/graph) is plotted, if false, this graph
   //               only shows up in the top plot
   // entryOption: this is the option passed to TLegend::AddEntry which determines
   //              what is graphically presented, i.e. "lpe" is line/point/error
   void AddTheory(TH1* const histo,const std::string legendLabel,
                      const std::string yaxisLabel = "", const bool includeRatio = true,
                      const std::string entryOption = "lpe")
   {
      m_theoryPlots.push_back(new PredictionPlot(histo,legendLabel,
                              m_dataPlot,m_ratio_order,
                              yaxisLabel,includeRatio,entryOption)
                             );
   }
   
   // graph is the graph to be plotted
   // legendLabel is the text which will appear in the legend for this graph
   // leg_hist is a histogram whose style is that which you would like to use in the legend instead of the graph
   //          if this is not included (pointer is 0) then the graph if used to set the legend style
   // yaxisLabel is the y-axis label to use, if none "Data/MC" is used
   // includeRatio: if set to true the ratio (reference/graph) is plotted, if false, this graph
   //               only shows up in the top plot
   // entryOption: this is the option passed to TLegend::AddEntry which determines
   //              what is graphically presented, i.e. "lpe" is line/point/error
   void AddTheory(TGraphAsymmErrors* const graph,const std::string legendLabel,
                  const std::string yaxisLabel = "",TH1* const leg_hist = 0,
                  const bool includeRatio = true,const std::string entryOption = "lpe")
   {
      m_theoryPlots.push_back(new PredictionPlot(graph,legendLabel,
                              m_dataPlot,m_ratio_order,
                              leg_hist,yaxisLabel,includeRatio,entryOption)
                             );
   }
   
   // For the moment, the only way to get the plot is to save it as a file, so pass a filename
   // following the TCanvas::SaveAs() naming conventions.
   void SaveAs(const std::string filename);
   
   void SetLegendLocation(const double x1,const double y1, const double x2, const double y2){
      m_leg_x1=x1;m_leg_y1=y1;m_leg_x2=x2;m_leg_y2=y2;
   };
   
   void SetRatioPadGridy(const bool value = true){m_ratio_pad_gridy = value;};
   void SetMainPadLogy(const bool value = true){
      m_main_pad_logy = value;
      if(m_pad_top) m_pad_top->SetLogy(value);
      m_redrawCanvas = true;
   };
   void SetXAxisTitle(const std::string title){m_x_axis_title = title;};
   void SetYAxisTitleTop(const std::string title){m_y_axis_title_top = title;};
   void SetYAxisTitleRatio(const std::string title){m_y_axis_title_ratio = title;};
   void SetYAxisMax(const double max){m_y_axis_max = max;};
   void SetYAxisMin(const double min){m_y_axis_min = min;};
   
   void SetAtlasLabelPosition(const double x,const double y){m_atlas_label_x=x;m_atlas_label_y=y;};
   void SetAtlasLumiPosition(const double x,const double y){m_atlas_lumi_x=x;m_atlas_lumi_y=y;};
   
   void SetCombineGeneratorRatios(const bool value = true){m_combine_generator_ratios = value;};
   
   static TGraphAsymmErrors* DivideAsymmErrors(TGraphAsymmErrors const * const numerator,
                                               TGraphAsymmErrors const * const denominator);
   
private:
   std::string                m_leg_label;
   RATIO_ORDER                m_ratio_order;
   bool                       m_combine_generator_ratios;
   
   bool                       m_main_pad_logy;
   bool                       m_ratio_pad_gridy;
   std::string                m_x_axis_title;
   std::string                m_y_axis_title_top;
   std::string                m_y_axis_title_ratio;
   double                     m_y_axis_max;
   static const double        Y_AXIS_MAX_NULL;
   double                     m_y_axis_min;
   static const double        Y_AXIS_MIN_NULL;
   
   DataPlot*                  m_dataPlot;
   std::vector<PredictionPlot*> m_generatorPlots;
   std::vector<PredictionPlot*> m_theoryPlots;
   
   TCanvas*                   m_canvas;
   TPad*                      m_pad_top;
   std::vector<TPad*>         m_pad_ratios;
   TF1*                       m_horizontalLine;
   
   TLegend*                   m_legend;
   double                     m_leg_x1;
   double                     m_leg_y1;
   double                     m_leg_x2;
   double                     m_leg_y2;
   
   double                     m_atlas_label_x;
   double                     m_atlas_label_y;
   double                     m_atlas_lumi_x;
   double                     m_atlas_lumi_y;
   
   void CreateRatios(void);
   
   static const unsigned int MAINPLOT_WIDTH  = 800;
   static const unsigned int MAINPLOT_HEIGHT = 500;
   static const unsigned int RATIOPLOT_HEIGHT = 125;
   
   static const double PAD_MARGIN_LEFT;
   static const double PAD_MARGIN_RIGHT;
   
   void GetAxisLimits(double& x_min,double& x_max,double& y_min,double& y_max);
   void GetAxisLimits(TGraph const * const graph,double& x_min,double& x_max,double& y_min,double& y_max,double& lowest_dataPoint) const;
   
   unsigned int GetNumRatioPlots();
   unsigned int GetNumRatioPads();
   
   void DivideAsymmErrors(TGraphAsymmErrors* numerator,TGraphAsymmErrors* denominator,TGraphAsymmErrors* prediction,TGraphAsymmErrors* data);
   //TGraphAsymmErrors* DivideAsymmErrors(TGraphAsymmErrors* numerator,TH1* denominator);
   
   bool m_updateCanvas;
   bool m_redrawCanvas;
   void CreateCanvas(void);
   void RedrawCanvas(void);
   void SetupCanvas(const double canvas_height,const double numRatioPads);
   TF1* GetLine();
   
public:

   class DataPlot{
   public:
      DataPlot(TH1* const histo,const std::string legendLabel);
      DataPlot(TGraphAsymmErrors* const graph,
               const std::string legendLabel,
               TH1* const leg_hist = 0);
      ~DataPlot();
      
      TGraphAsymmErrors* graph() const {return m_graph;};
      TH1* legend_histo() const {return m_legend_histo;};
      std::string legend_label() const {return m_legend_label;};
      RatioPlot* selfRatio() const {return m_selfRatio;};
   private:
      TGraphAsymmErrors*   m_graph;
      TH1*                 m_legend_histo;
      std::string          m_legend_label;
      bool                 m_delete_graph;
      RatioPlot*           m_selfRatio;
      TH1*                 m_histo;
   };
   
   class RatioPlot{
   public:
      RatioPlot(TGraphAsymmErrors const * const numerator,
                TGraphAsymmErrors const * const denominator);
      ~RatioPlot();
      
      TGraphAsymmErrors* graph(){return m_graph;};
      TH1*               histo(){return m_histo;};
      
      
      static const double RATIOPLOT_Y_MAX_A;
      static const double RATIOPLOT_Y_MIN_A;
      static const double RATIOPLOT_Y_MAX_B;
      static const double RATIOPLOT_Y_MIN_B;
      static const unsigned int RATIOPLOT_Y_NDIVISIONS = 3;
      static const double RATIOPLOT_X_AXIS_SIZE;
      
      static const double RATIOPLOT_YAXIS_TITLE_OFFSET;
      static const double RATIOPLOT_YAXIS_TITLE_SIZE;
      static const double RATIOPLOT_YAXIS_TICK_LENGTH;
      static const double RATIOPLOT_YAXIS_LABEL_SIZE;
      
      static const double RATIOPLOT_XAXIS_TITLE_OFFSET;
      static const double RATIOPLOT_XAXIS_TITLE_SIZE;
      static const double RATIOPLOT_XAXIS_TICK_LENGTH;
      static const double RATIOPLOT_XAXIS_LABEL_SIZE;
   private:
      TGraphAsymmErrors*   m_graph;
      TH1*                 m_histo;
   };
   
   
   class PredictionPlot{
   public:
      PredictionPlot(TH1* const histo,const std::string legendLabel,
                     DataPlot const * const dataPlot,
                     const RATIO_ORDER ratio_order,
                     const std::string yaxisLabel = "",
                     const bool includeRatio = true,
                     const std::string entryOption = "lpe");
      PredictionPlot(TGraphAsymmErrors* const graph,
                     const std::string legendLabel,
                     DataPlot const * const dataPlot,
                     const RATIO_ORDER ratio_order,
                     TH1* const leg_hist = 0,
                     const std::string yaxisLabel = "",
                     const bool includeRatio = true,
                     const std::string entryOption = "lpe");
      
      ~PredictionPlot();
      
      TGraphAsymmErrors* graph() const{return m_graph;};
      RatioPlot* ratio() const {return m_ratio;};
      RatioPlot* selfRatio() const {return m_selfRatio;};
      bool isPlotted() const {return m_plot_ratio;};
      TH1* histo()const {return m_histo;};
      
      TH1* legend_histo() const {return m_legend_histo;};
      std::string legend_draw_options() const {return m_legend_draw_options;};
      std::string legend_label() const {return m_legend_label;};
      
      std::string y_axis_label(){return m_y_axis_label;};
      
      static TH1D* GetHistoForPlotting(TGraphAsymmErrors const * const graph);
      
      void SetColors(const bool combined);
   private:
      
      TGraphAsymmErrors*   m_graph;
      TH1*                 m_histo;
      TH1*                 m_legend_histo;
      std::string          m_legend_label;
      std::string          m_y_axis_label;
      bool                 m_plot_ratio;
      std::string          m_legend_draw_options;
      RatioPlot*           m_ratio;
      RatioPlot*           m_selfRatio;
      
   };
   
   
};

#endif

