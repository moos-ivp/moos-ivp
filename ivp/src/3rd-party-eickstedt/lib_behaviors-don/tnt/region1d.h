// Template Numerical Toolkit (TNT) for Linear Algebra
//
// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates
//
// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology



#ifndef REGION1D_H
#define REGION1D_H


#include "subscrpt.h"
#include "index.h"
#include <iostream.h>
#include <assert.h>


template <class Array1D, class T>       // still needs T parameter
class const_Region1D;                   // see region2d.h

template <class Array1D /*, class T*/>
class Region1D
{
    protected:

        Array1D &  A_;
        Subscript offset_;          // 0-based
        Subscript dim_;

        typedef Array1D::element_type T;

    public:
        const Array1D & array()  const { return A_; }

        Subscript offset() const { return offset_;}
        Subscript dim() const { return dim_; }

        Subscript offset(Subscript i) const
        {
#ifdef TNT_BOUNDS_CHECK
            assert(i==TNT_BASE_OFFSET);
#endif
            return offset_;
        }

        Subscript dim(Subscript i) const
        {
#ifdef TNT_BOUNDS_CHECK
            assert(i== TNT_BASE_OFFSET);
#endif
            return offset_;
        }


        Region1D(Array1D &A, Subscript i1, Subscript i2) : A_(A)
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET <= i1 );
            assert(i2 <= A.dim() + (TNT_BASE_OFFSET-1));
            assert(i1 <= i2);
#endif
            offset_ = i1 - TNT_BASE_OFFSET;
            dim_ = i2-i1 + 1;
        }

        Region1D(Array1D &A, const Index1D &I) : A_(A)
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET <=I.lbound());
            assert(I.ubound() <= A.dim() + (TNT_BASE_OFFSET-1));
            assert(I.lbound() <= I.ubound());
#endif
            offset_ = I.lbound() - TNT_BASE_OFFSET;
            dim_ = I.ubound() - I.lbound() + 1;
        }

        Region1D(Region1D<Array1D> &A, Subscript i1, Subscript i2) :
                A_(A.A_)
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET <= i1 );
            assert(i2 <= A.dim() + (TNT_BASE_OFFSET - 1));
            assert(i1 <= i2);
#endif
                    //     (old-offset)        (new-offset)
                    //
            offset_ =  (i1 - TNT_BASE_OFFSET) + A.offset_;
            dim_ = i2-i1 + 1;
        }

        Region1D<Array1D> operator()(Subscript i1, Subscript i2)
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET <= i1);
            assert(i2 <= dim() + (TNT_BASE_OFFSET -1));
            assert(i1 <= i2);
#endif
                    // offset_ is 0-based, so no need for
                    //  ( - TNT_BASE_OFFSET)
                    //
            return Region1D<Array1D>(A_, i1+offset_,
                    offset_ + i2);
        }


        Region1D<Array1D> operator()(const Index1D &I)
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET<=I.lbound());
            assert(I.ubound() <= dim() + (TNT_BASE_OFFSET-1));
            assert(I.lbound() <= I.ubound());
#endif
            return Region1D<Array1D>(A_, I.lbound()+offset_,
                offset_ + I.ubound());
        }

        T & operator()(Subscript i)
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET <= i);
            assert(i <=  dim() + (TNT_BASE_OFFSET-1));
#endif
            return A_(i+offset_);
        }

        const T & operator() (Subscript i) const
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET <= i);
            assert(i <= dim() + (TNT_BASE_OFFSET-1));
#endif
            return A_(i+offset_);
        }


        Region1D<Array1D> & operator=(const Region1D<Array1D> &R)
        {
            // make sure both sides conform
            assert(dim() == R.dim());

            Subscript N = dim();
            Subscript i;
            Subscript istart = TNT_BASE_OFFSET;
            Subscript iend = istart + N-1;

            for (i=istart; i<=iend; i++)
                (*this)(i) = R(i);

            return *this;
        }



        Region1D<Array1D> & operator=(const const_Region1D<Array1D,T> &R)
        {
            // make sure both sides conform
            assert(dim() == R.dim());

            Subscript N = dim();
            Subscript i;
            Subscript istart = TNT_BASE_OFFSET;
            Subscript iend = istart + N-1;

            for (i=istart; i<=iend; i++)
                (*this)(i) = R(i);

            return *this;

        }


        Region1D<Array1D> & operator=(const T& t)
        {
            Subscript N=dim();
            Subscript i;
            Subscript istart = TNT_BASE_OFFSET;
            Subscript iend = istart + N-1;

            for (i=istart; i<= iend; i++)
                (*this)(i) = t;

            return *this;

        }


        Region1D<Array1D> & operator=(const Array1D &R)
        {
            // make sure both sides conform
            Subscript N = dim();
            assert(dim() == R.dim());

            Subscript i;
            Subscript istart = TNT_BASE_OFFSET;
            Subscript iend = istart + N-1;

            for (i=istart; i<=iend; i++)
                (*this)(i) = R(i);

            return *this;

        }

};

template <class Array1D>
ostream& operator<<(ostream &s, Region1D<Array1D> &A)
{
    Subscript N=A.dim();
    Subscript istart = TNT_BASE_OFFSET;
    Subscript iend = N - 1 + TNT_BASE_OFFSET;

    for (Subscript i=istart; i<=iend; i++)
        s << A(i) << endl;

    return s;
}


/*  ---------  class const_Region1D ------------ */

template <class Array1D, class T>
class const_Region1D
{
    protected:

        const Array1D &  A_;
        Subscript offset_;          // 0-based
        Subscript dim_;

    public:
        const Array1D & array()  const { return A_; }

        Subscript offset() const { return offset_;}
        Subscript dim() const { return dim_; }

        Subscript offset(Subscript i) const
        {
#ifdef TNT_BOUNDS_CHECK
            assert(i==TNT_BASE_OFFSET);
#endif
            return offset_;
        }

        Subscript dim(Subscript i) const
        {
#ifdef TNT_BOUNDS_CHECK
            assert(i== TNT_BASE_OFFSET);
#endif
            return offset_;
        }


        const_Region1D(const Array1D &A, Subscript i1, Subscript i2) : A_(A)
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET <= i1 );
            assert(i2 <= A.dim() + (TNT_BASE_OFFSET-1));
            assert(i1 <= i2);
#endif
            offset_ = i1 - TNT_BASE_OFFSET;
            dim_ = i2-i1 + 1;
        }

        const_Region1D(const Array1D &A, const Index1D &I) : A_(A)
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET <=I.lbound());
            assert(I.ubound() <= A.dim() + (TNT_BASE_OFFSET-1));
            assert(I.lbound() <= I.ubound());
#endif
            offset_ = I.lbound() - TNT_BASE_OFFSET;
            dim_ = I.ubound() - I.lbound() + 1;
        }

        const_Region1D(const_Region1D<Array1D,T> &A, Subscript i1, Subscript i2) :
                A_(A.A_)
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET <= i1 );
            assert(i2 <= A.dim() + (TNT_BASE_OFFSET - 1));
            assert(i1 <= i2);
#endif
                    //     (old-offset)        (new-offset)
                    //
            offset_ =  (i1 - TNT_BASE_OFFSET) + A.offset_;
            dim_ = i2-i1 + 1;
        }

        const_Region1D<Array1D,T> operator()(Subscript i1, Subscript i2)
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET <= i1);
            assert(i2 <= dim() + (TNT_BASE_OFFSET -1));
            assert(i1 <= i2);
#endif
                    // offset_ is 0-based, so no need for
                    //  ( - TNT_BASE_OFFSET)
                    //
            return const_Region1D<Array1D, T>(A_, i1+offset_,
                    offset_ + i2);
        }


        const_Region1D<Array1D,T> operator()(const Index1D &I)
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET<=I.lbound());
            assert(I.ubound() <= dim() + (TNT_BASE_OFFSET-1));
            assert(I.lbound() <= I.ubound());
#endif
            return const_Region1D<Array1D, T>(A_, I.lbound()+offset_,
                offset_ + I.ubound());
        }


        const T & operator() (Subscript i) const
        {
#ifdef TNT_BOUNDS_CHECK
            assert(TNT_BASE_OFFSET <= i);
            assert(i <= dim() + (TNT_BASE_OFFSET-1));
#endif
            return A_(i+offset_);
        }




};

template <class Array1D, class T>
ostream& operator<<(ostream &s, const_Region1D<Array1D,T> &A)
{
    Subscript N=A.dim();

    for (Subscript i=1; i<=N; i++)
        s << A(i) << endl;

    return s;
}



#endif
// const_Region1D_H
