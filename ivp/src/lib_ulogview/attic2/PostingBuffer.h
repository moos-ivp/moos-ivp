/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PostingBuffer.h                                      */
/*    DATE: September 24th, 201                                  */
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

#ifndef POSTING_BUFFER_HEADER
#define POSTING_BUFFER_HEADER

#include <string>
#include <vector>
#include "VarDataPair.h"

class PostingBuffer  {
public:
  PostingBuffer() {};
  ~PostingBuffer() {};

  VarDataPair  getPending(unsigned int index) const;
  void         clear();

  void         pushPending(const std::string&, const std::string&);
  void         pushPending(const std::string&, double);
  void         pushPending(const VarDataPair&);

  void         merge(const PostingBuffer&);

  unsigned int size() const {return(m_pairs.size());};
  
protected:
  std::vector<VarDataPair> m_pairs;
};
#endif

