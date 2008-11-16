/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FiltHandler.h                                        */
/*    DATE: August 6th, 2008                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef FILT_HANDLER_HEADER
#define FILT_HANDLER_HEADER


class FiltHandler
{
 public:
  FiltHandler();
  ~FiltHandler() {};

  void handle(const std::string&, const std::string& str="");
  bool setParam(const std::string& param, const std::string& value);

 protected:

  std::vector<std::string> m_keys;
  std::vector<bool>        m_pmatch;

  bool  m_chuck_strings;
  bool  m_chuck_numbers;
  bool  m_timeshift;

  FILE *m_file_in;
  FILE *m_file_out;

  double m_logstart;
};

#endif
