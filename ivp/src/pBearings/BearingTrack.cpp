//***************************************************************************
//*  BearingTrack.cpp                                                       *
//*                                                                         *
//*  Bearing Track is a real-time acoustic beamformer that reads in         *
//*  acoustic data files coming from a data acquisition system and provides *
//*  a bearing to an acoustic target.                                       *
//***************************************************************************      

#include "MOOSLib.h"
#include "MOOSGenLib.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "BearingTrack.h"

#if 1
FILE *fpPow = 0;  // debug beam power
#endif

//***************************************************************************
//*  CBearings::CBearings()                                                 *
//*                                                                         *
//*  The constructor for the CBearings class. Variables can be initialized  *
//*  here.                                                                  *
//*                                                                         *
//***************************************************************************   

CBearings::CBearings()
{
  Mail_Flag   = 0;
  Ping_Number = 0;
  num_elements_used = 0;
  Num_Blocks_Integrated = 0;
  Num_Files_Processed = 0;
  pitch = 0.0;
  det_count = 0;
  missed_count = 0;
  lrcount = 0;
  tracking = true;
  noise_var_set = false;
  dcount = 0;
  detect_mode = false;
  node = 0;
}

//***************************************************************************
//*  CBearings::~CBearings()                                                *
//*                                                                         *
//*  The destructor for the CBearings class. Dynamic memory can be freed    *
//*  here.                                                                  *
//*************************************************************************** 

CBearings::~CBearings()
{

  delete(Array_Elements_Used);
  delete(P);
  delete(Beam_angles);
  delete(pitch_filter);
  delete(yaw_filter);
}

//***************************************************************************
//*  CBearings::OnConnectToServer()                                         *
//*                                                                         *
//*  Called by the MOOSApp base class upon connecting to the MOOS database. *
//*  Register for variables here.                                           *
//*                                                                         *
//*************************************************************************** 

bool CBearings::OnConnectToServer()
{
  m_Comms.Register ("SAS_PING", 0);
  m_Comms.Register ("SAS_DIR", 0);
  m_Comms.Register ("NAV_YAW",0);
  m_Comms.Register ("NAV_PITCH",0);
  m_Comms.Register ("NAV_X",0);
  m_Comms.Register ("NAV_Y",0);
  m_Comms.Register ("PROSECUTE_STATE",0);
  m_Comms.Register ("VEHICLE_ID",0);

  string msg;
  char buff[200];
  sprintf(buff,"node=%d,state=%d,bearing=%.4f,xp=%.3f,yp=%.3f,beamno=%d,sigma=%.3f,time=%.3f",node,NO_TRACK,0.0,0.0,0.0,0,0.0,MOOSTime());
  msg = buff;
  m_Comms.Notify   ("BEARING_STAT",msg);
  m_Comms.Notify   ("TRACKING","NO_TRACK");  
  return true;
}

//***************************************************************************
//*  CBearings::OnNewMail()                                                 *
//*                                                                         *
//*  Called by the MOOSApp base class when a variable this app is subscribed*
//*  to changes.                                                            *
//*                                                                         *
//*************************************************************************** 

bool CBearings::OnNewMail(MOOSMSG_LIST &NewMail)
{
	
bool silent = true;
bool timing = false;
double raw_heading,raw_pitch;

MOOSMSG_LIST::iterator p;

for(p=NewMail.begin(); p!=NewMail.end(); p++)
    {
        CMOOSMsg & rMsg = *p;
        if (rMsg.m_sKey == "SAS_PING")
	  {
		Ping_Number = (int) rMsg.m_dfVal;
		if(!silent)
		  MOOSTrace ("File %d received\n",Ping_Number);  
		if(timing)
		  gettimeofday(&start,NULL);
		Mail_Flag = 1;
	  }
	else if (rMsg.m_sKey == "SAS_DIR")
	  {
		Data_Dir = rMsg.m_sVal;
		if (!silent)
		  MOOSTrace ("Data Directory = %s\n", Data_Dir.c_str());
	  }
	else if (rMsg.m_sKey == "NAV_YAW")
	  {
	    raw_heading = -rMsg.m_dfVal*180.0/M_PI;
	    if (raw_heading < 0.0)
                  raw_heading = raw_heading + 360.0;
	    if (filt_set)
	      yaw_filter->set_RawVal(raw_heading);
	    if (!silent)
	       MOOSTrace ("Heading = %lf\n",heading);
	    // publish heading in AEL_HEADING for ArrayTurn and ArrayAngle
	    // behaviors. This is a hack, obviously, should be published 
	    // together with NAV_YAW. HS 103106
                m_Comms.Notify   ("AEL_HEADING",raw_heading);
	  }
	else if (rMsg.m_sKey == "NAV_PITCH")
	  {
	    raw_pitch = rMsg.m_dfVal;
	    //pitch = rMsg.m_dfVal;
	    if(filt_set)
	      pitch_filter->set_RawVal(raw_pitch);
	    if (!silent)
	       MOOSTrace ("Pitch = %lf\n",pitch);
	  }
	else if(rMsg.m_sKey == "NAV_Y")
	  {	
	    yp = rMsg.m_dfVal;
	    
	  }
	else if(rMsg.m_sKey == "NAV_X")
	  {	
	    xp = rMsg.m_dfVal;
	  }
	else if(rMsg.m_sKey == "VEHICLE_ID")
	  {	
	    node = (int) rMsg.m_dfVal;
	  }
	else if(rMsg.m_sKey == "PROSECUTE_STATE")
	  {	
	    string p_state = rMsg.m_sVal;
            MOOSTrace("prosecute_state = %s\n",p_state.c_str());
            if(MOOSStrCmp(p_state,"PROSECUTE"))
              detect_mode = true;
            else
	      {
		detect_mode = false;
		reset_tracking();
	      }
	  }
    }

return true;
}

//***************************************************************************
//*  CBearings::Iterate()                                                   *
//*                                                                         *
//*  Called by the MOOSApp base class at a fixed rate specified in the      *
//*  .moos file. Calls to this method trigger processing of data that is    *
//*  ready to be processed.                                                                       *
//*************************************************************************** 

bool CBearings::Iterate()
{
  string pre;
  char filename[170];
  
      if(Mail_Flag == 1)
	{

	  //build the file name
	  sprintf(filename,"%s/%s%06d%s",Data_Dir.c_str(),"hfa",Ping_Number,".dat");
	 
	  //load the data file
	  if(LoadData(filename))
		
	    if (!Process_Data())
	      {
		MOOSTrace("Processing failed!\n");
		return false;
	      }
	    else
	      {
		Mail_Flag = 0;
		return true;
	      }
	}
return true;
}

//***************************************************************************
//*  CBearings::initialize()                                                *
//*                                                                         *
//*  Initialize all of the data structures and constants needed to process  *
//*  data.                                                                  *
//*                                                                         *
//*************************************************************************** 

bool CBearings::initialize()
{

bool silent = true;
double constant,freq,temp;
int k,z,m;
ColumnVector win_spatial_temp2;
ColumnVector win_spatial_temp;

 int filt_len;

//calculate a bunch of constants
Num_Blocks = (int)((1+(num_samples-fft_length)/((1-overlap)*fft_length)));

Int_Blocks = round((double)(integrate*sample_freq/((1-overlap)*fft_length)));

Actual_Integration = Int_Blocks*((1-overlap)*fft_length)/sample_freq;

center_bin = 1+round((double)(center_freq*fft_length/sample_freq));

constant = (element_spacing/sound_speed)*2*M_PI;

Num_Sidebins = (int) (ceil((bandwidth*0.5)/(sample_freq/fft_length)));

Num_Bins = 2*Num_Sidebins + 1; 

//size the NEWMAT arrays
win.ReSize(fft_length);
windowed_data.ReSize(fft_length);
win_spatial.ReSize(num_elements_used);

//NOTE: Assumes 8/16-channel AUV nose arrays!!!
//************************************************
win_spatial_temp.ReSize(num_channels);
win_spatial_temp2.ReSize(num_channels);

//Declare Matrix which contains all of the steering vectors for all beams defined in 
//		Beam_angles and for all DFT bins of interest
//		size of S: (NUM_BEAMS*Num_Bins) x NUM_ELEMENTS_USED 

S_real.ReSize(num_beams*Num_Bins, num_elements_used);
S_imag.ReSize(num_beams*Num_Bins, num_elements_used);

P = new double[num_beams+1];
zeroes(P,num_beams+1);

Beam_angles = new double[num_beams+1];

Array_Data.ReSize(num_samples+fft_length, num_channels);
Spectrum_real.ReSize((fft_length/2 + 1), num_elements_used); 
Spectrum_imag.ReSize((fft_length/2 + 1), num_elements_used);

//Specify the equally spaced beam "look directions"
for (k=1; k <= num_beams; k++)
{
  Beam_angles[k]= 1.0 - (2.0/(num_beams-1))*(k-1);
}

//Create time-domain window used before the DFT operation
hanning(win.Store(),fft_length);


//*************************************************
//NOTE: Assumes 8/16-channel AUV nose arrays!!!!!
//**************************************************
 if (MOOSStrCmp(sArray,"SINGLE"))
   {
   if(num_elements_used <= 8)
     {
      hanning(win_spatial_temp2.Store(),8);      
      for (k = 1; k <=8;k++)
	{
	  win_spatial_temp(2*k-1) = win_spatial_temp2(k);
	  win_spatial_temp(2*k) = win_spatial_temp2(k);
	}
	  
     }
   else
      hanning(win_spatial_temp.Store(),16);
   }     
 else if (MOOSStrCmp(sArray,"DUAL"))
   {
    hanning(win_spatial_temp2.Store(),8);
    for (k = 1; k <=8;k++)
      {
	 win_spatial_temp(k)   = win_spatial_temp2(k);
	 win_spatial_temp(k+8) = win_spatial_temp2(k);
      }   }
 else
   {
     MOOSDebugWrite("ERROR: BAD ARRAY\n");
     exit(-1);
   }

 for (k = 1; k <= num_elements_used; k++)
    win_spatial(k) = win_spatial_temp(Array_Elements_Used[k]);
     
 // for (k = 1; k <= num_elements_used; k++)
 //MOOSTrace("win_spatial(k) = %.16lf\n",win_spatial(k));


/*Precalculate the steering vectors 
The matrix S has the following structure:
	1)	Each row corresponds to a steering vector for a specific frequency bin and DOA
	2)	size of S: (NUM_BEAMS*Num_Bins) x NUM_ELEMENTS_USED 
	3) 	First set of Num_Bins rows correspond to first beam, Second set of Num_Bins rows correspond to second beam, etc.
*/

for (z=1; z <= num_beams; z++)
  {	for (k=1; k <= Num_Bins; k++)
	{
		freq=sample_freq*((center_bin-Num_Sidebins)+k-1)/fft_length;
		
		for (m=1; m <= num_elements_used; m++)
		{
			temp=-constant*(Array_Elements_Used[m]-1)*Beam_angles[z]*freq;
			S_real(k+(z-1)*Num_Bins, m)=cos(temp)*win_spatial(m);
			S_imag(k+(z-1)*Num_Bins, m)=sin(temp)*win_spatial(m);
		}
	}	
  }

//beam tracking constants
err_const = (2.0/3.0)*(1.0/pow(3,0.5));

//if (num_beams == 9)
//  K = -0.1185/err_const;
//else if (num_beams >= 17)
  K = 0.1185/err_const;

  //if (num_elements_used == 16)
  //K = 0.1185/err_const;


ufilt = 0.0;
udot = 0.0;
del_beam = 2.0/(num_beams-1);
del_beamo2 = del_beam/2.0;
head_filt = heading;
head_filt_dot = 0.0;
//alpha_head = 0.7;
beta_head = (alpha_head*alpha_head)/(2.0 - alpha_head);
beta = (alpha*alpha)/(2.0-alpha);
initial_time = true;
IPS = -100;
dcount = 0;
maxsum = 0.0;
sigmasum = 0.0;
sigma = 0.0;
idx_set = false;
endfire = 0;
u_estn = 0.0;
noise_var_set = false;

 filt_len = (int) round(Actual_Integration/0.2);
 //MOOSTrace("filter length = %d\n",filt_len);

 pitch_filter = new Mavg(filt_len);
 yaw_filter = new Mavg(filt_len);
 filt_set = true;

 if (!silent)
   {
     MOOSTrace("Num blocks = %d\n",Num_Blocks);
     MOOSTrace("Integration blocks = %d\n",Int_Blocks);
     MOOSTrace("Actual Integration = %lf\n",Actual_Integration);
     MOOSTrace("Center bin = %d\n",center_bin);
     MOOSTrace("Num sidebins = %d\n",Num_Sidebins);
     MOOSTrace("Num bins = %lf\n",Num_Bins);
     MOOSTrace("constant = %lf\n\n",constant);
     MOOSTrace("error constant = %lf \n",err_const);
   }

 return true;
}

//***************************************************************************
//*  CBearings::OnStartUp()                                                 *
//*                                                                         *
//*  Called by the MOOSApp base class when tis application is started       *
//*                                                                         *
//*************************************************************************** 

bool CBearings::OnStartUp()
{

 bool silent = true;

 double nc;

 if (!m_MissionReader.GetConfigurationParam("def_numchannels",nc))
    {
        MOOSDebugWrite("No default num channels available - FAIL\n");
        return false;
    }

 num_channels = (int) nc;

if (!m_MissionReader.GetConfigurationParam("def_spacing",element_spacing))
    {
        MOOSDebugWrite("No default element spacing available - FAIL\n");
        return false;
    }

 double ns;

if (!m_MissionReader.GetConfigurationParam("def_numsamples",ns))
    {
        MOOSDebugWrite("No default sample number available - FAIL\n");
        return false;
    }

 num_samples = (int) ns;

 double fl;

if (!m_MissionReader.GetConfigurationParam("def_fftlength",fl))
    {
        MOOSDebugWrite("No default FFT length available - FAIL\n");
        return false;
    }

 fft_length = (int) fl;

 double nb;

if (!m_MissionReader.GetConfigurationParam("def_beams",nb))
    {
        MOOSDebugWrite("No default beams number available - FAIL\n");
        return false;
    }

 num_beams = (int) nb;

if (!m_MissionReader.GetConfigurationParam("def_overlap",overlap))
    {
        MOOSDebugWrite("No default overlap number available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_integrate",integrate))
    {
        MOOSDebugWrite("No default integration time available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_soundspeed",sound_speed))
    {
        MOOSDebugWrite("No default sound speed available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_centerfreq",center_freq))
    {
        MOOSDebugWrite("No default sample number available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_bandwidth",bandwidth))
    {
        MOOSDebugWrite("No default bandwidth available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_samplerate",sample_freq))
    {
        MOOSDebugWrite("No default sample freq available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_alpha",alpha))
    {
        MOOSDebugWrite("No default alpha available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_dep_angle",dep_angle))
    {
        MOOSDebugWrite("No default dep angle  available - FAIL\n");
        return false;
    }
 

if (!m_MissionReader.GetConfigurationParam("def_array",sArray))
    {
        MOOSDebugWrite("No default array data  available - FAIL\n");
        return false;
    }



if (!m_MissionReader.GetConfigurationParam("def_det_thresh",DETECT_THRESHOLD))
    {
        MOOSDebugWrite("No default detect threshold  available - FAIL\n");
        return false;
    }

 double lrb;

if (!m_MissionReader.GetConfigurationParam("def_lrbeams",lrb))
    {
        MOOSDebugWrite("No default lr index  available - FAIL\n");
        return false;
    }

 lridx = (int) lrb;

string sChannels; 

if (!m_MissionReader.GetConfigurationParam("def_goodchannels",sChannels))
    {
        MOOSDebugWrite("No default good channels available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_Alpha_Head", alpha_head))
    {
        MOOSDebugWrite("No default Alpha Head available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_Turn_Rate_Threshold", Turn_Rate_Threshold))
    {
        MOOSDebugWrite("No default Turn Rate Threshold available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_Beam_Changed_Threshold", Beam_Changed_Threshold))
    {
        MOOSDebugWrite("No default Beam Changed Threshold available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_Max_Ubias_Iter", Max_Ubias_Iter))
    {
        MOOSDebugWrite("No default Max Ubias Iter available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_Abs_Uerror_Threshold", Abs_Uerror_Threshold))
    {
        MOOSDebugWrite("No default Absolute Uerror Threshold available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_Theta_Min", Theta_Min))
    {
        MOOSDebugWrite("No default Theta Min available - FAIL\n");
        return false;
    }

if (!m_MissionReader.GetConfigurationParam("def_Detection_Count_Threshold", Detection_Count_Threshold))
    {
        MOOSDebugWrite("No default Detection Count Threshold available - FAIL\n");
        return false;
    }

Array_Elements_Used = new int[num_channels+1];

 string schan,Chansave;
 int idx = 1;

 Chansave = sChannels;

 schan = MOOSChomp(sChannels,",");

 while (!schan.empty())
   { 
     Array_Elements_Used[idx] = atoi(schan.c_str());
     schan = MOOSChomp(sChannels,",");
     idx++;
     num_elements_used++;
   }

 if(!silent)
   {
     MOOSTrace("num channels = %d\n",num_channels); 
     MOOSTrace("num elements used = %d\n",num_elements_used);
     MOOSTrace("Good Channels = %s\n",Chansave.c_str());
     MOOSTrace("Element Spacing (m)= %lf\n",element_spacing);
     MOOSTrace("Number of Samples = %d\n",num_samples);
     MOOSTrace("FFT Length = %d\n",fft_length);
     MOOSTrace("Sound Speed (m/s) = %lf\n",sound_speed);
     MOOSTrace("Center frequency (Hz) = %lf\n",center_freq);
     MOOSTrace("Number of beams = %d\n",num_beams);
     MOOSTrace("Signal Bandwidth = %lf\n",bandwidth);
     MOOSTrace("Sample Rate = %lf\n",sample_freq);
     MOOSTrace("Overlap = %lf\n\n",overlap);
     MOOSTrace("Alpha = %lf\n",alpha);
     MOOSTrace ("Alpha_Head = %lf\n", alpha_head);
     MOOSTrace ("Turn Rate Threshold = %lf\n", Turn_Rate_Threshold);
     MOOSTrace ("Beam_Changed_Threshold = %d\n",Beam_Changed_Threshold);
     MOOSTrace ("Max_Ubias_Iter = %d\n",Max_Ubias_Iter);
     MOOSTrace ("Abs_Uerror_Threshold = %lf\n",Abs_Uerror_Threshold);
     MOOSTrace ("Theta_Min = %lf\n",Theta_Min);
     MOOSTrace ("Detection_Count_Threshold = %d\n",Detection_Count_Threshold);
   }


 initialize();

 return true;
}

//***************************************************************************
//*  CBearings::ProcessData()                                               *
//*                                                                         *
//*  Called after a new data file has been read. This method calculates     *
//*  target bearings.                                                       *
//*************************************************************************** 


bool CBearings::Process_Data()
{	
		
int m,z,k,idx1,idx2,w;
bool timing = false;
bool detect_flag;

Real P_real, P_imag, temp2;
ColumnVector s_real,s_imag;
//double DOA[Num_Blocks];
 double elapsed;
char bpwr_file[70];

w = 0;
z = 0;

buff_index = 0;

while ((buff_index + fft_length) < end_data) 
  {
 
	for (k=1; k <= num_elements_used; k++)
	{
	  windowed_data=SP(Array_Data.SubMatrix((buff_index+1),(buff_index+fft_length),Array_Elements_Used[k],Array_Elements_Used[k]),win);
	  RealFFT (windowed_data, s_real, s_imag);
	  Spectrum_real.Column(k) = s_real;
	  Spectrum_imag.Column(k) = s_imag;
	}
	
	//Find Power Spectrum
	for (k=1; k <= num_beams; k++)
	{
	     temp2=0;
	     for (m=1; m <= Num_Bins; m++)
	     {
	       idx1 = (m+Num_Bins*(k-1));
	       idx2 = center_bin-Num_Sidebins+m-1;
	       P_real=DotProduct(S_real.Row(idx1),Spectrum_real.Row(idx2)) + DotProduct(S_imag.Row(idx1),Spectrum_imag.Row(idx2));	
	       P_imag=DotProduct(S_real.Row(idx1),Spectrum_imag.Row(idx2)) - DotProduct(S_imag.Row(idx1),Spectrum_real.Row(idx2));
               temp2=temp2+P_real*P_real+P_imag*P_imag;
	     }
	      //Magnitude squared of the power
	      P[k]=P[k]+ temp2;
	}

	Num_Blocks_Integrated++;

	if (Num_Blocks_Integrated == Int_Blocks)
	  {	
#if 1

	  sprintf(bpwr_file,"%s/%s%06d%s",Data_Dir.c_str(),"hfa",Ping_Number,".bpw");
		fpPow = fopen(bpwr_file,"w");
		//}
                fwrite((void *) &num_beams, sizeof(int),1,fpPow);
		fwrite((void *) &P[1], sizeof(double), num_beams, fpPow);
                fclose(fpPow);
                m_Comms.Notify   ("BTR_FILE",bpwr_file);
#endif
	    
	    filter_heading();
	    detect_flag = detection(); //check for detection

	    //we have a detection
	    if (detect_flag)
	      {
		if (IPS  < -1) //prev state = no detection
		  {	      
		    IPS = 0;                 //now ambiguous
		    IPS = track_lr();                    
		  }
		else if (IPS == 0)   //prev_state = ambiguous
		  {	
		    IPS=track_lr();  
                    MOOSTrace("IPS = %d\n",IPS);
		  }
		else if ((IPS == 1)||(IPS == -1)) //prev_state = tracking
		  {
		    Track_Bearing();
		  }
	      } //if detect flag
	    else
	      {
		string msg;
		char buff[200];
		sprintf(buff,"node=%d,state=%d,bearing=%.4f,xp=%.3f,yp=%.3f,beamno=%d,sigma=%.3f,time=%.3f",node,NO_TRACK,0.0,xp,yp,0,0.0,MOOSTime());
		msg = buff;
		m_Comms.Notify ("BEARING_STAT",msg);
		m_Comms.Notify ("TRACKING","NO_TRACK");
		if (IPS >= -1)
		  {
		    reset_tracking();
		    IPS = -100;
		  }
	      }

	    zeroes(P,num_beams+1);
	    Num_Blocks_Integrated = 0;
	    w++;
	  }

	z++;
	buff_index= (int) (z*(1-overlap)*fft_length);
  }		

 if (timing)
   {
     gettimeofday(&finish,NULL);
     elapsed = calc_elapsed(start,finish);
     MOOSTrace("Elapsed = %lf seconds\n",elapsed);
   }

return true;
}


/*	Create a Hanning window of length M
	win: output window vector, with length M
*/
void CBearings::hanning(Real *window, int M)
{

int n;

for (n=0; n < M; n++)
  window[n] = (double) (0.5*(1 - cos(2*M_PI*(n+1)/(M+1))));

}

//round to the nearest integer
int CBearings::round(double num) 
{

if (num >= 0.0)
  return ((int) (num + 0.5));
else    
  return ((int) (num - 0.5));

}

//***************************************************************************
//*  CBearings::LoadData(string filename)                                   *
//*                                                                         *
//*  LoadData() reads in the most recent sonar file.                        *
//*                                                                         *
//*************************************************************************** 

bool CBearings::LoadData(string filename)
{	

FILE *fp;
char fnamechar[70];

int time0,time1,kk,jj,leftover;  
short *temp_data;

pitch = pitch_filter->get_FVal();
heading = yaw_filter->get_FVal();
// heading = heading;
//MOOSTrace("Filtered Pitch = %f, filtered heading = %f\n",pitch,heading);


Matrix Array_Temp(fft_length,num_channels);

temp_data = new short [num_samples];

sprintf(fnamechar,"%s",filename.c_str());

// MOOSTrace("end_data = %d, buff_index = %d\n",end_data,buff_index);

 if (Num_Files_Processed > 0)
   {
     leftover = (end_data - buff_index);
     for (int uu = 1; uu <= leftover; uu++)
	 Array_Data.Row(uu) = Array_Data.Row(buff_index+uu);
     end_data = leftover + num_samples;   
   }
 else
   {
     leftover = 0;
     end_data = num_samples;
   }

//open sample file
if ((fp = fopen(fnamechar,"rb")) == NULL)
  {
    MOOSTrace("Could not open sample file %s\n",fnamechar);
    return false;
  }
	  		
//read the two timetags out of the file   		  		
fread(&time0,sizeof(int),1,fp);
fread(&time1,sizeof(int),1,fp);

// signals from all channels
for (kk=0; kk < num_channels; kk++)
{
  fread(temp_data,sizeof(short),num_samples,fp);

   for (jj=leftover; jj < end_data; jj++)
     { 
      Array_Data(jj+1,kk+1) = temp_data[jj-leftover];
    }
}

Num_Files_Processed++;
delete(temp_data);
fclose(fp);

return true;
}


//return the index of the maximum element in vector x
int CBearings:: Max_Index(double *x, int len)
{
int index = 0;
//double maximum = x[0];
 double maximum = 0.0;

for (int nn = 1; nn <= len; nn++)
  {
    if (x[nn] > maximum)
      {
	maximum = x[nn];
	index = nn;
      }
  }

return index;
}

//initialize a vector with zeroes
void CBearings::zeroes(double * array,int length)
{
  for (int ii = 0; ii < length; ii++ )
    array[ii] = 0.0;
}
 

//calculate the elapsed time in seconds
//can only be used to time events lasting < 1 minute
double CBearings::calc_elapsed(struct timeval one, struct timeval two)
{

  double one_time = one.tv_sec + one.tv_usec/1000000.0;
  double two_time = two.tv_sec + two.tv_usec/1000000.0;

  if (two.tv_sec < one.tv_sec)
    two_time = two_time + 60;

  return (two_time-one_time);
}

void CBearings::filter_heading()
{
  double resid_H;

  if (heading > 360.0)
    heading = heading - abs(round(heading/360.0))*360.0;

  if (heading < 0.0)
    heading = heading + abs(round(heading/360.0))*360.0;

  if (heading > 360.0)
    heading = heading - 360.0;

  if (heading < 0.0)
    heading = heading + 360.0;

  resid_H = (heading-head_filt);
  
  if (resid_H >= 180.0)
    resid_H = resid_H - 360.0;
  else if (resid_H <= -180.0)
    resid_H = resid_H + 360.0;

  head_filt_dot = head_filt_dot + beta_head*(resid_H)/Actual_Integration;
  head_filt = head_filt + (head_filt_dot*Actual_Integration) + (alpha_head*resid_H);
  
  if (head_filt > 360.0)
      head_filt = head_filt - 360.0;
  else if (head_filt < 0.0)
      head_filt = head_filt + 360.0;
  m_Comms.Notify("head_filt_dot = %f\n",head_filt_dot);  
  if (fabs(head_filt_dot) > Turn_Rate_Threshold)
    turn = Sign(head_filt_dot);
  else
    turn = 0;
   

}

inline double CBearings::Min(double v1, double v2)
{
	return(v1 < v2 ? v1 : v2);
}



inline double CBearings::Max(double v1, double v2)
{
	return(v1 > v2 ? v1 : v2);
}

int CBearings::Sign(double num)
{
  if (num < 0.0)
    return (-1);
  else 
    return (1);
}

bool CBearings::Track_Bearing()
{

double f,df,bmid,bleft,bright,denom;
int nmid,iter;
double relative_az;//ADDED AJP 17NOV2006

// MOOSTrace("Tracking Bearing\n");

 if(initial_time)
   {
     old_heading = heading;
     beam_no = Max_Index(P,num_beams);
     old_max = beam_no;	
   
      thethat1n_sv =IPS* acos(Beam_angles[beam_no])+ (heading*M_PI/180.0);
   

      if ((beam_no > 1) && (beam_no < num_beams))
        endfire = 0;
      else if (beam_no == 1)
        {
        endfire = 1;
        DEN_SV = 0;
         }
      else if (beam_no = num_beams)
        {
        endfire = -1;
        DEN_SV = 0;
        }
   }

u_error = 0;

//filter_heading();
MOOSTrace("filtered heading = %f\n",heading);
beam_no = Max_Index(P,num_beams);	    

 if (abs(old_max - beam_no) > Beam_Changed_Threshold){
   beam_no = old_max;
   overiter = 1;
   }
 else{
   overiter = 0;
   old_max = beam_no;
 }

if (overiter == 0){

  if ((beam_no > 1) && (beam_no < num_beams))
  {
    bmid = P[beam_no];
    bleft = P[beam_no -1 ];
    bright = P[beam_no + 1];
    endfire = 0;
  }
  else if (beam_no == 1){
    endfire = 1;
    DEN_SV = 0;
  }
  else if (beam_no == num_beams){
    endfire = -1;
    DEN_SV = 0;
  }

  if (endfire == 0)
  {
    nmid = beam_no;
    denom = Max((bmid-bleft),(bmid-bright));
    del_cos = -(bright-bleft)/denom;
    u_est = del_cos;
    uguesso = del_cos;

    for (iter = 1; iter <= Max_Ubias_Iter; iter++)
      {
        f = (1+K)*uguesso - K*pow(uguesso,3) - u_est;
	df = (1+K) - 3*K*pow(uguesso,2);
	uguessn = uguesso - f/df;
		    
	if (fabs(uguessn - uguesso) < 0.01)
	     break;
		    
	uguesso = uguessn;
      }
   
    if (iter == Max_Ubias_Iter)
      MOOSTrace("Did not converge\n");

    u_estn = Beam_angles[nmid] + uguessn*del_beamo2;

   
    if (initial_time)
      {
	//MOOSTrace("Setting ufilt = %lf\n",u_estn);
	ufilt = u_estn;
	initial_time = false;
	thethat1n_sv = IPS*acos(ufilt) + (heading*M_PI/180.0);
      }
		
    stab_az = thethat1n_sv-(heading*M_PI/180);
    //MOOSTrace("stab_az 1 = %lf, thethat1n_sv = %lf, heading = %lf\n",stab_az,thethat1n_sv,heading);
    if(stab_az > (2.0*M_PI))
	  stab_az = stab_az - (2.0*M_PI);
    else if (stab_az < 0.0)
	  stab_az = stab_az + (2.0*M_PI);
    //MOOSTrace("stab_az 2 = %lf, thethat1n_sv = %lf, heading = %lf\n",stab_az,thethat1n_sv,heading);
    //MOOSTrace("IPS = %d\n",IPS);
    ufilt = cos(stab_az)*cos(pitch)*cos(dep_angle) + sin(pitch)*sin(dep_angle);
   
    //MOOSTrace("ufilt = %lf\n",ufilt);
    u_error = u_estn - ufilt;

    if (fabs(u_error) > Abs_Uerror_Threshold)
	  u_error = udot*Actual_Integration;

    // udot = udot + beta*(u_estn - ufilt)/Actual_Integration;
    udot = udot + (beta*u_error)/Actual_Integration;
    //ufilt = ufilt + (udot*Actual_Integration) + alpha*(u_estn - ufilt);
    ufilt = ufilt + (udot*Actual_Integration) + alpha*u_error;
    //MOOSTrace("ufilt = %lf, udot = %lf\n",ufilt,udot);
    DEN = (ufilt-sin(pitch)*sin(dep_angle))/(cos(pitch)*cos(dep_angle));

    DEN_SV = DEN;

    if(fabs(DEN) > 1.0)
	   DEN = (double) Sign(ufilt);

    NVM = IPS*pow((1-DEN*DEN),0.5);
    rel_bear = atan2(NVM,DEN);

    rel_bear_sv = rel_bear*180.0/M_PI;
    argg = (u_est-sin(dep_angle)*Sign(pitch))/(cos(pitch)*cos(dep_angle));

    if(fabs(argg) <= 1.0)
      thethat1 = IPS*acos(argg)*180.0/M_PI + heading;
    else if (fabs(argg) > 1.0)
      thethat1 = IPS*Sign(argg) + heading;

    thethat1n = (rel_bear*180.0/M_PI) + heading;
    
    if (thethat1n < 0.0)
	 thethat1n = thethat1n + 360.0;
    //yes, check twice, this is not an error
    if (thethat1n < 0.0)
	 thethat1n = thethat1n + 360.0;

//********************************
    if (thethat1n > 360.0)
	 thethat1n = thethat1n - 360.0;
    //yes, check twice					       
    if (thethat1n > 360.0)
	 thethat1n = thethat1n - 360.0;
//*********************************    

    thethat1n_sv = thethat1n*M_PI/180.0;
  }
else //endfire .ne. 0
  {
    u_estn = Sign(endfire);  

  if (initial_time == 0)
    {
    thethat1n = thethat1n_sv*180.0/M_PI;
    thethat1 = thethat1n;
    }
  else
    {
      if (endfire == 1)
	{
	thethat1n = heading;
        thethat1 = thethat1n;
	}
      else if (endfire == -1)
	{
	  thethat1n = 180.0 + heading;
	  thethat1 = thethat1n;
	}
      initial_time = 0;
    }
  rel_bear = thethat1n - heading;



  thethat1n_sv = thethat1n*M_PI/180.0;
  stab_az = thethat1n_sv-(heading*M_PI/180);
  rel_bear_sv = rel_bear;   
  }

 }//end if (overiter == 0)

ICOAST = 0;
IPSsv = IPS;
endfire_out = endfire;

if (!initial_time)
{
 ///////////////////////////////BEGIN CHANGE AJP 25NOV2006
  if (stab_az<=M_PI)
  	relative_az=stab_az;
  else
	relative_az=2*M_PI-stab_az;
 //if(((IPS*turn == 1)&&(fabs(stab_az*180.0/M_PI) < Theta_Min))||((IPS*turn == -1)&&(fabs(stab_az*180.0/M_PI) > (180.0-Theta_Min)))){ 
 if(((IPS*turn == 1)&&(fabs(relative_az*180.0/M_PI) < Theta_Min))||((IPS*turn == -1)&&(fabs(relative_az*180.0/M_PI) > (180.0-Theta_Min)))){
///////////////////////////////END CHANGE AJP 25NOV2006
   ICOAST = 1;
   thethat1 = aaa;
   thethat1n = bbb;
   thethat1n_sv = bbb*M_PI/180.0;
 }
}
if((turn == 0)||(fabs(sin(stab_az)) > sin(Theta_Min*M_PI/180.0)))
  {
    ICOAST = 0;
  }						  

if (abs(turn) ==  1)//turn ne 0
  {  
    //MOOSTrace("Changed IPS\n");
  IPS = Sign(sin(stab_az));
  m_Comms.Notify("STAB_AZ",stab_az*180.0/M_PI);
  m_Comms.Notify("IPS",IPS);
  if (IPS == 0)
    IPS = IPSsv;
  }

ICOAST_SV = ICOAST;
beam_no_sv = beam_no;
if(overiter == 0){
  aaa = thethat1;
  bbb = thethat1n;
  ccc = rel_bear_sv;
  ddd = IPSsv;
  eee = beam_no_sv;
  fff = head_filt_sv;
  ggg = head_filt_dot_sv;
  hhh = endfire_out;
  iii = DEN_SV;
  jjj = u_estn;
  kkk = turn;
}
else{
  thethat1=aaa;
  thethat1n=bbb;
  rel_bear_sv=ccc;
  IPSsv=ddd;
  beam_no_sv=eee;
  head_filt_sv=fff;
  head_filt_dot_sv=ggg;
  endfire_out=hhh;
  DEN_SV=iii;
  u_estn=jjj;
  turn=kkk; 
}

 //compute the measurement standard deviation
  double msigma = get_msigma(); 
  m_Comms.Notify ("TRACKING","TRACKING");
  string msg;
  char buff[200];
  sprintf(buff,"node=%d,state=%d,bearing=%.4f,xp=%.3f,yp=%.3f,beamno=%d,sigma=%.3f,time=%.3f",node,TRACKING,thethat1n,xp,yp,beam_no,msigma,MOOSTime());
  msg = buff;
  m_Comms.Notify("BEARING_STAT",msg); 
 

 MOOSTrace("Bearing = %lf\n",thethat1n);
 return true;
}

//***************************************************************************
//*  CBearings::track_lr()                                                  *
//*                                                                         *
//*  track_lr() determines whether the source is on the left or right side  *
//*  of our line array                                                      *
//*                                                                         *
//*************************************************************************** 
int CBearings::track_lr()
{

int maxidx,cw,enfidx;

 enfidx= (int) floor(lridx*1.5);

lrcount++;

 maxidx = Max_Index(P,num_beams);
 MOOSTrace("Max Index = %d\n",maxidx);

  string msg;
  char buff[200];
  sprintf(buff,"node=%d,state=%d,bearing=%.4f,xp=%.3f,yp=%.3f,beamno=%d,sigma=%.3f,time=%.3f",node,LR_TRACK,0.0,xp,yp,maxidx,0.0,MOOSTime());
  msg = buff;
  m_Comms.Notify("BEARING_STAT",msg); 
  m_Comms.Notify ("TRACKING","AMBIGUOUS"); 

  //MOOSTrace("AMBIGUOUS!!!!!!!\n");
 if(!idx_set)
   { 
     //make sure target is not close to endfire
     if ((maxidx > enfidx)&&(maxidx < (num_beams-enfidx+1)))
       {
	 save_idx = maxidx;
	 heading_save = heading;
         if(heading_save < 0.0)
	   heading_save = heading_save + 360.0;
         MOOSTrace("idx = %d, heading = %f\n",save_idx,heading_save);
	 idx_set = true;
	 return 0;
       }
     else
       return 0;
   }
 else
   { //have target moved enough beams to make a decision?
     if (abs(maxidx - save_idx) >= lridx)
       {
       
       //did we rotate CW or CCW?
       double curr_heading = heading;
       if(curr_heading < 0.0)
         curr_heading = curr_heading + 360.0;
       double hdiff = heading_save-curr_heading;	 
       MOOSTrace("heading = %f, hdiff = %f\n",heading,hdiff);

       if (hdiff < 0.0){
	  if (hdiff < -180.0)
	   cw = -1;
         else 
	   cw = 1;
       }
       else{
	 if (hdiff > 180.0)
	  cw = 1;
         else 
	   cw = -1;
       }
       //MOOSTrace("cw = %d\n",cw);
       //m_Comms.Notify("CW",cw);

       if (maxidx < save_idx)
	 return 1*cw;
       else
	 return -1*cw;
       }
     return 0;
   }
}

//***************************************************************************
//*  CBearings::detection()                                                 *
//*                                                                         *
//*  Detect() creates a statistic for the ambient noise power and then      *
//*  declares a detection if the max power in a beam exceeds this by some   *
//*  amount.                                                                *
//*************************************************************************** 

bool CBearings::detection()
{

  int maxidx;
  double maxpow,sqterm;

  //find the beam with the maximum power
  maxidx = Max_Index(P,num_beams);
  MOOSTrace("detection() - max index = %d\n",maxidx);  
  maxpow = P[maxidx];

if(detect_mode){ 
 
//statistic only valid for > 1 measurement
//determine if a beam exceeds our statistic
if(dcount > 1)
  {
    
  sigma = pow(sigmasum/(dcount-1),0.5);
  //MOOSTrace("Max power = %.16f, sigma = %.16f, timessigma = %.16f\n",maxpow,sigma,(maxpow/sigma));
  if (maxpow > (DETECT_THRESHOLD*sigma+maxmean))    
    {
    det_count++;
    missed_count = 0;
    noise_var_set = true;
    //wait for some number of detections in a row
    if (det_count >= Detection_Count_Threshold)
      {
	MOOSTrace("DETECTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
     	      
      return true;
      }    
    }
  //no basic detection 
  else
    {
    //no detection  
    det_count = 0;
    missed_count++;
    if (missed_count >= Detection_Count_Threshold)
      {
	MOOSTrace("UNDETECTION!!!!!!!!!!!!!!!!!!!\n");
        if(noise_var_set)	
          return false;
      }	
    }
  }
 }
//compute  a noise "variance" statistic for detection
if(!noise_var_set)
  {
    //MOOSTrace("Averaging Noise\n");  

  dcount ++;
 
  maxsum += maxpow;

  maxmean = maxsum/dcount;
  MOOSTrace("Maxsum = %.16f, Maxmean = %.16f\n",maxsum,maxmean);

  sqterm = pow((maxpow-maxmean),2);

  sigmasum += sqterm;
  }

//not 5 hits or misses in a row so don't change state
  if (IPS < -1)
      return false;
    else
      return true;
}

void CBearings::reset_tracking()
{
m_Comms.Notify   ("TRACKING","NO_TRACK");  
MOOSTrace("RESETTING!!!!!!!!!!!!!!\n");
ufilt = 0.0;
udot = 0.0;
head_filt = heading;
head_filt_dot = 0.0;
initial_time = true;
IPS = -100;
idx_set = false;
endfire = 0;
u_estn = 0.0;
lrcount = 0;
 string msg;
  char buff[200];
  sprintf(buff,"node=%d,state=%d,bearing=%.4f,xp=%.3f,yp=%.3f,beamno=%d,sigma=%.3f,time=%.3f",node,NO_TRACK,0.0,0.0,0.0,0,0.0,MOOSTime());
  msg = buff;
  m_Comms.Notify   ("BEARING_STAT",msg);
}


//***************************************************************************
//*  CBearings::get_msigma()                                                *
//*                                                                         *
//*  return the std dev of the current bearing measurement                  *
//*                                                                         *
//*************************************************************************** 

// Function Added Friday, Nov, 17, 2006, Andrew Poulsen

double CBearings::get_msigma()
{
  int maxidx;
  double maxpow, SNR, sigma_u, sigma_theta, sigma_theta_deg, rel_az, temp_az;

  maxidx = Max_Index(P,num_beams);
  maxpow = P[maxidx];
  SNR=(maxpow-maxmean)/maxmean;
  
  sigma_u = (sound_speed/(M_PI*center_freq*element_spacing)) * pow((3/(SNR*num_elements_used*(num_elements_used*num_elements_used-1))),0.5);
  
  if (stab_az<0)
  	temp_az=stab_az+2.0*M_PI;
  else if (stab_az>=(2.0*M_PI))
	temp_az=stab_az-2.0*M_PI;
  else
	temp_az=stab_az;

  if (stab_az<=M_PI)
  	rel_az=temp_az;
  else
	rel_az=2*M_PI-temp_az;

  if (rel_az <= M_PI*0.5)
	sigma_theta = acos(cos(rel_az)-sigma_u) - rel_az;
  else //rel_az >M_PI*0.5
	sigma_theta = rel_az - acos(cos(rel_az)+sigma_u);

  sigma_theta_deg=sigma_theta*180.0/M_PI;

// MOOSTrace("SNR (linear) = %.3f, SNR (dB) = %.3f, maxidx = %d, maxpow = %.3f, maxmean = %.3f, sigma_u=%.3f, sigma_theta=%.3f, sigma_theta_deg=%.3f, stab_az = %.3f\n, rel_az = %.3f\n",SNR,(10.0*log(SNR)/log(10.0)),maxidx,maxpow,maxmean,sigma_u,sigma_theta,sigma_theta_deg,(stab_az*180.0/M_PI),(rel_az*180.0/M_PI));

  return sigma_theta_deg;
}

//***************************************************************************
//*  Mavg::Mavg(int length)                                                 *
//*                                                                         *
//*  Constructor for the Mavg class.                                        *
//*                                                                         *
//*************************************************************************** 
Mavg::Mavg(int length)
{
  
  len = length;
  num_measurements = 0;
}

Mavg::~Mavg()
{
}

//***************************************************************************
//*  Mavg::set_RawVal()                                                     *
//*                                                                         *
//*  This method calculates the moving average of a sequence of angles.     *
//*                                                                         *
//*************************************************************************** 

void Mavg::set_RawVal(double val)
{
  int avg_len;
  double csum,ssum,s,c;

  if (num_measurements < len)
    {	  
      s = sin(val*M_PI/180.0);
      c = cos(val*M_PI/180.0);
      svals.push_back(s);
      cvals.push_back(c);
      avg_len = num_measurements + 1;
      num_measurements += 1;
    }
  else
    {
      s = sin(val*M_PI/180);
      c = cos(val*M_PI/180.0);
      svals.push_back(s);
      cvals.push_back(c);
      svals.pop_front();
      cvals.pop_front();
      avg_len = len;      
    }

  csum = 0.0;
  ssum = 0.0;

  for (int kk = 0; kk < avg_len; kk++)
    {   
      csum +=cvals[kk];
      ssum +=svals[kk];
    }
 
  avg = atan2(ssum,csum)*180.0/M_PI;

  //MOOSTrace("ssum = %f, csum = %f, average angle = %f\n",ssum,csum,avg);
}


//***************************************************************************
//*  Mavg::get_FVal()                                                       *
//*                                                                         *
//*  This method returns the value of the moving average                    *
//*                                                                         *
//*************************************************************************** 
double Mavg::get_FVal()  
{

  return (avg);
}
