//  This file has been adapted by Ding Wang to make both read and write columnwise.

#ifndef MATFILE_H
#define MATFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>

// check whether vectors are defined
#ifdef VEC_H
#define MATFILE_USE_VECTORS
#endif

// check whether FORTRAN-style matrices are defined
#ifdef FMAT_H
#define MATFILE_USE_FMAT
#endif

class MatFile {

public:

  enum FileMode {
    ReadOnly,
    Replace,
    Append
  };

  // attributes
  int mopt;
  int imag;
  int nr; 
  int nc;
  int name_len;
  char var_name[80];
  int endian;

  // methods
  MatFile(const char*, MatFile::FileMode);
  ~MatFile(void);
  int read_var(void);
  int read_var(const char*, int&);
  int read_var(const char*, float&);
  int read_var(const char*, double&);
  int read_var(const char*, int*&);
  int read_var(const char*, float*&);
  int read_var(const char*, double*&);
  int write_var(void);
  int write_var(const char*, const int*, int, int);
  int write_var(const char*, const float*, int, int); 
  int write_var(const char*, const double*, int, int); 
  int write_var(const char*, const double**, int, int); 
  int write_var(const char*, const complex<float>*, int, int); 
  int write_var(const char*, const complex<double>*, int, int); 
  int write_var(const char*, const int &);
  int write_var(const char*, const float &);
  int write_var(const char*, const double &);
  int write_var(const char*, const complex<float> &);
  int write_var(const char*, const complex<double> &);
  int write_var(const char*, const char *);
  void print_var(void);

#ifdef MATFILE_USE_VECTORS
  int read_var(const char*, Vector<int> &);
  int read_var(const char*, Vector<float> &);
  int read_var(const char*, Vector<double> &);
  int write_var(const char*, const Vector<int> &);
  int write_var(const char*, const Vector<float> &);
  int write_var(const char*, const Vector<double> &);
  int write_var(const char*, const Vector< complex<float> > &);
  int write_var(const char*, const Vector< complex<double> > &);
#endif

#ifdef MATFILE_USE_FMAT
  int read_var(const char*, Fortran_matrix<int> &);
  int read_var(const char*, Fortran_matrix<float> &);
  int read_var(const char*, Fortran_matrix<double> &);
  int write_var(const char*, const Fortran_matrix<int> &);
  int write_var(const char*, const Fortran_matrix<float> &);
  int write_var(const char*, const Fortran_matrix<double> &);
  int write_var(const char*, const Fortran_matrix< complex<float> > &);
  int write_var(const char*, const Fortran_matrix< complex<double> > &);
#endif

#ifdef MATFILE_USE_FCOOMAT
  int write_var(const char*, const Fortran_coordinate_matrix<int> &);
  int write_var(const char*, const Fortran_coordinate_matrix<float> &);
  int write_var(const char*, const Fortran_coordinate_matrix<double> &);
  int write_var(const char*, const Fortran_coordinate_matrix< complex<double> > &);
#endif
  //int write_var(const char*, const Vector <Fortran_coordinate_matrix< complex<double> > >&);////////////////////////for output Ambiguity///////////////


private:

  // attributes
  FILE *fp;
  char filename[80];
  char hosttype[80];
  int err;
  char machtype[80];
  double *buffer;
  int buffer_size;
  int must_swap;
  // methods
  void byte_swap(char*, int);

};


#endif


