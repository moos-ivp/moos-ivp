/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IO_Utilities.cpp                                     */
/*    DATE: June 2nd, 2004                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "IO_Utilities.h"

using namespace std;

//--------------------------------------------------------------
// Procedure: saveProblem
//   Purpose: 

bool saveProblem(IvPProblem *prob, const string& fileStr)
{
  bool okw = true;  // fprint returns neg # on error, or num chars outp

  for(int i=0; i<prob->getOFNUM(); i++)
    okw = okw && saveFunction(prob->getOF(i), fileStr, true);

  return(okw);
}

//--------------------------------------------------------------
// Procedure: savePDMap
//   Purpose: 

bool savePDMap(PDMap *pdmap, FILE *f)
{
  if(f==NULL) return(false);

  int dim       = pdmap->getDim();
  int boxCount  = pdmap->size();
  IvPBox gelbox = pdmap->getGelBox();
  int  wtc      = pdmap->bx(0)->getWtc();

  bool okw = true;  // fprint returns neg # on error, or num chars outp

  okw = okw && (fprintf(f, "G ") >= 0);
  for(int d=0; d<dim; d++) {
    okw = okw && (fprintf(f, "%d ", gelbox.pt(d, 0))  >= 0);
    okw = okw && (fprintf(f, "%d ", gelbox.pt(d, 1)) >= 0);
  }
  okw = okw && (fprintf(f, "\n") >= 0);

  for(int i=0; i<boxCount; i++) {
    IvPBox *ibox = pdmap->bx(i);
    okw = okw && (fprintf(f, "B ") >= 0);
    okw = okw && (fprintf(f, "%d ", wtc) >= 0);
    for(int d=0; d<dim; d++) {
      if(ibox->bd(d,0)==0) 
	okw = okw && (fprintf(f, "X") >=0);
      okw = okw && (fprintf(f, "%d ", ibox->pt(d, 0))  >= 0);
      if(ibox->bd(d,1)==0) 
	okw = okw && (fprintf(f, "X") >=0);
      okw = okw && (fprintf(f, "%d ", ibox->pt(d, 1)) >= 0);
    }

    int wtc = ibox->getWtc();
    for(int i=0; i<wtc; i++) {
      double iwt = ibox->wt(i);
      if(iwt == floor(iwt))
	fprintf(f, "%d ", (int)iwt);
      else
	fprintf(f, "%.5f ", iwt);
    }
    
    fprintf(f, "\n");
  }

  return(okw);
}

//--------------------------------------------------------------
// Procedure: readFunction

IvPFunction* readFunction(const string& str)
{
  vector<IvPFunction*> fvector = readFunctions(str);
  int vsize = fvector.size();
  if(vsize > 0)
    return(fvector[0]);
  else
    return(0);
}

//--------------------------------------------------------------
// Procedure: readFunctions

vector<IvPFunction*> readFunctions(const string& str)
{
  vector<IvPFunction*> rvector;

  FILE *f = fopen(str.c_str(), "r");
  if(f==0) 
    return(rvector);

  const int MAX_LINE_LENGTH = 1000;

  int     dim, boxCount, degree;
  float   pwt;
  char    c, buff[MAX_LINE_LENGTH];
  string  contextStr;
  string  domain_str;

  while(1) {
    // Handle the FUNCTION Line
    fscanf(f, "%c", &c);
    if(c == 'F') {
      int buff_ix = 0;
      while(c != '\n') {
	fscanf(f, "%c", &c);
	if(buff_ix < MAX_LINE_LENGTH-1)
	  buff[buff_ix] = c;
	buff_ix++;
      }
      buff[buff_ix] = '\0';
    }
    else {
      fclose(f);
      return(rvector);
    }
    
    string function = buff;
    vector<string> svector = parseString(function, ' ');
    int vsize = svector.size();
    for(int i=0; i<vsize; i++) {
      string item = stripBlankEnds(svector[i]);
      vector<string> ivector = parseString(item, '=');
      int isize = ivector.size();
      if(isize == 2) {
	string left  = ivector[0];
	string right = ivector[1];
	if(left == "dim")
	  dim = atoi(right.c_str());
	else if(left == "pcs")
	  boxCount = atoi(right.c_str());
	else if(left == "dom")
	  domain_str = right;
	else if(left == "deg")
	  degree = atoi(right.c_str());
	else if(left == "pwt")
	  pwt = atof(right.c_str());
      }
    }
    
    // Handle the AOF Line if any
    fscanf(f, "%c", &c);
    if(c == 'A') {
      int buff_ix = 0;
      while(c != '\n') {
	fscanf(f, "%c", &c);
	if(buff_ix < MAX_LINE_LENGTH)
	  buff[buff_ix] = c;
	buff_ix++;
      }
      buff[buff_ix] = '\0';
      contextStr = buff;
    }
    else
      ungetc(c, f);
    
    
    //IvPBox universe = stringToBox(domain_str);
    IvPDomain domain = stringToDomain(domain_str);
    
    PDMap *pdmap = readPDMap(f, dim, boxCount, domain, degree);
    IvPFunction *new_of = new IvPFunction(pdmap);
    new_of->setContextStr(contextStr);
    new_of->setPWT(pwt);
    
    rvector.push_back(new_of);
 
  }
}

//--------------------------------------------------------------
// Procedure: readPDMap

PDMap* readPDMap(FILE *f, int dim, int boxCount, IvPDomain domain, int deg)
{
  if(f==0) return(0);
  
  char c;
  fscanf(f, "%c", &c);
  if(c == 'B') 
    ungetc(c, f);
  else
    if(c != 'G')
      return(0);

  int     d, low, high, wtc;
  char    buff[500], lowBuff[80], highBuff[80];

  PDMap *pdmap = new PDMap(boxCount, domain, deg);

  IvPBox gelbox(dim);
  if(c == 'G') {
    for(d=0; d<dim; d++) {
      fscanf(f, "%d ", &low);
      fscanf(f, "%d ", &high);
      gelbox.setPTS(d, low, high);
    }
    pdmap->setGelBox(gelbox);
  }

  for(int i=0; i<boxCount; i++) {
    fscanf(f, "%c ", &c);
    fscanf(f, "%d ", &wtc);
    IvPBox *newbox = new IvPBox(dim, deg);
    for(d=0; d<dim; d++) {
      fscanf(f, "%s ", lowBuff);
      fscanf(f, "%s ", highBuff);
      if(lowBuff[0]=='X') {         // Check for bound Xclusive
	newbox->bd(d, 0) = 0;       // bound. If X is first char
	lowBuff[0] = '+';           // set bound to exclusive (0)
      }                             // and convert that X to a '+'.
      if(highBuff[0]=='X') {        // The '+' will be effectively
	newbox->bd(d, 1) = 0;       // ignored by the atoi function.
	highBuff[0] = '+';
      }
      low = atoi(lowBuff);
      high = atoi(highBuff);
      newbox->setPTS(d, low, high);
    }
    for(d=0; d<wtc; d++) {
      fscanf(f, "%s ", buff);
      newbox->wt(d) = atof(buff);
    }
    pdmap->bx(i) = newbox;
  }
  pdmap->updateGrid(1,1);
  return(pdmap);
}

//--------------------------------------------------------------
// Procedure: saveFunction

bool saveFunction(IvPFunction *ipf, const string& filename, bool append)
{
  if(!ipf)
    return(false);

  const PDMap* pdmap = ipf->getPDMap();
  if(!pdmap)
    return(false);
  
  FILE *f = 0;
  if(append)
    f = fopen(filename.c_str(), "a");
  else
    f = fopen(filename.c_str(), "w");

  if(f==0) {
    cout << "Error opening file: \"" << filename  <<"\""<< endl;
    return(false);
  }

  string header = "FUNCTION";
  header += " dim=";
  header += intToString(pdmap->getDim());
  header += " pcs=";
  header += intToString(pdmap->size());
  header += " deg=";
  header += intToString(pdmap->getDegree());
  header += " wgt=";
  header += dstringCompact(doubleToString(ipf->getPWT()));
  header += " dom=";
  header += domainToString(pdmap->getDomain());
  
  fprintf(f, "%s\n", header.c_str());
  
  string str = ipf->getContextStr();
  if(str != "")
    fprintf(f, "AOF %s \n", str.c_str());

  bool okw = savePDMap(ipf->getPDMap(), f);

  fclose(f);
  return(okw);
}


//--------------------------------------------------------------
// Procedure: printZAIC_PEAK

void printZAIC_PEAK(ZAIC_PEAK zaic)
{
  int zsize = zaic.getSummitCount();
  
  bool insist = zaic.getSummitInsist();
  bool wrap   = zaic.getValueWrap();


  cout << "ZAIC Summit Count: " << zsize << endl;
  cout << " SummitInsist (T/F): " << insist << endl;
  cout << " ValueWrap (T/F): "    << wrap << endl;

  for(int i=0; i<zsize; i++) {
     cout << "[" << i << "]: " << endl;
    double summit = zaic.getParam("summit", i);
    double basewidth = zaic.getParam("basewidth", i);
    double peakwidth = zaic.getParam("peakwidth", i);
    double delta = zaic.getParam("delta", i);
    double minutil = zaic.getParam("minutil", i);
    double maxutil = zaic.getParam("maxutil", i);

    cout << "  Summit: "    << summit << endl;
    cout << "  Basewidth: " << basewidth << endl;
    cout << "  Peakwidth: " << peakwidth << endl;
    cout << "  Delta: "     << delta << endl;
    cout << "  Minutil: "   << minutil << endl;
    cout << "  Maxutil: "   << maxutil << endl;
  }
}
