/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Utterance.cpp                                        */
/*    DATE: May 20, 2013                                         */
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
#include "Utterance.h"
#include "VoiceUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor

Utterance::Utterance()
{
  init();
}

//----------------------------------------------------------
// Constructor

Utterance::Utterance(std::string str)
{
  init();
  initFromString(str);
}

//----------------------------------------------------------
// Procedure: init

void Utterance::init()
{
  m_rate      = 0;        // indicates no preference
  m_priority  = 0;        // indicates no priority implied
  m_top_priority = false;

  m_time_received = 0;  
  m_time_posted   = 0;  
}

//----------------------------------------------------------
// Procedure: initFromString

bool Utterance::initFromString(std::string str)
{
  // Simple case supported: "hello"
  if(!strContains(tolower(str), "say={") && 
     !strContains(tolower(str), "file=") && 
     !strContains(tolower(str), "say=\"")) {
    m_text = str;
    return(true);
  }

  // General case: "say={hello}"
  vector<string> svector = parseStringZ(str, ',', "{");
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "say") {
      if(isQuoted(value))
	value = stripQuotes(value);
      if(isBraced(value))
	value = stripBraces(value);
      m_text = value;
    }
    else if((param == "rate") && isNumber(value))
      m_rate = atof(value.c_str());
    else if((param == "priority") && isNumber(value))
      m_priority = atof(value.c_str());
    else if((param == "priority") && (tolower(value) == "top"))
      m_top_priority = true;
    else if(param == "source")
      m_source = value;
    else if(param == "file")
      m_file = value;
    else if(param == "voice") {
      if(isVoice(value))
	m_voice = tolower(value);
    }
    else
      return(false);
  }

  return(true);
}

//----------------------------------------------------------
// Procedure: setVoice

bool Utterance::setVoice(string voice)
{
  if(!isVoice(voice))
    return(false);
  
  m_voice = true;
  return(true);
}


//----------------------------------------------------------
// Procedure: getSpec

string Utterance::getSpec() const
{
  string str;
  
  if(m_text != "")
    str += "say={" + m_text + "}";
  if(m_voice != "") 
    str = augmentSpec(str, "voice="+m_voice);
  if(m_file != "") 
    str = augmentSpec(str, "file="+m_file);
  if(m_source != "") 
    str = augmentSpec(str, "source="+m_source);
  if(m_node != "") 
    str = augmentSpec(str, "node="+m_node);
  if(m_rate != 0) 
    str = augmentSpec(str, "rate="+doubleToStringX(m_rate));

  if(m_top_priority) 
    str = augmentSpec(str, "priority=top");
  else if(m_priority != 0) 
    str = augmentSpec(str, "priority="+doubleToStringX(m_priority));
  

  return(str);
}
    
//----------------------------------------------------------
// Procedure: getType

string Utterance::getType() const
{
  if((m_text != "") && (m_file != ""))
    return("mixed");
  if(m_file != "")
    return("file");
  if(m_text != "")
    return("text");
  return("empty");
}

//---------------------------------------------------------
// Overload the < operator
bool operator< (const Utterance& utter1, const Utterance& utter2)
{
  return(utter1.getPriority() > utter2.getPriority());
}

//---------------------------------------------------------
// Overload the > operator
bool operator> (const Utterance& utter1, const Utterance& utter2)
{
  return(utter1.getPriority() < utter2.getPriority());
}






