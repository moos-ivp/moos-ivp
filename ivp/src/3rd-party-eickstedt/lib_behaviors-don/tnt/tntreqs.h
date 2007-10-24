// Template Numerical Toolkit (TNT) for Linear Algebra
//
// BETA VERSION INCOMPLETE AND SUBJECT TO CHANGE
// Please see http://math.nist.gov/tnt for updates
//
// R. Pozo
// Mathematical and Computational Sciences Division
// National Institute of Standards and Technology



// The requirements for the TNT bare-bones vector class:
//
//
//   o) must have 0-based [] indexing for const and
//          non-const objects
//   o) must have size() method to denote the number of
//          elements
//   o) must clean up after itself when destructed
//          (i.e. no memory leaks)
//
//   o) must have begin() and end() methods  (The begin()
//          method is necessary, because relying on 
//          &v_[0] may not work on a empty vector (i.e. v_ is NULL.)
//
//   o) must be templated
//   o) must have X::X(const &x) copy constructor (by *value*)
//   o) must have X::X(int N) constructor 
//          (NOTE: this constructor need *NOT* initalize elements)
//
//   o) must have X::X(int N, T scalar) constructor to initalize
//          elements to value of "scalar".
//
//   o) must have X::X(int N, const T* scalars) constructor to copy from
//              any C linear array
//
//   o) must have assignment A=B, by value
//
//  NOTE: this class is *NOT* meant to be derived from,
//  so its methods (particularly indexing) need not be
//  declared virtual.
//
//
//  Some things it *DOES NOT* need to do are
//
//  o) bounds checking
//  o) array referencing (e.g. reference counting)
//  o) support () indexing
//  o) I/O 
//
