README.txt
Last update: 16 September 2009
Maintainer: Christopher Gagner ( Christopher.Gagner@navy.mil )

************************************************************
***   THIS FILE IS A WORK IN PROGRESS.  CORRECTIONS OR   ***
***   ADDITIONS ARE GREATLY APPRECIATED                  ***
************************************************************

OVERVIEW
========
This file gives information for building libtiff under Windows.


RESOURCES
=========
Home Page:   http://www.libtiff.org
Mirror Site: http://dl.maptools.org/dl/libtiff/
Version:     tiff-3.8.2


CHANGES
=======

***********
* Windows *
***********

Under Windows, the following changes need to be made in order for libtiff 
to be compiled:

(1) Copy tiff-3.8.2\libtiff\tif_config.h.vc tiff-3.8.2\libtiff\tif_config.h
(2) Copy tiff-3.8.2\libtiff\tiffconf.h.vc tiff-3.8.2\libtiff\tiffconf.h
(3) Add the following includes to tiff-3.8.2\libtiff\tif_win32.c:
    (a) #include <Windows.h>
    (b) #include <WinBase.h>

NOTE: For version 3.8.2 these changes have already been made and checked in
      on the MOOS-IvP repository.

*******************
* Linux and Apple *
*******************
It is the case where libtiff can be build on all three platforms. However, on 
Mac and Linux, autoconf must be used to generate the following configuration 
headers:

lib_tiff/tiff-3.8.2/libtiff/tif_config.h 
lib_tiff/tiff-3.8.2/libtiff/tiffconf.h

After looking into converting the autoconf build files to a CMake build file, 
it was determined the level of effort seemed higher than the benefits, at 
least for building libtiff on Mac and Linux. Under Mac and Linux, MOOS-IvP 
should still require users to have the libtiff libraries installed on their 
system. 


