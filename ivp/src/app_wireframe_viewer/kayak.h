/*****************************************************************/
/*    NAME: Doug Maxwell and Michael Benjamin                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: kayak.h                                              */
/*    DATE: December 2007                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef KAYAK_HEADER
#define KAYAK_HEADER

class Kayak {
  public:
	Kayak(
		float = 0.0, float = 0.0, float = 0.0, 
		float = 0.0, float = 0.0, float = 0.0, float = 0.0);
	void setKayak(float, float, float, float, float, float, float);
	void drawKayak(float, float, float, float, float, float, float);

	float kx, ky, kz, kred, kblue, kgreen, kalpha;
};

#endif
