/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPFViewer.h                                          */
/*    DATE: May 21 2013                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef IPF_VIEWER_HEADER
#define IPF_VIEWER_HEADER

#include "BNG_Model.h"
#include "Common_IPFViewer.h"

class IPFViewer : public Common_IPFViewer
{
 public:
  IPFViewer(BNG_Model*,int x,int y,int w,int h,const char *l=0);
  ~IPFViewer() {};

  void   draw();

  void   setTypeIPF(const std::string&);
  void   resetIPF(int ix, int v);

 private:
  BNG_Model* m_cpa_model;

  int        m_view_type;
};

#endif 


