/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI - MIT Dept. Mechanical Engineering */
/*    FILE: BHV_ConstantDepth.h                                  */
/*    DATE: Jul 3rd 2005                                         */
/*                                                               */
/* IvPHelm - Interval Programming Autonomous Control Helm        */
/*                                                               */
/* A set of libraries and applications for a behavior-based      */
/* autonomous control architecture with IvP action selection.    */
/*                                                               */
/* Patent Pending under U.S. Patent Application                  */
/* Ser. Nos. 10/631,527 and 10/911,765.                          */
/*                                                               */
/* Permission to use, copy, modify, and distribute this          */
/* software is CURRENTLY NOT PERMITTED, except by the            */
/* author(s).  We intend to make this software available         */
/* under a BSD style license by the end of 2005, at which        */
/* time permission to copy, modify and distribute will be        */
/* granted for non-commercial purposes.                          */
/*                                                               */
/* IN NO EVENT SHALL THE U.S.GOVERNMENT BE LIABLE TO ANY PARTY   */
/* FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL   */
/* DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF    */
/* THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE U.S.         */
/* GOVERNMENT HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH        */
/* DAMAGE.                                                       */
/*                                                               */
/* THE U.S. GOVERNMENT SPECIFICALLY DISCLAIMS ANY WARRANTIES,    */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF      */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE     */
/* U.S. GOVERNMENT HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE,    */
/* SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.             */
/*****************************************************************/
 
#ifndef BHV_ADAPTIVE_YOYO_HEADER
#define BHV_ADAPTIVE_YOYO_HEADER

#include "IvPBehavior.h"

class BHV_Adaptive_YoYo : public IvPBehavior {
public:
  BHV_Adaptive_YoYo(IvPDomain);
  ~BHV_Adaptive_YoYo() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  double desired_depth;
};
#endif


