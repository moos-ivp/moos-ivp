#include <iostream>
#include <math.h> 
#include "BHV_ADP.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "MBUtils.h"
#include "Search.h"
#include "MatFile.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_ADP::BHV_ADP(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_ADP");

  m_domain = subDomain(m_domain, "course");

  desired_heading = 0;
  peakwidth       = 0;
  basewidth       = 4;

  // The default duration at the IvPBehavior level is "-1", which
  // indicates no duration applied to the behavior by default. By
  // setting to zero here, we force the user to provide a duration
  // value otherwise it will timeout immediately.
  m_duration      = 0;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_ADP::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if(param == "heading") {
    if(!isNumber(val))
      return(false);
    desired_heading = atof(val.c_str());
    return(true);
  }
  if(param == "peakwidth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    peakwidth = dval;
    return(true);
  }
  if(param == "basewidth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    basewidth = dval;
    return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState
//

IvPFunction *BHV_ADP::onRunState() 
{
  if(!m_domain.hasDomain("course")) {
    postEMessage("No 'heading' variable in the helm domain");
    return(0);
  }

  ZAIC_PEAK zaic(m_domain, "course");
  zaic.setSummit(desired_heading);
  zaic.setBaseWidth(basewidth);
  zaic.setPeakWidth(peakwidth);
  zaic.setValueWrap(true);
  
  IvPFunction *ipf = zaic.extractOF();
  if(ipf)
    ipf->setPWT(m_priority_wt);

  return(ipf);
}

template <class T>
void ADP<T>::Initialize()
{      /////////////////////////Load all variables from Data_for_AUV.mat/////////////////////////////////////////////////
  MatFile Data_for_AUV_read("Data_for_AUV.mat", MatFile::ReadOnly);
  Data_for_AUV_read.read_var("AUV_grid_r", AUV_grid_r);
  Data_for_AUV_read.read_var("AUV_grid_z", AUV_grid_z);
      
  Data_for_AUV_read.read_var("L", L);
  Data_for_AUV_read.read_var("Path_all", Path_all);
  Data_for_AUV_read.read_var("control_number", control_number);
  Data_for_AUV_read.read_var("stage_info", stage_info);
  Data_for_AUV_read.read_var("samples_r_all", samples_r_all);
  Data_for_AUV_read.read_var("samples_z_all", samples_z_all);
  Data_for_AUV_read.read_var("samples_info", samples_info);
  Data_for_AUV_read.read_var("obs_n", obs_n);
  stage=1;
  u=1;
  control_combi_i=1;
  state_part2=1;
  sample_index=0;
}
      
template <class T>
void ADP<T>::Set_measurement(const T sound_speed)
{
  sample_index=sample_index+1;
  Sound_speed.change_size(sample_index);
  Sound_speed(sample_index)=sound_speed;
}             
 
template <class T>
void ADP<T>::Get_waypoint(Vector<T> & waypoints_r, Vector<T> & waypoints_z)
{
  assert(stage<samples_info.num_cols());
  if (stage>=2&&stage<=samples_info.num_cols()-1)
    {
      int segment_i=stage-1;
      control_combi_i+=(u-1)*Prod(control_number, stage, control_number.dim());	  

      int sub_seg_n=floor(sample_index/(obs_n*1.)+0.5);//in each segment, number of point in each subsegment for observation via rounding.
      state_part1.change_size(segment_i*obs_n);//cout<<"h="<<sub_seg_n<<endl;
      int sub_start_i=1;
      for (int obs_i=1; obs_i<=obs_n-1; obs_i++)//sub_segment by sub_segment
	{
	  state_part1((segment_i-1)*obs_n+obs_i)=Sum(Sound_speed, sub_start_i, sub_start_i+sub_seg_n-1)/sub_seg_n;
	  sub_start_i+=sub_seg_n;
	}
      state_part1(segment_i*obs_n)=Sum(Sound_speed, sub_start_i, sample_index)/(sample_index-sub_start_i+1);	
      MatFile Data_for_AUV_read("Data_for_AUV.mat", MatFile::ReadOnly);

      state_part2=int(ceil(double(control_combi_i)/control_number(stage)));
      char buffer2[20];
      sprintf(buffer2, "C%d_%d", state_part2, stage);
      Data_for_AUV_read.read_var(buffer2, C);//cout<<buffer2<<C<<endl;
      sprintf(buffer2, "Q%d_%d", state_part2, stage);
      Data_for_AUV_read.read_var(buffer2, Q);//cout<<"state_part1="<<state_part1<<endl;

      int M=C.num_rows();
      Vector<double> k(M);
      int N=state_part1.dim();
      for (int i=1; i<=M; i++)
	for (int j=1; j<=N; j++)
	  {
	    k(i)=k(i)+pow(((state_part1(j)-C(i,j))/L(i)),2);
	  }//cout<<"k="<<k<<endl;
      for (int i=1; i<=M; i++)
	{k(i)=exp(-1.*k(i)/N);}
      k=k*(1/Sum(k));
      Vector<double> temp(transpose(Q)*k);//cout<<"temp="<<temp<<endl;
      Vector<double> conservative(temp.dim());
      conservative(1)=0.000*temp(1);
      Search_Min(temp-conservative, u);//cout<<"u="<<u<<endl;
    }

  int start_i=stage_info(stage)+1;//the starting point for the whole segment
  int end_i=stage_info(stage+1);//the endding point for the whole segment
  waypoints_r.newsize(end_i-start_i+1); waypoints_z.newsize(end_i-start_i+1);
  for (int j=start_i; j<=end_i; j++)//for all points in the whole segment
    {
      waypoints_r(j-start_i+1)=AUV_grid_r(j);
      waypoints_z(j-start_i+1)=AUV_grid_z(Path_all(control_combi_i,j));
    }
  stage++;
  sample_index=0;
}

MatFile::MatFile(const char *name, FileMode fmode) {
  memset(this, 0, sizeof(MatFile));
  strcpy(filename, name);

  char mode[5];
  switch (fmode) {
  case ReadOnly:
    strcpy(mode, "rb");
    break;
  case Replace:
    strcpy(mode, "wb");
    break;
  case Append:
    strcpy(mode, "ab");
    break;
  default:
    cerr << "Warning! MatFile: unknown file mode. defaulting"
	 << "         to ReadOnly" << endl;
    strcpy(mode, "rb");
    break;
  }

  if ( (fp=fopen(filename, mode))==NULL) {
    cerr << "Error! MatFile: cannot open " << filename << endl;
    err = 1;
    return;
  }
  err = 0;
  strcpy(hosttype, getenv("HOSTTYPE"));
	 
  if ( !strcmp(hosttype, "i386-linux") ||
       !strcmp(hosttype, "alpha")      ) {
    // big endian
    endian = 0;
  } else if ( !strcmp(hosttype, "sun4") ) {
    // little endian
    endian = 1;
  } else {
    cerr << "Warning! MatFile: unknown host type" << endl
	 << "         assuming little endian" << endl;
    endian = 0;
  }
};

MatFile::~MatFile(void) {
  fclose(fp);
  if (buffer_size!=0) delete []buffer;
};

void
MatFile::byte_swap(char *c, int n) {
  for (int i=0; i<(int)floor(n/2.0); i++) {
    char tmp = c[i];
    c[i] = c[n-1-i];
    c[n-1-i] = tmp;
  }
};

int
MatFile::read_var(const char *name, double &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  if (nr*nc!=1)
    cerr << "MatFile: Warning! read an array while expecting a scalar" << endl
	 << "         Using the first element of the array only." << endl;
  x = buffer[0];
  return 0;
};

int
MatFile::read_var(const char *name, float &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  if (nr*nc!=1)
    cerr << "MatFile: Warning! read an array while expecting a scalar" << endl
	 << "         Using the first element of the array only." << endl;
  x = (float)buffer[0];
  return 0;
};

int
MatFile::read_var(const char *name, int &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  if (nr*nc!=1)
    cerr << "MatFile: Warning! read an array while expecting a scalar" << endl
	 << "         Using the first element of the array only." << endl;
  x = (int)buffer[0];
  return 0;
};

int
MatFile::read_var(const char *name, double* &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  x = new double[nr*nc];
  memcpy(x, buffer, nr*nc*sizeof(double));
  return 0;
};

int
MatFile::read_var(const char *name, float* &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  x = new float[nr*nc];
  for (int i=0; i<nr; i++) {
    for (int j=0; j<nc; j++) {
      x[i*nc+j] = (float)buffer[i*nc+j];
    }
  }
  return 0;
};

int
MatFile::read_var(const char *name, int* &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  x = new int[nr*nc];
  for (int i=0; i<nr; i++) {
    for (int j=0; j<nc; j++) {
      x[i*nc+j] = (int)buffer[i*nc+j];
    }
  }
  return 0;
};

int
MatFile::read_var(void) {

  if ( fread(&mopt, sizeof(int), 1, fp)    !=1 ||
       fread(&nr, sizeof(int), 1, fp)      !=1 ||
       fread(&nc, sizeof(int), 1, fp)      !=1 ||
       fread(&imag, sizeof(int), 1, fp)    !=1 ||
       fread(&name_len, sizeof(int), 1, fp)!=1 ) 
    return 1;
  if (mopt<0 || mopt>2000) must_swap = 1;
  if (must_swap) {
    byte_swap((char*)&mopt, sizeof(int));
    byte_swap((char*)&nr, sizeof(int));
    byte_swap((char*)&nc, sizeof(int));
    byte_swap((char*)&imag, sizeof(int));
    byte_swap((char*)&name_len, sizeof(int));
  }
  if (fread(var_name, sizeof(char), name_len, fp)!=(unsigned int)name_len)
    return 1;
  if (buffer_size!=0) delete []buffer;
  if (!imag) buffer_size = nr*nc;
  else buffer_size = 2*nr*nc;
  buffer = new double[buffer_size];
  if (fread(buffer, sizeof(double), buffer_size, fp)
      !=(unsigned int)buffer_size)
    return 1;
  if (must_swap) {
    for (int i=0; i<buffer_size; i++)
      byte_swap((char*)&buffer[i], sizeof(double));
  }
  return 0;
};

int 
MatFile::write_var(void) {
  int err = 0;
  err += (fwrite(&mopt,     sizeof(int),  1,           fp)!=1);
  err += (fwrite(&nr,       sizeof(int),  1,           fp)!=1);
  err += (fwrite(&nc,       sizeof(int),  1,           fp)!=1);
  err += (fwrite(&imag,     sizeof(int),  1,           fp)!=1);
  err += (fwrite(&name_len, sizeof(int),  1,           fp)!=1);
  fwrite(var_name,  sizeof(char), name_len,    fp);
  fwrite(buffer, sizeof(double),  buffer_size, fp);
  return err;
}

int MatFile::write_var(const char *name, const char * x) // 7/24/05 added by Ding Wang. This is to write a char vector.
{
  nr=1;
  nc=strlen(x);
  mopt = 51;            // m-thousand digit; o hundred digit; p ten digit; t digit; look v4 mat file format for detail.
  imag = 0;             // 0: is real; 1: is complex 
  name_len=strlen(name)+1;
  int err = 0;
  err += (fwrite(&mopt,     sizeof(int),  1,           fp)!=1);
  err += (fwrite(&nr,       sizeof(int),  1,           fp)!=1);
  err += (fwrite(&nc,       sizeof(int),  1,           fp)!=1);
  err += (fwrite(&imag,     sizeof(int),  1,           fp)!=1);
  err += (fwrite(&name_len, sizeof(int),  1,           fp)!=1); // the above 5 lines are writing hearder for the matrix. totally 20-byte long.
  fwrite(name, sizeof(char), name_len, fp);
  fwrite(x, sizeof(char), nr*nc, fp);
  return err;
}

int
MatFile::write_var(const char *name, const int* x, int nr_, int nc_) {
  double* y = new double[nr_*nc_];
  for (int i=0; i<nr_*nc_; i++)
    y[i] = (double)(x[i]);
  int err = write_var(name, y, nr_, nc_);
  delete []y;

  return err;
}

int
MatFile::write_var(const char *name, const float* x, int nr_, int nc_) {
  double* y = new double[nr_*nc_];
  for (int i=0; i<nr_*nc_; i++)
    y[i] = (double)(x[i]);
  int err = write_var(name, y, nr_, nc_);
  delete []y;

  return err;
}

int
MatFile::write_var(const char *name, const double* x, int nr_, int nc_) {

  nr = nr_;
  nc = nc_;
  mopt = 0;
  mopt += 0;            // 1 if x was a string 
  mopt += 1000*endian;  // 0: little endian; 1: big endian 
  imag = 0;             // 0: is real; 1: is complex 
  strcpy(var_name, name);
  name_len = strlen(var_name)+1;

  if (buffer_size!=0) {
    delete []buffer;
    buffer_size = 0;
  }
  buffer_size = nr*nc;
  if (imag) buffer_size *= 2;
  buffer = new double[buffer_size];
  memcpy(buffer, x, buffer_size*sizeof(double));

  write_var();
  return 0;
}

int
MatFile::write_var(const char *name, const double** x, int nr_, int nc_) {

  nr = nr_;
  nc = nc_;
  mopt = 0;
  mopt += 0;            // 1 if x was a string 
  mopt += 1000*endian;  // 0: little endian; 1: big endian 
  imag = 0;             // 0: is real; 1: is complex 
  strcpy(var_name, name);
  name_len = strlen(var_name)+1;

  if (buffer_size!=0) {
    delete []buffer;
    buffer_size = 0;
  }
  buffer_size = nr*nc;
  if (imag) buffer_size *= 2;
  buffer = new double[buffer_size];
  for (int i=0; i<nr; i++)
    for (int j=0; j<nc; j++)
      buffer[i*nc+j] = x[i][j];

  write_var();
  return 0;
}

int
MatFile::write_var(const char *name, const complex<float>* x, int nr_, int nc_) {

  nr = nr_;
  nc = nc_;
  mopt = 0;
  mopt += 0;            // 1 if x was a string 
  mopt += 1000*endian;  // 0: little endian; 1: big endian 
  imag = 1;             // 0: is real; 1: is complex 
  strcpy(var_name, name);
  name_len = strlen(var_name)+1;

  if (buffer_size!=0) {
    delete []buffer;
    buffer_size = 0;
  }
  buffer_size = nr*nc;
  if (imag) buffer_size *= 2;
  buffer = new double[buffer_size];
  for (int i=0; i<nr; i++)
    for (int j=0; j<nc; j++) {
      buffer[i*nc+j]      = x[i*nc+j].real();
      buffer[(i+nr)*nc+j] = x[i*nc+j].imag();
    }
  
  write_var();
  return 0;
}

int
MatFile::write_var(const char *name, const complex<double>* x, int nr_, int nc_) {

  nr = nr_;
  nc = nc_;
  mopt = 0;
  mopt += 0;            // 1 if x was a string 
  mopt += 1000*endian;  // 0: little endian; 1: big endian 
  imag = 1;             // 0: is real; 1: is complex 
  strcpy(var_name, name);
  name_len = strlen(var_name)+1;

  if (buffer_size!=0) {
    delete []buffer;
    buffer_size = 0;
  }
  buffer_size = nr*nc;
  if (imag) buffer_size *= 2;
  buffer = new double[buffer_size];
  for (int i=0; i<nr; i++)
    for (int j=0; j<nc; j++) {
      buffer[i*nc+j]      = x[i*nc+j].real();
      buffer[(i+nr)*nc+j] = x[i*nc+j].imag();
    }
  
  write_var();
  return 0;
}

int
MatFile::write_var(const char* name, const int &x) {
  return write_var(name, &x, 1, 1);
}

int
MatFile::write_var(const char* name, const float &x) {
  return write_var(name, &x, 1, 1);
}

int
MatFile::write_var(const char* name, const double &x) {
  return write_var(name, &x, 1, 1);
}

int
MatFile::write_var(const char* name, const complex<float> &x) {
  return write_var(name, &x, 1, 1);
}

int
MatFile::write_var(const char* name, const complex<double> &x) {
  return write_var(name, &x, 1, 1);
}

void
MatFile::print_var(void) {
  cout << "mopt    : " << mopt << endl
       << "nr      : " << nr << endl
       << "nc      : " << nc << endl
       << "imag    : " << imag << endl
       << "var_name: " << var_name << endl;
  for (int i=0; i<nr; i++) {
    for (int j=0; j<nc; j++) {
      cout << buffer[i*nc+j];
      if (imag) cout << "+i*(" << buffer[(nr+i)*nc+j] << ")";
      cout << " ";
    }
    cout << endl;
  }
       
};

#ifdef MATFILE_USE_VECTORS
int
MatFile::read_var(const char*name, Vector<int> &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  x.newsize(nr*nc);
  for (int i=0; i<nr; i++) {
    for (int j=0; j<nc; j++) {
      x[i*nc+j] = (int)buffer[i*nc+j];
    }
  }
  return 0;
}

int
MatFile::read_var(const char*name, Vector<float> &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  x.newsize(nr*nc);
  for (int i=0; i<nr; i++) {
    for (int j=0; j<nc; j++) {
      x[i*nc+j] = (float)buffer[i*nc+j];
    }
  }
  return 0;
}

int
MatFile::read_var(const char*name, Vector<double> &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  x.newsize(nr*nc);
  memcpy(&x[0], buffer, nr*nc*sizeof(double));
  return 0;
}

int
MatFile::write_var(const char *name, const Vector<int> &x) {
  return write_var(name, &x[0], x.dim(), 1);
}

int
MatFile::write_var(const char *name, const Vector<float> &x) {
  return write_var(name, &x[0], x.dim(), 1);
}

int
MatFile::write_var(const char *name, const Vector<double> &x) {
  return write_var(name, &x[0], x.dim(), 1);
}

int
MatFile::write_var(const char *name, const Vector< complex<float> > &x) {
  return write_var(name, &x[0], x.dim(), 1);
}

int
MatFile::write_var(const char *name, const Vector< complex<double> > &x) {
  return write_var(name, &x[0], x.dim(), 1);
}
#endif

#ifdef MATFILE_USE_FMAT
int
MatFile::read_var(const char*name, Fortran_matrix<int> &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  x.newsize(nr,nc);
  for (int i=0; i<nr; i++) {
    for (int j=0; j<nc; j++) {
      x(i+1,j+1) = (int)buffer[i+j*nr];//x(i+1,j+1) = (int)buffer[i*nc+j];//this is the original one. Ding Wang adapted it.
    }
  }
  return 0;
}

int
MatFile::read_var(const char*name, Fortran_matrix<float> &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  x.newsize(nr,nc);
  for (int i=0; i<nr; i++) {
    for (int j=0; j<nc; j++) {
      x(i+1, j+1)=(float)buffer[i+j*nr];//x(i+1,j+1) = (float)buffer[i*nc+j];//this is the original one. Ding Wang adapted it.
    }
  }
  return 0;
}

int
MatFile::read_var(const char*name, Fortran_matrix<double> &x) {
  fseek(fp, 0, SEEK_SET);
  read_var();
  while ( strcmp(name, var_name)!=0 ) {
    if ( read_var() ) 
      // i/o error
      return 1; 
  }
  x.newsize(nr,nc);
  for (int i=0; i<nr; i++) {
    for (int j=0; j<nc; j++) {
      x(i+1,j+1) = (double)buffer[i+j*nr];//x(i+1,j+1) = (double)buffer[i*nc+j];//this is the original one. Ding Wang adapted it.
    }
  }
  return 0;
}

int 
MatFile::write_var(const char *name, const Fortran_matrix<int> &A) {
  Subscript M = A.num_rows();
  Subscript N = A.num_cols();
  Vector<int> v(M*N); 
  for (int i=0; i<M; i++)
    for (int j=0; j<N; j++) 
      v[i+j*M] = A(i+1,j+1);
  return write_var(name, &v[0], M, N);
}

int 
MatFile::write_var(const char *name, const Fortran_matrix<float> &A) {
  Subscript M = A.num_rows();
  Subscript N = A.num_cols();
  Vector<float> v(M*N); 
  for (int i=0; i<M; i++)
    for (int j=0; j<N; j++) 
      v[i+j*M] = A(i+1,j+1);
  return write_var(name, &v[0], M, N);
}

int 
MatFile::write_var(const char *name, const Fortran_matrix<double> &A) {
  Subscript M = A.num_rows();
  Subscript N = A.num_cols();
  Vector<double> v(M*N); 
  for (int i=0; i<M; i++)
    for (int j=0; j<N; j++) 
      v[i+j*M] = A(i+1,j+1);
  return write_var(name, &v[0], M, N);
}

int 
MatFile::write_var(const char *name, const Fortran_matrix< complex<float> > &A) {
  Subscript M = A.num_rows();
  Subscript N = A.num_cols();
  Vector< complex<double> > v(M*N); 
  for (int i=0; i<M; i++)
    for (int j=0; j<N; j++) 
      v[i+j*M] = A(i+1,j+1);
  return write_var(name, &v[0], M, N);
}

int 
MatFile::write_var(const char *name, const Fortran_matrix< complex<double> > &A) {
  Subscript M = A.num_rows();
  Subscript N = A.num_cols();
  Vector< complex<double> > v(M*N); 
  for (int i=0; i<M; i++)
    for (int j=0; j<N; j++) 
      v[i+j*M] = A(i+1,j+1);
  return write_var(name, &v[0], M, N);
}
#endif

#ifdef MATFILE_USE_FCOOMAT
int 
MatFile::write_var(const char *name, const Fortran_coordinate_matrix<int> &A) {
  Fortran_matrix<int> B;
  B = A;
  return write_var(name, B);
}

int 
MatFile::write_var(const char *name, const Fortran_coordinate_matrix<float> &A) {
  Fortran_matrix<float> B;
  B = A;
  return write_var(name, B);
}

int 
MatFile::write_var(const char *name, const Fortran_coordinate_matrix<double> &A) {
  Fortran_matrix<double> B;
  B = A;
  return write_var(name, B);
}

int 
MatFile::write_var(const char *name, const Fortran_coordinate_matrix< complex<double> > &A) {
  Fortran_matrix< complex<double> > B;
  B = A;
  return write_var(name, B);
}
#endif
