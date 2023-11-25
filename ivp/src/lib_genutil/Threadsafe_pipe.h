/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE:                                                      */
/*    DATE:                                                      */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
#ifndef THREADSAFE_PIPE_H
#define THREADSAFE_PIPE_H

#include <queue>
#include <cassert>
#ifdef _WIN32
   #include "MOOSLock.h"
#else
   #include <pthread.h>
#endif

/// This implements a thread-safe pipe.  To keep the implementation simple, this
/// class makes no promises regarding first-come, first-serve access to the
/// data.
template <typename T>
class Threadsafe_pipe {
  public:
    Threadsafe_pipe();
    
    /// Don't call this while there are still threads calling (or blocked on)
    /// this object's methods, including the dequeue method.
    virtual ~Threadsafe_pipe();
    
    /// Inserts a new element into the pipe.
    /// @return If the operation was successful, this is true.  If this returns
    ///   false that means the pipe has already been closed.
    bool enqueue(const T & value);
    
    /// Closes the pipe.  Future enqueues are disabled.  Once this method 
    /// executes and the queue is/becomes empty, any subsequent dequeue 
    /// operations will fail (return false).
    void close();
    
    /// If the queue isn't empty, this sets 'value' to the value of the item at
    /// the front of the queue and returns true.
    /// If the queue is empty and closed, this returns false.
    /// @return true means that 'value' has been set to the dequeued value.
    ///   'false' means that the dequeue operation was aborted because of an
    ///   empty and closed pipe.
    bool dequeue(T & value);

    /// Returns true iff the pipe is empty.  This method says nothing about
    /// whether or not the pipe is closed.
    bool empty();

  private:
    std::queue<T> data;
    bool closed;

#ifdef _WIN32
	CMOOSLock* m_lock;
#else
    pthread_mutex_t mtx;
#endif
};

//==============================================================================

template <typename T>
Threadsafe_pipe<T>::Threadsafe_pipe() 
  : closed(false)
{
  int rc;

#ifdef _WIN32
  m_lock = new CMOOSLock();
#else
  rc = pthread_mutex_init(& mtx, NULL);
  assert(!rc);
#endif
}

//==============================================================================

template <typename T>
Threadsafe_pipe<T>::~Threadsafe_pipe() 
{
  int rc;
#ifdef _WIN32
  delete m_lock;
#else
  rc = pthread_mutex_destroy(& mtx);
  assert(!rc);
#endif 
}

//==============================================================================

template <typename T>
bool Threadsafe_pipe<T>::enqueue(const T & value)
{
  int rc;

#ifdef _WIN32
	m_lock->Lock();
#else
  rc = pthread_mutex_lock(& mtx);
  assert(!rc);
#endif
  
  if (closed) {
#ifdef _WIN32
    m_lock->UnLock();
#else
    rc = pthread_mutex_unlock(& mtx);
    assert(!rc);
#endif

    return false;
  }
  
  data.push(value);

#ifdef _WIN32
  m_lock->UnLock();
#else
  rc = pthread_mutex_unlock(& mtx);
  assert(!rc);
#endif

  return true;
}

//==============================================================================

template <typename T>
void Threadsafe_pipe<T>::close()
{
  int rc;
#ifdef _WIN32
  m_lock->Lock();
#else
  rc = pthread_mutex_lock(& mtx);
  assert(!rc);
#endif

  closed = true;

#ifdef _WIN32
  m_lock->UnLock();
#else
  rc = pthread_mutex_unlock(& mtx);
  assert(!rc);
#endif
}

//==============================================================================

template <typename T>
bool Threadsafe_pipe<T>::dequeue(T & value)
{
  int rc;
#ifdef _WIN32
  m_lock->Lock();
#else
  rc = pthread_mutex_lock(& mtx);
  assert(!rc);
#endif

  // Is there data to get? ...
  if (! data.empty()) {
    value = data.front();
    data.pop();

#ifdef _WIN32
	m_lock->UnLock();
#else
    rc = pthread_mutex_unlock(& mtx);
    assert(!rc);
#endif
    
    return true;
  }
  else {
    // If we got here, then the pipe must have be empty and closed...
    assert(closed);

#ifdef _WIN32
	m_lock->UnLock();
#else
    rc = pthread_mutex_unlock(& mtx);
    assert(!rc);
#endif

    return false;
  }
}

//==============================================================================

template <typename T>
bool Threadsafe_pipe<T>::empty()
{
  int rc;
#ifdef _WIN32
  m_lock->Lock();
#else
  rc = pthread_mutex_lock(& mtx);
  assert(!rc);
#endif
  
  bool is_empty = data.empty();
  
#ifdef _WIN32
	m_lock->UnLock();
#else
    rc = pthread_mutex_unlock(& mtx);
    assert(!rc);
#endif
  
  return is_empty;
}

//==============================================================================

#endif








