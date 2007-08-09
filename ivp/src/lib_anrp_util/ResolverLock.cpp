/*************************************************************************
* ResolverLock.cpp - 
*************************************************************************
* (c) 2004 Andrew Patrikalakis <anrp@cml3.mit.edu>                      *
*                                                                       *
* This program is free software; you can redistribute it and/or modify  *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation; either version 2 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* This program is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program; if not, write to the Free Software           *
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
*************************************************************************/

#include "ResolverLock.h"
#include <MOOSLock.h>
#include <errno.h>

static CMOOSLock resm;

void LockResolver()
{
        resm.Lock();
}

void UnlockResolver()
{
        resm.UnLock();
}

// bool IsResolverLocked()
// {
// 	if(pthread_mutex_trylock(&resm) == EBUSY) {
// 		return true;
// 	} else {
// 		pthread_mutex_unlock(&resm);
// 		return false;
// 	}
// }

