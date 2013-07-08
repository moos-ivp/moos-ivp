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

#include "IvPFunction.h"
#include "BFactoryStatic.h"
#include "BFactoryDynamic.h"
#include "PairModel.h"
#include "Common_IPFViewer.h"

class IPFViewer : public Common_IPFViewer
{
 public:
  IPFViewer(PairModel*,int x,int y,int w,int h,const char *l=0);
  ~IPFViewer() {};

  void   draw();

  void   resetIPF(unsigned int v, bool full_reset=true);

  std::string getInfo(std::string);

 protected:
  void   resetIPF_AVD(unsigned int v);
  void   resetIPF_COL(unsigned int v, bool full_reset);

 private:
  PairModel       *m_pair_model;
  IvPBehavior     *m_behavior;
  IvPDomain        m_domain;
  InfoBuffer      *m_info_buffer;

  BFactoryStatic   m_bhv_factory_static;
  BFactoryDynamic  m_bhv_factory_dynamic;

  int              m_view_type;
};

#endif 
