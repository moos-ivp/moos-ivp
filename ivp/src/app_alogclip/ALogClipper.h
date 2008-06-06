/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ALogClipper.h                                        */
/*    DATE: June 5th, 2008                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ALOG_CLIPPER_HEADER
#define ALOG_CLIPPER_HEADER

#include <string>

class ALogClipper
{
 public:
  ALogClipper();
  ~ALogClipper() {};
  
  bool         openALogFileRead(std::string filename);
  bool         openALogFileWrite(std::string filename);
  unsigned int clip(double mintime, double maxtime);

  unsigned int getDetails(const std::string& statevar);

 protected:
  std::string getNextLine();
  bool        writeNextLine(const std::string& output);

  unsigned int m_kept_chars;
  unsigned int m_clipped_chars_front;
  unsigned int m_clipped_chars_back;
  unsigned int m_kept_lines;
  unsigned int m_clipped_lines_front;
  unsigned int m_clipped_lines_back;

 private:
  FILE *m_infile;
  FILE *m_outfile;
};

#endif 


