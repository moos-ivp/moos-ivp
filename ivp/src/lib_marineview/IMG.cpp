/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IMG.cpp                                              */
/*    DATE: Nov 23, 2003 Hanging out in Munich 1st day           */
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

#include <iostream>
#include <assert.h>
#include <string.h>
#include "IMG.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

IMG::IMG(int g_ymax)
{
  assert(g_ymax  > 0);

  ymax   = g_ymax;

  hval   = 0.0;
  lval   = 0.0;
  lfresh = true;
  hfresh = true;

  ival = new double*[360];
  for(int i=0; i<360; i++) {
    ival[i] = new double[ymax+1];
    for(int j=0; j<=ymax; j++)
      ival[i][j] = 999.0;
  }
}

//-----------------------------------------------------------
// Procedure: Constructor

IMG::IMG(const IMG &img)
{
  ymax = img.ymax;
  hval = img.hval;
  lval = img.lval;
  lfresh = img.lfresh;
  hfresh = img.hfresh;
  vname  = img.vname;
  bname  = img.bname;
  
  ival = new double*[360];
  for(int i=0; i<360; i++) {
    ival[i] = new double[ymax+1];
    for(int j=0; j<=ymax; j++)
      ival[i][j] = img.ival[i][j];
  }
}

//-----------------------------------------------------------
// Procedure: Constructor

const IMG& IMG::operator=(const IMG &right)
{
  ymax = right.ymax;
  hval = right.hval;
  lval = right.lval;
  lfresh = right.lfresh;
  hfresh = right.hfresh;
  vname  = right.vname;
  bname  = right.bname;
  
  ival = new double*[360];
  for(int i=0; i<360; i++) {
    ival[i] = new double[ymax+1];
    for(int j=0; j<=ymax; j++)
      ival[i][j] = right.ival[i][j];
  }
  return(*this);
}
 
//-----------------------------------------------------------
// Procedure: isValid

bool IMG::isValid()
{
  for(int i=0; i<360; i++) {
    for(int j=0; j<=ymax; j++)
      if((ival[i][j] < -10000000) || (ival[i][j] > 10000000)) {
	cout << "FAULT: " << ival[i][j] << endl;
	return(false);
      }
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: Destructor

IMG::~IMG()
{
  for(int i=0; i<360; i++)
    delete [] ival[i];
  delete [] ival;
}

//-----------------------------------------------------------
// Procedure: get_ival_ix
//      Note:

double IMG::get_ival_ix(int g_crs, int g_y) const
{
  assert((g_crs >= 0) && (g_crs <= 359));
  assert((g_y   >= 0) && (g_y   <= ymax));
  return(ival[g_crs][g_y]);
}

//-----------------------------------------------------------
// Procedure: set_ival_ix
//      Note:

void IMG::set_ival_ix(int g_crs, int g_y, double g_val)
{
  assert((g_crs >= 0) && (g_crs < 360));
  assert((g_y   >= 0) && (g_y  <= ymax));
  ival[g_crs][g_y] = g_val;

  if(hfresh) {
    hval  = g_val;
    hfresh = 0;
  }
  else
    if(g_val > hval)
      hval = g_val;

  if(lfresh) {
    if(g_val > -10000) {
      lval   = g_val;
      lfresh = 0;
    }
  }
  else
    if((g_val < lval) && (g_val > -10000))
      lval = g_val;
}







