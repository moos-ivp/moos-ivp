// Template Numerical Toolkit (TNT) for Linear Algebra
//
// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates
//
// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology


//  Templated sparse vector (Fortran conventions).
//  Used primarily to interface with Fortran sparse matrix libaries.
//  (CANNOT BE USED AS AN STL CONTAINER.)

#ifndef FSPVEC_H
#define FSPVEC_H

#include "tnt.h"
#include "vec.h"
#include <stdlib.h>
#include <assert.h>
#include <iostream.h>
#include <strstream.h>


template <class T>
class Fortran_sparse_vector 
{


  public:

    typedef Subscript   size_type;
    typedef         T   value_type;
    typedef         T   element_type;
    typedef         T*  pointer;
    typedef         T*  iterator;
    typedef         T&  reference;
    typedef const   T*  const_iterator;
    typedef const   T&  const_reference;

    Subscript lbound() const { return 1;}
 
  protected:
    Vector<T>   val_;
    Vector<Subscript> index_;
    Subscript dim_;                  // prescribed dimension


  public:

    // size and shape information

    Subscript dim() const { return dim_; }
    Subscript num_nonzeros() const { return val_.dim(); }

    // access

    T& val(Subscript i) { return val_(i); }
    const T& val(Subscript i) const { return val_(i); }

    Subscript &index(Subscript i) { return index_(i); }
    const Subscript &index(Subscript i) const { return index_(i); }

    // constructors

    Fortran_sparse_vector() : val_(), index_(), dim_(0)  {};
    Fortran_sparse_vector(Subscript N, Subscript nz) : val_(nz), 
            index_(nz), dim_(N)  {};
    Fortran_sparse_vector(Subscript N, Subscript nz, const T *values,
        const Subscript *indices): val_(nz, values), index_(nz, indices),
            dim_(N) {}

    Fortran_sparse_vector(const Fortran_sparse_vector<T> &S): 
        val_(S.val_), index_(S.index_), dim_(S.dim_) {}

    // initialize from string, e.g.
    //
    //  Fortran_sparse_vector<T> A(N, 2, "1.0 2.1", "1 3");
    //
    Fortran_sparse_vector(Subscript N, Subscript nz, char *v,
        char *ind) : val_(nz, v), index_(nz, ind), dim_(N) {}
    
    // assignments

    Fortran_sparse_vector<T> & newsize(Subscript N, Subscript nz)
    {
        val_.newsize(nz);
        index_.newsize(nz);
        dim_ = N;
        return *this;
    }

    Fortran_sparse_vector<T> & operator=( const Fortran_sparse_vector<T> &A)
    {
        val_ = A.val_;
        index_ = A.index_;
        dim_ = A.dim_;

        return *this;
    }

    // methods

    // PE 09/30/98: added <> (g++ quirk)
    friend istream& operator>><>(istream &s, Fortran_sparse_vector<T> &A);

};


/* ***************************  I/O  ********************************/

template <class T>
ostream& operator<<(ostream &s, const Fortran_sparse_vector<T> &A)
{
    // output format is :   N nz val1 ind1 val2 ind2 ... 
    Subscript nz=A.num_nonzeros();

    s <<  A.dim() << " " << nz << endl;

    for (Subscript i=1; i<=nz; i++)
        s   << A.val(i) << "  " << A.index(i) << endl;
    s << endl;

    return s;
}


template <class T>
istream& operator>>(istream &s, Fortran_sparse_vector<T> &A)
{
    // output format is :   N nz val1 ind1 val2 ind2 ... 

    Subscript N;
    Subscript nz;

    s >> N >> nz;

    A.newsize(N, nz);

    for (Subscript i=1; i<=nz; i++)
            s >>  A.val(i) >> A.index(i);


    return s;
}



#endif
// FSPVEC_H
