/************************************************************/
/*  NAME: Mike Benjamin                                     */
/*  ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA  */
/*  FILE: VoiceUtils.cpp                                    */
/*  DATE: May 27th, 2013                                    */
/************************************************************/

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
     (voice == "zarvox")) {
    return(true);
  }
  return(false);
}


