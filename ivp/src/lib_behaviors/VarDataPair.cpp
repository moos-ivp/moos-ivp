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

#include "VarDataPair.h"
#include "MBUtils.h"
#include <stdlib.h>

using namespace std;

//------------------------------------------------------------------
// Procedure: constructor

VarDataPair::VarDataPair(const string& var, double ddata)
{
  m_var       = stripBlankEnds(var);
  m_sdata     = "";
  m_ddata     = ddata;
  m_is_string = false;
}

//------------------------------------------------------------------
// Procedure: constructor

VarDataPair::VarDataPair(const string& var, const string& sdata)
{
  m_var       = stripBlankEnds(var);
  m_sdata     = stripBlankEnds(sdata);
  m_ddata     = 0;
  m_is_string = true;
}

//------------------------------------------------------------------
// Procedure: constructor
//      Note: For convenience, check whether the given data string is
//            meant to represent a numerical value and do the conversion.

VarDataPair::VarDataPair(const string& var, const string& sdata,
			 const string& hint)
{
  m_var   = stripBlankEnds(var);
  m_ddata = 0;

  string data = stripBlankEnds(sdata);

  if(hint != "auto")
    return;

  if(isNumber(data)) {
    m_ddata = atof(data.c_str());
    m_is_string = false;
  }
  else {
    if(isQuoted(data))
      m_sdata = stripQuotes(data);
    else
      m_sdata = data;
    m_is_string = true;
  }
}

//------------------------------------------------------------------
// Procedure: getPrintable
//  Notes: Return a single string representing the pair. The type
//         are expressed as follows.
//         foo = 27  var is of type double since 27 is a number.
//         foo = "27"  var is of type string since 27 is quoted.
//         foo = bar  var is of type string since bar is NOT a number.


string VarDataPair::getPrintable()
{
  string rstring = m_var;
  rstring += "=";

  if(m_is_string) {
    if(isNumber(m_sdata)) {
      rstring += "\"";
      rstring += m_sdata;
      rstring += "\"";
    }
    else
      rstring += m_sdata;
  }
  else {
    string dstr = doubleToString(m_ddata, 4);
    // Remove trailing zeros after the decimal point
    dstr = dstringCompact(dstr);
    rstring += dstr;
  }
  return(rstring);
}
