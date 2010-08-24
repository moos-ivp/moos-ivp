/*****************************************************************/
/*    NAME: Michael Benjamin, H. Schmidt, J. Leonard             */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: DomQuery.h                                           */
/*    DATE: Mar 10th, 2009                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef DOM_QUERY_HEADER
#define DOM_QUERY_HEADER

#include <vector>
#include <string>
#include "IvPDomain.h"

class DomQuery
{
 public:
  DomQuery();
  ~DomQuery() {};

  void buildDomain();
  void setInFile(std::string s) {m_infile=s;};  

  void requestPieceCount(int);
  
 protected:
  bool verifyInfile();

 protected:

  std::string m_infile;
  IvPDomain   m_domain;

};

#endif 
