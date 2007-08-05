/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GridGenerator.cpp                                    */
/*    DATE: Nov 25th, 2006 (Waterloo Thanksgiving)               */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include "GridGenerator.h"
#include "XYEncoders.h"
#include "FColorMap.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: initializeGrid()

bool GridGenerator::initializeGrid(string g_str)
{
  bool ok = m_grid.initialize(g_str);
  
  return(ok);
}

//-----------------------------------------------------------------
// Procedure: initializeAOF()
//      Note: "rings,exp,base,range"

bool GridGenerator::initializeAOF(string g_str)
{
  if(g_str == "")
    return(true);
 
  vector<string> svector = parseString(g_str, ',');
  if(svector.size() != 5)
    return(false);
  
  bool seed = true;
  if(seed) {
    unsigned long tseed = time(NULL);
    unsigned long hostid = gethostid();
    unsigned long pid = (long)getpid();
    unsigned long seed = (tseed%999999);
    seed = ((rand())*seed*hostid)%999999;
    seed = (seed*pid)%999999;
    srand(seed);
  }

  int rings     = atoi(svector[0].c_str());
  double exp    = atof(svector[1].c_str());
  double base   = atof(svector[2].c_str());
  double range  = atof(svector[3].c_str());
  string rpeak  = svector[4];

  XYSquare bsquare = m_grid.getSBound();
  int xl = (int)(bsquare.getVal(0,0));
  int xh = (int)(bsquare.getVal(0,1));
  int yl = (int)(bsquare.getVal(1,0));
  int yh = (int)(bsquare.getVal(1,1));
  
  cout << "BoundingSquare: " << XYSquareToString(bsquare) << endl;

  if(m_ringfunc)
    delete(m_ringfunc);

  IvPDomain domain;
  domain.addDomain("x", xl, xh, (xh-xl)+1); 
  domain.addDomain("y", yl, yh, (yh-yl)+1); 

  m_ringfunc = new AOF_Rings(domain);

  for(int i=0; i<rings; i++) {
    m_ringfunc->setParam("location", "random");
    m_ringfunc->setParam("base", base);
    m_ringfunc->setParam("range", range);
    m_ringfunc->setParam("exp", exp );
    m_ringfunc->setParam("radius", 0);
    m_ringfunc->setParam("peak", rpeak);
  }

  cout << "AOF_Rings::print(): " << endl;
  m_ringfunc->print();
  return(true);
}

//-----------------------------------------------------------------
// Procedure: setRandWeights()

void GridGenerator::setRandWeights(double g_low, double g_high)
{
  bool seed = true;
  if(seed) {
    unsigned long tseed = time(NULL);
    unsigned long hostid = gethostid();
    unsigned long pid = (long)getpid();
    unsigned long seed = (tseed%999999);
    seed = ((rand())*seed*hostid)%999999;
    seed = (seed*pid)%999999;
    srand(seed);
  }

  int range = (int)(g_high - g_low);

  int gsize = m_grid.size();
  for(int i=0; i<gsize; i++) {
    double rval = (rand() % range) + g_low;
    m_grid.setVal(i, rval);
  }
}

//-----------------------------------------------------------------
// Procedure: setAOFWeights()

void GridGenerator::setAOFWeights()
{
  if(m_ringfunc == 0)
    return;

  int gsize = m_grid.size();
  if(gsize == 0)
    return;

  IvPBox qbox(2);
  int cell_x = (int)(m_grid.getElement(0).getLengthX() / 2);
  int cell_y = (int)(m_grid.getElement(0).getLengthY() / 2);
  for(int i=0; i<gsize; i++) {
    int low_x = (int)(m_grid.getElement(i).getVal(0,0));
    int low_y = (int)(m_grid.getElement(i).getVal(1,0));
    qbox.setPTS(0, low_x+cell_x, low_x+cell_x);
    qbox.setPTS(1, low_y+cell_y, low_y+cell_y);
    
    double rval = m_ringfunc->evalBox(&qbox);
    m_grid.setVal(i, rval);
  }
}

//-----------------------------------------------------------------
// Procedure: writeToFile()

bool GridGenerator::writeToFile(string g_filename)
{
  FILE *f = fopen(g_filename.c_str(), "w+");
  if(f==0) 
    return(false);

  string str = XYGridToString(m_grid);
  
  fprintf(f, "fgrid = %s\n", str.c_str());
  
  fclose(f);
  return(true);
}

//-----------------------------------------------------------------
// Procedure: writeToFigFile()

bool GridGenerator::writeToFigFile(string g_filename)
{
  FILE *f = fopen(g_filename.c_str(), "w+");
  if(f==0) 
    return(false);

  int i;
  string str = XYGridToString(m_grid);

  FColorMap cmap;  

  fprintf(f, "#FIG 3.2  Produced by xfig version 3.2.5-alpha5\n");
  fprintf(f, "Landscape\n");
  fprintf(f, "Center\n");
  fprintf(f, "Metric\n");
  fprintf(f, "A4      \n");
  fprintf(f, "100.00\n");
  fprintf(f, "Single\n");
  fprintf(f, "-2\n");
  fprintf(f, "1200 2\n");
  fprintf(f, "0 32 #888888\n"); // Define first user color - grey

  int msize = cmap.size();
  int index = 33;
  for(i=0; i<msize; i++) {
    string red_str = doubleToHex(cmap.getRVal(i));
    string grn_str = doubleToHex(cmap.getGVal(i));
    string blu_str = doubleToHex(cmap.getBVal(i));


    fprintf(f, "0 %d #%s%s%s\n", index, red_str.c_str(), 
	    grn_str.c_str(), blu_str.c_str());
    index++;
  }  

  double minval = m_grid.getMinVal();
  double maxval = m_grid.getMaxVal();

  int gsize = m_grid.size();
  for(i=0; i<gsize; i++) {
    XYSquare square = m_grid.getElement(i);
    double val = m_grid.getVal(i);
    int xl  = (int)(square.getVal(0,0));
    int xh  = (int)(square.getVal(0,1));
    int yl  = (int)(square.getVal(1,0));
    int yh  = (int)(square.getVal(1,1));

    xl *= -100;
    xh *= -100;
    yl *= -100;
    yh *= -100;

    double pct = (val-minval)/(maxval-minval);
    int cmapix = (int)(pct * (double)(cmap.size()));
    if(cmapix == 0)
      cmapix = 1;

    int pw = 1;   // pen-width;
    int pc = 32;  // grey - defined above as the first user color

    fprintf(f, "2 2 0 %d %d %d 50 -1   20 0.000 0 0   -1 0 0 5\n", 
	    pw, pc, 32+cmapix);
    fprintf(f, "  %d %d %d %d %d %d %d %d %d %d\n", 
	    xl, yl, xl, yh, xh, yh, xh, yl, xl, yl);
  }

  fclose(f);
  return(true);
}

