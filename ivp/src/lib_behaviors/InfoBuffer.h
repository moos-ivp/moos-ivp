/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: InfoBuffer.H                                         */
/*    DATE: Oct 12th 2004 Thanksgiving in Waterloo               */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#ifdef _WIN32 //MSVC pragma added by PMN in FAF '05
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

  std::vector<std::string> sQueryDeltas(std::string, bool&) const;
  
  std::vector<double> dQueryDeltas(std::string, bool&) const;

  void   print() const;

public:
  bool  setValue(std::string, double, double=0);
  bool  setValue(std::string, std::string, double=0);
  void  clearDeltaVectors();

  void   setCurrTime(double t)         {m_curr_time_utc = t;};
  double getCurrTime() const           {return(m_curr_time_utc);};

protected:
  std::map<std::string, std::string> smap;
  std::map<std::string, double>      dmap;
  std::map<std::string, double>      tmap;

  std::map<std::string, std::vector<std::string> >  vsmap;
  std::map<std::string, std::vector<double> > vdmap;

  double m_curr_time_utc;
};
#endif

