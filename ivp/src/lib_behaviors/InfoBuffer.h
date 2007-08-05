/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: InfoBuffer.H                                         */
/*    DATE: Oct 12th 2004 Thanksgiving in Waterloo               */
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

#ifdef _WIN32 //MSVC pragma added by PMN in FAF
	#pragma warning(disable : 4786)
#endif

#ifndef INFO_BUFFER_IVP_HEADER
#define INFO_BUFFER_IVP_HEADER

#include <string>
#include <vector>
#include <map>
#include "InfoBuffer.h"

class InfoBuffer {
public:
  InfoBuffer() {};
  ~InfoBuffer() {};

public: // virtuals defined
  std::string sQuery(std::string, bool&) const;

  double dQuery(std::string, bool&) const;
  double tQuery(std::string) const;

  std::vector<std::string> svQuery(std::string, bool&) const;
  
  std::vector<double> dvQuery(std::string, bool&) const;

  void   print() const;

public:
  bool  set_val(std::string, double);
  bool  set_val(std::string, std::string);
  void  clearDeltaVectors();

  void   setCurrTime(double t)         {m_curr_time = t;};
  double getCurrTime() const           {return(m_curr_time);};

protected:
  std::map<std::string, std::string> smap;
  std::map<std::string, double>      dmap;
  std::map<std::string, double>      tmap;

  std::map<std::string, std::vector<std::string> >  vsmap;
  std::map<std::string, std::vector<double> > vdmap;

  double m_curr_time;
};
#endif


















