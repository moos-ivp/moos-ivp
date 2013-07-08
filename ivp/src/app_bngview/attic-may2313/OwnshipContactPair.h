/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: OwnshipContactPair.h                                 */
/*    DATE: May 21, 2013                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef OWNSHIP_CONTACT_PAIR_HEADER
#define OWNSHIP_CONTACT_PAIR_HEADER

class OwnshipContactPair
{
 public:
  OwnshipContactPair();
  ~OwnshipContactPair() {};

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

 public: // shifters
  void   shift_osx(double v)   {m_osx += v;};
  void   shift_osy(double v)   {m_osy += v;};
  void   shift_osv(double v)   {m_osv += v;};
  void   shift_osh(double v)   {m_osh += v;};

  void   shift_cnx(double v)   {m_cnx += v;};
  void   shift_cny(double v)   {m_cny += v;};
  void   shift_cnv(double v)   {m_cnv += v;};
  void   shift_cnh(double v)   {m_cnh += v;};

 public: // analyzers
  double range() const;
  double os_cn_relbearing() const;
  double cn_os_relbearing() const;
  double os_cn_absrelbearing() const;
  double cn_os_absrelbearing() const;
  double total_absrelbearing() const;
  
 private: // Member variables
  double  m_osx;
  double  m_osy;
  double  m_osv;
  double  m_osh;

  double  m_cnx;
  double  m_cny;
  double  m_cnv;
  double  m_cnh;
};

#endif 
