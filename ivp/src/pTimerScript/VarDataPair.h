/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VarDataPair.h                                        */
/*    DATE: Jul 4th 2005 Monday morning at Brueggers             */
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

#ifndef VAR_DATA_PAIR_HEADER
#define VAR_DATA_PAIR_HEADER

#include <string>

class VarDataPair
{
public:
  VarDataPair(const std::string& varname, 
	      double varvalue);

  VarDataPair(const std::string& varname, 
	      const std::string& varvalue);

  VarDataPair(const std::string& varname, 
	      const std::string& varvalue, 
	      const std::string& typehint);

  virtual ~VarDataPair() {};
  
  void        set_key(const std::string& s) {m_key=s;};

  std::string get_var()   const {return(m_var);};
  std::string get_sdata() const {return(m_sdata);};
  double      get_ddata() const {return(m_ddata);};
  bool        is_string() const {return(m_is_string);};
  std::string get_key()   const {return(m_key);};

  std::string getPrintable();

protected:
  std::string m_var;
  std::string m_sdata;
  double      m_ddata;
  bool        m_is_string;
  std::string m_key;
};

#endif 
