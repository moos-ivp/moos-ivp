/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BackImg.cpp                                          */
/*    DATE: Nov 16th 2004                                        */
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

#include <string>
#include <vector>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>

//#include <GL/gl.h>


// See http://www.cmake.org/pipermail/cmake/2003-March/003482.html
#if OPSYS_IS_LINUX
   #include <GL/gl.h>
#elif OPSYS_IS_OS_X
   #include <OpenGL/gl.h>
#else
   #error "Unknown OS"
#endif



#include "BackImg.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

// ----------------------------------------------------------
// Procedure: Constructor

BackImg::BackImg()
{
  // Just initialize member vars. First is most important.
  img_data     = 0;    
  img_width    = 0;
  img_height   = 0;    
  img_theta    = 0;

  // Below should be populated by an "info" file.
  img_centx    = 0;
  img_centy    = 0;
  img_meters   = 0;
  
  // Below should be derived after info file is read in.
  x_at_img_ctr = 0;
  y_at_img_ctr = 0;
}

// ----------------------------------------------------------
// Procedure: Constructor

BackImg::~BackImg()
{
  if(img_data) delete [] img_data;
}

// ----------------------------------------------------------
// Procedure: pixToPctX,Y()
//     Notes: pix is an absolute amount of the image

float BackImg::pixToPctX(float pix)
{
  return((float)(pix) / (float)(img_width));
}

float BackImg::pixToPctY(float pix)
{
  return((float)(pix) / (float)(img_height));
}

// ----------------------------------------------------------
// Procedure: readTiff
//   Purpose: This routine reads in a tiff data and info

bool BackImg::readTiff(string filename)
{
  filename = stripBlankEnds(filename);
  if(filename == "") {
    readBlankTiff();
    return(true);
  }

  string info_file = findReplace(filename, ".tif", ".info");

  bool ok1 = readTiffData(filename);  
  bool ok2 = readTiffInfo(info_file);

  if(ok1 && ok2)
    return(true);
  else {
    readBlankTiff();
    return(false);
  }
}


// ----------------------------------------------------------
// Procedure: readTiffData
//   Purpose: This routine reads in a tiff file and stores it 
//            in a very simple data structure

bool BackImg::readTiffData(string filename)
{
  if(img_data) delete [] img_data;
  img_height = 0;
  img_width  = 0;


  string file = filename;
  FILE *f = fopen(file.c_str(), "r");
  if(f) {
    fclose(f);
  }
  else {
    file = DATA_DIR;
    file += "/";
    file += filename;
  } 

  cout << "Attempting to open tif file: " << file << endl;

  // we turn off Warnings (maybe a bad idea) since many photoshop 
  // images have newfangled tags that confuse libtiff
  TIFFErrorHandler warn = TIFFSetWarningHandler(0);
  
  TIFF* tiff = TIFFOpen(file.c_str(), "r");
  
  // turn warnings back on, just in case
  TIFFSetWarningHandler(warn);

  if(tiff) {
    int rc = 1;			// what to return
    uint32 w, h;
    size_t npixels;
    uint32* raster;
    
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &h);
    
    // in case my data structures are different
    img_width  = w;
    img_height = h;
    
    npixels = w * h;
    raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
    if (raster != NULL) {
      if (TIFFReadRGBAImage(tiff, w, h, raster, 0)) {
	img_data = (unsigned char*) raster;
      } 
      else {
	rc=0;
	_TIFFfree(raster);
      }
    } 
    else 
      rc = 0;
    
    TIFFClose(tiff);
    return(rc);
  } 
  else
    return(0);
}

// ----------------------------------------------------------
// Procedure: readTiffInfo
//   Purpose: 

bool BackImg::readTiffInfo(string filename)
{
  string file = filename;
  FILE *f = fopen(file.c_str(), "r");
  if(f) {
    fclose(f);
  }
  else {
    file = DATA_DIR;
    file += "/";
    file += filename;
  } 

  cout << "Attempting to open: " << file << endl;

  vector<string> buffer = fileBuffer(file);
  //vector<string> buffer;
  int vsize = buffer.size();

  if(vsize == 0) {
    cout << file << " contains zero lines" << endl;
    return(false);
  }

  for(int i=0; i<vsize; i++) {
    string line = stripComment(buffer[i], "//");
    line = stripBlankEnds(line);

    if(line.size() > 0) {
      vector<string> svector = parseString(line, '=');
      if(svector.size() != 2) {
	cout << "A Problem w/ line " << i << " in " << file << endl;
	return(false);
      }
      string left  = stripBlankEnds(svector[0]);
      string right = stripBlankEnds(svector[1]);
      
      if(left == "img_centx") 
	img_centx = atof(right.c_str());
      else if(left == "img_centy") 
	img_centy = atof(right.c_str());

      else if(left == "img_meters") 
	img_meters = atof(right.c_str());
      else {
	cout << "Problem w/ line " << i << " in " << file << endl;
	cout << "Lefthand argument: " << left << endl;
	return(false);
      }
    }
  }

  double x_img_diff = 0.50 - img_centx;
  x_at_img_ctr = (x_img_diff * 100) / img_meters;
    
  double y_img_diff = 0.50 - img_centy;
  y_at_img_ctr = (y_img_diff * 100) / img_meters;
  
  x_at_img_left   = (-img_centx * 100) / img_meters;
  x_at_img_right  = ((1.0-img_centx) * 100) / img_meters;
  y_at_img_bottom = (-img_centy * 100) / img_meters;
  y_at_img_top    = ((1.0-img_centy) * 100) / img_meters;

  return(true);
}


// ----------------------------------------------------------
// Procedure: readBlankTiff

void BackImg::readBlankTiff()
{
  img_data    = 0;    
  img_width   = 2048;
  img_height  = 2048;    
  img_centx   = 0.5; 
  img_centy   = 0.5;
  img_meters  = 0.048828; 
}


// ----------------------------------------------------------
// Procedure: setTexture
//   Purpose: 

void BackImg::setTexture()
{
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
	       img_width, img_height, 0, 
	       GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 
	       (unsigned char *)img_data);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}


