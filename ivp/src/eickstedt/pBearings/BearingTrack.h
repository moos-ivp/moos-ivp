#include "math.h"
#include "newmatap.h"
#include <sys/time.h>
#include <deque>
#include <string>

#define NO_TRACK 0
#define LR_TRACK 1
#define TRACKING 2

// using namespace NEWMAT;
using namespace std;

class Mavg
{
 public:

 Mavg(int length);
 ~Mavg();

 double get_FVal();
 void set_RawVal(double);

 protected:

 int len;
 int num_measurements;
 double avg;

 deque<double>  svals;
 deque<double>  cvals;
};




class CBearings : public CMOOSApp
{
 public:

  CBearings();
  virtual ~CBearings();

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

  protected:

  ColumnVector win;
  ColumnVector windowed_data;
  Matrix       Array_Data;
  Matrix       Spectrum_real;
  Matrix       Spectrum_imag;
  Matrix       S_real;
  Matrix       S_imag;
  ColumnVector win_spatial;

  Mavg *pitch_filter;
  Mavg *yaw_filter;

  double *P;
  double *Beam_angles;

  string sArray;

  int Ping_Number;
  bool Mail_Flag;
  string Data_Dir;
  
  int Num_Blocks_Integrated;
  int *Array_Elements_Used;

  //number of good elements
  int num_elements_used;
  //number of array elements
  int num_channels;
  //element_spacing on array
  double element_spacing;
  //number of samples in sample file
  int num_samples;
  //length of DFT
  int fft_length;
  //number of beams for DOA estimation
  int num_beams;
  //Amount of overlap between FFT blocks
  double overlap;
  //Integration time (in seconds)
  double integrate;
  //sound speed of water
  double sound_speed;
  //center frequency of source (Hz)
  double center_freq;
  //Bandwidth of source signal (Hz)
  double bandwidth;
  //sampling frequency (Hz)
  double sample_freq;
  //Number of blocks
  int Num_Blocks;
  //number of integration blocks
  int Int_Blocks;
  //Actual integration
  double Actual_Integration;
  //number of sidebins
  int Num_Sidebins;
  //number of bins of interest
  int Num_Bins;
  //center bin of interest
  int center_bin;

  int end_data;
  int buff_index;
  int Num_Files_Processed;

  int Max_Index(double *x, int len);
  void hanning(Real *,int);
  int  round(double);
  bool LoadData(string);
  bool Process_Data();
  bool initialize();
  void zeroes(double *,int);
  double get_msigma();
  void reset_tracking();

  //beam tracking stuff
  double err_const,ufilt,udot,uguessn,uguesso,del_cos,u_est,u_estn;
  double alpha,stab_az,dep_angle,pitch;
  double K,thethat1n_sv,u_error,beta,DEN,NVM,rel_bear,thethat1n,thethat1;
  double del_beam,del_beamo2,maxmean;
  int endfire,beam_no,IPS,lridx,save_idx,det_count,lrcount,IPS_Old,IPS_Change,Endfire_Old;
  int ICOAST,IPSsv,endfire_out,turn,ICOAST_SV,beam_no_sv;
  double heading,heading_save,head_filt, head_filt_dot,beta_head,DEN_SV;
  double old_heading,maxsum,sigmasum,sigma,DETECT_THRESHOLD;
  double aaa,bbb,ccc,fff,ggg,iii,jjj,rel_bear_sv,argg,head_filt_sv,head_filt_dot_sv;
  int ddd,eee,hhh,kkk,old_max,overiter,missed_count;  
  bool initial_time,idx_set,tracking,noise_var_set,filt_set,detect_mode;
  int dcount;

  double alpha_head;                // Units are in degrees (0.7)
  double Turn_Rate_Threshold;       // Units are in degrees per second (0.1 for line array, 2 for swordfish)
  int    Beam_Changed_Threshold;    // Units are in beams (7)
  int    Max_Ubias_Iter;            // Units are in number of iterations (25)
  double Abs_Uerror_Threshold;      // Units are in ? (1.0)
  double Theta_Min;                 // Units are in degrees (10.0)
  int    Detection_Count_Threshold; // Units are in number of detections (5)

  //vehicle position
  double xp,yp;

  //local node number
  int node;
  
  bool detection();
  int track_lr();
  void filter_heading();
  bool Track_Bearing();
  double Min(double,double);
  double Max(double,double);
  int Sign(double);

  //timer stuff
  struct timeval start;
  struct timeval finish;
  double calc_elapsed(struct timeval,struct timeval);
  
};
