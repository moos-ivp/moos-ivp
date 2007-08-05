// Template Numerical Toolkit (TNT) for Linear Algebra
//
// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates
//
// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology

// File Name:fmat.h
// Adaption Author: P.E; Ding Wang
// P.Elisseeff 10/14/98: added sparse matrix conversion 
// Date: 11/26/04
// Description: In this header file, a new data type -- Fortran_matrix<T> and its functions are defined
// Fortran-compatible matrix: column oriented, 1-based (i,j) indexing

#ifndef FMAT_H
#define FMAT_H

#include "subscrpt.h"
#include "vec.h"
//#include <stdlib.h>
//#include <assert.h>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#ifdef TNT_USE_REGIONS
#include "region2d.h"
#endif

using namespace std; // Ding Wang: declare std namespace
template <class T> class Fortran_matrix;// Ding Wang: this line and the next one are to predeclare >>, so that >> can be a friend. 
template <class T> istream& operator>>(istream &, Fortran_matrix<T> &);
// simple 1-based, column oriented Matrix class

template <class T>
class Fortran_matrix 
{
  public:
 //define some types in understandable names
    typedef         T   value_type;
    typedef         T   element_type;
    typedef         T*  pointer;
    typedef         T*  iterator;
    typedef         T&  reference;
    typedef const   T*  const_iterator;
    typedef const   T&  const_reference;

    unsigned long int lbound() const { return 1;}// not used
 
  protected:
    T* v_;//0-offset array containing all elements of the matrix; v_[i] is the i+1th colomnwise element.
    unsigned long int m_;//row #
    unsigned long int n_;//column #
    T** col_;// these are adjusted to simulate 1-offset

    void initialize(unsigned long int M, unsigned long int N)//M row #, N column #, internal helper function to create the array of row pointers
      {
        // adjust col_[] pointers so that they are 1-offset:
        // col_[j][i] is really col_[j-1][i-1];
        // v_[] is the internal contiguous array, it is still 0-offset

	assert((v_ = new T[M*N]) != NULL);//dynamically generate v_[] and make sure it's not null
	assert((col_ = new T*[N]) != NULL);//dynamically generate col_[] and make sure it's not null

	m_ = M;//copy row #
	n_ = N;//copy column #
	T* p = v_ - 1;//p is the 1-offset pointer              
	for (unsigned long int i=0; i<N; i++)
	  {
	    col_[i] = p;//save 1-offset pointers of 1st row elements into col_[]
	    p += M ;//p always points to 1-offset position of the 1st row elements            
	  }
	col_ --;//col_[] becomes 1-offset array; col_[i>0]=1-offset pointer of (1,i)th element, i.e. col_[i>0]=the pointer of (M,i-1)th element 
      }
   
    void copy(const T* v)//copy array v[] to v_[]
    {
        unsigned long int N = m_ * n_;
        unsigned long int i;

#ifdef TNT_UNROLL_LOOPS//this part is not used
        unsigned long int Nmod4 = N & 4;
        unsigned long int N4 = N - Nmod4;

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
            v_[i] = v[i];//assignment
#endif      
    }

    void set(const T& val)//make all item of v[] equal to val
    {
        unsigned long int N = m_ * n_;
        unsigned long int i;

#ifdef TNT_UNROLL_LOOPS//this part is not used
        unsigned long int Nmod4 = N & 4;
        unsigned long int N4 = N - Nmod4;

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
            v_[i] = val;//assignment
        
#endif      
    }
    


    void destroy()//destruct the internal vector and free space
      {     
	if (v_ == NULL) return ;//do nothing, if no memory has been previously allocated 

	delete [] (v_); //if we are here, then matrix was previously allocated     
        col_ ++;// changed back to 0-offset
        delete [] (col_);
      }
 public:

    //////////////////////////////////////access/////////////////////////////////////////////////////////////

    T* begin() { return v_; }//return the first pointer
    const T* begin() const { return v_;}//return the first pointer

    T* end() { return v_ + m_*n_; }//return the last pointer
    const T* end() const { return v_ + m_*n_; }//return the last pointer
    
    ///////////////////////////////////////constructors//////////////////////////////////////////////////////

    Fortran_matrix() : v_(0), m_(0), n_(0), col_(0)  {};

    Fortran_matrix(const Fortran_matrix<T> &A)//initialize and copy matrix A
    {
        initialize(A.m_, A.n_);//initialize with dimension of A 
        copy(A.v_);//copy A
    }

    Fortran_matrix(unsigned long int M, unsigned long int N, const T& value = T(0))//initialize with dimension MxN and copy constant value
    {
        initialize(M,N);//initialize with dimension MxN
        set(value);//copy constant value to each item
    }

    Fortran_matrix(unsigned long int M, unsigned long int N, const T* v)//initialize and copy array v[]
    {
        initialize(M,N);//initialize with dimension MxN
        copy(v);//copy v
    }


    Fortran_matrix(unsigned long int M, unsigned long int N, char *s)//initialize and copy string s, adapted by Ding Wang
    {
        initialize(M,N);//initialize with dimension MxN
        istringstream ins(s);//covert s to stream

        unsigned long int i, j;

	for (j=1; j<=N; j++)
	  for (i=1; i<=M; i++)
	    ins >> (*this)(i,j);//copy, column first, row second
    }

    Fortran_matrix(unsigned long int M, unsigned long int N, const Vector<T> & x)//Added by Ding Wang; initialize and copy vector x
      {
	assert(x.dim()==M*N);//make sure the dimension is consistent
        initialize(M,N);//initialize with dimension MxN
	for (int i=1; static_cast<unsigned long int>(i)<=M*N; i++)
	  {
	    v_[i-1]=x(i);//copy vector x, column first, row second
	  }
      }

    // destructors
    ~Fortran_matrix(void) { destroy(); }

    ///////////////////////////////////////////assignments and access////////////////////////////////////////

    Fortran_matrix<T>& operator=(const Fortran_matrix<T> &A)//copy from a matrix
    {
        if (v_ == A.v_)//if they are the same, then just do nothing
            return *this;

        if (m_ == A.m_  && n_ == A.n_)//if the size is the same, no need to re-alloc
            copy(A.v_);

        else
        {//destroy first and rebuild a new one and then copy values
            destroy();
            initialize(A.m_, A.n_);
            copy(A.v_);
        }

        return *this;
    }

#ifdef FCOOMAT_H//don't understand what the below codes are doing
    Fortran_matrix<T> operator=(const Fortran_coordinate_matrix<T> &A) // added P.E. 10/14/98
    {
      unsigned long int M = A.num_rows();
      unsigned long int N = A.num_cols();
      unsigned long int nz = A.num_nonzeros();
      initialize(M,N);
      
      for (int i=1; i<=nz; i++)
      (*this)(A.row_ind(i), A.col_ind(i)) = A.val(i);
      
      return (*this);
    }
#endif

    Fortran_matrix<T>& operator=(const T& scalar)//copy from a constant scalar
    { 
        set(scalar); //copy to each item
        return *this;
    }

    inline reference operator()(unsigned long int i, unsigned long int j)//define extract symbol (), () is 1 offset
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i <= m_) ;
        assert(1<=j);
        assert(j <= n_);
#endif
        return col_[j][i];//note: col_ has been 1-offset 
    }

    inline const_reference operator() (unsigned long int i, unsigned long int j) const//define extract symbol (), () is 1 offset
    {
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i <= m_) ;
        assert(1<=j);
        assert(j <= n_);
#endif
        return col_[j][i];//note: col_ has been 1-offset  
    }


    friend istream& operator>><T>(istream &s, Fortran_matrix<T> &A);


#ifdef TNT_USE_REGIONS//the below codes are not used and don't understand

    typedef Region2D<Fortran_matrix<T> > Region;
    typedef const_Region2D<Fortran_matrix<T>,T > const_Region;

    Region operator()(const Index1D &I, const Index1D &J)
    {
        return Region(*this, I,J);
    }

    const_Region operator()(const Index1D &I, const Index1D &J) const
    {
        return const_Region(*this, I,J);
    }

#endif

    Fortran_matrix<T> From_Vector(unsigned long int M, unsigned long int N, const Vector<T> & x)//Added by Ding Wang; Transform vector x to a matrix
      {
	assert(static_cast<unsigned long int>(x.dim())==M*N);//make sure the dimension is consistent
        newsize(M,N);//initialize with dimension MxN
	for (int i=1; i<=M*N; i++)
	  {
	    v_[i-1]=x(i);//copy vector x, column first, row second
	  }
	return *this;
      }

    ////////////////////////////dimension, size change, row, column, norm ///////////////////////////////////

    unsigned long int dim(unsigned long int d) const //output row or column #
    {
#ifdef TNT_BOUNDS_CHECK
       assert( d >= 1);
        assert( d <= 2);
#endif
        return (d==1) ? m_ : ((d==2) ? n_ : 0); //1: output row #; 2: output column #
    }

    Vector<int> dim(void)//output row and column # in a vector
      {
	Vector<int> dimtemp(2);
	dimtemp(1)=m_;
	dimtemp(2)=n_;
	return dimtemp;
      }

    unsigned long int num_rows() const { return m_; }//output row #
    unsigned long int num_cols() const { return n_; }//output column #

    Fortran_matrix<T>& newsize(unsigned long int M, unsigned long int N)//adapted by Ding Wang; newsize can change matrix's size; if the new size is the same as the old one, nothing will be done; or not a new matrix will be created and all items will be unassigned, unstable
      {
        if (num_rows() == M && num_cols() == N)
	  return *this;
	else
	  {
	    destroy();
	    initialize(M,N);
	    return *this;
	  }
      }

    Fortran_matrix<T>& change_size(unsigned long int M, unsigned long int N)//added by Ding Wang; change_size can change matrix's size; if the new size is the same as the old one, nothing will be done; if longer, then new items will be 0 but old items keep the same; if shorter, just truncate
    {
      unsigned long int m_bk=m_;
      unsigned long int n_bk=n_;
      if (m_==M && n_==N) 
	return *this;
      else
	{
	  const Fortran_matrix<T> tempmat(*this);
	  destroy();initialize(M, N); //destroy and recreate
	  for (unsigned long int i=1; i<=M; i++)//copy old values and/or assign 0
	    {
	      for (unsigned long int j=1; j<=N; j++)
		{
		  col_[j][i]=(i<=m_bk&&j<=n_bk ? tempmat(i,j) : 0);//if new size is longer, assign 0 to extra items, col_ has been 1-offset  
		}
	    }
	}
      return *this;
    }

    Fortran_matrix<T> col (unsigned long int i)//output the whole column
      {
	assert(i>0 && i<=n_); // boundary check
	Fortran_matrix<T> column(num_rows(),1); //create a 1-column matrix
	for (int j=1; j<=num_rows(); j++) column(j,1)=col_[i][j]; //copy that column
	return column;
      }

    Fortran_matrix<T> row (unsigned long int i)//output the whole row
      {
	assert(i>0 && i<=m_); // boundary check
	Fortran_matrix<T> rows(1, num_cols()); //create a 1-row matrix      
	for (int j=1; j<=num_cols(); j++) rows(1,j)=col_[j][i]; //copy that row
	return rows;
      }

    T norm ()//output the norm of a 1 column matrix
      {
	T normal=0;
	assert(num_cols()==1);
	for (int j=1; j<=num_rows(); j++) normal+=pow(abs(col_[1][j]),2);
	normal=sqrt(normal);
	return normal;
      }

};


///////////////////////////////////////////  I/O Operator ///////////////////////////////////////////////////

template <class T>
ostream& operator<<(ostream &s, const Fortran_matrix<T> &A)//output the matrix's dimension and content to stream
{
    unsigned long int M=A.num_rows();
    unsigned long int N=A.num_cols();

    s << M << " " << N << endl; // output dimension info

    for (unsigned long int i=1; i<=M; i++)
    {
        for (unsigned long int j=1; j<=N; j++)
        {
	  s << A(i,j) << " "; // output content
        }
        s << endl;
    }


    return s;
}

template <class T>
istream& operator>>(istream &s, Fortran_matrix<T> &A)//input from stream
{

    unsigned long int M, N;

    s >> M >> N; // input dimension

    if ( !(M == A.m_ && N == A.n_) )
    {
        A.destroy();
        A.initialize(M,N); //destroy and recreate
    }

    for (unsigned long int j=1; j<=N; j++)
      for (unsigned long int i=1; i<=M; i++)//column first, row second
        {
	  s >>  A(i,j); //input content
        }


    return s;
}

//////////////////////////////////////// basic matrix algorithms ////////////////////////////////////////////

template <class T>
Fortran_matrix<T> operator+(const Fortran_matrix<T> &A, 
			    const Fortran_matrix<T> &B)//elementwise plus
{
    unsigned long int M = A.num_rows();
    unsigned long int N = A.num_cols();

    assert(M==B.num_rows());
    assert(N==B.num_cols());

    Fortran_matrix<T> tmp(M,N);
    unsigned long int i,j;

    for (i=1; i<=M; i++)
        for (j=1; j<=N; j++)
	  tmp(i,j) = A(i,j) + B(i,j); //add elementwise

    return tmp;
}

template <class T>
Fortran_matrix<T> operator-(const Fortran_matrix<T> &A, 
			    const Fortran_matrix<T> &B)//elementwise minus
{
    unsigned long int M = A.num_rows();
    unsigned long int N = A.num_cols();

    assert(M==B.num_rows());
    assert(N==B.num_cols());

    Fortran_matrix<T> tmp(M,N);
    unsigned long int i,j;

    for (i=1; i<=M; i++)
        for (j=1; j<=N; j++)
            tmp(i,j) = A(i,j) - B(i,j);//elementwise minus

    return tmp;
}

// element-wise multiplication  (use matmult() below for matrix
// multiplication in the linear algebra sense.)
template <class T>
Fortran_matrix<T> mult_element(const Fortran_matrix<T> &A, 
			       const Fortran_matrix<T> &B)//equal to .* in matlab
{
    unsigned long int M = A.num_rows();
    unsigned long int N = A.num_cols();

    assert(M==B.num_rows());
    assert(N==B.num_cols());

    Fortran_matrix<T> tmp(M,N);
    unsigned long int i,j;

    for (i=1; i<=M; i++)
        for (j=1; j<=N; j++)
	  tmp(i,j) = A(i,j) * B(i,j);//elementwise times

    return tmp;
}


template <class T>
Fortran_matrix<T> transpose(const Fortran_matrix<T> &A)//transpose
{
    unsigned long int M = A.num_rows();
    unsigned long int N = A.num_cols();

    Fortran_matrix<T> S(N,M);
    unsigned long int i, j;

    for (i=1; i<=M; i++)
        for (j=1; j<=N; j++)
            S(j,i) = A(i,j);

    return S;
}

template <class T>
Fortran_matrix< complex<T> > transconj(const Fortran_matrix< complex<T> > &A)// PE 09/30/98 added conjugate transpose 
{
  unsigned long int M = A.num_rows();
  unsigned long int N = A.num_cols();

  Fortran_matrix< complex<T> > S(N,M);
  unsigned long int i, j;

  for (i=1; i<=M; i++)
    for (j=1; j<=N; j++)
      S(j,i) = conj(A(i,j));

  return S;
}
    
template <class T>
inline Fortran_matrix<T> matmult(const Fortran_matrix<T>  &A, 
				 const Fortran_matrix<T> &B)//this is just matrix produce, two matrix produce and generate the 3rd matrix
{

#ifdef TNT_BOUNDS_CHECK
    assert(A.num_cols() == B.num_rows());
#endif

    unsigned long int M = A.num_rows();
    unsigned long int N = A.num_cols();
    unsigned long int K = B.num_cols();

    Fortran_matrix<T> tmp(M,K);
    T sum;

    for (unsigned long int i=1; i<=M; i++)
    for (unsigned long int k=1; k<=K; k++)
    {
        sum = 0;
        for (unsigned long int j=1; j<=N; j++)
            sum = sum +  A(i,j) * B(j,k);

        tmp(i,k) = sum; 
    }

    return tmp;
}

template <class T>
inline Fortran_matrix<T> operator*(const Fortran_matrix<T> &A, 
				   const Fortran_matrix<T> &B)//this is just matrix produce, two matrix produce and generate the 3rd matrix
{
    return matmult(A,B);
}

template <class T>
inline int matmult(Fortran_matrix<T>& C, const Fortran_matrix<T>  &A, 
		   const Fortran_matrix<T> &B)//C=A*B, two matrix produce and generate the 3rd matrix
{

    assert(A.num_cols() == B.num_rows());

    unsigned long int M = A.num_rows();
    unsigned long int N = A.num_cols();
    unsigned long int K = B.num_cols();

    C.newsize(M,K);         // adjust shape of C, if necessary


    T sum; 

    const T* row_i;
    const T* col_k;

    for (unsigned long int i=1; i<=M; i++)
    {
        for (unsigned long int k=1; k<=K; k++)
        {
            row_i = &A(i,1);
            col_k = &B(1,k);
            sum = 0;
            for (unsigned long int j=1; j<=N; j++)
            {
                sum +=  *row_i * *col_k;
                row_i += M;
                col_k ++;
            }
        
            C(i,k) = sum; 
        }

    }

    return 0;
}


template <class T>
Vector<T> matmult(const Fortran_matrix<T>  &A, 
		  const Vector<T> &x)//this is just A*x, matrix A times a vector and generate another vector
{

#ifdef TNT_BOUNDS_CHECK
    assert(A.num_cols() == x.dim());
#endif

    unsigned long int M = A.num_rows();
    unsigned long int N = A.num_cols();

    Vector<T> tmp(M);
    T sum;

    for (unsigned long int i=1; i<=M; i++)
    {
        sum = 0;
        for (unsigned long int j=1; j<=N; j++)
            sum = sum +  A(i,j) * x(j);

        tmp(i) = sum; 
    }

    return tmp;
}

template <class T>
inline Vector<T> operator*(const Fortran_matrix<T>  &A, 
			   const Vector<T> &x)//this is just A*x, matrix A times a vector and generate another vector
{
    return matmult(A,x);
}

template <class T,class TT>
inline Fortran_matrix<T> operator*(const Fortran_matrix<T>  &A, 
				   const TT &x)//this is just A*x, matrix A times a scalar
{
    unsigned long int M = A.num_rows();
    unsigned long int N = A.num_cols();

    unsigned long int MN = M*N; 

    Fortran_matrix<T> res(M,N);
    
    for (int t=1; t <=M; t++)
      for (int j=1; j<=N; j++) res(t,j)=A(t,j)*x;
    return res;
} 

//This is redundent 
/*  template <class T> */
/*  inline Fortran_matrix<T> operator*(const Fortran_matrix<T>  &A,  */
/*  				   const T &x)//this is just A*x, matrix A times a scalar */
/*  { */
/*      unsigned long int M = A.num_rows(); */
/*      unsigned long int N = A.num_cols(); */

/*      unsigned long int MN = M*N;  */

/*      Fortran_matrix<T> res(M,N); */
/*      const T* a = A.begin(); */
/*      T* t = res.begin(); */
/*      T* tend = res.end(); */

/*      for (t=res.begin(); t < tend; t++, a++) */
/*          *t = *a * x; */

/*      return res; */
/*  }  */

template <class T,class TT>
inline Fortran_matrix<T> operator/(const Fortran_matrix<T>  &A, 
				   const TT &x)//this is just A/x, matrix A is divided by a scalar
{
    unsigned long int M = A.num_rows();
    unsigned long int N = A.num_cols();

    unsigned long int MN = M*N; 

    Fortran_matrix<T> res(M,N);
    
    for (int t=1; t <=M; t++)
      for (int j=1; j<=N; j++) res(t,j)=A(t,j)/x;
    return res;
}

template <class T,class TT>
Fortran_matrix<T> Append_at_Bottom(const Fortran_matrix<T>  &A, 
			 const TT &x)//
{
    long int M = A.num_rows();
    long int N = A.num_cols();

    Fortran_matrix<T> B(A);
    B.change_size(M+1, N);
    
    for (int t=1; t <=N; t++) B(M+1,t)=x;
    return B;
}
#endif
