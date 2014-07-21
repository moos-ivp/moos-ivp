/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PostingBuffer.cpp                                    */
/*    DATE: September 24th, 201                                  */
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

#include "PostingBuffer.h"

using namespace std;

//-------------------------------------------------------------------
// Procedure: getPending

VarDataPair PostingBuffer::getPending(unsigned int ix) const
{
  VarDataPair empty_pair;
  if(ix < m_pairs.size())
    return(m_pairs[ix]);
  else
    return(empty_pair);
}

//-------------------------------------------------------------------
// Procedure: clear

void PostingBuffer::clear()
{
  m_pairs.clear();
}

//-------------------------------------------------------------------
// Procedure: pushPending

void PostingBuffer::pushPending(const string& var, const string& val)
{
  VarDataPair pair(var, val);
  pushPending(pair);
}


//-------------------------------------------------------------------
// Procedure: pushPending

void PostingBuffer::pushPending(const string& var, double val)
{
  VarDataPair pair(var, val);
  pushPending(pair);
}


//-------------------------------------------------------------------
// Procedure: pushPending

void PostingBuffer::pushPending(const VarDataPair& pair)
{
  m_pairs.push_back(pair);
}


//-------------------------------------------------------------------
// Procedure: merge

void PostingBuffer::merge(const PostingBuffer& pbuffer)
{
  unsigned int i, bsize = pbuffer.size();
  for(i=0; i<bsize; i++)
    pushPending(pbuffer.getPending(i));
}





