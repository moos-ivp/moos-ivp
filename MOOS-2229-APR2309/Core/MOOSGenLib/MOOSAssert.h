#ifndef __MOOSAssert_h
#define __MOOSAssert_h
// MOOS_ASSERT_LEVEL = {0, 1, 2}
// Level 0: No asserts
// Level 1: Soft assert - outputs error message and tries to carry on.
// Level 2: Hard assert - outputs error message and quits.

// ALLOW_RELEASE_ASSERTS
// If not defined then assert statements are tested ONLY in debug builds.

#include "MOOSGenLibGlobalHelper.h"

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



inline void MOOSAssert(bool cond, char *msg=NULL, const char *filename=NULL, int line=0)
{
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

