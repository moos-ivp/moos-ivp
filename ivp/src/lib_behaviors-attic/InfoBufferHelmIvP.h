/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IB_HelmIvP.H                                         */
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

#ifndef INFO_BUFFER_HELMIVP_HEADER
#define INFO_BUFFER_HELMIVP_HEADER

#include <map>
#include "InfoBuffer.h"

class InfoBufferHelmIvP : public InfoBuffer {
public:
  InfoBufferHelmIvP() {};
  ~InfoBufferHelmIvP() {};

public: // virtuals defined
  std::string sQuery(std::string vn, std::string va, bool *res) const
    {return(sQuery(va, res));}
  
  double dQuery(std::string vn, std::string va, bool *res) const
    {return(dQuery(va, res));};

  double tQuery(std::string vn, std::string va) const
    {return(tQuery(va));};
  
  
  std::vector<std::string> svQuery(std::string vn, std::string va,
				   bool *res) const
    {return(svQuery(va, res));};
  
  std::vector<double> dvQuery(std::string vn, std::string va,
			      bool *res) const
    {return(dvQuery(va, res));};
  
public:
  bool  set_val(std::string vn, std::string va, double vv)
    {return(set_val(va, vv));};

  bool  set_val(std::string vn, std::string va, std::string vv)
    {return(set_val(va, vv));};


  
protected:
  std::string sQuery(const std::string&, bool *res) const;

  double dQuery(std::string, bool *res) const;
  double tQuery(const std::string&) const;
  
  std::vector<std::string> svQuery(const std::string&,
				   bool *res) const;
  
  std::vector<double> dvQuery(const std::string&,
			      bool *res) const;

  void   print() const;


public:
  bool  set_val(const std::string&, double);
  bool  set_val(const std::string&, const std::string&);
  void  clearDeltaVectors();




protected:
  std::map<std::string, std::string>  smap;
  std::map<std::string, double>       dmap;
  std::map<std::string, double>       tmap;

  std::map<std::string, std::vector<std::string> > vsmap;
  std::map<std::string, std::vector<double> >      vdmap;

};
#endif


















