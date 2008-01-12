// Template Numerical Toolkit (TNT) for Linear Algebra
//
// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates
//
// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology


// Header file to define C/Fortran conventions (Platform specific)

// Modified P.Elisseeff 10/01/98

#ifndef FORTRAN_H
#define FORTRAN_H

// help map between C/C++ data types and Fortran types

typedef int     Fortran_integer;
typedef float   Fortran_float;
typedef double  Fortran_double;

typedef Fortran_double* fda_;        // (in/out) double precision array
typedef const Fortran_double* cfda_; // (in) double precsion array

typedef Fortran_double* fd_;        // (in/out)  single double precision
typedef const Fortran_double* cfd_; // (in) single double precision

typedef Fortran_float* ffa_;        // (in/out) float precision array
typedef const Fortran_float* cffa_; // (in) float precsion array

typedef Fortran_float* ff_;         // (in/out)  single float precision
typedef const Fortran_float* cff_;  // (in) single float precision

typedef Fortran_integer* fia_;          // (in/out)  single integer array
typedef const Fortran_integer* cfia_;   // (in) single integer array

typedef Fortran_integer* fi_;           // (in/out)  single integer
typedef const Fortran_integer* cfi_;    // (in) single integer

typedef char * fch_;                // (in/out) single character
typedef char * cfch_;               // (in) single character

typedef complex<float>  Fortran_complex8;
typedef complex<double> Fortran_complex16;

typedef Fortran_complex16* fcda_;        // (in/out) complex double precision array
typedef const Fortran_complex16* cfcda_; // (in) complex double precision array

typedef Fortran_complex16* fcd_;         // (in/out) complex double precision
typedef const Fortran_complex16* cfcd_;  // (in) complex double precision

typedef Fortran_complex8* fcfa_;         // (in/out) complex single precision array
typedef const Fortran_complex8* cfcfa_;  // (in) complex single precision array

typedef Fortran_complex8* fcf_;          // (in/out) complex single precision
typedef const Fortran_complex8* cfcf_;   // (in) complex single precision

typedef char ** fstr_;                   // (in/out) character string
typedef char ** cfstr_;                  // (in) character string



#ifndef TNT_SUBSCRIPT_TYPE
#define TNT_SUBSCRIPT_TYPE Fortran_integer
#endif


#endif
// FORTRAN_H




