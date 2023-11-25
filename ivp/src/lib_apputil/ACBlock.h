/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ACBlock.h                                            */
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

#ifndef ACBLOCK_HEADER
#define ACBLOCK_HEADER

#include <string>
#include <vector>

class ACBlock
{
 public:
  ACBlock();
  ACBlock(std::string, std::string, unsigned int, char c=',');
  virtual ~ACBlock() {}
  
 public: // Setters
  void setLabel(std::string s)    {m_label=s;}
  void setMessage(std::string s)  {m_message=s;}
  bool setColor(std::string);
  void setSeparator(char c)       {m_separator=c;}
  void setMaxLen(unsigned int v)  {m_maxlen=v;}

 public: // Getters
  std::string  getLabel() const      {return(m_label);}
  std::string  getMessage() const    {return(m_message);}
  std::string  getColor() const      {return(m_color);}
  char         getSeparator() const  {return(m_separator);}
  unsigned int getMaxLen() const     {return(m_maxlen);}

 public: // Main function:
  std::vector<std::string> getFormattedLines() const;
  std::string getFormattedString() const;

 private: 
  std::string  m_label;
  std::string  m_message;
  std::string  m_color;
  char         m_separator;
  unsigned int m_maxlen;
};

#endif 











