/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: VoiceUtils.cpp                                       */
/*    DATE: May 27th, 2013                                       */
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

#include "MBUtils.h"
#include "VoiceUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: isVoice

bool isVoice(string voice)
{
  voice = tolower(voice);
  if((voice == "agnes")      ||
     (voice == "albert")     ||
     (voice == "alex")       ||
     (voice == "bad news")   ||
     (voice == "bad_news")   ||
     (voice == "bahh")       ||
     (voice == "bells")      ||
     (voice == "boing")      ||
     (voice == "bruce")      ||
     (voice == "bubbles")    ||
     (voice == "cellos")     ||
     (voice == "deranged")   ||
     (voice == "fred")       ||
     (voice == "good_news")  ||
     (voice == "hysterical") ||
     (voice == "junior")     ||
     (voice == "kathy")      ||
     (voice == "pipe organ") ||
     (voice == "pipe_organ") ||
     (voice == "princess")   ||
     (voice == "ralph")      ||
     (voice == "trinoids")   ||
     (voice == "vicki")      ||
     (voice == "victoria")   ||
     (voice == "whisper")    ||
     (voice == "en-us")      ||
     (voice == "en-sc")      ||
     (voice == "zarvox")) {
    return(true);
  }
  return(false);
}








