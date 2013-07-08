/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PairModel.h                                          */
/*    DATE: May 21, 2013                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef PAIR_MODEL_HEADER
#define PAIR_MODEL_HEADER

class PairModel
{
 public:
  PairModel();
  ~PairModel() {};

 public: // initialize
  bool   initFromFile(std::string);

 public: // setters
  void   set_osx(double v)     {m_osx = v;};
  void   set_osy(double v)     {m_osy = v;};
  void   set_osv(double v)     {m_osv = v;};
  void   set_osh(double v)     {m_osh = v;};

  void   set_cnx(double v)     {m_cnx = v;};
  void   set_cny(double v)     {m_cny = v;};
  void   set_cnv(double v)     {m_cnv = v;};
  void   set_cnh(double v)     {m_cnh = v;};

  void   set_pwt_inner_dist(double v) {m_pwt_inner_dist=v;};
  void   set_pwt_outer_dist(double v) {m_pwt_outer_dist=v;};

 public: // getters
  double get_osx() const       {return(m_osx);};
  double get_osy() const       {return(m_osy);};
  double get_osv() const       {return(m_osv);};
  double get_osh() const       {return(m_osh);};

  double get_cnx() const       {return(m_cnx);};
  double get_cny() const       {return(m_cny);};
  double get_cnv() const       {return(m_cnv);};
  double get_cnh() const       {return(m_cnh);};

  double get_avgx() const      {return((m_cnx+m_osx)/2);};
  double get_avgy() const      {return((m_cny+m_osy)/2);};

  double get_pwt_inner_dist()  {return(m_pwt_inner_dist);};
  double get_pwt_outer_dist()  {return(m_pwt_outer_dist);};

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
  double range() const;
  double os_cn_bearing() const;
  double cn_os_bearing() const;
  double os_cn_relbearing() const;
  double cn_os_relbearing() const;
  double os_cn_absrelbearing() const;
  double cn_os_absrelbearing() const;
  double total_absrelbearing() const;
  double get_cpa(double tol=3600) const;
  bool   os_crosses_cn_bow()    const;
  bool   os_crosses_cn_stern()  const;
  bool   os_crosses_cn_either() const;
  double os_crosses_cn_bow_dist() const;
  double os_crosses_cn_stern_dist() const;

  double os_spd_in_cn_hdg() const;

  bool   os_fore_of_cn() const;
  bool   os_aft_of_cn()  const;
  bool   os_port_of_cn() const;
  bool   os_starboard_of_cn() const;
  bool   os_passes_cn()  const;
  bool   os_passes_cn_port() const;
  bool   os_passes_cn_starboard() const;
  
 private: // Member variables
  double  m_osx;
  double  m_osy;
  double  m_osv;
  double  m_osh;

  double  m_cnx;
  double  m_cny;
  double  m_cnv;
  double  m_cnh;

  double  m_pwt_inner_dist;
  double  m_pwt_outer_dist;
};

#endif 
