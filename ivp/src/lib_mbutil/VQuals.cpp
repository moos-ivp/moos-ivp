/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: VQuals.cpp                                           */
/*    DATE: Mar 9th, 2024                                        */
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

#include "VQuals.h"

using namespace std;

//---------------------------------------------------------
// Procedure: getVName()

string getIndexVName1(unsigned int ix)
{
  if(ix == 0) return("abe");
  if(ix == 1) return("ben");
  if(ix == 2) return("cal");
  if(ix == 3) return("deb");
  if(ix == 4) return("eve");
  if(ix == 5) return("fin");
  if(ix == 6) return("gil");
  if(ix == 7) return("hix");
  if(ix == 8) return("ike");
  if(ix == 9) return("jim");
  if(ix == 10) return("kim");
  if(ix == 11) return("lou");
  if(ix == 12) return("max");
  if(ix == 13) return("ned");
  if(ix == 14) return("oak");
  if(ix == 15) return("pal");
  if(ix == 16) return("que");
  if(ix == 17) return("ray");
  if(ix == 18) return("sam");
  if(ix == 19) return("tim");
  if(ix == 20) return("ula");
  if(ix == 21) return("val");
  if(ix == 22) return("wes");
  if(ix == 23) return("xiu");
  if(ix == 24) return("yen");
  if(ix == 25) return("zan");
  if(ix == 26) return("apia");
  if(ix == 27) return("baka");
  if(ix == 28) return("cary");
  if(ix == 29) return("doha");
  if(ix == 30) return("evie");
  if(ix == 31) return("fahy");
  if(ix == 32) return("galt");
  if(ix == 33) return("hays");
  if(ix == 34) return("iola");
  if(ix == 35) return("jing");
  if(ix == 36) return("kiev");
  if(ix == 37) return("lima");
  if(ix == 38) return("mesa");
  if(ix == 39) return("nuuk");
  if(ix == 40) return("oslo");
  if(ix == 41) return("pace");
  if(ix == 42) return("quay");
  if(ix == 43) return("rome");
  if(ix == 44) return("sako");
  if(ix == 45) return("troy");
  if(ix == 46) return("ubly");
  if(ix == 47) return("vimy");
  if(ix == 48) return("waco");
  if(ix == 49) return("xane");
  if(ix == 50) return("york");
  if(ix == 51) return("zahl");
  return("vname1");
}

//---------------------------------------------------------
// Procedure: getIndexVName2()

string getIndexVName2(unsigned int ix)
{
  if(ix == 0) return("avi");
  if(ix == 1) return("bee");
  if(ix == 2) return("cap");
  if(ix == 3) return("dan");
  if(ix == 4) return("ebb");
  if(ix == 5) return("fay");
  if(ix == 6) return("geo");
  if(ix == 7) return("ham");
  if(ix == 8) return("ivy");
  if(ix == 9) return("jan");
  if(ix == 10) return("kay");
  if(ix == 11) return("lee");
  if(ix == 12) return("mel");
  if(ix == 13) return("nat");
  if(ix == 14) return("ott");
  if(ix == 15) return("pat");
  if(ix == 16) return("qua");
  if(ix == 17) return("ron");
  if(ix == 18) return("sid");
  if(ix == 19) return("tad");
  if(ix == 20) return("umm");
  if(ix == 21) return("vik");
  if(ix == 22) return("wik");
  if(ix == 23) return("xik");
  if(ix == 24) return("yee");
  if(ix == 25) return("zed");
  if(ix == 26) return("abby");
  if(ix == 27) return("bill");
  if(ix == 28) return("clem");
  if(ix == 29) return("dana");
  if(ix == 30) return("eddy");
  if(ix == 31) return("fran");
  if(ix == 32) return("gabe");
  if(ix == 33) return("hans");
  if(ix == 34) return("ivan");
  if(ix == 35) return("jade");
  if(ix == 36) return("kent");
  if(ix == 37) return("lacy");
  if(ix == 38) return("mary");
  if(ix == 39) return("noel");
  if(ix == 40) return("olga");
  if(ix == 41) return("paul");
  if(ix == 42) return("quin");
  if(ix == 43) return("rick");
  if(ix == 44) return("sage");
  if(ix == 45) return("theo");
  if(ix == 46) return("uber");
  if(ix == 47) return("vick");
  if(ix == 48) return("ward");
  if(ix == 49) return("xavi");
  if(ix == 50) return("yoel");
  if(ix == 51) return("zack");
  return("vname2");
}

//---------------------------------------------------------
// Procedure: getIndexVName3()

string getIndexVName3(unsigned int ix)
{
  if(ix == 0) return("ada");
  if(ix == 1) return("bob");
  if(ix == 2) return("cam");
  if(ix == 3) return("dee");
  if(ix == 4) return("ema");
  if(ix == 5) return("flo");
  if(ix == 6) return("gus");
  if(ix == 7) return("hal");
  if(ix == 8) return("ira");
  if(ix == 9) return("jax");
  if(ix == 10) return("kia");
  if(ix == 11) return("leo");
  if(ix == 12) return("mia");
  if(ix == 13) return("noa");
  if(ix == 14) return("ora");
  if(ix == 15) return("pam");
  if(ix == 16) return("qix");
  if(ix == 17) return("roy");
  if(ix == 18) return("sky");
  if(ix == 19) return("tom");
  if(ix == 20) return("una");
  if(ix == 21) return("van");
  if(ix == 22) return("wim");
  if(ix == 23) return("xyz");
  if(ix == 24) return("yip");
  if(ix == 25) return("zip");
  if(ix == 26) return("adel");
  if(ix == 27) return("bama");
  if(ix == 28) return("chad");
  if(ix == 29) return("dash");
  if(ix == 30) return("emma");
  if(ix == 31) return("fern");
  if(ix == 32) return("gary");
  if(ix == 33) return("hank");
  if(ix == 34) return("isla");
  if(ix == 35) return("joan");
  if(ix == 36) return("kyle");
  if(ix == 37) return("lisa");
  if(ix == 38) return("mona");
  if(ix == 39) return("nick");
  if(ix == 40) return("oral");
  if(ix == 41) return("page");
  if(ix == 42) return("quip");
  if(ix == 43) return("rice");
  if(ix == 44) return("seth");
  if(ix == 45) return("tony");
  if(ix == 46) return("ugly");
  if(ix == 47) return("vice");
  if(ix == 48) return("webb");
  if(ix == 49) return("xray");
  if(ix == 50) return("yara");
  if(ix == 51) return("zula");
  return("vname3");
}

//---------------------------------------------------------
// Procedure: getIndexVName4()

string getIndexVName4(unsigned int ix)
{
  if(ix == 0) return("art");
  if(ix == 1) return("bud");
  if(ix == 2) return("coy");
  if(ix == 3) return("doc");
  if(ix == 4) return("ell");
  if(ix == 5) return("fed");
  if(ix == 6) return("guy");
  if(ix == 7) return("hip");
  if(ix == 8) return("icy");
  if(ix == 9) return("jed");
  if(ix == 10) return("ken");
  if(ix == 11) return("lex");
  if(ix == 12) return("may");
  if(ix == 13) return("nim");
  if(ix == 14) return("oma");
  if(ix == 15) return("pig");
  if(ix == 16) return("qal");
  if(ix == 17) return("rob");
  if(ix == 18) return("sue");
  if(ix == 19) return("ted");
  if(ix == 20) return("unk");
  if(ix == 21) return("von");
  if(ix == 22) return("wam");
  if(ix == 23) return("xoo");
  if(ix == 24) return("yap");
  if(ix == 25) return("zap");
  if(ix == 26) return("arlo");
  if(ix == 27) return("brad");
  if(ix == 28) return("chip");
  if(ix == 29) return("doug");
  if(ix == 30) return("evan");
  if(ix == 31) return("ford");
  if(ix == 32) return("greg");
  if(ix == 33) return("harm");
  if(ix == 34) return("ivor");
  if(ix == 35) return("jack");
  if(ix == 36) return("kurt");
  if(ix == 37) return("lane");
  if(ix == 38) return("mack");
  if(ix == 39) return("nina");
  if(ix == 40) return("owen");
  if(ix == 41) return("pete");
  if(ix == 42) return("quem");
  if(ix == 43) return("rosa");
  if(ix == 44) return("stan");
  if(ix == 45) return("troy");
  if(ix == 46) return("ulan");
  if(ix == 47) return("vipp");
  if(ix == 48) return("wood");
  if(ix == 49) return("xelp");
  if(ix == 50) return("yoga");
  if(ix == 51) return("zeke");
  return("vname4");
}

//---------------------------------------------------------
// Procedure: getIndexVColor()

string getIndexVColor(unsigned int ix)
{
  if(ix == 0) return("yellow"); 
  if(ix == 1) return("red"); 
  if(ix == 2) return("dodger_blue"); 
  if(ix == 3) return("green"); 
  if(ix == 4) return("purple"); 
  if(ix == 5) return("orange"); 
  if(ix == 6) return("white"); 
  if(ix == 7) return("dark_green"); 
  if(ix == 8) return("dark_red"); 
  if(ix == 9) return("cyan"); 
  if(ix == 10) return("coral"); 
  if(ix == 11) return("brown"); 
  if(ix == 12) return("bisque"); 
  if(ix == 13) return("white"); 
  if(ix == 14) return("pink");
  if(ix == 15) return("darkslateblue"); 
  if(ix == 16) return("brown"); 
  if(ix == 17) return("burlywood"); 
  if(ix == 18) return("goldenrod"); 
  if(ix == 19) return("ivory"); 
  if(ix == 20) return("khaki"); 
  if(ix == 21) return("lime"); 
  if(ix == 22) return("peru"); 
  if(ix == 23) return("powderblue"); 
  if(ix == 24) return("plum"); 
  if(ix == 25) return("sienna"); 
  if(ix == 26) return("sandybrown"); 
  if(ix == 27) return("navy"); 
  if(ix == 28) return("olive"); 
  if(ix == 29) return("magenta"); 
  return("yellow");
}
