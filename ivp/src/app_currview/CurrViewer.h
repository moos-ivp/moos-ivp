/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: MIT Cambridge MA                                     */
/*          Dept of Mechanical Engineering,                      */
/*          Computer Science and Artificial Intelligence Lab     */
/*    FILE: CurrViewer.h                                         */
/*    DATE: January 4th, 2011                                    */
/*****************************************************************/

#ifndef CURR_VIEWER_HEADER
#define CURR_VIEWER_HEADER

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "MarineViewer.h"
#include "CurrentField.h"

class CurrViewer : public MarineViewer
{
 public:
  CurrViewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int);
  void  handle_left_mouse(int, int);
  void  handle_right_mouse(int, int);
  bool  setParam(std::string p, std::string v)
    {return(MarineViewer::setParam(p,v));};
  bool  setParam(std::string p, double v);

public:
  bool   initCurrentField(std::string filename);
  bool   fileSave();
  bool   fileSaveAs();
  void   shiftHorzVector(double);
  void   shiftVertVector(double);
  void   rotateVector(double);
  void   growVector(double);
  void   clearMarked();
  void   deleteMarked();
  void   deleteLastAdded();
  int    getMode()                 {return(m_drop_mode);};
  double getSnap()                 {return(m_snap_val);};
  void   applySnap();   
  void   incrementActive();
  void   decrementActive();

  std::string getInfo(std::string param, int precision);

 private:
  unsigned int  m_active_vector;

  double  m_snap_val;
  int     m_drop_mode;

  CurrentField m_current_field;
  std::string  m_filename;
};

#endif 

