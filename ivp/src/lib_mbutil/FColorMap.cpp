/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FColorMap.cpp                                        */
/*    DATE: Jan 12th 2006                                        */
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
#include <math.h>
#include "FColorMap.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: getIRVal  "get Interpolated Red Value"

double FColorMap::getIRVal(double val) const
{
  if(val < 0) val=0.0;
  if(val > 1) val=1.0;

  int sz = rval.size();
  if(sz == 0)
    return(0);

  int ix  = (int)(((val * (double)sz) + 0.5) - 1.0);
  return(rval[ix]);
}

//-------------------------------------------------------------
// Procedure: getIGVal  "get Interpolated Green Value"


double FColorMap::getIGVal(double val) const
{
  if(val < 0) val=0.0;
  if(val > 1) val=1.0;

  int sz = gval.size();
  if(sz == 0)
    return(0);

  int ix  = (int)(((val * (double)sz) + 0.5) - 1.0);
  return(gval[ix]);
}

//-------------------------------------------------------------
// Procedure: getIBVal  "get Interpolated Blue Value"

double FColorMap::getIBVal(double val) const
{
  if(val < 0) val=0.0;
  if(val > 1) val=1.0;

  int sz = bval.size();
  if(sz == 0)
    return(0);

  int ix  = (int)(((val * (double)sz) + 0.5) - 1.0);
  return(bval[ix]);
}

//-------------------------------------------------------------
// Procedure: setType

void FColorMap::setType(const string &map_type)
{
  if(map_type == "copper")
    setCopperMap();
  else if(map_type == "bone")
    setBoneMap();
  else
    setStandardMap();
}


//-------------------------------------------------------------
// Procedure: setStandarMap

void FColorMap::applyMidWhite(double plateau, double basewidth)
{
  if((plateau < 0) || (basewidth < 0))
    return;
  
  int vsize = rval.size();

  cout << "plateau: "   << plateau   << endl;
  cout << "basewidth: " << basewidth << endl;

  double whitener = 0;
  for(int i=0; i<vsize; i++) {
    double ipct = (double)(i) / (double)(vsize);
    double delta = 0.50 - ipct;
    if(delta < 0) 
      delta *= -1;
    if(delta < plateau)
      whitener = 100.0;
    else if(delta > plateau+basewidth)
      whitener = 0;
    else {
      double pct = (delta-plateau) / basewidth;
      whitener = (1.0 - pct) * 100.0;
    }
    cout << "delta[" << i << "]: " << delta << endl;
    cout << "whitener[" << i << "]: " << whitener << endl;

#if 0
    rval[i] += ((1.0 - rval[i]) * whitener);
    gval[i] += ((1.0 - gval[i]) * whitener);
    bval[i] += ((1.0 - bval[i]) * whitener);
#endif
#if 1
    if(whitener >= 100.0) {
      rval[i] = 1.0;
      gval[i] = 1.0;
      bval[i] = 1.0;
    }
#endif

  }

}


//-------------------------------------------------------------
// Procedure: setStandarMap

void FColorMap::setStandardMap()
{
  rval.clear();
  gval.clear();
  bval.clear();
  addRGB(0.0,    0.0,    0.5625);
  addRGB(0.0,    0.0,    0.6250);
  addRGB(0.0,    0.0,    0.6875);
  addRGB(0.0,    0.0,    0.7500);
  addRGB(0.0,    0.0,    0.8125);
  addRGB(0.0,    0.0,    0.8750);
  addRGB(0.0,    0.0,    0.9375);
  addRGB(0.0,    0.0,    1.0000);
  addRGB(0.0,    0.0625, 1.0000);
  addRGB(0.0,    0.1250, 1.0000);
  addRGB(0.0,    0.1875, 1.0000);
  addRGB(0.0,    0.2500, 1.0000);
  addRGB(0.0,    0.3125, 1.0000);
  addRGB(0.0,    0.3750, 1.0000);
  addRGB(0.0,    0.4375, 1.0000);
  addRGB(0.0,    0.5000, 1.0000);
  addRGB(0.0,    0.5625, 1.0000);
  addRGB(0.0,    0.6250, 1.0000);
  addRGB(0.0,    0.6875, 1.0000);
  addRGB(0.0,    0.7500, 1.0000);
  addRGB(0.0,    0.8125, 1.0000);
  addRGB(0.0,    0.8750, 1.0000);
  addRGB(0.0,    0.9375, 1.0000);
  addRGB(0.0,    1.0000, 1.0000);
  addRGB(0.0625, 1.0000, 0.9375);
  addRGB(0.1250, 1.0000, 0.8750);
  addRGB(0.1875, 1.0000, 0.8125);
  addRGB(0.2500, 1.0000, 0.7500);
  addRGB(0.3125, 1.0000, 0.6875);
  addRGB(0.3750, 1.0000, 0.6250);
  addRGB(0.4375, 1.0000, 0.5625);
  addRGB(0.5000, 1.0000, 0.5000);
  addRGB(0.5625, 1.0000, 0.4375);
  addRGB(0.6250, 1.0000, 0.3750);
  addRGB(0.6875, 1.0000, 0.3125);
  addRGB(0.7500, 1.0000, 0.2500);
  addRGB(0.8125, 1.0000, 0.1875);
  addRGB(0.8750, 1.0000, 0.1250);
  addRGB(0.9375, 1.0000, 0.0625);
  addRGB(1.0000, 1.0000, 0.0);
  addRGB(1.0000, 0.9375, 0.0);
  addRGB(1.0000, 0.8750, 0.0);
  addRGB(1.0000, 0.8125, 0.0);
  addRGB(1.0000, 0.7500, 0.0);
  addRGB(1.0000, 0.6875, 0.0);
  addRGB(1.0000, 0.6250, 0.0);
  addRGB(1.0000, 0.5625, 0.0);
  addRGB(1.0000, 0.5000, 0.0);
  addRGB(1.0000, 0.4375, 0.0);
  addRGB(1.0000, 0.3750, 0.0);
  addRGB(1.0000, 0.3125, 0.0);
  addRGB(1.0000, 0.2500, 0.0);
  addRGB(1.0000, 0.1875, 0.0);
  addRGB(1.0000, 0.1250, 0.0);
  addRGB(1.0000, 0.0625, 0.0);
  addRGB(1.0000,    0.0, 0.0);
  addRGB(0.9375,    0.0, 0.0);
  addRGB(0.8750,    0.0, 0.0);
  addRGB(0.8125,    0.0, 0.0);
  addRGB(0.7500,    0.0, 0.0);
  addRGB(0.6875,    0.0, 0.0);
  addRGB(0.6250,    0.0, 0.0);
  addRGB(0.5625,    0.0, 0.0);
  addRGB(0.5000,    0.0, 0.0);
}

//-------------------------------------------------------------
// Procedure: setStandarMap

void FColorMap::setCopperMap()
{
  rval.clear();
  gval.clear();
  bval.clear();
  addRGB(0.0,    0.0,    0.0);
  addRGB(0.0198, 0.0124, 0.0079);
  addRGB(0.0397, 0.0248, 0.0158);
  addRGB(0.0595, 0.0372, 0.0237);
  addRGB(0.0794, 0.0496, 0.0316);
  addRGB(0.0992, 0.0620, 0.0395);
  addRGB(0.1190, 0.0744, 0.0474);
  addRGB(0.1389, 0.0868, 0.0553);
  addRGB(0.1587, 0.0992, 0.0632);
  addRGB(0.1786, 0.1116, 0.0711);
  addRGB(0.1984, 0.1240, 0.0790);
  addRGB(0.2183, 0.1364, 0.0869);
  addRGB(0.2381, 0.1488, 0.0948);
  addRGB(0.2579, 0.1612, 0.1027);
  addRGB(0.2778, 0.1736, 0.1106);
  addRGB(0.2976, 0.1860, 0.1185);
  addRGB(0.3175, 0.1984, 0.1263);
  addRGB(0.3373, 0.2108, 0.1342);
  addRGB(0.3571, 0.2232, 0.1421);
  addRGB(0.3770, 0.2356, 0.1500);
  addRGB(0.3968, 0.2480, 0.1579);
  addRGB(0.4167, 0.2604, 0.1658);
  addRGB(0.4365, 0.2728, 0.1737);
  addRGB(0.4563, 0.2852, 0.1816);
  addRGB(0.4762, 0.2976, 0.1895);
  addRGB(0.4960, 0.3100, 0.1974);
  addRGB(0.5159, 0.3224, 0.2053);
  addRGB(0.5357, 0.3348, 0.2132);
  addRGB(0.5556, 0.3472, 0.2211);
  addRGB(0.5754, 0.3596, 0.2290);
  addRGB(0.5952, 0.3720, 0.2369);
  addRGB(0.6151, 0.3844, 0.2448);
  addRGB(0.6349, 0.3968, 0.2527);
  addRGB(0.6548, 0.4092, 0.2606);
  addRGB(0.6746, 0.4216, 0.2685);
  addRGB(0.6944, 0.4340, 0.2764);
  addRGB(0.7143, 0.4464, 0.2843);
  addRGB(0.7341, 0.4588, 0.2922);
  addRGB(0.7540, 0.4712, 0.3001);
  addRGB(0.7738, 0.4836, 0.3080);
  addRGB(0.7937, 0.4960, 0.3159);
  addRGB(0.8135, 0.5084, 0.3238);
  addRGB(0.8333, 0.5208, 0.3317);
  addRGB(0.8532, 0.5332, 0.3396);
  addRGB(0.8730, 0.5456, 0.3475);
  addRGB(0.8929, 0.5580, 0.3554);
  addRGB(0.9127, 0.5704, 0.3633);
  addRGB(0.9325, 0.5828, 0.3712);
  addRGB(0.9524, 0.5952, 0.3790);
  addRGB(0.9722, 0.6076, 0.3869);
  addRGB(0.9921, 0.6200, 0.3948);
  addRGB(1.0000, 0.6324, 0.4027);
  addRGB(1.0000, 0.6448, 0.4106);
  addRGB(1.0000, 0.6572, 0.4185);
  addRGB(1.0000, 0.6696, 0.4264);
  addRGB(1.0000, 0.6820, 0.4343);
  addRGB(1.0000, 0.6944, 0.4422);
  addRGB(1.0000, 0.7068, 0.4501);
  addRGB(1.0000, 0.7192, 0.4580);
  addRGB(1.0000, 0.7316, 0.4659);
  addRGB(1.0000, 0.7440, 0.4738);
  addRGB(1.0000, 0.7564, 0.4817);
  addRGB(1.0000, 0.7688, 0.4896);
  addRGB(1.0000, 0.7812, 0.4975);
}

//-------------------------------------------------------------
// Procedure: setBondMap

void FColorMap::setBoneMap()
{
  rval.clear();
  gval.clear();
  bval.clear();

  addRGB(0.0000, 0.0000, 0.0052);
  addRGB(0.0139, 0.0139, 0.0243);
  addRGB(0.0278, 0.0278, 0.0434);
  addRGB(0.0417, 0.0417, 0.0625);
  addRGB(0.0556, 0.0556, 0.0816);
  addRGB(0.0694, 0.0694, 0.1007);
  addRGB(0.0833, 0.0833, 0.1198);
  addRGB(0.0972, 0.0972, 0.1389);
  addRGB(0.1111, 0.1111, 0.1580);
  addRGB(0.1250, 0.1250, 0.1771);
  addRGB(0.1389, 0.1389, 0.1962);
  addRGB(0.1528, 0.1528, 0.2153);
  addRGB(0.1667, 0.1667, 0.2344);
  addRGB(0.1806, 0.1806, 0.2535);
  addRGB(0.1944, 0.1944, 0.2726);
  addRGB(0.2083, 0.2083, 0.2917);
  addRGB(0.2222, 0.2222, 0.3108);
  addRGB(0.2361, 0.2361, 0.3299);
  addRGB(0.2500, 0.2500, 0.3490);
  addRGB(0.2639, 0.2639, 0.3681);
  addRGB(0.2778, 0.2778, 0.3872);
  addRGB(0.2917, 0.2917, 0.4062);
  addRGB(0.3056, 0.3056, 0.4253);
  addRGB(0.3194, 0.3194, 0.4444);
  addRGB(0.3333, 0.3385, 0.4583);
  addRGB(0.3472, 0.3576, 0.4722);
  addRGB(0.3611, 0.3767, 0.4861);
  addRGB(0.3750, 0.3958, 0.5000);
  addRGB(0.3889, 0.4149, 0.5139);
  addRGB(0.4028, 0.4340, 0.5278);
  addRGB(0.4167, 0.4531, 0.5417);
  addRGB(0.4306, 0.4722, 0.5556);
  addRGB(0.4444, 0.4913, 0.5694);
  addRGB(0.4583, 0.5104, 0.5833);
  addRGB(0.4722, 0.5295, 0.5972);
  addRGB(0.4861, 0.5486, 0.6111);
  addRGB(0.5000, 0.5677, 0.6250);
  addRGB(0.5139, 0.5868, 0.6389);
  addRGB(0.5278, 0.6059, 0.6528);
  addRGB(0.5417, 0.6250, 0.6667);
  addRGB(0.5556, 0.6441, 0.6806);
  addRGB(0.5694, 0.6632, 0.6944);
  addRGB(0.5833, 0.6823, 0.7083);
  addRGB(0.5972, 0.7014, 0.7222);
  addRGB(0.6111, 0.7205, 0.7361);
  addRGB(0.6250, 0.7396, 0.7500);
  addRGB(0.6389, 0.7587, 0.7639);
  addRGB(0.6528, 0.7778, 0.7778);
  addRGB(0.6745, 0.7917, 0.7917);
  addRGB(0.6962, 0.8056, 0.8056);
  addRGB(0.7179, 0.8194, 0.8194);
  addRGB(0.7396, 0.8333, 0.8333);
  addRGB(0.7613, 0.8472, 0.8472);
  addRGB(0.7830, 0.8611, 0.8611);
  addRGB(0.8047, 0.8750, 0.8750);
  addRGB(0.8264, 0.8889, 0.8889);
  addRGB(0.8481, 0.9028, 0.9028);
  addRGB(0.8698, 0.9167, 0.9167);
  addRGB(0.8915, 0.9306, 0.9306);
  addRGB(0.9132, 0.9444, 0.9444);
  addRGB(0.9349, 0.9583, 0.9583);
  addRGB(0.9566, 0.9722, 0.9722);
  addRGB(0.9783, 0.9861, 0.9861);
  addRGB(1.0000, 1.0000, 1.0000);
}




