// $Header: /home/cvsroot/project-marine-shell/src/pMineSim/CpMineSim.h,v 1.4 2007/06/18 18:14:32 ajshafer Exp $
// (c) 2007 Andrew Shafer <ajshafer@mit.edu> 
// CpMineSim.h: interface for the CpMineSim class.
////////////////////////////////////////////////

#ifndef __CpMineSim_h__
#define __CpMineSim_h__

#include "MOOSLib.h"

// Stuff for XYMineField class
#include <vector>

class XYMineField {
private:
  std::vector<double> m_pos_x;
  std::vector<double> m_pos_y;
  
public:
  bool addMine(double, double);
  int countMines();
  double distToMine(double, double);
  double distToMine(double, double, int);
  int closestMine(double, double);
  
  std::string mineBox(int);
  
  double getPosX(int);
  double getPosY(int);
  std::vector<int> getMines(double, double, double);
  std::string getField();
};
// End stuff for XYMineField class  
  
class CpMineSim : public CMOOSApp
{
public:
	CpMineSim();
	virtual ~CpMineSim();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// local vars go here
	double x, y; // current x, y
	static const double DetectionRadius = 10.0;
	std::vector<int> DetectedMines;  // MineField index of detected mines
	XYMineField MineField;
};

#endif /* __CpMineSim_h__ */
