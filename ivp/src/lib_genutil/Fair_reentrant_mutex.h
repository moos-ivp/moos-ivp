#ifndef FAIR_LOCK_H
#define FAIR_LOCK_H

#include <queue>
#include <pthread.h>

// Fair (FCFS), reentrant lock. 
//
// Currently only implemented for pthreads, but should be easily portable to
// moosthreads and/or win32 threads.
class Fair_reentrant_mutex {
   public:
      Fair_lock();
      virtual ~Fair_lock();
      
      void lock();
      void unlock();
      
   private:
      // How many holds current thread has on the lock.  A value of 0 indicates
      // that the lock is currently unheld.
      int hold_depth; 
      
      // Current holder is at the front of the queue.
      std::queue<pthread_t> waiters;
      
      pthread_mutex_t mtx;
      
      // Only the thread at the frond of the queue should be waiting on this
      // condition.  When that thread is signalled by this condition *and*
      // sees that hold_depth == 0, then the thread knows that it can proceed
      // with taking ownership of the lock.
      pthread_cond_t lock_available_cond;
};

#endif
