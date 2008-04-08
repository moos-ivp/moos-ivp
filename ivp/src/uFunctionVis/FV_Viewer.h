/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: FV_Viewer.h                                         */
/*    DATE: May 12th 2006                                       */
/****************************************************************/

#ifndef IPF_VIEWER_HEADER
#define IPF_VIEWER_HEADER

#include "FV_Model.h"
#include "Common_IPFViewer.h"

class FV_Viewer : public Common_IPFViewer
{
 public:
  FV_Viewer(int x, int y, int w, int h, const char *l=0);
  ~FV_Viewer() {};
  
  // Pure virtuals that need to be defined
  void draw() 
    {return Common_IPFViewer::draw();};
  
  int  handle(int event)
    {return Common_IPFViewer::handle(event);};


public:
  void  setModel(FV_Model *m) {m_model = m;};
  void  resetQuadSet();

private:

  FV_Model*  m_model;
};

#endif 
