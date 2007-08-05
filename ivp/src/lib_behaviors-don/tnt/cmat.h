// Template Numerical Toolkit (TNT) for Linear Algebra
//
// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates
//
// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology


// C compatible matrix: row-oriented, 0-based [i][j] and 1-based (i,j) indexing
//

#ifndef CMAT_H
#define CMAT_H

#include "subscrpt.h"
#include "vec.h"
#include <stdlib.h>
#include <assert.h>
#include <iostream.h>
#include <strstream.h>
#ifdef TNT_USE_REGIONS
#include "region2d.h"
#endif

template <class T>
class C_matrix 
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
    Subscript m_;
    Subscript n_;
    Subscript mn_;      // total size
    T* v_;                  
    T** row_;           
    T* vm1_ ;       // these point to the same data, but are 1-based 
    T** rowm1_;

    // internal helper function to create the array
    // of row pointers

    void initialize(Subscript M, Subscript N)
    {
        mn_ = M*N;
        m_ = M;
        n_ = N;

        assert((v_ = new T[mn_]) != NULL);
        assert((row_ = new T*[M]) != NULL);
        assert((rowm1_ = new T*[M]) != NULL);

        T* p = v_;              
        vm1_ = v_ - 1;
        for (Subscript i=0; i<M; i++)
        {
            row_[i] = p;
            rowm1_[i] = p-1;
            p += N ;
            
        }

        rowm1_ -- ;     // compensate for 1-based offset
    }
   
    void copy(const T*  v)
    {
        Subscript N = m_ * n_;
        Subscript i;

#ifdef TNT_UNROLL
        Subscript Nmod4 = N & 4;
        Subscript N4 = N - Nmod4;

        for (i=0; i<N4; i+=4)
        {
            v_[i] = v[i];
            v_[i+1] = v[i+1];
            v_[i+2] = v[i+2];
            v_[i+3] = v[i+3];
        }

        for (i=N4; i< N; i++)
            v_[i] = v[i];
#else

        for (i=0; i< N; i++)
            v_[i] = v[i];
#endif      
    }

    void set(const T& val)
    {
        Subscript N = m_ * n_;
        Subscript i;

#ifdef TNT_UNROLL
        Subscript Nmod4 = N & 4;
        Subscript N4 = N - Nmod4;

        for (i=0; i<N4; i+=4)
        {
            v_[i] = val;
            v_[i+1] = val;
            v_[i+2] = val;
            v_[i+3] = val; 
        }

        for (i=N4; i< N; i++)
            v_[i] = val;
#else

        for (i=0; i< N; i++)
            v_[i] = val;
        
#endif      
    }
    

    
    void destroy()
    {     
        /* do nothing, if no memory has been previously allocated */
        if (v_ == NULL) return ;

        /* if we are here, then matrix was previously allocated */
        if (v_ != NULL) delete [] (v_);     
        if (row_ != NULL) delete [] (row_);
        if (rowm1_ != NULL ) delete [] (rowm1_);
    }


  public:

    Subscript size() const { return mn_; }

    // constructors

    C_matrix() : m_(0), n_(0), mn_(0), v_(0), row_(0), vm1_(0), rowm1_(0) {};

    C_matrix(const C_matrix<T> &A)
    {
        initialize(A.m_, A.n_);
        copy(A.v_);
    }

    C_matrix(Subscript M, Subscript N, const T& value = T(0))
    {
        initialize(M,N);
        set(value);
    }

    C_matrix(Subscript M, Subscript N, const T* v)
    {
        initialize(M,N);
        copy(v);
    }

    C_matrix(Subscript M, Subscript N, char *s)
    {
        initialize(M,N);
        istrstream ins(s);

        Subscript i, j;

        for (i=0; i<M; i++)
            for (j=0; j<N; j++)
                ins >> row_[i][j];
    }

    C_matrix<T>& newsize(Subscript M, Subscript N)
    {
        if (num_rows() == M && num_cols() == N)
            return *this;

        destroy();
        initialize(M,N);
        
        return *this;
    }

    // destructor
    ~C_matrix(void) { /* destroy(); */ }

    // assignments
    //
    C_matrix<T>& operator=(const C_matrix<T> &A)
    {
        if (v_ == A.v_)
            return *this;

        if (m_ == A.m_  && n_ == A.n_)      // no need to re-alloc
            copy(A.v_);

        else
        {
            destroy();
            initialize(A.m_, A.n_);
            copy(A.v_);
        }

        return *this;
    }
        
    C_matrix<T>& operator=(const T& scalar)
    { 
        set(scalar); 
        return *this;
    }


    Subscript dim(Subscript d) const 
    {
#ifdef TNT_BOUNDS_CHECK
       assert( d >= 1);
        assert( d <= 2);
#endif
        return (d==1) ? m_ : ((d==2) ? n_ : 0); 
    }

    Subscript num_rows() const { return m_; }
    Subscript num_cols() const { return n_; }




    inline T* operator[](Subscript i)
    {
#ifdef TNT_BOUNDS_CHECK
        assert(0<=i);
        assert(i < m_) ;
#endif
        return row_[i];
    }

    inline const T* operator[](Subscript i) const
    {
#ifdef TNT_BOUNDS_CHECK
        assert(0<=i);
        assert(i < m_) ;
#endif
        return row_[i];
    }

    inline reference operator()(Subscript i)
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i <= mn_) ;
#endif
        return vm1_[i]; 
    }

    inline const_reference operator()(Subscript i) const
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i <= mn_) ;
#endif
        return vm1_[i]; 
    }



    inline reference operator()(Subscript i, Subscript j)
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i <= m_) ;
        assert(1<=j);
        assert(j <= n_);
#endif
        return  rowm1_[i][j]; 
    }


    
    inline const_reference operator() (Subscript i, Subscript j) const
    {
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i <= m_) ;
        assert(1<=j);
        assert(j <= n_);
#endif
        return rowm1_[i][j]; 
    }
    
    // PE 09/29/98: added <> below (g++ quirk)
    friend istream& operator>><>(istream &s, C_matrix<T> &A);


#ifdef TNT_USE_REGIONS

    typedef Region2D<C_matrix<T> > Region;
    

    Region operator()(const Index1D &I, const Index1D &J)
    {
        return Region(*this, I,J);
    }


    typedef const_Region2D<C_matrix<T>,T > const_Region;
    const_Region operator()(const Index1D &I, const Index1D &J) const
    {
        return const_Region(*this, I,J);
    }

#endif


};


/* ***************************  I/O  ********************************/

template <class T>
ostream& operator<<(ostream &s, const C_matrix<T> &A)
{
    Subscript M=A.num_rows();
    Subscript N=A.num_cols();

    s << M << " " << N << endl;

    for (Subscript i=0; i<M; i++)
    {
        for (Subscript j=0; j<N; j++)
        {
            s << A[i][j] << " ";
        }
        s << endl;
    }


    return s;
}

template <class T>
istream& operator>>(istream &s, C_matrix<T> &A)
{

    Subscript M, N;

    s >> M >> N;

    if ( !(M == A.m_ && N == A.n_) )
    {
        A.destroy();
        A.initialize(M,N);
    }


    for (Subscript i=0; i<M; i++)
        for (Subscript j=0; j<N; j++)
        {
            s >>  A[i][j];
        }


    return s;
}

//*******************[ basic matrix algorithms ]***************************


template <class T>
C_matrix<T> operator+(const C_matrix<T> &A, 
    const C_matrix<T> &B)
{
    Subscript M = A.num_rows();
    Subscript N = A.num_cols();

    assert(M==B.num_rows());
    assert(N==B.num_cols());

    C_matrix<T> tmp(M,N);
    Subscript i,j;

    for (i=0; i<M; i++)
        for (j=0; j<N; j++)
            tmp[i][j] = A[i][j] + B[i][j];

    return tmp;
}

template <class T>
C_matrix<T> operator-(const C_matrix<T> &A, 
    const C_matrix<T> &B)
{
    Subscript M = A.num_rows();
    Subscript N = A.num_cols();

    assert(M==B.num_rows());
    assert(N==B.num_cols());

    C_matrix<T> tmp(M,N);
    Subscript i,j;

    for (i=0; i<M; i++)
        for (j=0; j<N; j++)
            tmp[i][j] = A[i][j] - B[i][j];

    return tmp;
}

template <class T>
C_matrix<T> mult_element(const C_matrix<T> &A, 
    const C_matrix<T> &B)
{
    Subscript M = A.num_rows();
    Subscript N = A.num_cols();

    assert(M==B.num_rows());
    assert(N==B.num_cols());

    C_matrix<T> tmp(M,N);
    Subscript i,j;

    for (i=0; i<M; i++)
        for (j=0; j<N; j++)
            tmp[i][j] = A[i][j] * B[i][j];

    return tmp;
}


template <class T>
C_matrix<T> transpose(const C_matrix<T> &A)
{
    Subscript M = A.num_rows();
    Subscript N = A.num_cols();

    C_matrix<T> S(N,M);
    Subscript i, j;

    for (i=0; i<M; i++)
        for (j=0; j<N; j++)
            S[j][i] = A[i][j];

    return S;
}


    
template <class T>
inline C_matrix<T> matmult(const C_matrix<T>  &A, 
    const C_matrix<T> &B)
{

#ifdef TNT_BOUNDS_CHECK
    assert(A.num_cols() == B.num_rows());
#endif

    Subscript M = A.num_rows();
    Subscript N = A.num_cols();
    Subscript K = B.num_cols();

    C_matrix<T> tmp(M,K);
    T sum;

    for (Subscript i=0; i<M; i++)
    for (Subscript k=0; k<K; k++)
    {
        sum = 0;
        for (Subscript j=0; j<N; j++)
            sum = sum +  A[i][j] * B[j][k];

        tmp[i][k] = sum; 
    }

    return tmp;
}

template <class T>
inline C_matrix<T> operator*(const C_matrix<T>  &A, 
    const C_matrix<T> &B)
{
    return matmult(A,B);
}

template <class T>
inline int matmult(C_matrix<T>& C, const C_matrix<T>  &A, 
    const C_matrix<T> &B)
{

    assert(A.num_cols() == B.num_rows());

    Subscript M = A.num_rows();
    Subscript N = A.num_cols();
    Subscript K = B.num_cols();

    C.newsize(M,K);

    T sum;

    const T* row_i;
    const T* col_k;

    for (Subscript i=0; i<M; i++)
    for (Subscript k=0; k<K; k++)
    {
        row_i  = &(A[i][0]);
        col_k  = &(B[0][k]);
        sum = 0;
        for (Subscript j=0; j<N; j++)
        {
            sum  += *row_i * *col_k;
            row_i++;
            col_k += K;
        }
        C[i][k] = sum; 
    }

    return 0;
}


template <class T>
Vector<T> matmult(const C_matrix<T>  &A, const Vector<T> &x)
{

#ifdef TNT_BOUNDS_CHECK
    assert(A.num_cols() == x.dim());
#endif

    Subscript M = A.num_rows();
    Subscript N = A.num_cols();

    Vector<T> tmp(M);
    T sum;

    for (Subscript i=0; i<M; i++)
    {
        sum = 0;
        const T* rowi = A[i];
        for (Subscript j=0; j<N; j++)
            sum = sum +  rowi[j] * x[j];

        tmp[i] = sum; 
    }

    return tmp;
}

template <class T>
inline Vector<T> operator*(const C_matrix<T>  &A, const Vector<T> &x)
{
    return matmult(A,x);
}

#endif
// CMAT_H
