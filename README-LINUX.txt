README-LINUX.txt
Last updated: 20 June 2008
Maintainer: Christian Convey ( christian.convey@navy.mil )

************************************************************
***   THIS FILE IS A WORK IN PROGRESS.  CORRECTIONS OR   ***
***   ADDITIONS ARE GREATLY APPRECIATED                  ***
************************************************************

OVERVIEW
========
This file gives Linux-specific steps for building and running 
MOOS-IvP software.


PACKAGES
========
The following software packages must must installed on the 
Linux computer in order to build MOOS-IvP.  For each package 
we've listed some information on how it can be obtained.

*******************
* Ubuntu packages *
*******************

(1) OpenGL development libraries.
We're a little sketchy on which particular packages we need 
for OpenGL support.  The following is our best guess:

(1A) Regardless of your graphics card, you need: 
   libglut-dev

(1B) Then you also need basic 3D support within X-Windows on 
   your system.  To get it, you may need to install one of
   the following packages.  Which one you install depends on
   your video card:

   nVidia-based card --> 
      nvidia-glx-new      (Ubuntu 8.04)
      nvidia-glx-new-dev  (Ubuntu 8.04)
      nvidia-glx-dev      (Ubuntu 8.10)  ** You'll have to choose a particular
                                         ** version, such as nvidia-glx-177
      libsm-dev
      libXext-dev

   ATI-based card (open-source driver) --> xorg-driver-fglrx,  
       and maybe xorg-driver-fglrx-dev

   ATI-based card (closed-source driver) --> xorg-driver-fglrx,
       and maybe xorg-driver-fglrx-dev

(2) libx11-dev

(3) libtiff
Website: http://libtiff.org
Ubuntu package: libtiff-dev

(4) CMake (at least version 2.4)
Website: cmake.org
Ubuntu package: cmake

(5) The Gnu C++ compiler (recommend version >= 4.0)
Website: http://gcc.gnu.org/
Ubuntu package: g++


BUILDING MOOS-IvP
=================
Please refer to the README file found in this directory.


ENVIRONMENT VARIABLES
=====================
The MOOS-IvP source tree contains the source code for two somewhat independent
software packages: MOOS, and IvP.  

When you build the MOOS software, the MOOS executable programs get placed in 
the "MOOS/MOOSBin" subdirectory of the source code tree.

When you build the IvP software, the IvP executable programs get placed in
the "ivp/src/bin" subdirectory of the source code tree.

We recommend that you put the absolute path to both of those directories into
your PATH environment variable.  This is especially important because the 
"pAntler" program, which can launch other MOOS/IvP programs, relies on the
PATH variable to find those programs.

We normally just add lines to our ~/.bashrc or ~/.cshrc files to always append
these two directories to the PATH environment variable.



