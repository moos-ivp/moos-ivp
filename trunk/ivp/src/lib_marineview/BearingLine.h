/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BearingLine.h                                        */
/*    DATE: Jan 21st 2010                                        */
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

#ifndef BEARING_LINE_HEADER
#define BEARING_LINE_HEADER

class BearingLine
{
 public:
  BearingLine() {
    m_bearing      = 0; 
    m_bearing_set  = false;
    m_bearing_absolute = true;
    m_range        = 50;
    m_time_limit   = -1; 
    m_time_stamp   = -1;
    m_vector_width = 1;
    m_vector_color = "orange";
  }

  BearingLine(double bearing, double range=50, 
		double tl=-1, double ts=-1) {
    m_bearing      = bearing;
    m_bearing_set  = true;
    m_bearing_absolute = true; 
    m_range        = range;
    m_time_limit   = tl; 
    m_time_stamp   = ts;
    m_vector_width = 1;
    m_vector_color = "orange";
  }

  ~BearingLine() {}

  bool isValid() const            {return(m_bearing_set);}
  
  void setBearing(double v)       {m_bearing=v; m_bearing_set=true;}
  void setBearingAbsolute(bool v) {m_bearing_absolute=v;}
  void setRange(double v)         {m_range=v;}
  void setTimeLimit(double v)     {m_time_limit=v;}
  void setTimeStamp(double v)     {m_time_stamp=v;}
  void setVectorWidth(double v)   {m_vector_width=v;}

  void setVectorColor(std::string str) {m_vector_color=str;}
  void setLabel(std::string str)  {m_label=str;}
  
  double  getBearing() const        {return(m_bearing);}
  bool    isBearingAbsolute() const {return(m_bearing_absolute);}
  double  getRange() const          {return(m_range);}
  double  getTimeLimit() const      {return(m_time_limit);}
  double  getTimeStamp() const      {return(m_time_stamp);}
  double  getVectorWidth() const    {return(m_vector_width);}

  std::string  getVectorColor() const {return(m_vector_color);}
  std::string  getLabel() const       {return(m_label);}

 private:
  double m_bearing; 
  bool   m_bearing_absolute; 
  double m_range;
  double m_time_limit;
  double m_time_stamp;
  double m_vector_width;

  bool   m_bearing_set;

  std::string m_vector_color;
  std::string m_label;
};

#endif

















