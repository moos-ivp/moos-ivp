/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ACBlock.cpp                                          */
/*    DATE: Aug 30th 2012                                        */
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

#include <sstream>
#include <iostream>
#include "MBUtils.h"
#include "ACBlock.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

ACBlock::ACBlock()
{
  m_separator = ',';
  m_maxlen    = 0;
}

//----------------------------------------------------------------
// Constructor

ACBlock::ACBlock(string label, string msg, unsigned int maxlen, char c)
{
  m_label     = label;
  m_message   = msg;
  m_separator = c;
  m_maxlen    = maxlen;
}

//----------------------------------------------------------------
// Procedure: setColor

bool ACBlock::setColor(string color)
{
  if(!isTermColor(color))
    return(false);

  m_color = color;
  return(true);
}

//----------------------------------------------------------------
// Procedure: getFormattedLines
//   Example: 
//
//  PHI_HOST_INFO:       community=henry,hostip=localhost,
//                       port_db=9001,port_udp=9201,timewarp=2

vector<string> ACBlock::getFormattedLines() const
{
  vector<string> rvector;

  // Part 1: Handle special case where the message is an empty string
  if(m_message == "") {
    rvector.push_back(m_label);
    return(rvector);
  }

  // Part 2: Handle the general case where the message is non-empty

  // Build an "empty label" simply for formating lines after line 1
  unsigned int label_len = m_label.length();
  string empty_label(label_len, ' ');

  vector<string> svector = parseStringQ(m_message, m_separator, m_maxlen);
  for(unsigned int i=0; i<svector.size(); i++) {
    if(i==0) 
      rvector.push_back(m_label + svector[i]);
    else
      rvector.push_back(empty_label + svector[i]);
  }
  
  return(rvector);
}

//----------------------------------------------------------------
// Procedure: getFormattedString
//   Example: 
//
//  PHI_HOST_INFO:       community=henry,hostip=localhost,
//                       port_db=9001,port_udp=9201,timewarp=2

string ACBlock::getFormattedString() const
{
  stringstream ss;

  // Part 1: Handle special case where the message is an empty string
  if(m_message == "") 
    return(m_label);

  // Part 2: Handle the general case where the message is non-empty
  // Build an "empty label" simply for formating lines after line 1
  unsigned int label_len = m_label.length();
  string empty_label(label_len, ' ');

  vector<string> svector = parseStringQ(m_message, m_separator, m_maxlen);
  for(unsigned int i=0; i<svector.size(); i++) {
    if(i==0) 
      ss << (m_label + svector[i]) << endl;
    else
      ss << (empty_label + svector[i]) << endl;
  }
  
  return(ss.str());
}










