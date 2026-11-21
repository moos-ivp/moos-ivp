///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
#ifndef __MOOSAssert_h
#define __MOOSAssert_h
// MOOS_ASSERT_LEVEL = {0, 1, 2}
// Level 0: No asserts
// Level 1: Soft assert - outputs error message and tries to carry on.
// Level 2: Hard assert - outputs error message and quits.

// ALLOW_RELEASE_ASSERTS
// If not defined then assert statements are tested ONLY in debug builds.

#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"

#ifndef MOOS_ASSERT_LEVEL
#define MOOS_ASSERT_LEVEL 1
#endif

// This is a helper macro, to automatically provide filename and
// line number details to the MOOSAssert function.
// It also ensures that asserts are only compiled in when required.
#if ((defined(_DEBUG) || defined(ALLOW_RELEASE_ASSERTS)) && (MOOS_ASSERT_LEVEL > 0))
#define _MOOSASSERT(bCond, msg) MOOSAssert(bCond, msg, __FILE__, __LINE__)
#else
#define _MOOSASSERT(bCond, msg)
#endif



inline void MOOSAssert(bool cond, const char *msg=NULL, const char *filename=NULL, int line=0)
{
  // Suppress unused parameter warnings, since some MOOS_ASSERT_LEVEL code paths
  // don't use these parameters.
  UNUSED_PARAMETER(cond);
  UNUSED_PARAMETER(msg);
  UNUSED_PARAMETER(filename);
  UNUSED_PARAMETER(line);

#if (defined(_DEBUG) || defined(ALLOW_RELEASE_ASSERTS))
#if (MOOS_ASSERT_LEVEL > 0)

    if (!cond)
    {
        if (filename && line>0)
        {
            if (msg)
            {
                MOOSTrace("Assert failed at %s:%d. %s\n", filename, line, msg);
            }
            else
            {
                MOOSTrace("Assert failed at %s:%d.\n", filename, line);
            }
        }
        else
        {
            if (msg)
            {
                MOOSTrace("Assert failed. %s\n", msg);
            }
            else
            {
                MOOSTrace("Assert failed.\n");
            }
        }
                

#if (MOOS_ASSERT_LEVEL == 2)
        exit();
#endif
    }
#endif
#endif
}

#endif // __MOOSAssert_h

