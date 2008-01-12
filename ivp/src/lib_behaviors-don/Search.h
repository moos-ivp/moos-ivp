
// File Name: Search.h
// Author: Ding Wang.
// Description: In this file we store several often used functions.

#ifndef SEARCH_H
#define SEARCH_H

#include "tnt/vec.h"
#include "tnt/fmat.h"

template <class T>
T Prod(const Vector<T> & v_, const int & start_, const int & end_) // calculate product of all v_'s elements.
{
  assert(start_>=1&&start_<=end_&&end_<=v_.dim());
  T temp_prod=1;
  for(unsigned long int i=start_; i<=end_; i++)
    {temp_prod*=v_(i);}
  return temp_prod;
}

template <class T>
T Sum(const Vector<T> & v_) // calculate sum of all v_'s elements.
{
  T temp_sum=0;
  for(unsigned long int i=1; i<=v_.dim(); i++)
    {temp_sum+=v_(i);}
  return temp_sum;
}

template <class T>
T Sum(const Vector<T> & v_, const int & start_, const int & end_) // calculate sum of all v_'s elements.
{
  assert(start_>=1&&start_<=end_&&end_<=v_.dim());
  T temp_sum=0;
  for(unsigned long int i=start_; i<=end_; i++)
    {temp_sum+=v_(i);}
  return temp_sum;
}

template <class T>
void Search_Min(const Vector<T> v_, int & position_) // find the min element in vector v_
{
  T tmp_store=v_(1); // put the first element in v_ in a temporary storage scalar.
  position_=1; // initialize position_ equal to 1
  for (int i=1; i<=v_.dim(); i++) // scan one element by another one.
    
    if (v_(i)<tmp_store) // if the new one is smaller
      {
        tmp_store=v_(i); //yes, then update our temporary storage
        position_=i; // and update position_
      }

}
#endif



