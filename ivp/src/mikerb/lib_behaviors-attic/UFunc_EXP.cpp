/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: UFunc_EXP.cpp                                        */
/*    DATE: June 16th, 2003                                      */
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

#include <iostream.h>
#include <math.h>
#include "UFunc_EXP.h"

//----------------------------------------------------------
// Procedure: Constructor

UFunc_EXP::UFunc_EXP(const char*  g_label,
		     double g_exp,
		     double g_deltaMax,
		     double g_centerVal,
		     double g_multiplier) :
  UFunc(g_label)
{
  exp       = g_exp;
  deltaMax  = g_deltaMax;
  centerVal = g_centerVal;
  multiplier = g_multiplier;

}

//----------------------------------------------------------
// Procedure: Constructor

UFunc_EXP::UFunc_EXP() : UFunc("domain")
{
  exp        = 1.0;
  deltaMax   = 1.0;
  centerVal  = 0.0;
  multiplier = 1.0;
}

//----------------------------------------------------------------
// Procedure: eval

double UFunc_EXP::eval(double val) const
{
  double delta = val - centerVal;
  if(delta < 0) delta = delta * -1.0;

  if(delta >= deltaMax)
    return(0.0);
  else {
    double frac = (deltaMax - delta)/deltaMax;
    double processedFrac = pow(frac, exp);
    return(multiplier * processedFrac);
  }
}








