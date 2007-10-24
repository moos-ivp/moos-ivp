/*****************************************************************************/
// Filename:
/*****************************************************************************/
// Description: 

// Constraints: None.
//
// Author:      Mirza A. Shah (mas644@only.arl.psu.edu)
//
// Date:        2005-2006
/*****************************************************************************/
// Penn State University Applied Research Laboratory Proprietary Software
// (c) Copyright 2006 The Pennsylvania State University
// This comment block may not be removed
/*****************************************************************************/

#ifndef COMMON_PRIMITIVES_H
#define COMMON_PRIMITIVES_H

#ifdef WIN32
#include <basetsd.h>
#else
typedef int           INT32;
typedef unsigned int  UINT32;
typedef long          INT64;
typedef unsigned long UINT64;
#endif
typedef float         FLOAT32;
typedef double        FLOAT64;

#endif