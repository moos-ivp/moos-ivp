/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Viewer.cpp                                           */
/*    DATE: Apr 15th 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "math.h"
#include <stdlib.h>
#include "Viewer.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "MBTimer.h"

#define GL_PI 3.1415926f

using namespace std;

//--------------------------------------------------------------
// Constructor

Viewer::Viewer(int x, int y, 
	       int width, int height, const char *l)
  : Common_IPFViewer(x,y,width,height,l)
{
  m_base_aof   = -100;      // For shifting the AOF rendering
  m_base_ipf   =  150;      // For shifting the IPF rendering
  m_patch      = 5;         // Size of patch rendering the AOF
  m_draw_aof   = true;
  m_draw_ipf   = true;
  m_unif_ipf   = 0;
  m_unifsize   = 10;
  m_polar      = 0;
  m_strict_rng = true;
  m_create_time = -1;

  setParam("set_scale", 1);
  setParam("reset_view", "1");
  setParam("clear_color", "white");
  setParam("frame_color", "gray");

  m_smart_refine   = false;
  m_smart_count    = 1000;

  m_focus_box      = false;
  m_focus_box_x    = 0;
  m_focus_box_y    = 0;
  m_focus_box_len  = 100;
  m_focus_unif_len = 5;

  m_zoom = m_zoom * 1.25 * 1.25;  // Two zoom clicks in.
}

//-------------------------------------------------------------
// Procedure: draw()

void Viewer::draw()
{
  Common_IPFViewer::draw();

  glPushMatrix();
  glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);

  if(m_draw_ipf && m_unif_ipf)
    drawIvPFunction(m_unif_ipf);

  if(m_draw_aof) {
    if(m_aof_cache.getAOF())
      drawAOF();
  }

  if(m_draw_frame) {
    drawFrame();
    //drawFocusBox();
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

int Viewer::handle(int event)
{
  return Common_IPFViewer::handle(event);
}

//-------------------------------------------------------------
// Procedure: setAOF

void Viewer::setAOF(AOF *aof)
{
  if(!aof)
    return;

  m_domain = aof->getDomain();

  if(m_domain.size() != 2)
    return;

  if(m_unif_ipf) {
    delete(m_unif_ipf); 
    m_unif_ipf = 0;
  }
  m_create_time = -1;

  m_aof_cache.setAOF(aof);
  m_rater.setAOF(aof);
  
  int dim_0_size = m_domain.getVarPoints(0);
  int dim_1_size = m_domain.getVarPoints(0);

  m_focus_box_x    = dim_0_size / 2;
  m_focus_box_y    = dim_1_size / 2;

  //cout << "m_focus_box_x: "    << m_focus_box_x << endl;
  //cout << "m_focus_box_y: "    << m_focus_box_y << endl;
  //cout << "m_focus_box_len: "  << m_focus_box_len << endl;
  //cout << "m_focus_unif_len: " << m_focus_unif_len << endl;
}

//-------------------------------------------------------------
// Procedure: setParam

bool Viewer::setParam(string param, string value)
{
  if(Common_IPFViewer::setParam(param, value))
    return(true);

  return(true);
}

//-------------------------------------------------------------
// Procedure: setParam

bool Viewer::setParam(string param, double value)
{
  if(Common_IPFViewer::setParam(param, value))
    return(true);

  if(param == "set_base_ipf")
    m_base_ipf = value;
  else if(param == "mod_base_ipf")
    m_base_ipf += value;
  else if(param == "set_base_aof")
    m_base_aof = value;
  else if(param == "mod_base_aof")
    m_base_aof += value;
  else if(param == "set_scale") {
    m_scale = value;
    if(m_scale < 0)
      m_scale = 0;
  }
  else if(param == "mod_scale") {
    m_scale += value;
    if(m_scale < 0)
      m_scale = 0;
  }
  else
    return(false);

  redraw();
  return(true);
}

//-------------------------------------------------------------
// Procedure: printParams

void Viewer::printParams()
{
  cout << endl << endl;
  Common_IPFViewer::printParams();

  cout << "# app_ffview Viewer -----------------------" << endl;
  cout << "set_base_ipf=" << m_base_ipf << endl;
  cout << "set_base_aof=" << m_base_aof << endl;
}


//-------------------------------------------------------------
// Procedure: toggleUniformAug

void Viewer::toggleUniformAug()
{
  m_focus_box = !m_focus_box;

  if(m_unif_ipf)
    makeUniformIPF();
}


//-------------------------------------------------------------
// Procedure: toggleSmartAug

void Viewer::toggleSmartAug()
{
  m_smart_refine = !m_smart_refine;

  if(m_unif_ipf)
    makeUniformIPF();
}

//-------------------------------------------------------------
// Procedure: makeUniformIPFxN

void Viewer::makeUniformIPFxN(int iterations)
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

void Viewer::makeUniformIPF(int usize)
{
  AOF *aof = m_aof_cache.getAOF();
  if(!aof)
    return;

  if(usize <= 0)
    usize = m_unifsize;
  else
    m_unifsize = usize;

  OF_Reflector reflector(aof, 1);

  bool ok = true;
  string dim0_name = m_domain.getVarName(0);
  string dim1_name = m_domain.getVarName(1);

  // Example String: "discrete @ x:40,y:20"
  string unif_str = "discrete @ ";
  unif_str += dim0_name + ":" + intToString(usize) + ",";
  unif_str += dim1_name + ":" + intToString(usize);

  ok = ok && reflector.setParam("uniform_piece", unif_str);

  if(m_strict_rng)
    ok = ok && reflector.setParam("strict_range", "true");
  else
    ok = ok && reflector.setParam("strict_range", "false");
  
  if(m_focus_box) {
    string foc_region1 = "";
    foc_region1 += dim0_name + ":" + "100:300" + ",";
    foc_region1 += dim1_name + ":" + "100:300";
    
    string res_box = "discrete @";
    res_box += dim0_name + ":" + intToString(m_focus_unif_len) + ",";
    res_box += dim1_name + ":" + intToString(m_focus_unif_len);
    
    string foc_region2 = "";
    foc_region2 += dim0_name + ":" + "285:410" + ",";
    foc_region2 += dim1_name + ":" + "0:110";
    
    ok = ok && reflector.setParam("refine_region", foc_region1);
    ok = ok && reflector.setParam("refine_piece", res_box);
    ok = ok && reflector.setParam("refine_region", foc_region2);
    ok = ok && reflector.setParam("refine_piece", res_box);    
  }
  
  if(m_smart_refine) {
    string pstr = intToString(m_smart_count);
    ok = ok && reflector.setParam("smart_amount", pstr);
  }

  reflector.create();

  if(m_unif_ipf)
    delete(m_unif_ipf);
  // false means do not normalize as part of extractOF()
  m_unif_ipf = reflector.extractOF(false);

  if(m_unif_ipf && m_unif_ipf->getPDMap())
    m_rater.setPDMap(m_unif_ipf->getPDMap());

}

//-------------------------------------------------------------
// Procedure: modColorMap

void Viewer::modColorMap(const string &str)
{
  m_cmap.setType(str);
  //m_cmap.applyMidWhite(0.3, 0);
  m_aof_cache.applyFColorMap(m_cmap);
  redraw();
}

//-------------------------------------------------------------
// Procedure: modPatchAOF

void Viewer::modPatchAOF(int amt)
{
  m_patch += amt; 
  if(m_patch < 1)  
    m_patch = 1; 
  redraw();
}

//-------------------------------------------------------------
// Procedure: modUniformAug

void Viewer::modUniformAug(int amt)
{
  m_focus_unif_len += amt; 
  if(m_focus_unif_len < 1)  
    m_focus_unif_len = 1; 

  if(m_unif_ipf)
    makeUniformIPF();

  redraw();
}

//-------------------------------------------------------------
// Procedure: modSmartAugAmt

void Viewer::modSmartAugAmt(int amt)
{
  if(amt >= 0)
    m_smart_count = amt;

  if(m_unif_ipf)
    makeUniformIPF();

  redraw();
}

//-------------------------------------------------------------
// Procedure: runScript

void Viewer::runScript()
{
  int save_file_ix = 0;
  int delta = 4;
  for(int i=0; i<360; i=i+delta) {
    setParam("mod_z_rotation", delta);
    redraw();
    Fl::flush();
    capture(save_file_ix);
    save_file_ix++;
  }
}

//-------------------------------------------------------------
// Procedure: capture

void Viewer::capture(int save_file_ix)
{
  //string collect = "1024x768";
  //string collect = "720x480";
  //string collect = "800x600";
  //string collect = "640x480";

  string command;
  command += "import -window aof-ipf-function-viewer ";
  //command += "-crop " + collect + "+50+90 save_file_";
  command += " save_file_";
  if(save_file_ix < 10)   command += "0";
  if(save_file_ix < 100)  command += "0";
  if(save_file_ix < 1000) command += "0";
  command += intToString(save_file_ix) + ".png";
  system(command.c_str());
}

//-------------------------------------------------------------
// Procedure: getParam()

double Viewer::getParam(const string& param, bool&ok)
{
  if(!m_aof_cache.getAOF() || !m_unif_ipf) {
    ok = false;
    return(0);
  }

  ok = true;

  int cnt = m_rater.getSampleCount();;

  if(param == "sample_count")
    return(cnt);
  
  if(param == "piece_count")
    if(m_unif_ipf)
      return(m_unif_ipf->getPDMap()->size());
 
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
  else if((param == "unif_aug_size"))
    return((double)(m_focus_unif_len));
  else if((param == "create_time")) {
    if(m_create_time == -1)
      ok = false;
    return((double)(m_create_time));
  }

  ok = false;
  return(0);
}

//-------------------------------------------------------------
// Procedure: drawIvPFunction

void Viewer::drawIvPFunction(IvPFunction *ipf)
{
  PDMap *pdmap = ipf->getPDMap();
  int amt = pdmap->size();

  double hval = pdmap->getMaxWT();
  double lval = pdmap->getMinWT();
  
  IvPBox univ = pdmap->getUniverse();
  int xpts = (univ.pt(0,1) - univ.pt(0,0)) + 1;
  int ypts = (univ.pt(1,1) - univ.pt(1,0)) + 1;
  
  for(int i=0; i<amt; i++) {
    const IvPBox* ibox = pdmap->bx(i);
    drawIvPBox(*ibox, lval, hval, xpts, ypts);
  }
}

//-------------------------------------------------------------
// Procedure: drawIvPBox

void Viewer::drawIvPBox(const IvPBox &box, double lval, double hval, 
			int xpts, int ypts)
{
  if(box.getDim() != 2) {
    cout << "Wrong Box Dim" << endl;
    return;
  }
  
  if(lval==hval) {
    cout << "Dubious lval, hval in Viewer::drawIvPBox()" << endl;
    return;
  }

  Quad3D q;
  q.base = m_base_ipf;
  q.xl   = box.pt(0,0);
  q.xh   = box.pt(0,1);
  q.yl   = box.pt(1,0);
  q.yh   = box.pt(1,1);
  q.xpts = xpts;
  q.ypts = ypts;

  int degree = box.getDegree();
  double pct;

  IvPBox ebox(2,1);

  if(degree == 0)
    q.llval = q.hlval = q.hhval = q.lhval = box.maxVal();
  else if(degree != 1) {
    cout << "Wrong Degree" << endl;
    return;
  }
  else {
    ebox.setPTS(0, (int)q.xl, (int)q.xl);
    ebox.setPTS(1, (int)q.yl, (int)q.yl);
    q.llval = box.ptVal(&ebox);
    pct = (q.llval-lval)/(hval-lval);
    q.llval_r = m_cmap.getIRVal(pct);
    q.llval_g = m_cmap.getIGVal(pct);
    q.llval_b = m_cmap.getIBVal(pct);

    ebox.setPTS(0, (int)q.xh, (int)q.xh);
    q.hlval = box.ptVal(&ebox);
    pct = (q.hlval-lval)/(hval-lval);
    q.hlval_r = m_cmap.getIRVal(pct);
    q.hlval_g = m_cmap.getIGVal(pct);
    q.hlval_b = m_cmap.getIBVal(pct);

    ebox.setPTS(1, (int)q.yh, (int)q.yh);
    q.hhval = box.ptVal(&ebox);
    pct = (q.hhval-lval)/(hval-lval);
    q.hhval_r = m_cmap.getIRVal(pct);
    q.hhval_g = m_cmap.getIGVal(pct);
    q.hhval_b = m_cmap.getIBVal(pct);

    ebox.setPTS(0, (int)q.xl, (int)q.xl);
    q.lhval = box.ptVal(&ebox);
    pct = (q.lhval-lval)/(hval-lval);
    q.lhval_r = m_cmap.getIRVal(pct);
    q.lhval_g = m_cmap.getIGVal(pct);
    q.lhval_b = m_cmap.getIBVal(pct);
  }

  q.xl -=1;
  q.yl -=1;
  
  drawQuad(q);
}

//-------------------------------------------------------------
// Procedure: drawAOF

void Viewer::drawAOF()
{
  AOF *aof = m_aof_cache.getAOF();
  if(!aof)
    return;

  IvPDomain domain = aof->getDomain();
  int dim = domain.size();
  if((dim != 2) && (dim != 3))
    return;
  
  int xmin = 0;
  int xmax = domain.getVarPoints(0)-1;
  int ymin = 0;
  int ymax = domain.getVarPoints(1)-1;

  int yc = ymin;
  int xc = xmin;
  Quad3D q;
  q.base = m_base_aof; 
  q.scale= m_scale; 
  q.lines= false;
  q.xpts = (xmax - xmin) + 1;
  q.ypts = (ymax - ymin) + 1;
  while(yc < ymax) {
    xc = xmin; 
    while(xc < xmax) {
      q.xl = xc;
      q.xh = xc + m_patch;
      q.yl = yc;
      q.yh = yc + m_patch;
      if(q.xh > xmax) q.xh = xmax;
      if(q.yh > ymax) q.yh = ymax;

      q.llval   = m_aof_cache.getFVal((int)q.xl, (int)q.yl);
      q.llval_r = m_aof_cache.getRVal((int)q.xl, (int)q.yl);
      q.llval_g = m_aof_cache.getGVal((int)q.xl, (int)q.yl);
      q.llval_b = m_aof_cache.getBVal((int)q.xl, (int)q.yl);

      q.hlval   = m_aof_cache.getFVal((int)q.xh, (int)q.yl);
      q.hlval_r = m_aof_cache.getRVal((int)q.xh, (int)q.yl);
      q.hlval_g = m_aof_cache.getGVal((int)q.xh, (int)q.yl);
      q.hlval_b = m_aof_cache.getBVal((int)q.xh, (int)q.yl);

      q.hhval   = m_aof_cache.getFVal((int)q.xh, (int)q.yh);
      q.hhval_r = m_aof_cache.getRVal((int)q.xh, (int)q.yh);
      q.hhval_g = m_aof_cache.getGVal((int)q.xh, (int)q.yh);
      q.hhval_b = m_aof_cache.getBVal((int)q.xh, (int)q.yh);

      q.lhval   = m_aof_cache.getFVal((int)q.xl, (int)q.yh);
      q.lhval_r = m_aof_cache.getRVal((int)q.xl, (int)q.yh);
      q.lhval_g = m_aof_cache.getGVal((int)q.xl, (int)q.yh);
      q.lhval_b = m_aof_cache.getBVal((int)q.xl, (int)q.yh);

      drawQuad(q);
      xc += m_patch;
    }
    yc += m_patch;
  }  
}

//-------------------------------------------------------------
// Procedure: drawFocusBox

void Viewer::drawFocusBox()
{
  double w = 250;

  double bxl = m_focus_box_x - (m_focus_box_len / 2);
  double bxh = m_focus_box_x + (m_focus_box_len / 2);
  double byl = m_focus_box_y - (m_focus_box_len / 2);
  double byh = m_focus_box_y + (m_focus_box_len / 2);

  bxl -= 250;
  bxh -= 250;
  byl -= 250;
  byh -= 250;

  glPushMatrix();
  glColor3f(0.08f, 0.09f, 0.08f);

  glLineWidth(1.0);
  glBegin(GL_LINE_STRIP);

  glVertex3f(bxl, byl, -w+1);  
  glVertex3f(bxl, byh, -w+1);  
  glVertex3f(bxh, byh, -w+1);  
  glVertex3f(bxh, byl, -w+1);  
  glVertex3f(bxl, byl, -w+1);  
  glEnd();
  glPopMatrix();

  glFlush();

}

