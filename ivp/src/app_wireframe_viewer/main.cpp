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

#include <iostream>
#include <fstream>
#include <FL/Fl.H>
#include "WireframeViewer.h"
#include "GUI.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: IdleCallBack

void IdleCallback(void* pData)
{
  if(pData != 0) {
    WireframeViewer* pWindow = reinterpret_cast<WireframeViewer*>(pData);
    if(pWindow->isAnimating()) {
      float increment = pWindow->getRotationIncrement();
      float rotation  = pWindow->getRotationAngle();
      float new_rval  = rotation + increment;

      pWindow->setRotation(new_rval);
      pWindow->redraw();
    }
  }
}


//-----------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[]){
  string key;
  float temp = 0;
  float temp_x = 0, temp_y = 0, temp_z = 0, 
	temp_xx = 0, temp_yy = 0, temp_zz = 0,
	temp_red = 0, temp_green = 0, temp_blue = 0, temp_alpha = 0;
  int circle_counter = 0, point_counter = 0, line_counter = 0, 
	viewpoint_counter = 0, arc_counter = 0, kayak_counter = 0;

  GUI *gui = new GUI(1000,600, "WireframeViewer");

  Fl::add_idle(IdleCallback, gui->viewer);

  if(argc != 2)
	cout << "Use File->Open to access a data file."  << endl;
  else{
	ifstream inFile( argv[1], ios::in);
	if(!inFile){  cout << argv[1] << " could not be accessed." << endl;return -1;}
	cout << argv[1] << " is valid" << endl;
	while(!inFile.eof()){
          inFile >> key >> temp >> 
	  temp_x >> temp_y >> temp_z >> 
	  temp_xx >> temp_yy >> temp_zz >>
	  temp_red >> temp_green >> temp_blue >> temp_alpha;
	if( key == "circle" ){
        gui->viewer->m_circle_radius[circle_counter] = temp;
        gui->viewer->m_circle_x[circle_counter] = temp_x;
	gui->viewer->m_circle_y[circle_counter] = temp_y;
	gui->viewer->m_circle_z[circle_counter] = temp_z;
	gui->viewer->m_circle_red[circle_counter] = temp_red;
	gui->viewer->m_circle_green[circle_counter] = temp_green;
	gui->viewer->m_circle_blue[circle_counter] = temp_blue;
	gui->viewer->m_circle_alpha[circle_counter] = temp_alpha;
        gui->viewer->circle[circle_counter].drawCircle(
		gui->viewer->m_circle_radius[circle_counter], 
		gui->viewer->m_circle_x[circle_counter], 
		gui->viewer->m_circle_y[circle_counter], 
		gui->viewer->m_circle_z[circle_counter],
		gui->viewer->m_circle_red[circle_counter],
		gui->viewer->m_circle_green[circle_counter],
		gui->viewer->m_circle_blue[circle_counter],
		gui->viewer->m_circle_alpha[circle_counter]);		
        circle_counter++;
      	}
	if( key == "arc" ){
        gui->viewer->m_arc_radius[arc_counter] = temp;
        gui->viewer->m_arc_x[arc_counter] = temp_x;
	gui->viewer->m_arc_y[arc_counter] = temp_y;
	gui->viewer->m_arc_z[arc_counter] = temp_z;
	gui->viewer->m_arc_angle_start[arc_counter] = temp_xx;
	gui->viewer->m_arc_angle_end[arc_counter] = temp_yy;
	gui->viewer->m_arc_red[arc_counter] = temp_red;
	gui->viewer->m_arc_green[arc_counter] = temp_green;
	gui->viewer->m_arc_blue[arc_counter] = temp_blue;
	gui->viewer->m_arc_alpha[arc_counter] = temp_alpha;
        gui->viewer->arc[arc_counter].drawArc(
		gui->viewer->m_arc_radius[arc_counter], 
		gui->viewer->m_arc_x[arc_counter], 
		gui->viewer->m_arc_y[arc_counter], 
		gui->viewer->m_arc_z[arc_counter],
		gui->viewer->m_arc_angle_start[arc_counter],
		gui->viewer->m_arc_angle_end[arc_counter],
		gui->viewer->m_arc_red[arc_counter],
		gui->viewer->m_arc_green[arc_counter],
		gui->viewer->m_arc_blue[arc_counter],
		gui->viewer->m_arc_alpha[arc_counter]);	
        arc_counter++;
      }
      else if( key == "point" ){
	gui->viewer->m_point_x[point_counter] = temp_x;
	gui->viewer->m_point_y[point_counter] = temp_y;
	gui->viewer->m_point_z[point_counter] = temp_z;
	gui->viewer->m_point_red[point_counter] = temp_red;
	gui->viewer->m_point_green[point_counter] = temp_green;
	gui->viewer->m_point_blue[point_counter] = temp_blue;
	gui->viewer->m_point_alpha[point_counter] = temp_alpha;
	gui->viewer->point[point_counter].drawPoint(
		gui->viewer->m_point_x[point_counter],
		gui->viewer->m_point_y[point_counter],
		gui->viewer->m_point_z[point_counter],
		gui->viewer->m_point_red[point_counter],
		gui->viewer->m_point_green[point_counter],
		gui->viewer->m_point_blue[point_counter],
		gui->viewer->m_point_alpha[point_counter]);
	point_counter++;
      }      
      else if( key == "kayak" ){
	gui->viewer->m_kayak_cent_x[kayak_counter] = temp_x;
	gui->viewer->m_kayak_cent_y[kayak_counter] = temp_y;
	gui->viewer->m_kayak_cent_z[kayak_counter] = temp_z;
	gui->viewer->m_kayak_red[kayak_counter] = temp_red;
	gui->viewer->m_kayak_green[kayak_counter] = temp_green;
	gui->viewer->m_kayak_blue[kayak_counter] = temp_blue;
	gui->viewer->m_kayak_alpha[kayak_counter] = temp_alpha;
	gui->viewer->kayak[kayak_counter].drawKayak(
		gui->viewer->m_kayak_cent_x[kayak_counter],
		gui->viewer->m_kayak_cent_y[kayak_counter],
		gui->viewer->m_kayak_cent_z[kayak_counter],
		gui->viewer->m_kayak_red[kayak_counter],
		gui->viewer->m_kayak_green[kayak_counter],
		gui->viewer->m_kayak_blue[kayak_counter],
		gui->viewer->m_kayak_alpha[kayak_counter]);
	kayak_counter++;
	gui->viewer->m_kayak_counter++;
      }
      else if( key == "line" ){
	gui->viewer->m_line_start_x[line_counter] = temp_x;
	gui->viewer->m_line_start_y[line_counter] = temp_y;
	gui->viewer->m_line_start_z[line_counter] = temp_z;
	gui->viewer->m_line_end_x[line_counter]   = temp_xx;
	gui->viewer->m_line_end_y[line_counter]   = temp_yy;
	gui->viewer->m_line_end_z[line_counter]   = temp_zz;
	gui->viewer->m_line_red[line_counter] 	= temp_red;
	gui->viewer->m_line_green[line_counter] 	= temp_green;
	gui->viewer->m_line_blue[line_counter] 	= temp_blue;
	gui->viewer->m_line_alpha[line_counter] 	= temp_alpha;
	gui->viewer->line[line_counter].drawLine(
		gui->viewer->m_line_start_x[line_counter],
		gui->viewer->m_line_start_y[line_counter],
		gui->viewer->m_line_start_z[line_counter],
		gui->viewer->m_line_end_x[line_counter],
		gui->viewer->m_line_end_y[line_counter],
		gui->viewer->m_line_end_z[line_counter],
		gui->viewer->m_line_red[line_counter],
		gui->viewer->m_line_green[line_counter],
		gui->viewer->m_line_blue[line_counter],
		gui->viewer->m_line_alpha[line_counter]);
	line_counter++;
      }
	}
  }


  Fl::run();

  return(0);
}
