// Template Numerical Toolkit (TNT) for Linear Algebra
//
// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates
//
// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology


#ifndef TVECTOR_H
#define TVECTOR_H

#include <stdlib.h>
#include <iostream.h>
#include <strstream.h>
#include <assert.h>

#include "subscrpt.h"

#ifdef TNT_USE_REGIONS
#include "region1d.h"
#endif

//  see "tntreq.h" for TNT requirements for underlying vector
//  class.  This need NOT be the STL vector<> class, but a subset
//  that provides minimal services.
//
//  This is a container adaptor that provides the following services.
//
//      o)  adds 1-offset operator() access ([] is always 0 offset)
//      o)  adds TNT_BOUNDS_CHECK to () and []
//      o)  adds initialization from strings, e.g.  "1.0 2.0 3.0";
//      o)  adds newsize(N) function (does not preserve previous values)
//      o)  adds dim() and dim(1)
//      o)  adds free() function to release memory used by vector
//      o)  adds regions, e.g. A(Index(1,10)) = ... 
//      o) 

template <class BBVec>
class TNT_Vector
{

  public:
    typedef   BBVec::value_type T;
    typedef         T   value_type;
    typedef         T   element_type;
    typedef         T*  pointer;
    typedef         T*  iterator;
    typedef         T&  reference;
    typedef const   T*  const_iterator;
    typedef const   T&  const_reference;
    
    Subscript lbound() const { return 1; }

  protected:
    BBVec v_;
    T* vm1_;

  public:

    Subscript size() const { return v_.size(); }

    iterator begin() { return v_.begin();}
    iterator end()   { return v_.end(); }
    const_iterator begin() const { return v_.begin();}
    const_iterator end()  const { return v_.end(); }

    Subscript dim() const { return v_.size(); }
    Subscript dim(Subscript i)
    {
#ifdef TNT_BOUNDS_CHECK
        assert(i==TNT_BASE_OFFSET);
#endif
        return (i==TNT_BASE_OFFSET ? v_.size() : 0 );
    }
    TNT_Vector() : v_() {};
    TNT_Vector(const TNT_Vector<BBVec> &A) : v_(A.v_) 
    { 
        vm1_ = v_.begin() -1; 
    } 

    TNT_Vector(Subscript N, /*const*/ char *s) : v_(N) 
    {
        istrstream ins(s);
        for (Subscript i=0; i<N; i++)
            ins >> v_[i] ;

        vm1_ = v_.begin() -1 ;
    }; 

    TNT_Vector(Subscript N, const T& value = T()) : v_(N, value)
    {
        vm1_ = v_.begin() -1;
    }

    TNT_Vector(Subscript N, const T* values) : v_(N)
    {
        for (Subscript i=0; i<N; i++)
             v_[i]  = values[i];
        vm1_ = v_.begin() -1;

    } 
    TNT_Vector(const BBVec & A) : v_(A) 
    {
        vm1_ = v_.begin() -1;
    }

    // NOTE: this assumes that BBVec(0) constructor creates an 
    //  null vector that does not take up space...  It would be
    //  great to require that BBVec have a corresponding free()
    //  function, but in particular STL vectors do not.
    //
    TNT_Vector<BBVec>& free()
    {
        return *this = TNT_Vector<BBVec>(0);
    }

    TNT_Vector<BBVec>& operator=(const TNT_Vector<BBVec> &A) 
    { 
        v_ = A.v_ ; 
        vm1_ = v_.begin() - 1;
        return *this;
    }

    TNT_Vector<BBVec>& newsize(Subscript N)
    {
        // NOTE: this is not as efficient as it could be
        // but to retain compatiblity with STL interface
        // we cannot assume underlying implementation
        // has a newsize() function.

        return *this = TNT_Vector<BBVec>(N);

    }

    TNT_Vector<BBVec>& operator=(const T &a) 
    {
        Subscript i;
        Subscript N = v_.size();    
        for (i=0; i<N; i++)
            v_[i] = a;

        return *this;
    }

    TNT_Vector<BBVec>& resize(Subscript N) 
    { 
        if (N == size()) return *this;

        TNT_Vector<BBVec> tmp(N);
        Subscript n =  (N < size() ? N : size());  // min(N, size());
        Subscript i;

        for (i=0; i<n; i++)
            tmp[i] = v_[i];
            

        return (*this = tmp);

    }


    reference operator()(Subscript i)
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i<=dim());
#endif
        return vm1_[i]; 
    }

    const_reference operator()(Subscript i) const
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i<=dim());
#endif
        return vm1_[i]; 
    }

    reference operator[](Subscript i)
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(0<=i);
        assert(i<dim());
#endif
        return v_[i]; 
    }

    const_reference operator[](Subscript i) const
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(0<=i);
        assert(i<dim());
#endif
        return v_[i]; 
    }


#ifdef TNT_USE_REGIONS
    // "index-aware" features, all of these are 1-based offsets

    typedef Region1D<TNT_Vector<BBVec> > Region;

    typedef const_Region1D<TNT_Vector<BBVec>,T> const_Region;

    Region operator()(const Index1D &I)
    {   return Region(*this, I); }

    Region operator()(const Subscript i1, Subscript i2)
    {   return Region(*this, i1, i2); }

    const_Region operator()(const Index1D &I) const
    {   return const_Region(*this, I); }

    const_Region operator()(const Subscript i1, Subscript i2) const
    {   return const_Region(*this, i1, i2); }
#endif
// TNT_USE_REGIONS


};

#include <iostream.h>

template <class BBVec>
ostream& operator<<(ostream &s, const TNT_Vector<BBVec> &A)
{
    Subscript M=A.size();

    s << M << endl;
    for (Subscript i=1; i<=M; i++)
            s << A(i) << endl;
    return s;
}

template <class BBVec>
istream& operator>>(istream &s, const TNT_Vector<BBVec> &A)
{
    Subscript N;
    
    s >> N;

    TNT_Vector<BBVec,T> tmp(N);
    for (Subscript i=1; i<=N; i++)
        s >> tmp(i);

    return s;
}


#endif
