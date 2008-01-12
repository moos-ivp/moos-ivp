// Template Numerical Toolkit (TNT) for Linear Algebra
//
// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates
//
// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology


#ifndef TNT_H
#define TNT_H

//---------------------------------------------------------------------
//  tnt.h       TNT general header file.  Defines default types
//              and conventions.
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// Define the data type used for matrix and vector Subscripts.
// This will default to "int", but it can be overriden at compile time,
// e.g.
// 
//      g++ -DTNT_SUBSCRIPT_TYPE='unsinged long' ...
//
// See subscrpt.h for details.
//---------------------------------------------------------------------

#include "subscrpt.h"



//---------------------------------------------------------------------
// Define this macro if you want  TNT to ensure all refernces
// are within the bounds of the array.  This encurs a run-time
// overhead, of course, but is recommended while developing
// code.  It can be turned off for production runs.
// 
//       #define TNT_BOUNDS_CHECK
//---------------------------------------------------------------------
//
#define TNT_BOUNDS_CHECK
#ifdef TNT_NO_BOUNDS_CHECK
#undef TNT_BOUNDS_CHECK
#endif

//---------------------------------------------------------------------
// Define this macro if you want to utilize matrix and vector
// regions.  This is typically on, but you can save some
// compilation time by turning it off.  If you do this and
// attempt to use regions you will get an error message.
//
//       #define TNT_USE_REGIONS
//---------------------------------------------------------------------
//
// #define TNT_USE_REGIONS

//---------------------------------------------------------------------
//  
//---------------------------------------------------------------------
// Include basic scalar functions like abs(), max(), sign().
// Most of these should be available on your system, otherwise
// you find them in tntmath.cc.   By default, these are not included
// because they cause conflicts with ANSI platforms.  (For example,
// STL defines it own min() and max() functions.)  
//---------------------------------------------------------------------
//
// 
// #include "tntmath.h"


#endif
// TNT_H




