#ifndef TOPDECAYCHANNEL_H
#define TOPDECAYCHANNEL_H

#include <vector>
#include <string>

class TopDecayChannel{
public:
   enum Channel{EJETS,MUJETS,COMBINED,NUM_CHANNELS};
   
   
   TopDecayChannel(){m_channel = NUM_CHANNELS;};
   TopDecayChannel(const Channel channel){SetChannel(channel);};
   TopDecayChannel(const std::string channel){SetChannel(channel);};
   TopDecayChannel(const TopDecayChannel& channel){m_channel = channel.GetChannel();}
   TopDecayChannel& operator=(const TopDecayChannel& rhs){
      m_channel = rhs.GetChannel();
      return *this;
   }
   TopDecayChannel& operator=(const Channel& rhs){
      m_channel = rhs;
      return *this;
   }
   bool operator==(const TopDecayChannel& rhs) const{
      if(m_channel == rhs.GetChannel())
         return true;
      else
         return false;
   }
   bool operator!=(const TopDecayChannel& rhs) const{
      if(m_channel != rhs.GetChannel())
         return true;
      else
         return false;
   }
   
   bool IsSet() const{
      if(m_channel == NUM_CHANNELS)
         return false;
      else
         return true;
   }
   
   void SetChannel(const std::string channel){
      for(unsigned int i=0;i<NUM_CHANNELS;++i){
         if(channel.find(m_array_channelStrings[i]) != std::string::npos){
            SetChannel((Channel)i);
            return;
         }
      }
   }
   void SetChannel(const Channel channel){
      m_channel = channel;
   }
   Channel GetChannel() const {
      return m_channel;
   }
   
   std::string GetChannelString() const{
      if(m_channel != NUM_CHANNELS)
         return m_array_channelStrings[m_channel];
   }
   
   std::string GetChannelStringPretty() const{
      if(m_channel != NUM_CHANNELS)
         return m_array_channelStrings_pretty[m_channel];
   }
   
   std::string GetChannelStringLatex() const{
      if(m_channel != NUM_CHANNELS)
         return m_array_channelStrings_latex[m_channel];
   }
   
   std::string GetChannelStringROOT() const{
      if(m_channel != NUM_CHANNELS)
         return m_array_channelStrings_root[m_channel];
   }
   
   void SetChannelUsingFilename(const std::string& filename){
      if(filename.find("_el_") != std::string::npos){
         m_channel = EJETS;
      }
      else if(filename.find("_mu_") != std::string::npos){
         m_channel = MUJETS;
      }
      else if(filename.find("_co_") != std::string::npos){
         m_channel = COMBINED;
      }
   }
   
private:
   Channel m_channel;
   
   static const char *m_array_channelStrings[];
   static const char *m_array_channelStrings_pretty[];
   static const char *m_array_channelStrings_latex[];
   static const char *m_array_channelStrings_root[];
};


#endif
