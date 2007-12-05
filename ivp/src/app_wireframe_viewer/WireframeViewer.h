/*****************************************************************/
/*    NAME: Doug Maxwell - Mike Benjamin                         */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: WireframeViewer.h                                    */
/*    DATE: November 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef WIREFRAME_VIEWER_HEADER
#define WIREFRAME_VIEWER_HEADER

#include <FL/Fl_Gl_Window.H>

class WireframeViewer : public Fl_Gl_Window{
 public:
  WireframeViewer(int x, int y, int wid, int hgt, char* title=0);
  virtual ~WireframeViewer() {};

  virtual void draw();
  virtual int handle(int event);

  void  InitializeGL();
  void  DrawAxes();
  void  DrawKayak(float, float, float);

  bool  isAnimating()          {return(m_animating);};
  bool  isCOI()		       {return(m_COI);};
  float getRotationIncrement() {return(m_rotation_increment);};
  float getRotationAngle()     {return(m_rotation);};
  float getZoom()	       {return(m_zoom);};
  float getCameraPitch()       {return(m_camera_pitch);};
  float getCameraYaw()         {return(m_camera_yaw);};

  void  setRotation(float v)   {m_rotation  = v;};
  void  setCameraX(float v)    {m_camera_x  = v;};
  void  setCameraY(float v)    {m_camera_y  = v;};
  void  setCameraZ(float v)    {m_camera_z  = v;};
  void  incCameraX(float v)    {m_camera_x += v;};
  void  incCameraY(float v)    {m_camera_y += v;};
  void  incCameraZ(float v)    {m_camera_z += v;};

  void  setTargetX(float v)    {m_target_x = v;};
  void  setTargetY(float v)    {m_target_y = v;};
  void  setTargetZ(float v)    {m_target_z = v;};

  void  modZoom(float v) {};
  void  reset();

  void  setViewMode(int v)     {m_view_mode = v;};
  void  setCameraYaw(float);
  void  incCameraYaw(float);
  void  setCameraPitch(float);
  void  incCameraPitch(float);
  void  setCameraAngle();
  void  setCOI(float, float, float);
  void  translateCamera(float, float, float);

  void  toggleAnimation() {m_animating = !m_animating;};
  void  toggleCOI() {m_COI = !m_COI;};

 //protected:
  float m_rotation;
  float m_rotation_increment;
  bool  m_animating;
  bool  m_COI;

  float m_camera_x;
  float m_camera_y;
  float m_camera_z;

  float m_camera_pitch;  // 0:down  180:up
  float m_camera_yaw;    // 0:north 270:west
  float m_camera_roll;

  float m_target_x;
  float m_target_y;
  float m_target_z;

  int   m_view_mode;
 
  float m_zoom;

};

#endif
