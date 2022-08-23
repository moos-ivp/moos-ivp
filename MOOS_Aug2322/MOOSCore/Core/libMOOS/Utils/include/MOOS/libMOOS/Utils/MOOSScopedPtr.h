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
//   http://www.gnu.org/licenses/lgpl.txtgram is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
/*
 * MOOSScopedPtr.h
 *
 *  Created on: Jun 19, 2018
 *      Author: arh
 */

#ifndef MOOSSCOPEDPTR_H
#define MOOSSCOPEDPTR_H

namespace MOOS {

// This is a very basic RAII class that owns a resource and ensures that it is
// correctly deleted when the ScopedPtr goes out of scope.
// This class behaves like an auto_ptr, but without the surprising copy and
// assignment behaviour.  If MOOS transitions to C++11 then all instances of
// ScopedPtr can be trivially replaced with std::unique_ptr.
template <class T>
class ScopedPtr {
 public:
  typedef T element_type;

  explicit ScopedPtr(T* p = 0) : ptr_(p) {}
  ~ScopedPtr() { delete ptr_; }

 private:
  // Copy and assign do not fit with the single-owner semantics of ScopedPtr,
  // so they are left unimplemented.
  ScopedPtr(ScopedPtr const& p);
  ScopedPtr& operator=(ScopedPtr& p);

 public:
  T& operator*() const { return *ptr_; }
  T* operator->() const { return ptr_; }

  T* get() const { return ptr_; }

  T* release() {
    T* temp = ptr_;
    ptr_ = 0;
    return temp;
  }

  void reset(T* p = 0) {
    if (ptr_ != p)
      delete ptr_;
    ptr_ = p;
  }

 private:
  T* ptr_;
};

}  // namespace MOOS

#endif  // MOOSSCOPEDPTR_H
