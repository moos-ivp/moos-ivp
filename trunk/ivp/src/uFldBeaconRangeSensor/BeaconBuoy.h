/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BeaconBuoy.h                                         */
/*    DATE: Feb 1st, 2011                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef BEACON_BUOY_HEADER
#define BEACON_BUOY_HEADER

#include <string>

class BeaconBuoy
{
 public:
  BeaconBuoy();
  virtual ~BeaconBuoy() {}

  // Setters
  void    setX(double x)               {m_x=x;}
  void    setY(double y)               {m_y=y;}
  void    setLabel(std::string label)  {m_label=label;}
  void    incPingsReceived()           {m_pings_recvd++;}
  void    incPingsReplied()            {m_pings_repld++;}
  void    incPingsUnsol()              {m_pings_unsol++;}

  bool    setFrequencyRange(std::string);
  bool    setPushDist(std::string);
  bool    setPullDist(std::string);
  bool    setShape(std::string);
  bool    setWidth(std::string);
  bool    setBuoyColor(std::string);
  bool    setPulseLineColor(std::string);
  bool    setPulseFillColor(std::string);
  
  // Getters
  double  getFrequency() const   {return(m_frequency);}
  double  getTimeStamp() const   {return(m_timestamp);}
  double  getX() const           {return(m_x);}
  double  getY() const           {return(m_y);}
  double  getWidth() const       {return(m_width);}
  double  getPushDist() const    {return(m_push_dist);}
  double  getPullDist() const    {return(m_pull_dist);}
  
  unsigned int getPingsReceived() const {return(m_pings_recvd);} 
  unsigned int getPingsReplied()  const {return(m_pings_repld);} 
  unsigned int getPingsUnsol()    const {return(m_pings_unsol);} 

  std::string getLabel() const     {return(m_label);}
  std::string getShape() const     {return(m_shape);}
  std::string getBuoyColor() const {return(m_buoy_color);}
  std::string getSpec(bool=false) const;
  std::string getFreqSetting() const;

 public: // Updating beacon history
  bool    setTimeStamp(double tstamp);

 protected:
  void    resetFrequency();

 protected: // Unique identifier
  std::string  m_label;

 protected: // Key properties
  double  m_x;
  double  m_y;
  double  m_report_range;
  double  m_push_dist;
  double  m_pull_dist;
  double  m_frequency;
  double  m_timestamp;

  double  m_freq_low;
  double  m_freq_hgh;

 protected: // State variables
  unsigned int m_pings_recvd;
  unsigned int m_pings_repld;
  unsigned int m_pings_unsol;

 protected: // Drawing hints
  double       m_width;
  std::string  m_shape;
  std::string  m_buoy_color;
  std::string  m_pulse_line_color;
  std::string  m_pulse_fill_color;
};

#endif 









