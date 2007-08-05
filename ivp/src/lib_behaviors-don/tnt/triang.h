// Template Numerical Toolkit (TNT) for Linear Algebra
//
// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates
//
// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology


// Triangular Matrices (Views and Adpators)

#ifndef TRIANG_H
#define TRIANG_H

// default to use lower-triangular portions of arrays
// for symmetric matrices.


template <class Matrix>
class LowerTriangularView
{
    protected:


        const Matrix  &A_;
        const Matrix::element_type zero_;

    public:


    typedef Matrix::const_reference const_reference;
    typedef const Matrix::element_type element_type;
    typedef const Matrix::element_type value_type;
    typedef element_type T;

    Subscript dim(Subscript d) const {  return A_.dim(d); }
    Subscript lbound() const { return A_.lbound(); }
    Subscript num_rows() const { return A_.num_rows(); }
    Subscript num_cols() const { return A_.num_cols(); }
    
    
    // constructors

    LowerTriangularView(const Matrix &A) : A_(A),  zero_(0) {}


    inline const_reference get(Subscript i, Subscript j) const
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(lbound()<=i);
        assert(i<=A_.num_rows() + lbound() - 1);
        assert(lbound()<=j);
        assert(j<=A_.num_cols() + lbound() - 1);
#endif
        if (i<j) 
            return zero_;
        else
            return A_(i,j);
    }


    inline const_reference operator() (Subscript i, Subscript j) const
    {
#ifdef TNT_BOUNDS_CHECK
        assert(lbound()<=i);
        assert(i<=A_.num_rows() + lbound() - 1);
        assert(lbound()<=j);
        assert(j<=A_.num_cols() + lbound() - 1);
#endif
        if (i<j) 
            return zero_;
        else
            return A_(i,j);
    }

#ifdef TNT_USE_REGIONS 

    typedef const_Region2D<LowerTriangularView<Matrix>,T > 
                    const_Region;

    const_Region operator()(const Index1D &I,
            const Index1D &J) const
    {
        return const_Region(*this, I, J);
    }

    const_Region operator()(Subscript i1, Subscript i2,
            Subscript j1, Subscript j2) const
    {
        return const_Region(*this, i1, i2, j1, j2);
    }



#endif
// TNT_USE_REGIONS

};


/* *********** Lower_triangular_view() algorithms ****************** */

template <class Matrix, class Vector>
Vector matmult(const LowerTriangularView<Matrix> &A, Vector &x)
{
    Subscript M = A.num_rows();
    Subscript N = A.num_cols();

    assert(N == x.dim());

    Subscript i, j;
    Vector::element_type sum=0.0;
    Vector result(M);

    Subscript start = A.lbound();
    Subscript Mend = M + A.lbound() -1 ;

    for (i=start; i<=Mend; i++)
    {
        sum = 0.0;
        for (j=start; j<=i; j++)
            sum = sum + A(i,j)*x(j);
        result(i) = sum;
    }

    return result;
}

template <class Matrix, class Vector>
inline Vector operator*(const LowerTriangularView<Matrix> &A, Vector &x)
{
    return matmult(A,x);
}

template <class Matrix>
class UnitLowerTriangularView
{
    protected:

        const Matrix  &A_;
        const Matrix::element_type zero;
        const Matrix::element_type one;

    public:

    typedef Matrix::const_reference const_reference;
    typedef Matrix::element_type element_type;
    typedef Matrix::element_type value_type;
    typedef element_type T;

    Subscript lbound() const { return 1; }
    Subscript dim(Subscript d) const {  return A_.dim(d); }
    Subscript num_rows() const { return A_.num_rows(); }
    Subscript num_cols() const { return A_.num_cols(); }

    
    // constructors

    UnitLowerTriangularView(const Matrix &A) : A_(A), zero(0), one(1) {}


    inline const_reference get(Subscript i, Subscript j) const
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i<=A_.dim(1));
        assert(1<=j);
        assert(j<=A_.dim(2));
        assert(0<=i && i<A_.dim(0) && 0<=j && j<A_.dim(1));
#endif
        if (i>j)
            return A_(i,j);
        else if (i==j)
            return one;
        else 
            return zero;
    }


    inline const_reference operator() (Subscript i, Subscript j) const
    {
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i<=A_.dim(1));
        assert(1<=j);
        assert(j<=A_.dim(2));
#endif
        if (i>j)
            return A_(i,j);
        else if (i==j)
            return one;
        else 
            return zero;
    }


#ifdef TNT_USE_REGIONS 
  // These are the "index-aware" features

    typedef const_Region2D<UnitLowerTriangularView<Matrix>,T > 
                    const_Region;

    const_Region operator()(const Index1D &I,
            const Index1D &J) const
    {
        return const_Region(*this, I, J);
    }

    const_Region operator()(Subscript i1, Subscript i2,
            Subscript j1, Subscript j2) const
    {
        return const_Region(*this, i1, i2, j1, j2);
    }
#endif
// TNT_USE_REGIONS
};

template <class Matrix>
LowerTriangularView<Matrix> Lower_triangular_view(
    const Matrix &A)
{
    return LowerTriangularView<Matrix>(A);
}


template <class Matrix>
UnitLowerTriangularView<Matrix> Unit_lower_triangular_view(
    const Matrix &A)
{
    return UnitLowerTriangularView<Matrix>(A);
}

template <class Matrix, class Vector>
Vector matmult(const UnitLowerTriangularView<Matrix> &A, Vector &x)
{
    Subscript M = A.num_rows();
    Subscript N = A.num_cols();

    assert(N == x.dim());

    Subscript i, j;
    Vector::element_type sum=0.0;
    Vector result(M);

    Subscript start = A.lbound();
    Subscript Mend = M + A.lbound() -1 ;

    for (i=start; i<=Mend; i++)
    {
        sum = 0.0;
        for (j=start; j<i; j++)
            sum = sum + A(i,j)*x(j);
        result(i) = sum + x(i);
    }

    return result;
}

template <class Matrix, class Vector>
inline Vector operator*(const UnitLowerTriangularView<Matrix> &A, Vector &x)
{
    return matmult(A,x);
}


//********************** Algorithms *************************************



template <class Matrix>
ostream& operator<<(ostream &s, const LowerTriangularView<Matrix>&A)
{
    Subscript M=A.num_rows();
    Subscript N=A.num_cols();

    s << M << " " << N << endl;

    for (Subscript i=1; i<=M; i++)
    {
        for (Subscript j=1; j<=N; j++)
        {
            s << A(i,j) << " ";
        }
        s << endl;
    }


    return s;
}

template <class Matrix>
ostream& operator<<(ostream &s, const UnitLowerTriangularView<Matrix>&A)
{
    Subscript M=A.num_rows();
    Subscript N=A.num_cols();

    s << M << " " << N << endl;

    for (Subscript i=1; i<=M; i++)
    {
        for (Subscript j=1; j<=N; j++)
        {
            s << A(i,j) << " ";
        }
        s << endl;
    }


    return s;
}



// ******************* Upper Triangular Section **************************

template <class Matrix>
class UpperTriangularView
{
    protected:


        const Matrix  &A_;
        const Matrix::element_type zero_;

    public:


    typedef Matrix::const_reference const_reference;
    typedef const Matrix::element_type element_type;
    typedef const Matrix::element_type value_type;
    typedef element_type T;

    Subscript dim(Subscript d) const {  return A_.dim(d); }
    Subscript lbound() const { return A_.lbound(); }
    Subscript num_rows() const { return A_.num_rows(); }
    Subscript num_cols() const { return A_.num_cols(); }
    
    
    // constructors

    UpperTriangularView(const Matrix &A) : A_(A),  zero_(0) {}


    inline const_reference get(Subscript i, Subscript j) const
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(lbound()<=i);
        assert(i<=A_.num_rows() + lbound() - 1);
        assert(lbound()<=j);
        assert(j<=A_.num_cols() + lbound() - 1);
#endif
        if (i>j) 
            return zero_;
        else
            return A_(i,j);
    }


    inline const_reference operator() (Subscript i, Subscript j) const
    {
#ifdef TNT_BOUNDS_CHECK
        assert(lbound()<=i);
        assert(i<=A_.num_rows() + lbound() - 1);
        assert(lbound()<=j);
        assert(j<=A_.num_cols() + lbound() - 1);
#endif
        if (i>j) 
            return zero_;
        else
            return A_(i,j);
    }

#ifdef TNT_USE_REGIONS 

    typedef const_Region2D<UpperTriangularView<Matrix>,T > 
                    const_Region;

    const_Region operator()(const Index1D &I,
            const Index1D &J) const
    {
        return const_Region(*this, I, J);
    }

    const_Region operator()(Subscript i1, Subscript i2,
            Subscript j1, Subscript j2) const
    {
        return const_Region(*this, i1, i2, j1, j2);
    }



#endif
// TNT_USE_REGIONS

};


/* *********** Upper_triangular_view() algorithms ****************** */

template <class Matrix, class Vector>
Vector matmult(const UpperTriangularView<Matrix> &A, Vector &x)
{
    Subscript M = A.num_rows();
    Subscript N = A.num_cols();

    assert(N == x.dim());

    Subscript i, j;
    Vector::element_type sum=0.0;
    Vector result(M);

    Subscript start = A.lbound();
    Subscript Mend = M + A.lbound() -1 ;

    for (i=start; i<=Mend; i++)
    {
        sum = 0.0;
        for (j=i; j<=N; j++)
            sum = sum + A(i,j)*x(j);
        result(i) = sum;
    }

    return result;
}

template <class Matrix, class Vector>
inline Vector operator*(const UpperTriangularView<Matrix> &A, Vector &x)
{
    return matmult(A,x);
}

template <class Matrix>
class UnitUpperTriangularView
{
    protected:

        const Matrix  &A_;
        const Matrix::element_type zero;
        const Matrix::element_type one;

    public:

    typedef Matrix::const_reference const_reference;
    typedef Matrix::element_type element_type;
    typedef Matrix::element_type value_type;
    typedef element_type T;

    Subscript lbound() const { return 1; }
    Subscript dim(Subscript d) const {  return A_.dim(d); }
    Subscript num_rows() const { return A_.num_rows(); }
    Subscript num_cols() const { return A_.num_cols(); }

    
    // constructors

    UnitUpperTriangularView(const Matrix &A) : A_(A), zero(0), one(1) {}


    inline const_reference get(Subscript i, Subscript j) const
    { 
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i<=A_.dim(1));
        assert(1<=j);
        assert(j<=A_.dim(2));
        assert(0<=i && i<A_.dim(0) && 0<=j && j<A_.dim(1));
#endif
        if (i<j)
            return A_(i,j);
        else if (i==j)
            return one;
        else 
            return zero;
    }


    inline const_reference operator() (Subscript i, Subscript j) const
    {
#ifdef TNT_BOUNDS_CHECK
        assert(1<=i);
        assert(i<=A_.dim(1));
        assert(1<=j);
        assert(j<=A_.dim(2));
#endif
        if (i<j)
            return A_(i,j);
        else if (i==j)
            return one;
        else 
            return zero;
    }


#ifdef TNT_USE_REGIONS 
  // These are the "index-aware" features

    typedef const_Region2D<UnitUpperTriangularView<Matrix>,T > 
                    const_Region;

    const_Region operator()(const Index1D &I,
            const Index1D &J) const
    {
        return const_Region(*this, I, J);
    }

    const_Region operator()(Subscript i1, Subscript i2,
            Subscript j1, Subscript j2) const
    {
        return const_Region(*this, i1, i2, j1, j2);
    }
#endif
// TNT_USE_REGIONS
};

template <class Matrix>
UpperTriangularView<Matrix> Upper_triangular_view(
    const Matrix &A)
{
    return UpperTriangularView<Matrix>(A);
}


template <class Matrix>
UnitUpperTriangularView<Matrix> Unit_upper_triangular_view(
    const Matrix &A)
{
    return UnitUpperTriangularView<Matrix>(A);
}

template <class Matrix, class Vector>
Vector matmult(const UnitUpperTriangularView<Matrix> &A, Vector &x)
{
    Subscript M = A.num_rows();
    Subscript N = A.num_cols();

    assert(N == x.dim());

    Subscript i, j;
    Vector::element_type sum=0.0;
    Vector result(M);

    Subscript start = A.lbound();
    Subscript Mend = M + A.lbound() -1 ;

    for (i=start; i<=Mend; i++)
    {
        sum = x(i);
        for (j=i+1; j<=N; j++)
            sum = sum + A(i,j)*x(j);
        result(i) = sum + x(i);
    }

    return result;
}

template <class Matrix, class Vector>
inline Vector operator*(const UnitUpperTriangularView<Matrix> &A, Vector &x)
{
    return matmult(A,x);
}


//********************** Algorithms *************************************



template <class Matrix>
ostream& operator<<(ostream &s, const UpperTriangularView<Matrix>&A)
{
    Subscript M=A.num_rows();
    Subscript N=A.num_cols();

    s << M << " " << N << endl;

    for (Subscript i=1; i<=M; i++)
    {
        for (Subscript j=1; j<=N; j++)
        {
            s << A(i,j) << " ";
        }
        s << endl;
    }


    return s;
}

template <class Matrix>
ostream& operator<<(ostream &s, const UnitUpperTriangularView<Matrix>&A)
{
    Subscript M=A.num_rows();
    Subscript N=A.num_cols();

    s << M << " " << N << endl;

    for (Subscript i=1; i<=M; i++)
    {
        for (Subscript j=1; j<=N; j++)
        {
            s << A(i,j) << " ";
        }
        s << endl;
    }


    return s;
}







#endif 
//TRIANG_H

