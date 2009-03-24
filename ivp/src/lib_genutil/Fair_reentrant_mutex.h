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
      Fair_reentrant_mutex();
      virtual ~Fair_reentrant_mutex();
      
      void lock();
      void unlock();
      
   private:
      // How many holds current thread has on the lock.  A value of 0 indicates
      // that the lock is currently unheld.
      int hold_depth; 
      
      // Current holder is at the front of the queue.
      std::queue<pthread_t> waiters;
      
      pthread_mutex_t mtx;
      
      // Indicates that the lock might be available for acquisition.  Only the
      // thread that's next in line for the lock should wait on this condition.
      pthread_cond_t lock_available_cond;
};

#endif
