//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztañaga 2005-2006. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_POSIX_SEMAPHORE_WRAPPER_HPP
#define BOOST_INTERPROCESS_POSIX_SEMAPHORE_WRAPPER_HPP

#include <boost/interprocess/sync/posix/ptime_to_timespec.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/interprocess/exceptions.hpp>

namespace boost {
namespace interprocess {
namespace detail {

class semaphore_wrapper : private boost::noncopyable
{
 public:
   semaphore_wrapper(int initialCount)
   {
      if(sem_init(&m_sem, 1, initialCount) != 0){
         throw interprocess_exception(system_error_code());
      }
   }

   ~semaphore_wrapper()
   {
      if(sem_destroy(&m_sem) != 0){  
         assert(0);
      }
   }

   void post()
   {
      if(sem_post(&m_sem) != 0){
         throw interprocess_exception(system_error_code());
      }
   }

   void wait()
   {
      if(sem_wait(&m_sem) != 0){
         throw interprocess_exception(system_error_code());
      }
   }

   bool try_wait()
   {
      int res = sem_trywait(&m_sem);
      if(res == 0)
         return true;
      if(system_error_code() == EAGAIN){
         return false;
      }
      throw interprocess_exception(system_error_code());
      return false;
   }

   bool timed_wait(const boost::posix_time::ptime &abs_time)
   {
      timespec tspec = detail::ptime_to_timespec(abs_time);
      for (;;){
         int res = sem_timedwait(&m_sem, &tspec);
         if(res == 0)
            return true;
         if (res > 0){
            //buggy glibc, copy the returned error code to errno
            errno = res;
         }
         if(system_error_code() == ETIMEDOUT){
            return false;
         }
         throw interprocess_exception(system_error_code());
      }
      return false;
   }
/*
   int get_count()const
   {
      int count;
      sem_getvalue(&m_sem, &ret);
      return count;
   }
*/
   private:
   sem_t       m_sem;
};

}  //namespace detail {
}  //namespace interprocess {
}  //namespace boost {

#endif   //#ifndef BOOST_INTERPROCESS_POSIX_SEMAPHORE_WRAPPER_HPP
