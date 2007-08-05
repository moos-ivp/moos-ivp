// Template Numerical Toolkit (TNT) for Linear Algebra
//
// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates
//
// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology


#ifndef TNT_COMPLEX_H
#define TNT_COMPLEX_H

//---------------------------------------------------------------------
//  Definitions for complex number support.  The ANSI C++ proposal 
//  specifies a templated class for this, e.g. complex<double> or 
//  complex<float>, but not all compilers currently support this.  TNT
//  provides a mechanism to accomodate any complex number class,
//  using "DComplex" and "FComplex" names for double-precision
//  and single-precision instances, respectively.  For example,
//  if your system uses the older "Complex" number class, compiles as
//
//   CC -DTNT_DCOMPLEX_TYPE='Complex' 
//  
//  Now DComplex 

#ifndef TNT_DCOMPLEX_TYPE
#define TNT_DCOMPLEX_TYPE complex<double>
#endif

typedef TNT_DCOMPLEX_TYPE DComplex;

#if 0
#ifndef TNT_FCOMPLEX_TYPE
#define TNT_FCOMPLEX_TYPE complex<float>
#endif

typedef TNT_FCOMPLEX_TYPE FComplex;
#endif


#endif
// TNT_COMPLEX_H




