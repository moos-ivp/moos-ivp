// Template Numerical Toolkit (TNT) for Linear Algebra
//
// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates
//
// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology


// Triangular Solves

#ifndef TRISLV_H
#define TRISLV_H


#include "triang.h"

template <class Matrix, class Vector>
Vector Lower_triangular_solve(const Matrix &A, const Vector &b)
{
    Subscript N = A.num_rows();

    // make sure matrix sizes agree; A must be square

    assert(A.num_cols() == N);
    assert(b.dim() == N);

    Vector x(N);

    Subscript i;
    for (i=1; i<=N; i++)
    {

        Vector::element_type tmp=0;

        for (Subscript j=1; j<i; j++)
                tmp += A(i,j)*x(j);

        x(i) =  (b(i) - tmp)/ A(i,i);
    }

    return x;
}


template <class Matrix, class Vector>
Vector Unit_lower_triangular_solve(const Matrix &A, const Vector &b)
{
    Subscript N = A.num_rows();

    // make sure matrix sizes agree; A must be square

    assert(A.num_cols() == N);
    assert(b.dim() == N);

    Vector x(N);

    Subscript i;
    for (i=1; i<=N; i++)
    {

        Matrix::element_type tmp=0;

        for (Subscript j=1; j<i; j++)
                tmp = tmp + A(i,j)*x(j);

        x(i) =  b(i) - tmp;
    }

    return x;
}


template <class Matrix, class Vector>
Vector linear_solve(const LowerTriangularView<Matrix> &A, const Vector &b)
{
    return Lower_triangular_solve(A, b);
}
    
template <class Matrix, class Vector>
Vector linear_solve(const UnitLowerTriangularView<Matrix> &A, const Vector &b)
{
    return Unit_lower_triangular_solve(A, b);
}
    


//********************** Upper triangular section ****************

template <class Matrix, class Vector>
Vector Upper_triangular_solve(const Matrix &A, const Vector &b)
{
    Subscript N = A.num_rows();

    // make sure matrix sizes agree; A must be square

    assert(A.num_cols() == N);
    assert(b.dim() == N);

    Vector x(N);

    Subscript i;
    for (i=N; i>=1; i--)
    {

        Matrix::element_type tmp=0;

        for (Subscript j=i+1; j<=N; j++)
                tmp = tmp + A(i,j)*x(j);

        x(i) =  (b(i) - tmp)/ A(i,i);
    }

    return x;
}


template <class Matrix, class Vector>
Vector Unit_upper_triangular_solve(const Matrix &A, const Vector &b)
{
    Subscript N = A.num_rows();

    // make sure matrix sizes agree; A must be square

    assert(A.num_cols() == N);
    assert(b.dim() == N);

    Vector x(N);

    Subscript i;
    for (i=N; i>=1; i--)
    {

        Matrix::element_type tmp=0;

        for (Subscript j=i+1; j<i; j++)
                tmp = tmp + A(i,j)*x(j);

        x(i) =  b(i) - tmp;
    }

    return x;
}


template <class Matrix, class Vector>
Vector linear_solve(const UpperTriangularView<Matrix> &A, const Vector &b)
{
    return Upper_triangular_solve(A, b);
}
    
template <class Matrix, class Vector>
Vector linear_solve(const UnitUpperTriangularView<Matrix> &A, const Vector &b)
{
    return Unit_upper_triangular_solve(A, b);
}
    


#endif
// TRISLV_H
