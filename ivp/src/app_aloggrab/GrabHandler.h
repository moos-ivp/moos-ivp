/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: GrabHandler.h                                        */
/*    DATE: August 6th, 2008                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef GRAB_HANDLER_HEADER
#define GRAB_HANDLER_HEADER


class GrabHandler
{
 public:
  GrabHandler() {m_file_in=0; m_file_out=0;};
  ~GrabHandler() {}

  void handle(const std::string&, const std::string& str="");
  void addKey(std::string str);
  
 protected:

  std::vector<std::string> m_keys;
  std::vector<bool>        m_pmatch;

  FILE *m_file_in;
  FILE *m_file_out;
};

#endif
