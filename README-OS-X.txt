README-OS-X.txt
Last updated: 20 June 2008
Maintainer: Christian Convey ( christian.convey@navy.mil )

************************************************************
***   THIS FILE IS A WORK IN PROGRESS.  CORRECTIONS OR   ***
***   ADDITIONS ARE GREATLY APPRECIATED                  ***
************************************************************

OVERVIEW
========
This file gives OS X-specific steps for building and running 
MOOS-IvP software.


PACKAGES
========
The following software packages must must installed on the 
OS X computer in order to build MOOS-IvP.  For each package 
we've listed some information on how it can be obtained.

(1) libtiff
Website: http://libtiff.org

(a) The page that has the actual tarfile to download is:
    http://dl.maptools.org/dl/libtiff/
(b) Look at the README, but it amounts to:
    ./configure; make; sudo make install


(2) CMake (at least version 2.4)
Website: cmake.org



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

We normally just at lines to our ~/.bash_profile files to always append
these two directories to the PATH environment variable.
*** NOTE: On OS X, ~/.bashrc isn't automatically executed, as it is on Linux.
*** However, ~/.bash_profile *is* automatically executed.


EXECUTING GUI APPS ON OS X
==========================
In order to make OS X happy, our graphical apps have to be built into OS X
"Application Bundles".  An Application Bundle is a whole directory of
files that represent a single program.  For example, on Linux we have:
   moos-ivp/ivp/src/bin/pShipsideViewer

But on OS X we have a directory tree rooted at:
   moos-ivp/ivp/src/bin/pShipsideViewer.app/

There are several ways to launch an Application Bundle from the command-line.
Apple seems to prefer using the "open" command.  For example:
   open -a pShipsideViewer <arg1> <arg2> ...
or
   open ...../moos-ivp/ivp/src/bin/pShipsideViewer <arg1> <arg2> ...
(The first version works when moos-ivp/src/bin appears in the PATH environment
variable.)

The problem with using "open" is that if you forget to provide all of the
command-line arguments that the program was expecting, "open" will not show
you the usage statement printed by the program onto stderr.  Instead you just
get an error code.

So if you want to see stderr, you can't use "open".  Instead, you can launch
the executable file that's burried deep within the Application Bundle.
For example:
   moos-ivp/ivp/src/bin/pShipsideViewer.app/Contents/MacOS/pShipsideViewer <arg1> <arg2> ...


