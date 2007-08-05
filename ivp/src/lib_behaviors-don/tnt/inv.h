#ifndef inv_H
#define inv_H

template <class Matrix>
Matrix inv(const Matrix &A) {
  
  int i, j;
  Subscript nr = A.num_rows();
  Subscript nc = A.num_cols();

  // basic checks
  if (nr*nc<1) {
    cerr << "Error! inv(): null matrix" << endl;
    exit(0);
  }
  if (nr!=nc) {
    cerr << "Error! inv(): matrix is not square" << endl;
    exit(0);
  }

  // perform LU decomposition
  Matrix C = A;
  Vector<Subscript> ipiv;
  if ( LU_factor(C, ipiv)!=0 ) {
    cerr << "Error! inv(): LU decomposition failed" << endl;
    exit(0);
  }

  // find inverse by columns
  Matrix D(A);
  Vector<typename Matrix::element_type> b(nc);
  for (j=0; j<nc; j++) {
    for (i=0; i<nc; i++) b[i] = 0.0;
    b[j] = 1.0;
    if ( LU_solve(C, ipiv, b)!=0 ) {
      cerr << "Error! LU backsubstitution failed" << endl;
      exit(0);
    }
    for (i=0; i<nc; i++) D(i+1,j+1) = b[i];
  } // next j
  
  return D;
}

/* C_matrix< complex<double> >
inv(C_matrix< complex<double> > &A) {

  int nr = A.num_rows();
  int nc = A.num_cols();
  if (nr!=nc) {
    cerr << "Error! inv(): matrix is not square" << endl;
    exit(0);
  }

  C_matrix<double> D(nr, nc);
  C_matrix<double> E(nr, nc);
  
  {
    C_matrix<double> B(nr, nc);
    C_matrix<double> C(nr, nc);
    for (int i=0; i<nr; i++) {
      for (int j=0; j<nc; j++) {
	B[i][j] = -A[i][j].real();
	C[i][j] = A[i][j].imag();
      }
    }
    D = inv(C)*B;
    E = inv(B*D - C);
    D = D*E;
  }
  
  C_matrix< complex<double> > A1(nr, nc);
  complex<double> ci(0.0, 1.0);
  for (int i=0; i<nr; i++)
    for (int j=0; j<nc; j++)
      A1[i][j] = D[i][j] + ci*E[i][j];

  return A1;
}

*/

#endif
