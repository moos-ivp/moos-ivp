//
// RAM (C++ version)
//
// Author: Pierre Elisseeff
// Copyright 1999 Massachusetts Institute of Technology
// This code is a derivative work of RAM (see below)
// 
// Note: this software is provided under the terms of 
//       the GNU Public License (GPL)
//
// The public interface definition is as follows:
//    
//    Ram( freq, zr, zs, 
//         rmax, dr, ndr, 
//         zmax, dz, ndz, zmplt, 
//         c0, np, ns, rs, 
//         rb, zb, zcw, cw, zcb, cb, 
//         zrhob, rhob, zattn, attn, rp );
//
//             instantiates a RAM object whose input 
//             is given by the argument list. This list
//             mimicks the FORTRAN-style input file ram.in
//             used in the FORTRAN version
//
//           scalars:
//             freq:   frequency (Hz)
//             zr:     not used
//             zs:     source depth (m)
//             rmax:   maximum range of computational grid (m)
//             dr:     range step (m)
//             ndr:    range axis decimation factor for output purposes
//             zmax:   maximum depth of computational grid (m)
//             dz:     depth step (m)
//             ndz:    depth axis decimation factor for output purposes
//             zmplt:  maximum depth of output grid (m)
//             c0:     reference sound speed (m/s)
//             np:     number of Pade coefficients
//             ns:     number of constraints
//             rs:     stability range (m)
//
//           arrays:
//             rb(NBATH):      bathymetry range axis (m)
//             zb(NBATH):      bathymetry depth (m)
//             zcw(N1):        water sound speed field depth axis (m)
//             cw(N1,NPROF):   water sound speed field (m/s)
//             zcb(N2):        bottom sound speed field depth axis (m)
//             cb(N2,NPROF):   bottom sound speed field (m/s)
//             zrhob(N3):      bottom density field depth axis (m)
//             rhob(N3,NPROF): bottom density field (g/cm3)
//             zattn(N4):      bottom attenuation field depth axis (m)
//             attn(N4,NPROF): bottom attenuation field (dB/lambda)
//             rp(NPROF):      range axis common to cw, cb, rhob and 
//                             attn (m)
//
//    Fortran_matrix< complex<T> > run( int option );
//
//             returns the complex acoustic field computed by RAM.
//             'option' controls the run-time output (1:verbose, 
//             0: silent).
//
//    c
//    c   ******************************************************************
//    c   ***** Range-dependent Acoustic Model, Version 1.0, 17-Sep-95 *****
//    c   ******************************************************************
//    c
//    c   This code was developed by Michael D. Collins at the Naval
//    c   Research Laboratory in Washington, DC. It solves range-dependent 
//    c   ocean acoustics problems with the split-step Pade algorithm 
//    c   [M. D. Collins, J. Acoust. Soc. Am. 93, 1736-1742 (1993)]. A 
//    c   user's guide and updates of the code are available via anonymous 
//    c   ftp from ram.nrl.navy.mil. Version 1.0 is designed for running on 
//    c   a single processor. Version 1.0p (which requires double precision 
//    c   for very large range steps) is designed for parallel processing.
//    c
//    c Changes (Brian J. Sperry, MIT):
//    c       (1) converted to double precision
//    c       (2) explicitly declared all variables
//    c       (3) monitor energy conservation
//    c       (4) added n^2-linear interpolation
//
#include <stdio.h>
#include <iostream.h>
#include <complex.h>
#include <math.h>
#include "tnt.h"
#include "vec.h"
#include "fmat.h"
#include "MatFile.hh"

#define NCHAR 256
#ifndef PI
#define PI 3.141592653589793
#endif

template <class T>
T square(T x) { return x*x; }

template <class T>
T max(T a, T b) {
  if (a>b) return a;
  return b;
}

template <class T>
T min(T a, T b) {
  if (a<b) return a;
  return b;
}

template <class T>
class Ram {

protected:
  
  // attributes
  double EPSS;
  complex<T> ci;
  Vector< complex<T> > ksq;
  Vector< complex<T> > ksqb;
  Vector< complex<T> > u;
  Vector< complex<T> > v;
  Vector< complex<T> > pd1;
  Vector< complex<T> > pd2;
  Fortran_matrix< complex<T> > r1;
  Fortran_matrix< complex<T> > r2;
  Fortran_matrix< complex<T> > r3;
  Fortran_matrix< complex<T> > s1;
  Fortran_matrix< complex<T> > s2;
  Fortran_matrix< complex<T> > s3;
  T k0;
  Vector<T> ksqw;
  Vector<T> rb;
  Vector<T> zb;
  Vector<T> cw;
  Vector<T> cb;
  Vector<T> rhob;
  Vector<T> attn;
  Vector<T> alpw;
  Vector<T> alpb;
  Vector<T> f1;
  Vector<T> f2;
  Vector<T> f3;
  Fortran_matrix<T> tlg;
  Fortran_matrix< complex<T> > fld;
  T dir;
  T dr;
  T dz;
  T eta;
  T eps;
  T omega;
  T rmax;
  T zs; 
  T zr;
  T zmax;
  T zmplt;
  T c0;
  T r;
  T rp;
  T rs;
  T freq;
  int nz;
  int np;
  int ns;
  int mdr;
  int ndr;
  int ndz;
  int iz;
  int jz;
  int nzplt;
  int lz;
  int lr;
  int ib;
  int ir;
  int imr;
  int nrupdt;
  int profl_index;
  int op2;   // tl.line output file (good for testing)
  int op3;   // total power output to MTV file
  int op4;   // select verbose output mode
  int op5;   // select transmission loss grid mtv file
  int nrout;
  int nb;
  int mp;
  int op_override;
  FILE *fp3;
  FILE *fp5;
  FILE *fp10;
  Vector<T> rb_input;
  Vector<T> zb_input;
  Vector<T> zcw_input;
  Fortran_matrix<T> cw_input;
  Vector<T> zcb_input;
  Fortran_matrix<T> cb_input;
  Vector<T> zrhob_input;
  Fortran_matrix<T> rhob_input;
  Vector<T> zattn_input;
  Fortran_matrix<T> attn_input;
  Vector<T> rp_input;

  // methods
  int laguer( const Vector< complex<T> > &,
	      const int &, 
	      complex<T> &, 
	      int & );
  int zroots( const Vector< complex<T> > &,
	      const int &,
	      Vector< complex<T> > & );
  int pivot( const int &, 
	     const int &, 
	     const int &, 
	     Fortran_matrix< complex<T> > &, 
	     Vector< complex<T> > & );
  int gauss( const int &,  
	     const int &,  
	     Fortran_matrix< complex<T> > &, 
	     Vector< complex<T> > &);
  complex<T> g( const complex<T> &, 
		     const T &,
		     const T &,
		     const T & );
  int deriv( const int &, 
	     const int &, 
	     const T &,
	     const T &, 
	     Vector< complex<T> > &, 
	     Vector< complex<T> > &, 
	     Vector< complex<T> > &, 
	     const Fortran_matrix<T> & );
  int epade( const int & );
  int selfs( void );
  int updat( void );
  int solve( const int & );
  int matrc( const int &, const int & );
  int zread( const Vector<T> &, 
	     const Fortran_matrix<T> &, 
	     Vector<T> & );
  int zread2( const Vector<T> &, 
	      const Fortran_matrix<T> &, 
	      Vector<T> & );
  int profl( void );
  int setup( void );
  int outpt( void );
  int init( void );
  int main_block( void );

public:

  // attributes

  // methods
  Ram( void );
  Ram( const Ram &);
  Ram( const T &, const T &, const T &, 
       const T &, const T &, const int &, 
       const T &, const T &, const int &, const T &, 
       const T &, const int &, const int &, const T &, 
       const Vector<T> &, const Vector<T> &zb_, 
       const Vector<T> &, const Fortran_matrix<T> &, 
       const Vector<T> &, const Fortran_matrix<T> &, 
       const Vector<T> &, const Fortran_matrix<T> &, 
       const Vector<T> &, const Fortran_matrix<T> &, 
       const Vector<T> &);
  ~Ram(void) {};
  Fortran_matrix< complex<T> > run( int );

};

template <class T>
Ram<T>::Ram( void ) {

  cerr << "Illegal instantiation method. Please specify input parameters:" << endl
       << "    Ram<T> ram(.....);" << endl;
  exit(0);
}

template <class T>
Ram<T>::Ram( const Ram<T> &ram ) {

  op_override = 0;

  // system parameters
  freq = ram.freq;
  zr   = ram.zr;
  zs   = ram.zs;
  rmax = ram.rmax;
  dr   = ram.dr;
  ndr  = ram.ndr;
  zmax = ram.zmax;
  dz   = ram.dz;
  ndz  = ram.ndz;
  zmplt= ram.zmplt;
  c0   = ram.c0;
  np   = ram.np;
  ns   = ram.ns;
  rs   = ram.rs;
  EPSS = ram.EPSS;
  
  // environmental parameters
  rb_input    = ram.rb_input;     // bathymetry range
  zb_input    = ram.zb_input;     // bathymetry depth
  zcw_input   = ram.zcw_input;    // water sound speed depth axis
  cw_input    = ram.cw_input;     // water sound speed map
  zcb_input   = ram.zcb_input;    // bottom sound speed depth axis
  cb_input    = ram.cb_input;     // bottom sound speed map
  zrhob_input = ram.zrhob_input;  // bottom density depth axis
  rhob_input  = ram.rhob_input;   // bottom density map
  zattn_input = ram.zattn_input;  // bottom attenuation depth axis
  attn_input  = ram.attn_input;   // bottom attenuation map
  rp_input    = ram.rp_input;     // profile range axis
}

template <class T> 
Ram<T>::Ram( const T &freq_, const T &zr_, const T &zs_,
	     const T &rmax_, const T &dr_, const int &ndr_, 
	     const T &zmax_, const T &dz_, const int &ndz_, const T &zmplt_, 
	     const T &c0_, const int &np_, const int &ns_, const T &rs_, 
	     const Vector<T> &rb_input_, const Vector<T> &zb_input_, 
	     const Vector<T> &zcw_input_, const Fortran_matrix<T> &cw_input_, 
	     const Vector<T> &zcb_input_, const Fortran_matrix<T> &cb_input_, 
	     const Vector<T> &zrhob_input_, const Fortran_matrix<T> &rhob_input_, 
	     const Vector<T> &zattn_input_, const Fortran_matrix<T> &attn_input_, 
	     const Vector<T> &rp_input_) {

  op_override = 0;

  // system parameters
  freq = freq_;
  zr   = zr_;
  zs   = zs_;
  rmax = rmax_;
  dr   = dr_;
  ndr  = ndr_;
  zmax = zmax_;
  dz   = dz_;
  ndz  = ndz_;
  zmplt= zmplt_;
  c0   = c0_;
  np   = np_;
  ns   = ns_;
  rs   = rs_;
  
  // environmental parameters
  rb_input    = rb_input_;     // bathymetry range
  zb_input    = zb_input_;     // bathymetry depth
  zcw_input   = zcw_input_;    // water sound speed depth axis
  cw_input    = cw_input_;     // water sound speed map
  zcb_input   = zcb_input_;    // bottom sound speed depth axis
  cb_input    = cb_input_;     // bottom sound speed map
  zrhob_input = zrhob_input_;  // bottom density depth axis
  rhob_input  = rhob_input_;   // bottom density map
  zattn_input = zattn_input_;  // bottom attenuation depth axis
  attn_input  = attn_input_;   // bottom attenuation map
  rp_input    = rp_input_;     // profile range axis

  // define accuracy of laguer() depending on data type
  switch (sizeof(freq)) {
  case 8: // double precision
    EPSS = 2.0e-15;
    break;
  case 4: // single precision
    EPSS = 2.0e-7;
    break;
  default:
    cerr << "Warning! Ram::Ram(): unknown data type" << endl;
    EPSS = 2.0e-15;
  }


}
	     
template <class T>
Fortran_matrix< complex<T> >
Ram<T>::run( int option ) {

  op2 = 0;
  op3 = 0;
  op4 = option;
  op5 = 0;
  op_override = 1;
  main_block();

  return fld;
}

template <class T>
int
Ram<T>::main_block( void ) {

  //
  if (op_override!=1) {
    op2 = 1;   // tl.line output file (good for testing)
    op3 = 1;   // total power output to MTV file
    op4 = 1;   // select verbose output mode
    op5 = 1;   // select transmission loss grid mtv file
  }

  //
  if (op2)
    if ((fp3=fopen("tl.mtv", "w"))==NULL) {
      cerr << "Cannot open tl.mtv" << endl;
      exit(1);
    }
  if (op3)
    if ((fp5=fopen("pwr.mtv", "w"))==NULL) {
      cerr << "Cannot open pwr.mtv" << endl;
      exit(1);
    }
  if (op5)
    if ((fp10=fopen("ram.mtv", "w"))==NULL) {
      cerr << "Cannot open ram.mtv" << endl;
      exit(1);
    }

  //
  setup();

  //ccc                                              ccc
  //ccc     March the acoustic field out in range.   ccc
  //ccc                                              ccc
  while (r<rmax) {
    updat();
    solve(np);
    r += dr;
    outpt();
  }

  //
  if (op2) fclose(fp3);
  if (op3) fclose(fp5);
  if (op5) {
    for (int i=1; i<=lz; i++)
      for (int j=1; j<=lr; j++)
	fprintf(fp10, "%f7.2\n", tlg(i,j));
    for (int j=2; j<=nb; j++) {
      fprintf(fp10, "@ line x1=%f9.1 y1=%f9.1 z1=0.0 ", rb(j-1), zb(j-1));
      fprintf(fp10, "x2=%f9.1 y2=%f9.1 z2=0.0  linewidth=3\n", rb(j), zb(j));
    }
    fclose(fp10);
  }

  return 0;
}

//-----------------------------------------------------------------------
// initialize vectors and matrices
//
template <class T>
int
Ram<T>::init( void ) {

  ksq.newsize(nz+2);
  ksqw.newsize(nz+2);
  ksqb.newsize(nz+2);
  pd1.newsize(np);
  pd2.newsize(np);
  r1.newsize(nz+2, np);
  r2.newsize(nz+2, np);
  r3.newsize(nz+2, np);
  u.newsize(nz+2);
  v.newsize(nz+2);
  s1.newsize(nz+2, np);
  s2.newsize(nz+2, np);
  s3.newsize(nz+2, np);
  cw.newsize(nz+2);
  cb.newsize(nz+2);
  rhob.newsize(nz+2);
  attn.newsize(nz+2);
  alpw.newsize(nz+2);
  alpb.newsize(nz+2);
  f1.newsize(nz+2);
  f2.newsize(nz+2);
  f3.newsize(nz+2);
  if (op5) tlg.newsize(lz+1, lr+1);
  fld.newsize(lz+1, lr+1);

  return 0;
}


//-----------------------------------------------------------------------
//     Output transmission loss and mode coefficients from decomposition.
//
template <class T>
int
Ram<T>::outpt( void ) {

  // complex<T> ci(0.0,1.0);
  complex<T> ur;
  T tl;

  //
  //     Write out transmission loss for rcvr depth @ each range step
  //
  if (op2) {
    ur = (1.0-dir)*f3(ir)*u(ir)+dir*f3(ir+1)*u(ir+1);
    tl = -20.0*log10(abs(ur)+eps)+10.0*log10(r+eps);
    fprintf(fp3, "%f9.1 %f7.2\n", r, tl);   
  }

  //
  mdr++;
  if (mdr==ndr) {

    mdr = 0;

    //
    //        Compute total energy in PE field - in entire column
    //
    if (op3) {
      T unrg = 0.0; 
      for (int i=1; i<=nz; i++) 
	unrg += square(abs(u(i)));
      unrg *= dz;
      fprintf(fp5, "%f9.1 %e11.4\n", r, unrg);
    }

    //
    //     Output complex PE field
    //
    complex<T> scale = exp(ci * 2.0*PI * freq/c0 * r)/sqrt(r);
    int ii = 1;
    for (int i=max(2,ndz); i<=nzplt; i+=ndz) {
      fld(ii,nrout) = u(i)*f3(i)*scale;
      ii++;
    }

    //
    //     Compute transmission loss grid.
    //
    if (op5) {
      int j = 0;
      // int iflag = 1;
      int iflag = 0;
      for (int i=max(2,ndz); i<=nzplt; i+=ndz) {
	ur = u(i)*f3(i);
	j++;
	tlg(j,nrout) = -20.0*log10(abs(ur)+eps)+10.0*log10(r+eps);
	//  Mark the ocean bottom.
	if ((i>iz)&&(iflag==1)) {
	  tlg(j,nrout) = 0.0;
	  iflag = 0;
	}
      }
    }
    nrout++; 
  }

  return 0;
}


//-----------------------------------------------------------------------
//     Initialize the parameters, acoustic field, and matrices.
//
template <class T>
int
Ram<T>::setup( void ) {

  //
  if( op4==1) 
    cout << "********** RAM **********" << endl
	 << "    freq,zs,zr: " << freq << " " << zs << " " << zr << endl
	 << "   rmax,dr,ndr: " << rmax << " " << dr << " " << ndr << endl
	 << "   zmax,dz,ndz: " << zmax << " " << dz << " " << ndz << endl
	 << "         zmplt: " << zmplt << endl
	 << "   c0,np,ns,rs: " << c0 << " " << np << " " << ns << " " << rs << endl
	 << "   # Pade coefs  = " << np << endl
	 << "   # constraints = " << ns << endl
	 << "   release range = " << rs << endl;
  
  //
  //     Read in bathymetry data
  //
  rb.newsize(rb_input.dim()+1);
  zb.newsize(zb_input.dim()+1);
  int i = 1;
  while (1) {
    rb(i) = rb_input(i);
    zb(i) = zb_input(i);
    if (rb(i) < 0.0) break;
    if (i==rb_input.dim()) {
      i++;
      break;
    }
    i++;
  }
  rb(i) = 2.0*rmax;
  zb(i) = zb(i-1);
  nb = i;
  //
  complex<T> ci_(0.0,1.0);
  ci = ci_;
  eta = 1.0/(40.0*PI*log10(exp(1.0)));
  eps = 1.0e-20;
  ib = 1;
  mdr = 0;
  r = dr;
  omega = 2.0*PI*freq;
  T ri = 1.0+zr/dz;
  ir = (int)(ri);
  dir = ri-(T)ir;
  k0 = omega/c0;
  nz = (int)(zmax/dz-0.5);
  nzplt = (int)(zmplt/dz-0.5);
  T z = zb(1);
  iz = (int)(1.0 + z/dz);
  iz = max(2,iz);
  iz = min(nz,iz);
  if (rs<dr) rs=2.0*rmax;
  //
  if (op4==1) 
    cout << "      nz,nzplt:" << nz << " " << nzplt << endl;

  //
  //     Find size of output grid
  //
  lz = 0;
  for (int i=max(2,ndz); i<=nzplt; i+=ndz)
    lz++;
  lr = 0;
  imr = (int)(1.0 + rmax/dr);
  for (int i=1+ndr; i<=imr; i+=ndr)
    lr++;
  if (op4==1) 
    cout << " O/P file size: lr=" << lr << " lz=" << lz << endl
	 << " " << endl;
  
  // allocate vectors and matrices
  init();

  //
  for (int j=1; j<=np; j++) {
    r3(1,j) = 0.0;
    r1(nz+2,j) = 0.0;
  }
  for (int i=1; i<=nz+2; i++) {
    u(i)=0.0;
    v(i)=0.0;
  }

  //
  //     The initial profiles and starting field.
  //
  nrupdt = 0;
  profl_index = 1;
  nrout = 1;
  profl();
  selfs(); 

  //
  //     Set up MTV file formats
  //
  if (op2==1) {
    fprintf(fp3,"$DATA=COLUMN\n");
    fprintf(fp3,"%%fitpage=true\n");
    fprintf(fp3,"%%xlabel = \"range (m)\"\n");
    fprintf(fp3,"%%toplabel = \"Transmission Loss\" \n");
    fprintf(fp3,"%%subtitle = \"Frequency = %f \" \n", freq);
  }
  if (op3==1) {
    fprintf(fp5,"$DATA=COLUMN\n");
    fprintf(fp5,"%%fitpage=true\n");
    fprintf(fp5,"%%xlabel = \"range (m)\"\n");
    fprintf(fp5,"%%toplabel = \"Total Energy in PE field\" \n");
    fprintf(fp5,"%%subtitle = \"Frequency = %f \" \n", freq);
  }
  if (op5==1) {
    fprintf(fp10,"$DATA=CONTOUR\n");
    fprintf(fp10,"%%contfill\n");
    fprintf(fp10,"%%yflip = true\n");
    fprintf(fp10,"%%fitpage = true\n");
    fprintf(fp10,"%%xlabel = \"range (km)\"\n");
    fprintf(fp10,"%%ylabel = \"depth (m)\"\n");
    fprintf(fp10,"%%toplabel = \"RAM TL\" \n");
    fprintf(fp10,"%%subtitle = \"freq=%f \" \n", freq);
    fprintf(fp10,"%%xmin=0.0  xmax=%f nx=%d \n", rmax, lr);
    fprintf(fp10,"%%ymin=0.0  ymax=%f ny=%d \n", zmplt, lz);
    fprintf(fp10,"%%cmin=60  cmax=100\n");
    fprintf(fp10,"%%nsteps=40\n");
  }

  outpt();

  //
  //     The propagation matrices.
  //
  epade(1);
  matrc(np,1);

  return 0;
}

//----------------------------------------------------------------------
//     Set up the profiles.
//
template <class T>
int
Ram<T>::profl( void ) {

  zread2(zcw_input, cw_input, cw);
  zread2(zcb_input, cb_input, cb);
  zread(zrhob_input, rhob_input, rhob);
  zread(zattn_input, attn_input, attn);
  rp = 2.0 * rmax;
  if (profl_index<rp_input.dim())
    rp = rp_input(++profl_index);

  //
  T k02 = square(k0);
  for (int i=1; i<=nz+2; i++) {
    ksqw(i) = square(omega/cw(i))-k02;
    ksqb(i) = square((omega/cb(i))*(1.0+ci*eta*attn(i)))-k02;
    alpw(i) = sqrt(cw(i)/c0);
    alpb(i) = sqrt(rhob(i)*cb(i)/c0);
  }

  return 0;
}

//-----------------------------------------------------------------------
//     Profile reader and interpolator.
//     
template <class T>
int 
Ram<T>::zread( const Vector<T> &z_input, 
	       const Fortran_matrix<T> &prof_input,
	       Vector<T> &prof ) {

  for (int i=1; i<=nz+2; i++)
         prof(i)=-1.0;
  
  //
  //     load actual data into proper positions in array 
  //
  T zi, profi;
  int depth_index = 1;
  zi = z_input(depth_index);
  profi = prof_input(depth_index, profl_index);
  depth_index++;

  prof(1) = profi;
  int i = (int)(1.5+zi/dz);
  prof(i) = profi;
  int iold = i;
  while (depth_index<=z_input.dim()) {
    zi = z_input(depth_index);
    profi = prof_input(depth_index, profl_index);
    if (depth_index>=z_input.dim()) break;
    depth_index++;    
    if(zi<0.0) break;
    i = (int)(1.5+zi/dz);
    if (i==iold) i++;
    prof(i) = profi;
    iold = i;
  }
  prof(nz+2) = prof(i);
  
  //
  //      Find entries in prof to be filled and interpolate.
  //
  i = 1;
  int j = 1;
  while (j<nz+2) {
    i++;
    while (prof(i)<0.0) i++;
    if (i-j!=1) 
      for (int k=j+1; k<=i-1; k++) 
	prof(k)=prof(j)+((T)(k-j))*(prof(i)-prof(j))/(i-j);
    j = i;
  }

  return 0;
}


//-----------------------------------------------------------------------
//     Profile reader and n-squared linear interpolator.
//     
template <class T>
int 
Ram<T>::zread2( const Vector<T> &z_input, 
		const Fortran_matrix<T> &prof_input,
	        Vector<T> &prof ) {

  for (int i=1; i<=nz+2; i++)
         prof(i)=-1.0;
  
  //
  //     load actual data into proper positions in array 
  //
  T zi, profi;
  int depth_index = 1;
  zi = z_input(depth_index);
  profi = prof_input(depth_index, profl_index);
  depth_index++;
    
  prof(1) = profi;
  int i = (int)(1.5+zi/dz);
  prof(i) = profi;
  int iold = i;
  while (depth_index<=z_input.dim()) {
    zi = z_input(depth_index);
    profi = prof_input(depth_index, profl_index);
    if (depth_index>=z_input.dim()) break;
    depth_index++;
    if(zi<0.0) break;
    i = (int)(1.5+zi/dz);
    if (i==iold) i++;
    prof(i) = profi;
    iold = i;
  }
  prof(nz+2) = prof(i);
  
  //
  //      Find entries in prof to be filled and interpolate.
  //
  i = 1;
  int j = 1;
  while (j<nz+2) {
    i++;
    while (prof(i)<0.0) i++;
    if (i-j!=1) 
      for (int k=j+1; k<=i-1; k++) {
	T n2i = 1.0 / (prof(i)*prof(i));
	T n2j = 1.0 / (prof(j)*prof(j));
	prof(k) = 1.0 / sqrt(n2j+((T)(k-j))*(n2i-n2j)/(i-j));
      }
    j = i;
  }

  return 0;
}


//-----------------------------------------------------------------------
//     The tridiagonal matrices.
//
template <class T>
int
Ram<T>::matrc( const int &inp, const int &matrc_flag ) {

  T a1 = k0*k0/6.0;
  T a2 = 2.0*k0*k0/3.0;
  T a3 =k0*k0/6.0;
  T cfact = 0.5/dz/dz;
  T dfact = 1.0/12.0;
  int i1, i2;

  //
  //     New matrices when iz==jz !! matrc_flag==1.
  //
  if (iz==jz || matrc_flag==1) {
    i1 = 2;
    i2 = nz+1;
    for (int i=1; i<=iz; i++) {
      f1(i)  = 1.0/alpw(i);
      f2(i)  = 1.0;
      f3(i)  = alpw(i);
      ksq(i) = ksqw(i);
    }
    for (int i=iz+1; i<=nz+2; i++) {
      f1(i)  = rhob(i)/alpb(i);
      f2(i)  = 1.0/rhob(i);
      f3(i)  = alpb(i);
      ksq(i) = ksqb(i);
    }
  }

  //
  //     Updated matrices when iz!=jz.
  //
  if (iz>jz && matrc_flag==0) {
    i1 = jz;
    i2 = iz+1;
    for (int i=jz+1; i<=iz; i++) {
      f1(i)  = 1.0/alpw(i);
      f2(i)  = 1.0;
      f3(i)  = alpw(i);
      ksq(i) = ksqw(i);
    }
  }
  //
  if (iz<jz && matrc_flag==0) {
    i1 = iz;
    i2 = jz+1;
    for (int i=iz+1; i<=jz; i++) {
      f1(i)  = rhob(i)/alpb(i);
      f2(i)  = 1.0/rhob(i);
      f3(i)  = alpb(i);
      ksq(i) = ksqb(i);
    }
  }
  //
  for (int i=i1; i<=i2; i++) {
    //
    //     Discretization by Galerkin's method.
    //
    T c1 = cfact*f1(i)*(f2(i-1)+f2(i))*f3(i-1);
    T c2 = -cfact*f1(i)*(f2(i-1)+2.0*f2(i)+f2(i+1))*f3(i);
    T c3 = cfact*f1(i)*(f2(i)+f2(i+1))*f3(i+1);
    complex<T> d1 = c1+dfact*(ksq(i-1)+ksq(i));
    complex<T> d2 = c2+dfact*(ksq(i-1)+6.0*ksq(i)+ksq(i+1));
    complex<T> d3 = c3+dfact*(ksq(i)+ksq(i+1));
    //
    for (int j=1; j<=inp; j++) {
      r1(i,j) = a1+pd2(j)*d1;
      r2(i,j) = a2+pd2(j)*d2;
      r3(i,j) = a3+pd2(j)*d3;
      s1(i,j) = a1+pd1(j)*d1;
      s2(i,j) = a2+pd1(j)*d2;
      s3(i,j) = a3+pd1(j)*d3;
    }
  }

  //
  //     The matrix decomposition.
  //
  for (int j=1; j<=inp; j++) {
    //
    for (int i=i1; i<=iz; i++) {   
      complex<T> rfact = 1.0/(r2(i,j)-r1(i,j)*r3(i-1,j));
      r1(i,j) = r1(i,j)*rfact;
      r3(i,j) = r3(i,j)*rfact; 
      s1(i,j) = s1(i,j)*rfact; 
      s2(i,j) = s2(i,j)*rfact;
      s3(i,j) = s3(i,j)*rfact;
    }
    //
    for (int i=i2; i>=iz+2; i--) {
      complex<T> rfact = 1.0/(r2(i,j)-r3(i,j)*r1(i+1,j));
      r1(i,j) = r1(i,j)*rfact;
      r3(i,j) = r3(i,j)*rfact;
      s1(i,j) = s1(i,j)*rfact;
      s2(i,j) = s2(i,j)*rfact;
      s3(i,j) = s3(i,j)*rfact;
    }
    //
    r2(iz+1,j) = r2(iz+1,j)-r1(iz+1,j)*r3(iz,j);
    r2(iz+1,j) = r2(iz+1,j)-r3(iz+1,j)*r1(iz+2,j);
    r2(iz+1,j) = 1.0/r2(iz+1,j);
  }
  
  return 0;
}

//-----------------------------------------------------------------------
//     The tridiagonal solver.
//
template <class T>
int
Ram<T>::solve( const int &inp ) {

  for (int j=1; j<=inp; j++) {
    //
    //     The right side.
    //
    for (int i=2; i<=nz+1; i++) 
      v(i) = s1(i,j) * u(i-1) + s2(i,j) * u(i) + s3(i,j) * u(i+1);

    //
    //     The elimination steps.
    //
    for (int i=3; i<=iz; i++)
      v(i) = v(i) - r1(i,j) * v(i-1);
    for (int i=nz; i>=iz+2; i--)
      v(i) = v(i) - r3(i,j) * v(i+1);

    u(iz+1) = ( v(iz+1) - r1(iz+1,j) * v(iz) - r3(iz+1,j) * v(iz+2) ) * r2(iz+1,j);

    //
    //     The back substitution steps.
    //
    for (int i=iz; i>=2; i--) 
      u(i) = v(i) - r3(i,j) * u(i+1);
    for (int i=iz+2; i<=nz+1; i++)
      u(i) = v(i) - r1(i,j) * u(i-1);
  }

  return 0;
}

//-----------------------------------------------------------------------
//     Matrix updates.
//
template <class T>
int
Ram<T>::updat( void ) {

  //
  //     Varying bathymetry.
  //
  if(r >= rb(ib+1)) ib++;
  jz = iz;
  T z = zb(ib) + (r+0.5*dr-rb(ib)) * (zb(ib+1)-zb(ib)) / (rb(ib+1)-rb(ib));
  iz = (int)(1.0 + z/dz);
  iz = max(2,iz);
  iz = min(nz,iz);
  if( iz!=jz) matrc(np,0);

  //
  //     Varying profiles.
  //
  if(r >= rp) {
    nrupdt++;
    profl();
    matrc(np,1);
  }

  //
  //     Turn off the stability constraints.
  //
  if(r >= rs) {
    ns = 0;
    rs = 2.0*rmax;
    epade(1);
    matrc(np,1);
  }

  return 0;  
}

//-----------------------------------------------------------------------
//     The self-starter.
//
template <class T>
int 
Ram<T>::selfs( void ) {

  //
  //     Conditions for the delta function.
  //
  T si  = 1.0 + zs/dz;
  int is     = (int)(si);
  T dis = si-(float)(is);
  u(is)   = (1.0-dis) * sqrt(2.0*PI/k0) / (dz*alpw(is));
  u(is+1) = dis * sqrt(2.0*PI/k0) / (dz*alpw(is));

  //
  //     Divide the delta function by (1+X)**2 to get a smooth rhs.
  //
  pd1(1)=0.0;
  pd2(1)=1.0;
  matrc(1,1);
  solve(1);
  solve(1);
  
  //
  //     Operate on the rhs with (1+X)**(7/4)*exp(ci*k0*r*dsqrt(1+X)).
  //
  epade(2);
  matrc(np,1);
  solve(np);

  return 0;
}


//-----------------------------------------------------------------------
//     The coefficients of the rational approximation.
//
template <class T>
int
Ram<T>::epade( const int &ip ) {
  
  //
  const T sig = k0*dr;
  const int n = 2*np;
  const int m = 40;

  // 
  T alp;
  if (ip==1) alp = 0.0;
  else alp = 1.75;
  
  //
  //     The factorials.
  //
  Vector<T> fact(n);
  fact(1) = 1.0;
  for (int i=2; i<=n; i++)
    fact(i) = fact(i-1)*i;

  //
  //     The binomial coefficients.
  //
  Fortran_matrix<T> bin(n+1,n+1);
  for (int i=1; i<=n+1; i++) {
    bin(i,1) = 1.0;
    bin(i,i) = 1.0;
  }
  for (int i=3; i<=n+1; i++) 
    for (int j=2; j<=i-1; j++)
      bin(i,j) = bin(i-1,j-1) + bin(i-1,j);
  //
  Fortran_matrix< complex<T> > a(n,n);
  for (int i=1; i<=n; i++)
    for (int j=1; j<=n; j++)
      a(i,j) = 0.0;

  //
  //     The accuracy constraints.
  //
  Vector< complex<T> > dh1(n);
  Vector< complex<T> > dh2(n);
  Vector< complex<T> > dg(n+1);  
  deriv(m, n, sig, alp, dg, dh1, dh2, bin);
  //
  Vector< complex<T> > b(n);
  for (int i=1; i<=n; i++) b(i) = dg(i+1);
  for (int i=1; i<=n; i++) {
    if (2*i-1 <= n)
      a(i, 2*i-1) = fact(i);
    for (int j=1; j<=i; j++)
      if (2*j<=n)
        a(i,2*j) = -bin(i+1,j+1)*fact(j)*dg(i-j+1);
  }

  //
  //     The stability constraints.
  //
  if (ns >= 1) {
    T z1 = -3.0;
    b(n) = -1.0;
    for (int j=1; j<=np; j++) {
      a(n, 2*j-1) = pow(z1, j);
      a(n, 2*j)   = 0.0;
    }
  }
  //
  if (ns>=2) {
    T z1 = -1.5;
    b(n-1) = -1.0;
    for (int j=1; j<=np; j++) {
      a(n-1, 2*j-1) = pow(z1, j);
      a(n-1, 2*j)   = 0.0;
    }
  }
  //
  gauss(n, n, a, b);
  //
  dh1(1) = 1.0;
  for (int j=1; j<=np; j++) 
    dh1(j+1) = b(2*j-1);
  zroots(dh1, np, dh2);
  for (int j=1; j<=np; j++)
    pd1(j) = -1.0/dh2(j);
  //
  dh1(1) = 1.0;
  for (int j=1; j<=np; j++)
    dh1(j+1) = b(2*j);
  zroots(dh1, np, dh2);
  for (int j=1; j<=np; j++)
    pd2(j) = -1.0/dh2(j);

  //
  return 0;
}

//-----------------------------------------------------------------------
//
//     The operator function.
//
template <class T>
complex<T>
Ram<T>::g( const complex<T> &ci, 
	   const T &sig,
	   const T &x,
	   const T &alp ) {

  complex<T> g = exp(alp*log(1.0+x)+ci*sig*(-1.0+sqrt(1.0+x)));
  
  return g;
}

//-----------------------------------------------------------------------
//     The derivatives of the operator function at x=0.
//
template <class T>
int
Ram<T>::deriv( const int &m, 
	       const int &n, 
	       const T &sig,
	       const T &alp, 
	       Vector< complex<T> > &dg, 
	       Vector< complex<T> > &dh1, 
	       Vector< complex<T> > &dh2, 
	       const Fortran_matrix<T> &bin ) {

  //
  complex<T> ci(0.0,1.0);

  //
  dh1(1) = 0.5*ci*sig;
  dh2(1) = alp;
  T exp1 = -0.5;
  T exp2 = -1.0;
  for (int i=2; i<=n; i++) {
    dh1(i) = dh1(i-1)*exp1;
    exp1 -= 1;
    dh2(i) = dh2(i-1)*exp2;
    exp2 -= 1;
  }
  
  //
  dg(1) = 1.0;
  dg(2) = dh1(1) + dh2(1);
  for (int i=2; i<=n; i++) {
    dg(i+1) = dh1(i) + dh2(i);
    for (int j=1; j<=i-1; j++) 
      dg(i+1) += bin(i,j)*(dh1(j)+dh2(j))*dg(i-j+1);
  }

  return 0;
}

//-----------------------------------------------------------------------
//     Gaussian elimination.
//
template <class T>
int
Ram<T>::gauss( const int &m,  
       const int &n,  
       Fortran_matrix< complex<T> > &a, 
       Vector< complex<T> > &b) {
  
  //
  //     Downward elimination.
  //
  for (int i=1; i<=n; i++) {
    if (i<n) pivot(m, n, i, a, b);
    a(i,i) = 1/a(i,i);
    b(i) *= a(i,i);
    if (i<n) {
      for (int j=i+1; j<=n; j++)
        a(i,j) *= a(i,i);
      for (int k=i+1; k<=n; k++) {
        b(k) -= a(k,i)*b(i);
        for (int j=i+1; j<=n; j++) 
          a(k,j) -= a(k,i)*a(i,j);
      }
    }
  }

  //
  //     Back substitution.
  //
  for (int i=n-1; i>=1; i--) 
    for (int j=i+1; j<=n; j++)
      b(i) -= a(i,j)*b(j);
  
  return 0;
}


//-----------------------------------------------------------------------
//    Rows are interchanged for stability.
//
template <class T>
int
Ram<T>::pivot( const int &m, 
	       const int &n, 
	       const int &i, 
	       Fortran_matrix< complex<T> > &a, 
	       Vector< complex<T> > &b ) {

  int i0 = i;
  T amp;
  T amp0 = abs(a(i,i));
  for (int j=i+1; j<=n; j++) {
    amp = abs(a(j,i));
    if (amp > amp0) {
      i0 = j;
      amp0 = amp;
    }
  } // next j
  if (i0==i) return 0;
  
  //
  complex<T> temp = b(i);
  b(i) = b(i0);
  b(i0) = temp;
  for (int j=1; j<=n; j++) {
    temp = a(i,j);
    a(i,j) = a(i0,j);
    a(i0,j) = temp;
  } // next j
  return 0;
}


//-----------------------------------------------------------------------
//    Subroutine zroots modified from Numerical Recipes.
//
#define MAXM 101
#define EPS 1e-14
template <class T>
int 
Ram<T>::zroots( const Vector< complex<T> > &a,
		const int &m,
		Vector< complex<T> > &roots ) {

  complex<T> b, c;
  Vector< complex<T> > ad = a;
  T xre, xim;
  int its, i, j;

  // 
  for (j=m; j>=1; j--) {
    complex<T> x(0.0, 0.0);
    laguer(ad, j, x, its);
    xre = 2.0*fabs(x.real())*EPS*EPS;
    xim = fabs(x.imag());
    if (xim <= xre) x = x.real();
    roots(j) = x;
    b = ad(j+1);
    for (int jj=j; jj>=1; jj--) {
      c = ad(jj);
      ad(jj) = b;
      b = x*b + c;
    } // next jj 
  } // next j

  //
  for (j=1; j<=m; j++)
    laguer(a, m, roots(j), its);

  //
  for (j=2; j<=m; j++) {
    complex<T> x = roots(j);
    for (i=j-1; i>=1; i--) {
      if (roots(i).real() <= x.real())
        break;
      roots(i+1) = roots(i);
    } // next i
    // i = 0;
    roots(i+1) = x;
  } // next j

  return 0;
}
#undef EPS
#undef MAXM

//-----------------------------------------------------------------------
//     Subroutine laguer modified from Numerical Recipes.
//
#define MR 8
#define MT 10
#define MAXIT (MT*MR)
template <class T>
int
Ram<T>::laguer( const Vector< complex<T> > &a,
		const int &m, 
		complex<T> &x, 
		int &its ) {
  
  complex<T> dx, x1, b, g, h, sq, gp, gm, g2;
  T abx, err, abp, abm;
  
  //
  const T frac_data[] = {.5, .25, .75, .13, .38, .62, .88, 1.};
  const Vector<T> frac(8, frac_data);
  
  //
  for (int iter=1; iter<=MAXIT; iter++) {
    its = iter;
    b = a(m+1);
    err = abs(b);
    complex<T> d(0.0, 0.0);
    complex<T> f(0.0, 0.0);
    abx = abs(x);
    for (int j=m; j>=1; j--) {
      f = x*f + d;
      d = x*d + b;
      b = x*b + a(j);
      err = abs(b) + abx*err;
    } // next j
    //
    err *= EPSS;
    if (abs(b) <= err ) {
      return 0;
    } else {
      g = d/b;
      g2 = g*g;
      h = g2-2.0*f/b;
      sq = sqrt(((float)(m-1))*(((float)m)*h-g2));
      gp = g + sq;
      gm = g - sq;
      abp = abs(gp);
      abm = abs(gm);
      if (abp < abm) gp = gm;
      if (abp>0 || abm >0) 
        dx = ((float)m)/gp;
      else
        dx = exp(log(1+abx)+ci*iter);
    }
    x1 = x - dx;
    if (x==x1) return 0;
    if (iter % MT) 
      x = x1;
    else
      x = x-dx*frac(iter/MT);
  } // next iter
  
  //
  cout << "   too many iterations in laguer" << endl;
  return 1;
}
#undef EPSS
#undef MR
#undef MT
#undef MAXIT











