/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FFV_Viewer.cpp                                       */
/*    DATE: Apr 15th 2005                                        */
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

#include <iostream>
#include <cmath>
#include <cstdlib>
#include "FFV_Viewer.h"
#include "IPF_Utils.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "MBTimer.h"

#define GL_PI 3.1415926f

using namespace std;

//--------------------------------------------------------------
// Constructor

FFV_Viewer::FFV_Viewer(int x, int y, int wid, int hgt, const char *label)
  : Common_IPFViewer(x, y, wid, hgt, label)
{
  // Config params for building IPF
  m_patch_aof    = 5;         
  m_strict_range = true;
  m_create_time  = -1;
  m_piece_count  = 0;

  m_smart_refine    = false;
  m_directed_refine = false;
  m_autopeak_refine = false;
  m_uniform_piece_size = 10;

  m_aof        = 0;
  m_unif_ipf   = 0;

  // Set config params of the superclass
  setParam("polar", 0);
  setParam("set_scale", 1);
  setParam("reset_view", "1");
  setParam("clear_color", "white");
  setParam("frame_color", "gray");
  setParam("set_zoom", 3);

  m_refresh_quadset_ipf_pending = true;
  m_refresh_quadset_aof_pending = true;
}

//-------------------------------------------------------------
// Procedure: draw()

void FFV_Viewer::draw()
{
  Common_IPFViewer::draw();

  //cout << "============================================" << endl;
  //cout << "m_draw_base  = " << boolToString(m_draw_base) << endl;
  //cout << "m_draw_frame = " << boolToString(m_draw_frame) << endl;
  //cout << "m_scale = " << m_scale << endl;
  //cout << "============================================" << endl;

  glPushMatrix();
  glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);

  //cout << "m_base_aof: " << m_base_aof << endl;
  //cout << "m_base_ipf: " << m_base_ipf << endl;
  //cout << "m_scale: " << m_scale << endl;
  
  if(m_draw_ipf)
    drawIPF();
    
  if(m_draw_aof) 
    drawAOF();

  if(m_draw_frame) {
    if(m_polar)
      drawPolarFrame(true);
    else
      drawFrame(true);
  }
  
  // Restore transformations
  glPopMatrix();
  glFlush();

  GLenum err = glGetError();
  if(err != GL_NO_ERROR)
    cout << "WARNING!!!!! GL ERROR DETECTED!!!!" << endl;
}


//-------------------------------------------------------------
// Procedure: handle

int FFV_Viewer::handle(int event)
{
  return Common_IPFViewer::handle(event);
}

//-------------------------------------------------------------
// Procedure: setAOF

void FFV_Viewer::setAOF(AOF *aof)
{
  if((!aof) || (aof->getDomain().size() != 2))
    return;

  if(m_unif_ipf) {
    delete(m_unif_ipf); 
    m_unif_ipf = 0;
  }
  m_create_time = -1;

  m_aof = aof;
  m_rater.setAOF(aof);
}

//-------------------------------------------------------------
// Procedure: toggleSmartAug

void FFV_Viewer::toggleSmartAug()
{
  m_smart_refine = !m_smart_refine;

  if(m_unif_ipf)
    makeUniformIPF();
}

//-------------------------------------------------------------
// Procedure: setParam

bool FFV_Viewer::setParam(string param, string value)
{
  if(Common_IPFViewer::setParam(param, value))
    return(true);

  value = stripBlankEnds(value);
  if(param == "uniform_piece")
    m_uniform_piece_str = value;
  else if(param == "refine_region")
    m_refine_regions.push_back(value);
  else if(param == "refine_piece")
    m_refine_pieces.push_back(value);
  else if(param == "directed_refine") 
    return(setBooleanOnString(m_directed_refine, value));
  else if(param == "auto_peak") 
    return(setBooleanOnString(m_autopeak_refine, value));
  else if(param == "strict_range") 
    return(setBooleanOnString(m_strict_range, value));
  else
    return(false);

  return(true);
}

//-------------------------------------------------------------
// Procedure: setParam

bool FFV_Viewer::setParam(string param, double value)
{
  if(Common_IPFViewer::setParam(param, value))
    return(true);

  if(param == "uniform_amount")
    m_piece_count = (int)(value);
  else if((param == "smart_percent") && (value >= 0)) 
    m_smart_percent = value;
  else if((param == "smart_amount") && (value >= 0))
    m_smart_amount = value;
  else if(param == "uniform_piece") {
    if(value >= 1) {
      m_uniform_piece_size = (int)(value);
      m_uniform_piece_str  = "";
    }
  }
  else if(param == "mod_uniform_piece") {
    m_uniform_piece_size += (int)(value);
    m_uniform_piece_str  = "";
  }
  else
    return(false);

  redraw();
  return(true);
}

//-------------------------------------------------------------
// Procedure: printParams

void FFV_Viewer::printParams()
{
  cout << endl << endl;
  Common_IPFViewer::printParams();

  cout << "# app_ffview Viewer -----------------------" << endl;
  cout << "set_base_ipf=" << m_base_ipf << endl;
  cout << "set_base_aof=" << m_base_aof << endl;
}

//-------------------------------------------------------------
// Procedure: makeUniformIPFxN

void FFV_Viewer::makeUniformIPFxN(int iterations)
{
  MBTimer create_timer;
  create_timer.start();

  for(int i=0; i<iterations; i++) 
    makeUniformIPF();

  create_timer.stop();
  m_create_time = create_timer.get_float_cpu_time();
}  

//-------------------------------------------------------------
// Procedure: makeUniformIPF

void FFV_Viewer::makeUniformIPF()
{
  if(!m_aof)
    return;

  OF_Reflector reflector(m_aof, 1);

  IvPDomain domain = m_aof->getDomain();
  
  string dim0_name = domain.getVarName(0);
  string dim1_name = domain.getVarName(1);

  if(m_uniform_piece_str == "") {
    m_uniform_piece_str = "discrete @ ";
    m_uniform_piece_str += dim0_name + ":";
    m_uniform_piece_str += intToString(m_uniform_piece_size) + ",";
    m_uniform_piece_str += dim1_name + ":";
    m_uniform_piece_str += intToString(m_uniform_piece_size);
  }

  if(m_piece_count > 0) 
    reflector.setParam("uniform_amount", m_piece_count);
  else
    reflector.setParam("uniform_piece", m_uniform_piece_str);

  reflector.setParam("strict_range", boolToString(m_strict_range));
  reflector.setParam("auto_peak", boolToString(m_autopeak_refine));

  
  if(m_directed_refine) {
    cout << "In Directed refine: " << endl;
    cout << "refine_regions: " << m_refine_regions.size() << endl;
    cout << "refine_pieces:  " << m_refine_pieces.size() << endl;
    
    if(m_refine_regions.size() != m_refine_pieces.size()) {
      m_refine_regions.clear();
      m_refine_pieces.clear();
    }

    for(unsigned int i=0; i<m_refine_regions.size(); i++) {
      reflector.setParam("refine_region", m_refine_regions[i]);
      reflector.setParam("refine_piece",  m_refine_pieces[i]);
    }
  }
  if(m_smart_refine) {
    if(m_smart_percent != 0)
      reflector.setParam("smart_percent", m_smart_percent);
    else if(m_smart_amount != 0)
      reflector.setParam("smart_amount",  m_smart_amount);
  }

  if(reflector.stateOK()) {
    reflector.create();
    m_reflector_warnings = "";
  }
  else {
    m_reflector_warnings = reflector.getWarnings();
    cout << "Warnings: " << m_reflector_warnings << endl;
  }

  if(m_unif_ipf)
    delete(m_unif_ipf);
  // false means do not normalize as part of extractOF()
  m_unif_ipf = reflector.extractOF(false);
  m_refresh_quadset_ipf_pending = true;

  if(m_unif_ipf) {
    cout << "*********ipf is valid: " << boolToString(m_unif_ipf->valid()) << endl;
  }
								     
  if(m_unif_ipf && m_unif_ipf->getPDMap())
    m_rater.setPDMap(m_unif_ipf->getPDMap());
  redraw();
}

//-------------------------------------------------------------
// Procedure: modPatchAOF

void FFV_Viewer::modPatchAOF(int amt)
{
  m_patch_aof += amt; 
  if(m_patch_aof < 1)  
    m_patch_aof = 1; 
  m_refresh_quadset_aof_pending = true;
  redraw();
}

//-------------------------------------------------------------
// Procedure: getParam()

double FFV_Viewer::getParam(const string& param, bool&ok)
{
  if(!m_aof || !m_unif_ipf) {
    ok = false;
    return(0);
  }

  ok = true;

  int cnt = m_rater.getSampleCount();;

  if(param == "sample_count")
    return(cnt);
  
  if(param == "piece_count") {
    if(m_unif_ipf)
      return(m_unif_ipf->getPDMap()->size());
    else
      return(0);
  }
    
  double samp_high = m_rater.getSampHigh();
  double samp_low  = m_rater.getSampLow();
  double range     = samp_high - samp_low;
  double factor    = 100.0 / range;
    
  if((param == "sample_high") && cnt)
    return(samp_high);
  else if((param == "sample_low") && cnt)
    return(samp_low);
  else if((param == "avg_err") && cnt)
    return(m_rater.getAvgErr()*factor);
  else if((param == "worst_err") && cnt)
    return(m_rater.getWorstErr()*factor);
  else if((param == "squared_err") && cnt)
    return(sqrt(m_rater.getSquaredErr())*factor);
  else if((param == "create_time")) {
    if(m_create_time == -1)
      ok = false;
    return((double)(m_create_time));
  }

  ok = false;
  return(0);
}

//-------------------------------------------------------------
// Procedure: getParam()

string FFV_Viewer::getParam(const string& param)
{
  if(param == "uniform_piece")
    return(m_uniform_piece_str);
  else if(param == "reflector_errors")
    return(m_reflector_warnings);
  else if(param == "reflector_warnings")
    return(m_reflector_warnings);
  else if(param == "auto_peak")
    return(boolToString(m_autopeak_refine));
  
  return("");
}

//-------------------------------------------------------------
// Procedure: drawIPF

void FFV_Viewer::drawIPF()
{
  if(!m_unif_ipf)
    return;

  if(m_refresh_quadset_ipf_pending) {
    m_quadset_ipf = buildQuadSetFromIPF(m_unif_ipf);
    m_refresh_quadset_ipf_pending = false;

    m_quadset_ipf.normalize(0, 100);
    m_quadset_ipf.applyColorMap(m_color_map);
    m_quadset_ipf.applyColorIntensity(m_intensity);
    m_quadset_ipf.applyScale(m_scale);
    cout << "---- m_base_ipf: " << m_base_ipf << endl; 
    m_quadset_ipf.applyBase(m_base_ipf);
    m_quadset_ipf.interpolate(1);
    
    if(m_polar == 0)
      m_quadset_ipf.applyTranslation();
    else if(m_polar == 1)
      m_quadset_ipf.applyPolar(m_rad_ratio, 1);
    else if(m_polar == 2)
      m_quadset_ipf.applyPolar(m_rad_ratio, 2);
    
  }

  m_show_pieces = true;
  Common_IPFViewer::drawQuadSet(m_quadset_ipf);
}
    
//-------------------------------------------------------------
// Procedure: drawAOF

void FFV_Viewer::drawAOF()
{
  // Part 1 - Sanity Checks
  if(!m_aof)
    return;

  IvPDomain domain = m_aof->getDomain();
  int dim = domain.size();
  if((dim != 2) && (dim != 3))
    return;
  int xmax = domain.getVarPoints(0)-1;
  int ymax = domain.getVarPoints(1)-1;

  //cout << "In drawAOF..............." << endl;

  if(m_refresh_quadset_aof_pending) {
    m_quadset_aof = buildQuadSetDense2DFromAOF(m_aof, m_patch_aof);
    m_refresh_quadset_aof_pending = false;
    
    m_quadset_aof.normalize(0, 100);
    m_quadset_aof.applyColorMap(m_color_map);
    m_quadset_aof.applyColorIntensity(m_intensity);
    m_quadset_aof.applyScale(m_scale);
    m_quadset_aof.applyBase(m_base_aof);
    m_quadset_aof.interpolate(1);
    
    if(m_polar == 0)
      m_quadset_aof.applyTranslation(-(xmax/2), -(ymax/2));
    else if(m_polar == 1)
      m_quadset_aof.applyPolar(m_rad_ratio, 1);
    else if(m_polar == 2)
      m_quadset_aof.applyPolar(m_rad_ratio, 2);
  }
    
  bool draw_pclines_save = m_draw_pclines;
  bool show_pieces_save = m_draw_pclines;
  m_draw_pclines = false;
  m_show_pieces = false;

  Common_IPFViewer::drawQuadSet(m_quadset_aof);

  m_draw_pclines = draw_pclines_save;
  m_show_pieces = show_pieces_save;
}

