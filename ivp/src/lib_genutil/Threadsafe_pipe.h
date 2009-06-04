#ifndef THREADSAFE_PIPE_H
#define THREADSAFE_PIPE_H

#include <queue>
#include <cassert>
#include <pthread.h>

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
    pthread_mutex_t mtx;
};

//==============================================================================

template <typename T>
Threadsafe_pipe<T>::Threadsafe_pipe() 
  : closed(false)
{
  int rc;
  rc = pthread_mutex_init(& mtx, NULL);
  assert(!rc);
}

//==============================================================================

template <typename T>
Threadsafe_pipe<T>::~Threadsafe_pipe() 
{
  int rc;
  rc = pthread_mutex_destroy(& mtx);
  assert(!rc);
}

//==============================================================================

template <typename T>
bool Threadsafe_pipe<T>::enqueue(const T & value)
{
  int rc;
  rc = pthread_mutex_lock(& mtx);
  assert(!rc);
  
  if (closed) {
    rc = pthread_mutex_unlock(& mtx);
    assert(!rc);

    return false;
  }
  
  data.push(value);

  rc = pthread_mutex_unlock(& mtx);
  assert(!rc);

  return true;
}

//==============================================================================

template <typename T>
void Threadsafe_pipe<T>::close()
{
  int rc;
  rc = pthread_mutex_lock(& mtx);
  assert(!rc);

  closed = true;

  rc = pthread_mutex_unlock(& mtx);
  assert(!rc);
}

//==============================================================================

template <typename T>
bool Threadsafe_pipe<T>::dequeue(T & value)
{
  int rc;
  rc = pthread_mutex_lock(& mtx);
  assert(!rc);

  // Is there data to get? ...
  if (! data.empty()) {
    value = data.front();
    data.pop();

    rc = pthread_mutex_unlock(& mtx);
    assert(!rc);
    
    return true;
  }
  else {
    // If we got here, then the pipe must have be empty and closed...
    assert(closed);

    rc = pthread_mutex_unlock(& mtx);
    assert(!rc);

    return false;
  }
}

//==============================================================================

template <typename T>
bool Threadsafe_pipe<T>::empty()
{
  int rc;
  rc = pthread_mutex_lock(& mtx);
  assert(!rc);
  
  bool is_empty = data.empty();
  
  rc = pthread_mutex_unlock(& mtx);
  assert(!rc);
  
  return is_empty;
}

//==============================================================================

#endif
