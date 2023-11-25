/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BackImg.cpp                                          */
/*    DATE: Nov 16th 2004                                        */
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

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <tiffio.h>

// As of Release 15.4 this is now set in CMake, defaulting to be defined
// #define USE_UTM 

// See http://www.cmake.org/pipermail/cmake/2003-March/003482.html
#ifdef _WIN32
	#include <windows.h>
	#include <GL/gl.h>
	#include "glext.h" // http://www.opengl.org/registry/api/glext.h
	typedef UINT32 uint32_t;
#elif OPSYS_IS_LINUX
   #include <GL/gl.h>
#elif OPSYS_IS_OS_X
  //Defined before OpenGL/GLUT includes to avoid deprecation messages
   #define GL_SILENCE_DEPRECATION
   #include <OpenGL/gl.h>
#else
   #error "Unknown OS"
#endif

#include "BackImg.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"

using namespace std;

// ----------------------------------------------------------
// Procedure: Constructor

BackImg::BackImg()
{
  // Just initialize member vars. First is most important.
  m_img_data       = 0;    
  m_img_pix_width  = 0;
  m_img_pix_height = 0;    
  m_img_mtr_width  = 0;
  m_img_mtr_height = 0;
  
  // Boundary information from the .info file in the "old style"
  m_img_centx    = 0;
  m_img_centy    = 0;
  m_img_meters_x = 0;
  m_img_meters_y = 0;

  // Boundary in formation from the .info file in the "new style"
  m_lat_north = 0;
  m_lat_south = 0;
  m_lon_west  = 0;
  m_lon_east  = 0;
  m_boundary_set = false;

  // Below should be derived after info file is read in.
  m_x_at_img_left  = 0;
  m_x_at_img_right = 0;
  m_y_at_img_top   = 0;
  m_y_at_img_bottom = 0;
  m_x_at_img_ctr = 0;
  m_y_at_img_ctr = 0;

  // datum information
  m_datum_lat = 0;
  m_datum_lon = 0;
  m_datum_lat_set = false;
  m_datum_lon_set = false;

  m_verbose = false;
}

// ----------------------------------------------------------
// Procedure: copy()

void BackImg::copy(const BackImg &right)
{
  //m_img_data = 0;

  m_img_pix_width  = right.m_img_pix_width;
  m_img_pix_height = right.m_img_pix_height;    
  m_img_mtr_width  = right.m_img_mtr_width;
  m_img_mtr_height = right.m_img_mtr_height;
  
  // Boundary information from the .info file in the "old style"
  m_img_centx    = right.m_img_centx;
  m_img_centy    = right.m_img_centy;
  m_img_meters_x = right.m_img_meters_x;
  m_img_meters_y = right.m_img_meters_y;

  // Boundary in formation from the .info file in the "new style"
  m_lat_north = right.m_lat_north;
  m_lat_south = right.m_lat_south;
  m_lon_west  = right.m_lon_west;
  m_lon_east  = right.m_lon_east;
  m_boundary_set = right.m_boundary_set;

  // Below should be derived after info file is read in.
  m_x_at_img_left  = right.m_x_at_img_left;
  m_x_at_img_right = right.m_x_at_img_right;
  m_y_at_img_top   = right.m_y_at_img_top;
  m_y_at_img_bottom = right.m_y_at_img_bottom;
  m_x_at_img_ctr = right.m_x_at_img_ctr;
  m_y_at_img_ctr = right.m_y_at_img_ctr;

  // datum information
  m_datum_lat = right.m_datum_lat;
  m_datum_lon = right.m_datum_lon;
  m_datum_lat_set = right.m_datum_lat_set;
  m_datum_lon_set = right.m_datum_lon_set;

  m_verbose = right.m_verbose;
}

// ----------------------------------------------------------
// Procedure: Destructor

BackImg::~BackImg()
{
  if(m_img_data)
    _TIFFfree(m_img_data);
  m_img_data = 0;
}

// ----------------------------------------------------------
// Procedure: clearData()

void BackImg::clearData()
{
  if(m_img_data)
    _TIFFfree(m_img_data);
  m_img_data = 0;
}

// ----------------------------------------------------------
// Procedure: pixToPctX,Y()
//     Notes: pix is an absolute amount of the image

double BackImg::pixToPctX(double pix) const
{
  return((double)(pix) / (double)(m_img_pix_width));
}

double BackImg::pixToPctY(double pix) const
{
  return((double)(pix) / (double)(m_img_pix_height));
}

// ----------------------------------------------------------
// Procedure: readTiff
//   Purpose: This routine reads in a tiff data and info

bool BackImg::readTiff(string filename)
{
  filename = stripBlankEnds(filename);
  if(filename == "")
    return(false);

  locateTiffAndInfoFiles(filename);

  bool ok1 = readTiffData(m_tiff_file);  
  bool ok2 = readTiffInfo(m_info_file);

#if 0
  string info_file = findReplace(filename, ".tif", ".info");

  bool ok1 = readTiffData(filename);  
  bool ok2 = readTiffInfo(info_file);
#endif
  
  return(ok1 && ok2);
}


// ----------------------------------------------------------
// Procedure: locateTiffAndInfoFiles()

bool BackImg::locateTiffAndInfoFiles(string tiff_file)
{
  tiff_file = stripBlankEnds(tiff_file);
  if(tiff_file == "")
    return(false);

  string info_file = findReplace(tiff_file, ".tif", ".info");

  // =========================================================
  // Part 1: Most often (likely) image is just alpha or MIT mission
  // =========================================================
  cout << "[1] Looking for " << tiff_file << " and " << info_file << " in:" << endl;
  cout << "    Dir: [" << DATA_DIR << "]" << endl;
  string dtiff_file = "/" + tiff_file;
  dtiff_file = DATA_DIR + dtiff_file;
  string dinfo_file = "/" + info_file;
  dinfo_file = DATA_DIR + dinfo_file;
  if(okFileToRead(dtiff_file) && okFileToRead(dinfo_file)) {
    cout << "    FOUND!!" << endl;
    m_tiff_file = dtiff_file;
    m_info_file = dinfo_file;
    return(true);
  }
  else 
    cout << "    Not found." << endl;
    

  // =========================================================
  // Part 2: Look in the Path of IVP_IMAGE_DIRS
  // =========================================================
  const char* dirs = getenv("IVP_IMAGE_DIRS");
  if(dirs) {
    string image_dirs = dirs;

    vector<string> svector = parseString(image_dirs, ':');
    for(unsigned int i=0; i<svector.size(); i++) {
      cout << "[" << i+2 << "] ";
      cout << "Looking for " << tiff_file << " and " << info_file << " in:" << endl;
      cout << "    Dir: [" << svector[i] << "]" << endl;
      string xtiff_file = svector[i] + "/" + tiff_file;
      string xinfo_file = svector[i] + "/" + info_file;
      if(okFileToRead(xtiff_file) && okFileToRead(xinfo_file)) {
	cout << "    FOUND!!" << endl;
	m_tiff_file = xtiff_file;
	m_info_file = xinfo_file;
	return(true);
      }
    }
  }

  // =========================================================
  // Part 3: Last chance - look in the current working director
  // =========================================================
  cout << "[*] Looking for " << tiff_file << " and " << info_file << " in:" << endl;
  cout << "    Dir: [./]" << endl;
  if(okFileToRead(tiff_file) && okFileToRead(info_file)) {
    cout << "    FOUND!!" << endl;
    m_tiff_file = tiff_file;
    m_info_file = info_file;
    return(true);
  }
  else 
    cout << "    Not found." << endl;

  // If we get this far without success, produce a warning about
  // the IVP_IMAGE_DIRS environment variable if it was not set.
  
  if(!dirs) {
    cout << "**************************************************" << endl;
    cout << "*******             WARNING           ************" << endl;
    cout << "**************************************************" << endl;
    cout << "Environment variable IVP_IMAGE_DIRS not set. This " << endl;
    cout << "colon-separated path may point to image/tiff files" << endl;
    cout << "For example (in your .bashrc file) use:           " << endl;
    cout << "  IVP_IMAGE_DIRS=~/my_folder1                     " << endl;
    cout << "  IVP_IMAGE_DIRS+=:~/my_folder2                   " << endl;
    cout << "  export IVP_IMAGE_DIRS                           " << endl;
    cout << "**************************************************" << endl;
    cout << endl;
  }

  cout << "Could not find the pair of files: " << endl;
  cout << tiff_file << " and " << info_file << endl;
  return(false);
}


// ----------------------------------------------------------
// Procedure: readTiffData()
//   Purpose: This routine reads in a tiff file and stores it 
//            in a very simple data structure

bool BackImg::readTiffData(string filename)
{
  if(m_img_data) 
    _TIFFfree(m_img_data);

  m_img_pix_height = 0;
  m_img_pix_width  = 0;
  
  // We turn off Warnings (maybe a bad idea) since many photoshop 
  // images have newfangled tags that confuse libtiff
  TIFFErrorHandler warn = TIFFSetWarningHandler(0);
  
  cout << "Opening Tiff: " << filename << endl;
  TIFF* tiff = TIFFOpen(filename.c_str(), "r");
  if(tiff) {
    cout << "  Success! " << endl;
    m_tiff_file = filename;
  }
  else {
    cout << "  Failed!!!!!!!!!" << endl;
    return(false);
  }
  
  // turn warnings back on, just in case
  TIFFSetWarningHandler(warn);

  if(tiff) {
    bool rval = true;			// what to return
    uint32_t w, h;
    size_t npixels;
    uint32_t* raster;
    
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &h);
    
    // in case my data structures are different
    m_img_pix_width  = w;
    m_img_pix_height = h;
    
    npixels = w * h;
    raster = (uint32_t*) _TIFFmalloc(npixels * sizeof (uint32_t));
    string short_filename = rbiteString(filename, '/');
    cout << "Loading Texture from Tiff file: " << short_filename << endl;

    if (raster != NULL) {
      if (TIFFReadRGBAImage(tiff, w, h, raster, 0)) {
	m_img_data = (unsigned char*) raster;
	cout << "  Success! " << endl;
      } 
      else {
	cout << "  Failed!!!!!!!!!" << endl;
	rval=false;
	_TIFFfree(raster);
      }
    } 
    else 
      rval = false;
    
    TIFFClose(tiff);
    return(rval);
  } 
  else
    return(false);
}

// ----------------------------------------------------------
// Procedure: readTiffInfo()

bool BackImg::readTiffInfo(string info_file)
{
  FILE *f = fopen(info_file.c_str(), "r");
  if(f) 
    fclose(f);
  else {
    cout << "Could not find or read contents of " << info_file << endl;
    return(false);
  }
      
  vector<string> buffer = fileBuffer(info_file);
  if(buffer.size() == 0) {
    if(m_verbose)
      cout << info_file << " contains zero lines. " << endl;
    return(false);
  }
  
  cout << "Successfully found info file:" << info_file << endl;
  
  bool img_centx_set  = false;
  bool img_centy_set  = false;
  bool img_meters_set = false;

  bool lat_north_set  = false;
  bool lat_south_set  = false;
  bool lon_west_set   = false;
  bool lon_east_set   = false;

  for(unsigned int i=0; i<buffer.size(); i++) {
    string line = stripComment(buffer[i], "//");
    line = stripBlankEnds(line);

    if(line.size() > 0) {
      vector<string> svector = parseString(line, '=');
      if(svector.size() != 2)
	return(false);
      string left  = stripBlankEnds(svector[0]);
      string right = stripBlankEnds(svector[1]);
      
      if((left == "img_centx") && (isNumber(right))) {
	m_img_centx = atof(right.c_str());
	img_centx_set = true;
      }
      else if((left == "img_centy") && (isNumber(right))) {
	m_img_centy = atof(right.c_str());
	img_centy_set = true;
      }
      else if((left == "img_meters") && (isNumber(right))) {
	m_img_meters_x = atof(right.c_str());
	m_img_meters_y = atof(right.c_str());
	img_meters_set = true;
      }

      //--------------------------------------------------
      else if((left == "lat_north") && (isNumber(right))) {
	m_lat_north = atof(right.c_str());
	lat_north_set = true;
      }
      else if((left == "lat_south") && (isNumber(right))) {
	m_lat_south = atof(right.c_str());
	lat_south_set = true;
      }
      else if((left == "lon_west") && (isNumber(right))) {
	m_lon_west = atof(right.c_str());
	lon_west_set = true;
      }
      else if((left == "lon_east") && (isNumber(right))) {
	m_lon_east = atof(right.c_str());
	lon_east_set = true;
      }
      else if((left == "datum_lat") && (isNumber(right))) {
	if(!m_datum_lat_set) {
	  m_datum_lat = atof(right.c_str());
	  m_datum_lat_set = true;
	}
      }
      else if((left == "datum_lon") && (isNumber(right))) {
	if(!m_datum_lon_set) {
	  m_datum_lon = atof(right.c_str());
	  m_datum_lon_set = true;
	}
      }
      else
	return(false);
    }
  }
  
  // Must provide the essential info in one of the two allowable
  // forms. Either with the six lat/lon/datum values or with the
  // three image values.

  bool style_old = false;
  bool style_new = false;

  if(img_centx_set && img_centy_set && img_meters_set)
    style_old = true;
  if(lat_north_set && lat_south_set && lon_west_set && lon_east_set)
    style_new = true;

  if(!style_old && !style_new)
    return(false);
  
  if(style_new) {
    m_boundary_set = true;
    if((m_lat_north <= m_lat_south) || (m_lon_east <= m_lon_west)) {
      cout << "Problem with BackImg Lat/Lon specs. " << endl;
      cout << "lat_north: " << m_lat_north << endl;
      cout << "lat_south: " << m_lat_south << endl;
      cout << "lon_west:  " << m_lon_west  << endl;
      cout << "lon_east:  " << m_lon_east  << endl;
      return(false);
    }
  }

  bool result = processConfiguration();
  // print();
  return(result);
}

// ----------------------------------------------------------
// Procedure: readTiffInfoEmpty()

bool BackImg::readTiffInfoEmpty(double lat_north, double lat_south, 
				double lon_east, double lon_west)
{
  m_lat_north = lat_north;
  m_lat_south = lat_south;
  m_lon_east  = lon_east;
  m_lon_west  = lon_west;
  m_boundary_set = true;

  m_img_pix_width  = 1000;
  m_img_pix_height = 1000;
  m_img_mtr_width  = 1000;
  m_img_mtr_height = 1000;

  bool result = processConfiguration();
  return(result);
}

// ----------------------------------------------------------
// Procedure: processConfiguration

bool BackImg::processConfiguration()
{
  if(m_boundary_set) {
    // The "datum" used to initialize the Geodesy just needs to be
    // in the "ballbark". So just use the center of the image. The
    // Geodesy is just used to get the extents of the image in terms
    // of meters.

    double pseudo_datum_lat = (m_lat_north + m_lat_south) / 2.0;
    double pseudo_datum_lon = (m_lon_west + m_lon_east)   / 2.0;

    if(m_verbose) {
      cout << "boundary set:" << endl;
      cout << "psuedo_datum_lat:" << pseudo_datum_lat << endl;
      cout << "psuedo_datum_lon:" << pseudo_datum_lon << endl;
    }
    
    CMOOSGeodesy geodesy;
    geodesy.Initialise(pseudo_datum_lat, pseudo_datum_lon);
    double x1,x2,y1,y2;

#ifdef USE_UTM
    if(m_verbose)
      cout << "************************ Using UTM" << endl;
    bool ok1 = geodesy.LatLong2LocalUTM(m_lat_north, m_lon_west, y1, x1);
    bool ok2 = geodesy.LatLong2LocalUTM(m_lat_south, m_lon_east, y2, x2);
#else
    if(m_verbose)
      cout << "************************ NOT Using UTM" << endl;
    bool ok1 = geodesy.LatLong2LocalGrid(m_lat_north, m_lon_west, y1, x1);
    bool ok2 = geodesy.LatLong2LocalGrid(m_lat_south, m_lon_east, y2, x2);
#endif

    if(!ok1 || !ok2) {
      cout << "Problem with BackImg Geodesy conversion. " << endl;
      return(false);
    }
    double img_mtr_height = abs(y1-y2);
    double img_mtr_width  = abs(x1-x2);

    if(m_verbose) {
      cout << "img_mtr_height:" << doubleToStringX(img_mtr_height) << endl;
      cout << "img_mtr_width:"  << doubleToStringX(img_mtr_width) << endl;
    }
    
    if(!m_datum_lat_set)
      m_datum_lat = pseudo_datum_lat;
    if(!m_datum_lon_set)
      m_datum_lon = pseudo_datum_lon;

    m_img_meters_x = (1 / (img_mtr_width / 100.0));
    m_img_meters_y = (1 / (img_mtr_height  / 100.0));
    m_img_centx = (m_datum_lon - m_lon_west) / 
      (m_lon_east - m_lon_west);
    m_img_centy = (m_datum_lat - m_lat_south) / 
      (m_lat_north - m_lat_south);
  }

  double x_img_diff = 0.50 - m_img_centx;
  double y_img_diff = 0.50 - m_img_centy;
  m_x_at_img_ctr    = (x_img_diff * 100) / m_img_meters_x;
  m_y_at_img_ctr    = (y_img_diff * 100) / m_img_meters_y;  

  m_x_at_img_left   = (-m_img_centx * 100) / m_img_meters_x;
  m_x_at_img_right  = ((1.0-m_img_centx) * 100) / m_img_meters_x;

  m_y_at_img_bottom = (-m_img_centy * 100) / m_img_meters_y;
  m_y_at_img_top    = ((1.0 - m_img_centy) * 100) / m_img_meters_y;

  m_img_mtr_width   = abs(m_x_at_img_right - m_x_at_img_left);
  m_img_mtr_height  = abs(m_y_at_img_top - m_y_at_img_bottom);

  
  if(m_verbose) {
    cout << "Geodesy: m_img_centx: " << m_img_centx << endl;
    cout << "Geodesy: m_img_centy: " << m_img_centy << endl;
    cout << "Geodesy: m_img_meters_x: " << doubleToStringX(m_img_meters_x) << endl;
    cout << "Geodesy: m_img_meters_y: " << doubleToStringX(m_img_meters_y) << endl;
    cout << "Geodesy:   m_x_at_img_ctr: " << m_x_at_img_ctr << endl;
    cout << "Geodesy:   m_y_at_img_ctr: " << m_y_at_img_ctr << endl;
    cout << "Geodesy:     m_x_at_img_left: " << m_x_at_img_left << endl;
    cout << "Geodesy:     m_x_at_img_right: " << m_x_at_img_right << endl;
    cout << "Geodesy:     m_y_at_img_top: " << m_y_at_img_top << endl;
    cout << "Geodesy:     m_y_at_img_bottom: " << m_y_at_img_bottom << endl;
    cout << "Geodesy:       m_img_mtr_width: "  << doubleToStringX(m_img_mtr_width,2) << endl;
    cout << "Geodesy:       m_img_mtr_height: " << doubleToStringX(m_img_mtr_height,2) << endl;
  }
  
  return(true);
}

// ----------------------------------------------------------
// Procedure: setDatumLatLon()

void BackImg::setDatumLatLon(double datum_lat, double datum_lon)
{
  m_datum_lat     = datum_lat;
  m_datum_lon     = datum_lon;
  m_datum_lat_set = true;
  m_datum_lon_set = true;

  if(m_boundary_set)
    processConfiguration();
}

// ----------------------------------------------------------
// Procedure: setTexture()

void BackImg::setTexture()
{
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
	       m_img_pix_width, m_img_pix_height, 0, 
	       GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, 
	       (unsigned char *)m_img_data);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}


// ----------------------------------------------------------
// Procedure: print()

void BackImg::print()
{
  cout << "--------------------------------------- BackImg::print" << endl;
  cout << "m_img_meters_x:" << m_img_meters_x << endl;
  cout << "m_img_meters_y:" << m_img_meters_y << endl;
  cout << "m_img_centx:   " << m_img_centx << endl;
  cout << "m_img_centy:   " << m_img_centy << endl;

  cout << "m_x_at_img_left:   " << m_x_at_img_left << endl;
  cout << "m_x_at_img_right:  " << m_x_at_img_right << endl;
  cout << "m_y_at_img_top:    " << m_y_at_img_top << endl;
  cout << "m_y_at_img_bottom: " << m_y_at_img_bottom << endl;
  cout << "m_x_at_img_ctr:    " << m_x_at_img_ctr << endl;
  cout << "m_y_at_img_ctr:    " << m_y_at_img_ctr << endl;

  cout << "m_img_mtr_width:   " << m_img_mtr_width << endl;
  cout << "m_img_mtr_height:  " << m_img_mtr_height << endl;

  cout << "m_img_pix_width:  " << m_img_pix_width << endl;
  cout << "m_img_pix_height: " << m_img_pix_height << endl;
  cout << "m_img_mtr_width:  " << m_img_mtr_width << endl;
  cout << "m_img_mtr_height: " << m_img_mtr_height << endl;

  cout << "pix_per_mtr_x: " << (m_img_pix_width / m_img_mtr_width) << endl;
  cout << "pix_per_mtr_y: " <<(m_img_pix_height / m_img_mtr_height) << endl;

}

// ----------------------------------------------------------
// Procedure: printTerse()

void BackImg::printTerse()
{
  cout << "-------------------- BackImg::printTerse" << endl;
  cout << "mtr_wid: " << doubleToStringX(m_img_mtr_width,2) << endl;
  cout << "mtr_hgt: " << doubleToStringX(m_img_mtr_height,2) << endl;
					 
  cout << "pix_wid  " << doubleToStringX(m_img_pix_width,2) << endl;
  cout << "pix_hgt: " << doubleToStringX(m_img_pix_height,2) << endl;

  cout << "pix_per_mtr_x: " << (m_img_pix_width / m_img_mtr_width) << endl;
  cout << "pix_per_mtr_y: " << (m_img_pix_height / m_img_mtr_height) << endl;

}





