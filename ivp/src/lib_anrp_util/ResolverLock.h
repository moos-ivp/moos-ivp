/*************************************************************************
* ResolverLock.h - 
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

#ifndef __ResolverLock_h__
#define __ResolverLock_h__

void LockResolver();
void UnlockResolver();

// To make IvP also build on Windows, we're using CMOOSLock.  Unfortunately
// that class doesn't support an equivalent of pthread_mutex_trylock, on
// which the function below relies.
//
// So for now I'm commenting out this function.  In the long term, we should
// consider either adding a 'trylock'-like operation to CMOOSLock, or have
// IvP (and possibly MOOS) use a Win32 pthreads library such as this:
//      http://sourceware.org/pthreads-win32/index.html
//
// -CJC
//
// bool IsResolverLocked();

#endif
