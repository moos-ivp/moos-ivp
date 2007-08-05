// Template Numerical Toolkit (TNT) for Linear Algebra

// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates

// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology
// Basic TNT  numerical vector (0-based [i] AND (i) indexing )

// File Name: vec.h
// Adaption Author: P.E; Ding Wang
// Date: 8/29/04
// Description: In this header file, a new data type -- Vector<T> and related functions are defined. 

#ifndef VEC_H
#define VEC_H

#include "subscrpt.h"
//#include <stdlib.h>
//#include <assert.h>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <complex>
#include <sstream>

using namespace std; // Ding Wang: declare std namespace
template <class T> class Vector;// Ding Wang: this line and the next one are to predeclare >>, so that >> can be a friend of Vector.
template <class T> istream& operator>>(istream &, Vector<T> &);

template <class T>
class Fortran_matrix; // Because Ding Wang added a Fortran_matrix append function

template <class T>
class Vector 
{
 public:
  //define some types' alias.
  typedef unsigned long int   size_type; // adapted by Ding Wang
  typedef         T   value_type;
  typedef         T   element_type;
  typedef         T*  pointer;
  typedef         T*  iterator;
  typedef         T&  reference;
  typedef const   T*  const_iterator;
  typedef const   T&  const_reference;

  unsigned long int lbound() const { return 1;}// not used
 
 protected:
  T* v_; // 0-offset array containing all elements of the vector; v_[i] is the i+1th element of the vector.
  T* vm1_; // 1-offset array containing all elements of the vector; v_[i] is the ith element of the vector.
  unsigned long int n_; // array's size, i.e. vector's length

  void initialize(unsigned long int N)//internal helper function to initialize v_, vm1_ and n_; N is vector's length, nonnegative.
    {
      assert(v_ == NULL);//assume v_[] is an empty array, dimension is 0
      v_ = new T[N];//the pointer of an array with size N is assigned to v_, v_[] is the internal contiguous array, it is still 0-offset 
      assert(v_  != NULL);//make sure dimension of v_[] is not 0
      vm1_ = v_-1;//adjust pointers so that they are 1-offset
      n_ = N;//copy and store size length
    }
   
  void copy(const T*  v) // copy array v[] to v_[]; this function can only be used after initialize is used firstly; sizes of v must be equal to n_!!!
    {
      unsigned long int N = n_; // copy length information to another variable, this requires that n_=length of v!!!
      unsigned long int i; // declare the index used in the below

#ifdef TNT_UNROLL //this part is not used and don't understand what it does
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

      for (i=0; i< N; i++) // loop for each element
	v_[i] = v[i]; // elementwise copy
#endif      
    }

  void set(const T& val) // copy val to all elements of v[].
    {
      unsigned long int N = n_; // copy length information in another variable
      unsigned long int i; // declare the index used in the below

#ifdef TNT_UNROLL //this part is not used and don't understand what it does
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

      for (i=0; i< N; i++) // loop for each element
	v_[i] = val; // elementwise copy
        
#endif      
    }
    
  void destroy() // destruct the vector and free space
    {     
      if (v_ == NULL) return ;//do nothing, if no memory has been previously allocated

      delete [] (v_);// if we are here, then matrix was previously allocated. delete v_[].    

      v_ = NULL; // make pointer null
      vm1_ = NULL; // make pointer null
    }

 public:

  //////////////////////////////////////access///////////////////////////////////////////////////////////////

  iterator begin() { return v_;}//return the pointer pointing to the first element
  iterator end()   { return v_ + n_; }//return the pointer pointing to the last element
  const iterator begin() const { return v_;}//return the pointer pointing to the first element, the 1st const means the returned pointer pointing to a const datum, the 2nd const means this function is a const function which will not change the object. Note that the returned pointer is not constant.
  const iterator end() const  { return v_ + n_; }//return the pointer pointing to the last element, the 1st const means the returned pointer pointing to a const datum, the 2nd const means this function is a const function which will not change the object. Note that the returned pointer is not constant.

  ////////////////////////////////////constructors///////////////////////////////////////////////////////////

  Vector() : v_(0), vm1_(0), n_(0)  {}; // initialize and assign 0 to length, null to arrays.

  Vector(const Vector<T> &A) : v_(0), vm1_(0), n_(0) // initialize and make a copy of vector A.
    {
      initialize(A.n_); // initialize with length of A 
      copy(A.v_); // copy A
    }

  Vector(unsigned long int N, const T& value = T(0)) :  v_(0), vm1_(0), n_(0) // initialize with lenght N >=0 and copy constant value to each element
    {
      initialize(N);//initialize with lenght N
      set(value);//copy constant value to each item
    }

  Vector(unsigned long int N, const T* v) :  v_(0), vm1_(0), n_(0) // initialize with length N and copy array v[]; N must be the length of v
    {
      initialize(N);//initialize with lenght N
      copy(v);//copy array v[]
    }

  Vector(unsigned long int N, char *s) :  v_(0), vm1_(0), n_(0) // initialize with length N and copy string s. N must be equal to the total number or items in s
    {
      initialize(N); // initialize with lenght N

      istringstream ins(s); // covert s to stream

      unsigned long int i; // declare index used below

      for (i=0; i<N; i++) // loop for each element
	ins >> v_[i];//copy thru I/O stream
    }//e.g. char p[]={'1',' ','2'}; Vector<float> k(2, p); k will be [1,2]

  ~Vector(void) { destroy(); } // destructor (added P.E. 10/12/98), delete the vector and free space

  ////////////////////////////////////////assignments and access operator////////////////////////////////////

  Vector<T>& operator=(const Vector<T> &A)//copy from vector A
  {
    if (v_ == A.v_)//if they are the same, then just do nothing
      return *this; // return back the original vector

    if (n_ == A.n_) //if the size is the same, no need to re-alloc
      copy(A.v_); // copy A to current vector

    else
      { 
	destroy(); // destroy first
	initialize(A.n_); // rebuild a new one with the same length
	copy(A.v_); // copy values
      }

    return *this; // return back the new vector
  }
        
  Vector<T>& operator=(const T& scalar)//copy constant scalar to each element of the current vector
  { 
    set(scalar);//copy to each item
    return *this;// return back the new vector
  }

  unsigned long int dim() const//output vector's length 
    {
      return  n_; // return back the length
    }

  unsigned long int size() const//output vector's length  
    {
      return  n_; // return back the length 
    }

  inline reference operator()(unsigned long int i)//define extract symbol (), () is 1 offset. i must be >=1 and <=n_. return the ith element. 
    { 
#ifdef TNT_BOUNDS_CHECK
      assert(1<=i); // i must >=1
      assert(i <= n_) ; // i must <= the length
#endif
      return vm1_[i]; // return the ith element 
    }

  inline const_reference operator() (unsigned long int i) const//define extract symbol (), () is 1 offset. this is a constant function and return back a constant variable, so no assignment is allowed. i must be >=1 and <=n_. return the ith element. 
    {
#ifdef TNT_BOUNDS_CHECK
      assert(1<=i); // i must >=1
      assert(i <= n_); // i must <= the length
#endif
      return vm1_[i]; // return the ith element  
    }

  inline reference operator[](unsigned long int i)//define extract symbol [], [] is 0 offset. i must be >=0 and <=n_-1. return the i-1th element.
    { 
#ifdef TNT_BOUNDS_CHECK
      assert(0<=i); // i must >=0
      assert(i < n_) ; // i must < the length
#endif
      return v_[i]; // return the i-1th element  
    }

  inline const_reference operator[](unsigned long int i) const//define extract symbol [], [] is 0 offset. i must be >=0 and <=n_-1. return the i-1th element.
    {
#ifdef TNT_BOUNDS_CHECK
      assert(0<=i); // i must >=0
      assert(i < n_) ; // i must < the length
#endif
      return v_[i]; // return the i-1th element  
    }

  friend istream& operator>> <T> (istream &s, Vector<T> &A);//adapted by Ding Wang; pay more attention to <T>; this is necessary if >> is defined as a template; and notice predeclare line 27 and line 28; the one used by P.E can not be linked correctly


  ///////////////////////////////////Size change and Append methods//////////////////////////////////////////

  Vector<T>& newsize(unsigned long int N)//adapted by Ding Wang; newsize can change vector's size; if the new size is the same as the old one, nothing will be done; or not a new vector will be created and all items will be unassigned, unstable. new size N>=0. returns back the new vector.
    {
      if (n_ == N) // if new size is equal to the current length
	return *this; // just return back without doing anything
      else // if new size is not equal to the current length
	{
	  destroy(); // destruct the current vector first
	  initialize(N); // create a new one with new size length
	  return *this; // return back the new one
	}
    }

  Vector<T>& change_size(unsigned long int N)//added by Ding Wang; change_size can change vector's size; if the new size is the same as the old one, nothing will be done; if longer, then new items will be 0 but old items keep the same; if shorter, just truncate. new size N>=0. returns back the new vector.
    {
      unsigned long int n_bk=n_; // store the old size
      if (n_ == N) // if new size is equal to the current length 
	return *this; // just return back without doing anything
      else // if new size is not equal to the current length
	{
	  const Vector<T> tempvec(*this); //create a backup for the old vector
	  destroy();initialize(N); // destroy the old one and rebuild it with length N
	  for (unsigned long int i=0; i<N; i++)//copy old values and/or assign 0
	    {
	      v_[i]=(i<n_bk ? tempvec(i+1) : 0); // if new size is longer, assign 0 to extra items; otherwise just copy old vector's elements
	    }
	}
      return *this; // return back the new one
    }

  Vector<T>& Append(const Fortran_matrix<T> & mat_) // added by Ding Wang; the matrix mat_ will be attached to vector columnwise. returns back the new vector.
    {
      Fortran_matrix<T> mat_temp=mat_; // make a backup for mat_, since mat_ is constant, this is not so convenient
      int n=mat_temp.dim()(1); // extract the row number of mat_
      int m=mat_temp.dim()(2); // extract the column number of mat_
      unsigned long int k=(*this).dim(); // extract the length of the current vector
      (*this).change_size(k+n*m); // change the length of the current vector
      for (int i=1; i<=n; i++) // loop for each row of mat_
	for (int j=1; j<=m; j++) // loop for each column of mat_
	  {
	    (*this)(k+(j-1)*n+i)=mat_temp(i,j);//copy, column first, row second
	  }
      return *this; // return back the new vector
    }

  Vector<T>& Append(const Vector<T> & vec_)//added by Ding Wang; vec_ is attached to the vector. returns back the new vector.
    {
      Vector<T> vec_temp=vec_; // backup vec_
      int n=vec_temp.dim(); // extract length of vec_
      int k=(*this).dim(); // extract the length of the current vector
      (*this).change_size(k+n); // change the length of the current vector
      for (int i=1; i<=n; i++) // loop for each element
	(*this)(k+i)=vec_temp(i); // copy vec_ to extra elements of the current vector
      return *this; // return back the new vector
    }

};

////////////////////////////////////////////I/O Operator/////////////////////////////////////////////////////

template <class T>
ostream& operator<<(ostream &s, const Vector<T> &A)//output the vector A's length and content to stream s
{
  unsigned long int N=A.dim(); // extract the length of A

  s <<  N << endl; // output the length information first

  for (unsigned long int i=0; i<N; i++) // loop for each element
    s   << A[i] << " " << endl; // output the i+1th element
  s << endl; // finish outputting

  return s; // return back the I/O stream
}

template <class T>
istream& operator>>(istream &s, Vector<T> &A) // input length and contents to vector A from I/O stream s. type length first and then content
{

  unsigned long int N; // this is going to store length information of A

  s >> N; // input the length of A

  if ( !(N == A.n_) ) // if N is not equal to the length of A
    {
      A.destroy(); // destory A
      A.initialize(N); // rebuild it with length N
    }


  for (unsigned long int i=0; i<N; i++) // loop for each element
    s >>  A[i]; // input the i+1th element

  return s; // return back the I/O stream
}//e.g. Vector<float> y;cin>>y; type 2, enter, 1, enter, 3, enter; it'll show y=[1,3]

///////////////////////////////////basic matrix algorithms///////////////////////////////////////////////////

template <class T>
Vector<T> operator+(const Vector<T> &A, 
		    const Vector<T> &B)//elementwise plus. A and B have the same length. returns back A+b.
{
  unsigned long int N = A.dim(); // A's length

  assert(N==B.dim()); // make sure A and B are the same long

  Vector<T> tmp(N); // create a vector with length N
  unsigned long int i; // declare the index used below

  for (i=0; i<N; i++) // loop for each element
    tmp[i] = A[i] + B[i]; //elementwise plus

  return tmp; // return back the sum vector
}

template <class T>
Vector<T> operator+(const Vector<T> &A, // added P.E. 11/13/98
		    const T &b)//each item of A plus b. returns back A+b.
{
  unsigned long int N = A.dim(); // A's length

  Vector<T> tmp(N); // create a vector with length N
  unsigned long int i; // declare the index used below

  for (i=0; i<N; i++) // loop for each element
    tmp[i] = A[i] + b; // add b to each element

  return tmp; // return back the sum vector
}

template <class T>
Vector<T> operator+(const T &b, // added P.E. 11/13/98
		    const Vector<T> &A)//each item of A plus b. returns back b+A.
{
  unsigned long int N = A.dim(); // A's length

  Vector<T> tmp(N); // create a vector with length N
  unsigned long int i; // declare the index used below

  for (i=0; i<N; i++) // loop for each element
    tmp[i] = A[i] + b; // add b to each element

  return tmp; // return back the sum vector
}

template <class T>
Vector<T> operator-(const Vector<T> &A, 
		    const Vector<T> &B) // elementwise minus A-B. A and B has the same length. returns back A-B.
{
  unsigned long int N = A.dim(); // A's length

  assert(N==B.dim()); // make sure A and B are the same long

  Vector<T> tmp(N); // create a vector with length N
  unsigned long int i; // declare the index used below

  for (i=0; i<N; i++) // loop for each element
    tmp[i] = A[i] - B[i]; //elementwise minus

  return tmp; // return back the result
}

template <class T>
Vector<T> operator-(const Vector<T> &A, // added P.E. 11/13/98
		    const T &b)//each item of A minus b. returns back A-b.
{
  unsigned long int N = A.dim(); // A's length

  Vector<T> tmp(N); // create a vector with length N
  unsigned long int i; // declare the index used below

  for (i=0; i<N; i++) // loop for each element
    tmp[i] = A[i] - b; // distract b from each element

  return tmp; // return back the sum vector
}

template <class T>
Vector<T> operator-(const T &b, // added P.E. 11/13/98
		    const Vector<T> &A)//each item of A minus b; returns back A-b. this operator definition is not good
{
  unsigned long int N = A.dim(); // A's length

  Vector<T> tmp(N); // create a vector with length N
  unsigned long int i; // declare the index used below

  for (i=0; i<N; i++) // loop for each element
    tmp[i] = A[i] - b; // distract b from each element

  return tmp; // return back the sum vector
}

template <class T>
Vector<T> operator*(const Vector<T> &A, 
		    const Vector<T> &B)//elementwise times A.*B. A and B has the same length.
{
  unsigned long int N = A.dim(); // A's length

  assert(N==B.dim()); // make sure A and B are the same long

  Vector<T> tmp(N); // create a vector with length N
  unsigned long int i; // declare the index used below

  for (i=0; i<N; i++) // loop for each element
    tmp[i] = A[i] * B[i]; //elementwise times

  return tmp; // return back the result
}

template <class T>
Vector<T> operator*(const T &a, // added P.E. 11/06/98
		    const Vector<T> &B)//each item of B times a. returns back a*B.
{
  unsigned long int N = B.dim(); // B's length

  Vector<T> tmp(N); // create a vector with length N
  unsigned long int i; // declare the index used below

  for (i=0; i<N; i++) // loop for each element
    tmp[i] = a * B[i]; // each element times a

  return tmp; // return back the result
}

template <class T>
Vector<T> operator*(const Vector<T> &B, // added P.E. 11/06/98
		    const T &a)//each item of B times a. returns back B*a. 
{
  unsigned long int N = B.dim(); // B's length

  Vector<T> tmp(N); // create a vector with length N
  unsigned long int i; // declare the index used below

  for (i=0; i<N; i++) // loop for each element
    tmp[i] = a * B[i]; // each element times a

  return tmp; // return back the result
}

template <class T>
bool operator==(const Vector<T> &A,
		const Vector<T> &B)// added by Ding Wang 8/18/04. Compare two vectors. if A and B are identical, returns back 1; or not, returns back 0.
{
  unsigned long int M = A.dim(); // A's length
  unsigned long int N = B.dim(); // B's length
  if (M!=N) return 0; //if dimensions are different, then return 0

  for (unsigned long int i=1; i<=N; i++) // loop for each element
    {
      if (A(i)!=B(i)) return 0; //if any item is different, then return 0
    }

  return 1; //if all are the same, then return 1
}

template <class T>
T dot_prod(const Vector<T> &A, 
	   const Vector<T> &B)//dot product. A and B must have the same length. returns back A dot B.
{
  unsigned long int N = A.dim(); // A's length
  assert(N == B.dim()); // make sure A and B are the same long

  unsigned long int i; // declare the index used below
  T sum = 0; // create the variable for sum and initialize it as 0

  for (i=0; i<N; i++) // loop for each element
    sum += A[i] * B[i]; // elementwise produce and accumulate them

  return sum; // return back the result
}


// PE 10/06/98
template <class T>
complex<T> dot_prod(const Vector< complex<T> > &A, 
		    const Vector< complex<T> > &B)//dot product.  A and B must have the same length. returns back conj(A) dot B.
{
  unsigned long int N = A.dim(); // A's length
  assert(N == B.dim()); // make sure A and B are the same long

  unsigned long int i; // declare the index used below
  complex<T> sum = 0; // create the variable for sum and initialize it as 0

  for (i=0; i<N; i++) // loop for each element
    sum += conj(A[i]) * B[i]; // elementwise conjugately multiplication

  return sum; // return back the result
}

// PE 10/06/98
template <class T>
T norm(const Vector<T> &v) { return sqrt(dot_prod(v, v)); }//calculate norm of vector and return it back

// PE 10/06/98
template <class T>
T norm(const Vector< complex<T> > &v) { return sqrt(abs(dot_prod(v, v))); }//calculate norm of vector and return it back

// PE 10/06/98
template <class T>
Vector< complex<T> > conj(const Vector< complex<T> > &v)//calculate conjugate of vector
{
  int n = v.dim(); // extract the length of v
  Vector< complex<T> > w(n);  // create a vector with the same length as v
  for (int i=1; i<=n; i++) w(i) = conj(v(i)); // loop for each element and calculate conjugate for each element
  return w; // return back the conjugate vector
}

///////////////////////////////////////Append Functions//////////////////////////////////////////////////////
template <class T>
Vector<T> Append(const Vector<T> &A, 
		 const T &b)//added by Ding Wang; attach a scalar b to vector A; return back the new vector.
{

  Vector<T> append(A.dim()+1); // create a vector which is 1 element longer that A
  for (unsigned long int i=1; i<=A.dim(); i++) // loop for each element
    {
      append(i)=A(i); // copy A into the new vector
    }

  append(A.dim()+1)=b; // copy b to the last element of the new vector


  return append; // return back the result
}

template <class T>
Vector<T> Append(const Vector<T> &A, 
		 const Vector<T> &B)//added by Ding Wang; attach B to A; return back the new vector.
{

  Vector<T> append(A.dim()+B.dim()); // create a new vecoter whose length is the sum of length of A and B
  for (unsigned long int i=1; i<=A.dim(); i++) // loop for each element in A
    {
      append(i)=A(i); // copy A into the new vector
    }

  for (unsigned long int i=A.dim()+1; i<=A.dim()+B.dim(); i++) // loop for each element in B
    {
      append(i)=B(i-A.dim()); // copy B into the new vector
    }

  return append; // return back the result
}


template <class T>
Vector<complex<T> > Append(const Vector<complex<T> > &A, 
			   const Vector<complex<T> > &B)//added by Ding Wang; attach B to A; return back the new vector.
{

  Vector<complex<T> > append(A.dim()+B.dim()); // create a new vecoter whose length is the sum of length of A and B
  for (int i=1; i<=static_cast<int>(A.dim()); i++) // loop for each element in A
    {
      append(i)=A(i); // copy A into the new vector
    }

  for (int i=A.dim()+1; i<=static_cast<int>(A.dim()+B.dim()); i++) // loop for each element in B
    {
      append(i)=B(i-A.dim()); // copy B into the new vector
    }

  return append; // return back the result
}

template <class T>
Vector<T> Append(const Vector<T> &A, 
		 const Fortran_matrix<T> &C)//added by Ding Wang; attach matrix C to A with column first and row second; return back the new vector.
{
  Vector<T> tmp; // create a vector to contain A and B
  tmp=A; // copy A to the new vector
  tmp.Append(C); // attach C to the new vector
  return tmp; // return back the result
}

template <class T>
Vector<complex<T> > Append(const Vector<complex<T> > &A, 
			   const Fortran_matrix<complex<T> > &C)//added by Ding Wang; attach matrix C to A with column first and row second; return back the new vector.
{
  Vector<complex<T> > tmp; // create a vector to contain A and B
  tmp=A; // copy A to the new vector
  tmp.Append(C); // attach C to the new vector
  return tmp; // return back the result
}

#endif

