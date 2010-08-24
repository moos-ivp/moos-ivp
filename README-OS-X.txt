README-OS-X.txt
Last updated: 11 June 2010
Maintainer: issues@moos-ivp.org 

******************************************************************************
***            THIS FILE IS A WORK IN PROGRESS. CORRECTIONS OR             ***
***                  ADDITIONS ARE GREATLY APPRECIATED                     ***
******************************************************************************

==============================================================================
OVERVIEW
==============================================================================
This file gives OS X-specific steps for building and running 
MOOS-IvP software.

==============================================================================
IMPORTANT INFORMATION
==============================================================================
The MOOS-IvP software is capable of compiling and running on both Mac OS X 
Leopard (10.5) and Snow Leopard (10.6). However, in the transition from 10.5
to 10.6, Apple decided to switch the compiler to default to 64-bit rather than
32-bit. Additionally, Apple has decided to only support Carbon (one of their
GUI Libraries) in 32-bit. Since FLTK is dependent on Carbon, and MOOS-IvP is
dependent on FLTK, we have been forced to compile in 32-bit until FLTK makes
the transition to use Cocoa (another Apple GUI library).

==============================================================================
NEEDED APPLICATIONS
==============================================================================
The following applications are needed in order to compile MOOS-IvP.

------------------------------------------------------------------------------
1) XCODE
------------------------------------------------------------------------------
You need to install the XCode development package for OS-X available 
at http://developer.apple.com/technology/xcode.html

Alternatively, XCode can be found on the 2nd installation disk that ships 
with most Apple Computers.

------------------------------------------------------------------------------
2) MacPorts
------------------------------------------------------------------------------
Website: http://www.macports.org

MacPorts is an open-source project that provides an easy-to-use system for
compiling, installing, and upgrading common packages. In the past, we have
typically had instructions for downloading source code, compiling the source
code, and installing the source code. However, we have found that MacPorts
makes it simplifies this process.

Instructions for downloading and installing MacPorts can be found at the 
following address:

   http://www.macports.org/install.php

It is recommended that you use the Package Installer. By default, this will
install MacPorts into the /opt/local directory and add /opt/local/bin to 
the default user's PATH environment variable. More details on environment
variables can be found below.

------------------------------------------------------------------------------
3) CMake
------------------------------------------------------------------------------
Website: http://www.cmake.org

CMake is an open-source, cross-platform build system. There are two methods
for installing CMake:

   1) CMake provides a binary package for Mac OS X at the following address:
         http://www.cmake.org/cmake/resources/software.html

   2) MacPorts has an available package for CMake, which can be installed
      by using the following command:
         # sudo port install cmake

We currently support CMake version 2.4 through version 2.8. However, it is
recommended that the latest available version be installed.


==============================================================================
MACPORT PACKAGES
==============================================================================
The following MacPort packages must be installed in order to compile MOOS-IvP.
As discussed above, for MOOS-IvP libraries must be 32-bit. However, MacPorts
allows packages to be install as a 'universal' variant. This installs both
the 64-bit and 32-bit libraries and allows the compiler to choose the correct
library to use. 

------------------------------------------------------------------------------
1) Tiff
------------------------------------------------------------------------------
Website: http://libtiff.org

The Tiff package installed the tiff development libraries that allow graphical
applications to display tiff images. To install the package, use the following
command:
   # sudo apt-get install tiff jpeg zlib +universal
NOTE: The jpeg and zlib packages are dependencies of the tiff package. 
      Normally, dependencies are automatically resolved by MacPorts. However,
      we specify them here to force the packages to be installed with
      the universal variant.
      
==============================================================================
BUILDING MOOS-IvP
==============================================================================
The MOOS-IvP source tree contains the source code for two somewhat independent
software packages: MOOS, and IvP.

To build the software in this "moos-ivp" repository, first build the MOOS 
software using the following command:
   # ./build-moos.sh

The above should only need to be done once upon an initial checkout from 
the SVN server

The ivp tree can then be build by using the following command:
   # ./build-ivp.sh


==============================================================================
ENVIRONMENT VARIABLES
==============================================================================
When you build the MOOS software, the MOOS executable programs get placed in 
the "MOOS/MOOSBin" sub-directory of the source code tree.

When you build the IvP software, the IvP executable programs get placed in
the "bin" sub-directory of the source code tree.

We recommend that you put the absolute path to both of those directories into
your PATH environment variable.  This is especially important because the 
"pAntler" program, which can launch other MOOS/IvP programs, relies on the
PATH variable to find those programs.

We normally just at lines to our ~/.bash_profile files to always append
these two directories to the PATH environment variable.

*** NOTE: On OS X, ~/.bashrc isn't automatically executed, as it is on Linux.
*** However, ~/.bash_profile *is* automatically executed.


