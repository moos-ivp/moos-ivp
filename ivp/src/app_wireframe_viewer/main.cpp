/*****************************************************************/
/*    NAME: Doug Maxwell - Mike Benjamin                         */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: November 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <FL/Fl.H>
#include "WireframeViewer.h"
#include "GUI.h"

//-----------------------------------------------------------
// Procedure: IdleCallBack

void IdleCallback(void* pData)
{
  if(pData != 0) {
    WireframeViewer* pWindow = reinterpret_cast<WireframeViewer*>(pData);
    if(pWindow->isAnimating()) {
      float increment = pWindow->getRotationIncrement();
      float rotation  = pWindow->getRotationAngle();
      float new_rval  = rotation + increment / 360;

      pWindow->setRotation(new_rval);
      pWindow->redraw();
    }
  }
}


//-----------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  GUI *gui = new GUI(800,600, "WireframeViewer");

  Fl::add_idle(IdleCallback, gui->viewer);
  Fl::run();

  return(0);
}

