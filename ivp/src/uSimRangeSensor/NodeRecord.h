/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NodeRecord.h                                         */
/*    DATE: Feb 7th, 2011                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef NODE_RECORD_HEADER
#define NODE_RECORD_HEADER

#include <string>

class NodeRecord
{
 public:
  NodeRecord();
  virtual ~NodeRecord() {};
 
  // Setters
  void  setName(std::string s)   {m_name=s;};
  void  setX(double v)           {m_x=v;};
  void  setY(double v)           {m_y=v;};
  void  setSpeed(double v)       {m_speed=v;};
  void  setTimeStamp(double v)   {m_timestamp=v;};
  void  setQueryRange(double v)  {m_query_range=v;};
  void  setQueryFreq(double v)   {m_query_freq=v;};
  void  setValid(bool v)         {m_valid=v;};

  // Getters
  std::string  getName() const   {return(m_name);};
  double  getX() const           {return(m_x);};
  double  getY() const           {return(m_y);};
  double  getSpeed() const       {return(m_speed);};
  double  getTimeStamp() const   {return(m_timestamp);};
  double  getQueryRange() const  {return(m_query_range);};
  double  getQueryFreq() const   {return(m_query_freq);};
  bool    valid() const          {return(m_valid);};

  std::string getSpec() const;

  // Determine if a query is possible at the moment
  bool   queryAllowed(double curr_time) const;
  double getElapsedTime(double curr_time) const;

 protected: // State variables
  double      m_x;            // Last known x-position
  double      m_y;            // Last known y-position
  double      m_speed;        // Last known speed
  double      m_timestamp;    // Last time query was made
  bool        m_valid;        // true if position set

 protected: // Configuration variables
  std::string m_name;         // Name of the vehicle
  double      m_query_freq;   // Min time between queries
  double      m_query_range;  // Range at which beacon rpts heard
};
#endif 

