#ifndef CURVEFITTING_HEADER
#define CURVEFITTING_HEADER
#include <vector>
#include "XYSegList.h"
#include "TurnPoly.h"
#include "TurnSequence.h"
#include "Seglr.h"

// least squares polynomial fitting of turn data
std::vector<double> leastSquares(XYSegList,const int=4);

std::vector<double> polyfit1(XYSegList);
std::vector<double> polyfit2(XYSegList);
std::vector<double> polyfit3(XYSegList);
std::vector<double> polyfit4(XYSegList);

std::vector<double> polyfit1(TurnSequence);
std::vector<double> polyfit2(TurnSequence);
std::vector<double> polyfit3(TurnSequence);
std::vector<double> polyfit4(TurnSequence);


// cleaning of turn data for better processing
TurnSequence removeLinearBeginning(TurnSequence, double& xstart, double& ystart, double heading_tolerance=2);
TurnSequence removeLinearTail(TurnSequence, double heading_tolerance=5);
bool truncateNFacingTurnSeqAtMaxAngle(TurnSequence&, double);

// simplification of piecewise linear approximations
XYSegList simplifyLinearRegions(XYSegList, double ang_tol=5);



//bool generateTurnPolyFromTurnSequence(TurnSequence, TurnPoly&, double rotation_angle=22.5);

// weighted interpolation of two turns
bool turnPolyInterpolator(TurnPoly, TurnPoly, double, double, TurnPoly&, std::string interp_type="coefficients");
double calcInterpolationWeight(TurnPoly, TurnPoly, double, double, double);

// finding helpful information about turns
double giveYforFirstXInstance(XYSegList, double, double precision = 0.1);


// helpful tools


// helpful output
std::string getMatlabVector(XYSegList);
#endif
