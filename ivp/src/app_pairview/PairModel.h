/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PairModel.h                                          */
/*    DATE: Jan 9th, 2025                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef PAIR_MODEL_HEADER
#define PAIR_MODEL_HEADER

#include <vector>

class PairModel
{
public:
  PairModel();
  ~PairModel() {};

public: // initialize
  bool   initFromFile(std::string);

  void   update();
  
public: // setters
  void   set_osx(double v)     {m_osx = v;};
  void   set_osy(double v)     {m_osy = v;};
  void   set_osv(double v)     {m_osv = v;};
  void   set_osh(double v)     {m_osh = v;};

  void   set_cnx(double v)     {m_cnx = v;};
  void   set_cny(double v)     {m_cny = v;};
  void   set_cnv(double v)     {m_cnv = v;};
  void   set_cnh(double v)     {m_cnh = v;};

  void   set_curr_time(double v) {m_curr_time=v; update();}  
  void   set_alert_range(double v)  {m_alert_range=v;};
  void   set_warning_time(double v) {m_warning_time=v;};
  
  void   copy(PairModel, std::string which="both");
  void   swap();
  
public: // getters
  double get_osx() const       {return(m_osx);}
  double get_osy() const       {return(m_osy);}
  double get_osv() const       {return(m_osv);}
  double get_osh() const       {return(m_osh);}

  double get_cnx() const       {return(m_cnx);}
  double get_cny() const       {return(m_cny);}
  double get_cnv() const       {return(m_cnv);}
  double get_cnh() const       {return(m_cnh);}

  double get_avgx() const      {return((m_cnx+m_osx)/2);}
  double get_avgy() const      {return((m_cny+m_osy)/2);}

  double get_alert_range() const {return(m_alert_range);}
  double get_warning_time() const {return(m_warning_time);}

  bool   get_alerted() const {return(m_alerted);}
  bool   get_warned() const {return(m_warned);}
  
  double get_alerted_time() const {return(m_alert_time);}
  double get_warned_time() const {return(m_warn_time);}
  
  double get_curr_time() const {return(m_curr_time);}
  double get_delt_time() const {return(m_delt_time);}
  
  double get_warning_range() const;
  
 public: // shifters
  void   shift_osx(double v)   {m_osx += v;};
  void   shift_osy(double v)   {m_osy += v;};
  void   shift_osv(double v)   {m_osv += v;};
  void   shift_osh(double v);

  void   shift_cnx(double v)   {m_cnx += v;};
  void   shift_cny(double v)   {m_cny += v;};
  void   shift_cnv(double v)   {m_cnv += v;};
  void   shift_cnh(double v);

  void   move_os(double seconds);
  void   move_cn(double seconds);
  void   alt_os_range(double meters);
  void   alt_os_bearing(double degrees);
  void   alt_os_bearing_holdrb(double degrees);
  
 public: // analyzers
  double rate_of_closure() const;
  double range_rate() const;
  double range() const;
  double get_cpa(double) const;
  
 private: // Member variables
  double  m_osx;
  double  m_osy;
  double  m_osv;
  double  m_osh;

  double  m_cnx;
  double  m_cny;
  double  m_cnv;
  double  m_cnh;

  double  m_alert_range;
  double  m_warning_time;

  bool    m_alerted;
  bool    m_warned;
  bool    m_alerted_prev;
  bool    m_warned_prev;

  double  m_alert_time;
  double  m_warn_time;

  double  m_curr_time;
  double  m_delt_time;
};

#endif 
