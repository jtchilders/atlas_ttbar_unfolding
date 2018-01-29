#ifndef SYSTEMATICBIN_H
#define SYSTEMATICBIN_H

#include <string>
#include <sstream>
#include <map>
#include <iomanip>

class SystematicBin{
public:
   SystematicBin(){Clear();};
   SystematicBin(const SystematicBin& value) {Clear();Copy(value);};
   SystematicBin(const double up,const double down)
   {Clear();m_up=up;m_down=down;};
   
   // systematic uncertainty (set to 0 for asymmetric uncertainties)
   double syst() const {return m_syst;};
   void   syst(const double value){m_syst = value;};
   
   // up systematic uncertainty (set to 0 for symmetric uncertainties)
   double up() const {return m_up;};
   void   up(const double value){m_up = value;};
   
   // square of the up systematic uncertainty
   double up2() const {return m_up*m_up;};
   
   // down systematic uncertainty (set to 0 for symmetric uncertainties)
   double down() const {return m_down;};
   void   down(const double value){m_down = value;};
   
   // square of the down systematic uncertainty
   double down2() const {return m_down*m_down;};
   
   // The bin content from the unfolded systematically shifted DOWN
   // distribution, which is used to calculate up/down
   double shape_down() const {return m_shape_down;};
   void   shape_down(const double value){m_shape_down = value;};
   
   // The bin content from the unfolded systematically shifted UP
   // distribution, which is used to calculate up/down
   double shape_up() const {return m_shape_up;};
   void   shape_up(const double value){m_shape_up = value;};
   
   // statistical uncertainty
   double stat() const {return m_stat;};
   void   stat(const double value){m_stat = value;};
   
   // x-axis high edge
   double high() const {return m_high;};
   void   high(const double value){m_high = value;};
   
   // x-axis low edge
   double low() const {return m_low;};
   void   low(const double value){m_low = value;};
   
   // bin content
   double content() const {return m_content;};
   void   content(const double value){m_content = value;};
   
   void Scale(const double scaler);
   
   // list of fractional contributions of each systematic uncertainty to the
   // total uncertainty stored in syst()
   std::map<std::string,double> percentContribution() const {return m_percentContribution;};
   void percentContribution(const std::map<std::string,double>& contribution) { m_percentContribution = contribution;};
   void percentContribution(const std::string& systematicName,const double& percent) { m_percentContribution[systematicName] = percent;};
   
   bool extractStatistical();
   
   // list of fractional contributions of each systematic uncertainty to the
   // total uncertainty stored in up()
   std::map<std::string,double> percentContributionUp() const {return m_up_percentContribution;};
   void percentContributionUp(const std::map<std::string,double>& contribution) { m_up_percentContribution = contribution;};
   void percentContributionUp(const std::string& systematicName,const double& percent) { m_up_percentContribution[systematicName] = percent;};
   
   bool extractStatisticalUp();
   
   // list of fractional contributions of each systematic uncertainty to the
   // total uncertainty stored in down()
   std::map<std::string,double> percentContributionDown() const {return m_down_percentContribution;};
   void percentContributionDown(const std::map<std::string,double>& contribution) { m_down_percentContribution = contribution;};
   void percentContributionDown(const std::string& systematicName,const double& percent) { m_down_percentContribution[systematicName] = percent;};
   
   bool extractStatisticalDown();
   
   
   void Clear(){
      m_up=0.;m_down=0.;m_syst=0.;
      m_shape_up=0.;m_shape_down=0.;
      m_stat=0.;
      m_high=0.;m_low=0.;
      m_content=0.;
      m_up_percentContribution.clear();
      m_down_percentContribution.clear();
      m_percentContribution.clear();
   };
   
   SystematicBin& operator= (const SystematicBin& rhs)
   {
      Copy(rhs);
      return *this;
   };
   
   std::string str() const {
      std::stringstream tmp;
      tmp.setf(std::ios::fixed);
      tmp << "content: ";
      tmp.precision(4);
      tmp.width(10);
      tmp << m_content;
      
      tmp << " binLow: ";
      tmp.precision(0);
      tmp.width(4);
      tmp << m_low;
      tmp << " binHigh: ";
      tmp.precision(0);
      tmp.width(4);
      tmp << m_high;
      
      tmp << " stat: ";
      tmp.precision(6);
      tmp.width(10);
      tmp << m_stat;
      tmp << " syst: ";
      tmp.precision(6);
      tmp.width(10);
      tmp << m_syst;
      tmp << " systUp: ";
      tmp.precision(6);
      tmp.width(10);
      tmp << m_up;
      tmp << " systDown: ";
      tmp.precision(6);
      tmp.width(10);
      tmp << m_down;
      tmp << " shapeUp: ";
      tmp.precision(6);
      tmp.width(10);
      tmp << m_shape_up;
      tmp << " shapeDown: ";
      tmp.precision(6);
      tmp.width(10);
      tmp << m_shape_down;
      
      return tmp.str();
   }
   
private:
   double m_syst; // systematic uncertainty if symmetric
   double m_up; // systematic uncertainty up
   double m_down; // systematic uncertainty down
   // used for correlation matrix calculation
   double m_shape_up; // systematic unfolded bin data for up shift
   double m_shape_down; // systematic unfolded bin data for down shift
   
   double m_stat; // statistical uncertainty
   
   double m_high; // bin high edge
   double m_low; // bin low edge
   
   double m_content; // bin content
   
   // percent contribution from BLUE combination
   // percent w.r.t total uncertainty
   std::map<std::string,double> m_up_percentContribution;
   std::map<std::string,double> m_down_percentContribution;
   std::map<std::string,double> m_percentContribution;
   
   void Copy(const SystematicBin& rhs){
      m_up = rhs.up();m_down = rhs.down();m_syst = rhs.syst();
      m_shape_up = rhs.shape_up();m_shape_down = rhs.shape_down();
      m_stat = rhs.stat();
      m_high = rhs.high();m_low = rhs.low();
      m_content = rhs.content();
      m_up_percentContribution.clear();
      m_up_percentContribution = rhs.percentContributionUp();
      m_down_percentContribution.clear();
      m_down_percentContribution = rhs.percentContributionDown();
      m_percentContribution.clear();
      m_percentContribution = rhs.percentContribution();
   }
};


#endif
