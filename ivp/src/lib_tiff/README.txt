README.txt
Last update: 31 August 2009
Maintainer: Christopher Gagner ( Christopher.Gagner@navy.mil )

************************************************************
***   THIS FILE IS A WORK IN PROGRESS.  CORRECTIONS OR   ***
***   ADDITIONS ARE GREATLY APPRECIATED                  ***
************************************************************

OVERVIEW
========
This file gives information for building libtiff under all platforms.


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

*********
* Linux *
*********

Under Linux, autoconf should be used to generate the configuration headers.
Libtiff should also be linked against the following libraries:

(1) jpeg
(2) zip
(3) z

NOTE: Under Linux, the changes are handled by CMake.

*********
* Apple *
*********

Under Apple, the changes need are to be determined.


