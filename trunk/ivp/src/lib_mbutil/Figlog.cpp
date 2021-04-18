/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Figlog.cpp                                           */
/*    DATE: Dec 16th 2010                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include "Figlog.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: getMessage

string Figlog::getMessage(unsigned int ix)
{
  if(ix >= m_messages.size())
    return("");
  return(m_messages[ix]);
}

//----------------------------------------------------------------
// Procedure: getWarning

string Figlog::getWarning(unsigned int ix)
{
  if(ix >= m_warnings.size())
    return("");
  return(m_warnings[ix]);
}

//----------------------------------------------------------------
// Procedure: getError

string Figlog::getError(unsigned int ix)
{
  if(ix >= m_warnings.size())
    return("");
  return(m_warnings[ix]);
}

//----------------------------------------------------------------
// Procedure: augMessage

void Figlog::augMessage(string msg)
{
  unsigned int vsize = m_messages.size();
  if(vsize == 0)
    m_messages.push_back(msg);
  else
    m_messages[vsize-1] += msg;
}

//----------------------------------------------------------------
// Procedure: augWarning

void Figlog::augWarning(string msg)
{
  unsigned int vsize = m_warnings.size();
  if(vsize == 0)
    m_warnings.push_back(msg);
  else
    m_warnings[vsize-1] += msg;
}

//----------------------------------------------------------------
// Procedure: augError

void Figlog::augError(string msg)
{
  unsigned int vsize = m_errors.size();
  if(vsize == 0)
    m_errors.push_back(msg);
  else
    m_errors[vsize-1] += msg;
}

//----------------------------------------------------------------
// Procedure: clear

void Figlog::clear()
{
  m_messages.clear();
  m_warnings.clear();
  m_errors.clear();
}

//----------------------------------------------------------------
// Procedure: print

void Figlog::print() const
{
  cout << "=====================================================" << endl;
  cout << "Figlog Summary: ";
  if(m_label == "")
    cout << endl;
  else 
    cout << "(" << m_label << ")" << endl;

  unsigned int i, isize = m_messages.size();
  unsigned int j, jsize = m_warnings.size();
  unsigned int k, ksize = m_errors.size();

  cout << "Messages: (" << isize << ")" << endl;
  for(i=0; i<isize; i++) 
    cout << "  [" << i << "] " << m_messages[i] << endl;

  cout << "Warnings: (" << jsize << ")" << endl;
  if(jsize > 0) {
    cout << termColor("blue") << flush;
    for(j=0; j<jsize; j++) 
      cout << "  [" << j << "] " << m_warnings[j] << endl;
    cout << termColor() << flush;
  }
  
  cout << "Errors: (" << jsize << ")" << endl;
  if(ksize > 0) {
    cout << termColor("red") << flush;
    for(k=0; k<ksize; k++) 
      cout << "  [" << k << "] " << m_errors[k] << endl;
    cout << termColor() << flush;
  }
  cout << "=====================================================" << endl;
  
}









