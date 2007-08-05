/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: TestRelAng.h                                         */
/*    DATE:                                                      */
/*****************************************************************/

#include <iostream>
#include <string.h>
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

int testRelAng(int argc, char *argv[])
{
  double cx = atof(argv[1]);
  double cy = atof(argv[2]);
  double dist = atof(argv[3]);
  double deg  = atof(argv[4]);

  double newx, newy;

  projectPoint(deg, dist, cx, cy, newx, newy);
  cout << "new_x: " << newx << "  new_y: " << newy << endl;
  exit(0);
  
  if((argc!=5) || (!strcmp(argv[1], "-h"))) {
    cout << "Usage: relang pt1x pt1y pt2x pt2y" << endl;
    cout << "                               " << endl;
    cout << "             0                 " << endl;
    cout << "             |                 " << endl;
    cout << "             |                 " << endl;
    cout << "   270 ----- A ----- 90        " << endl;
    cout << "             |                 " << endl;
    cout << "             |                 " << endl;
    cout << "            180                " << endl << endl;
    exit(0);
  }

  double pt1x = atof(argv[1]);
  double pt1y = atof(argv[2]);
  double pt2x = atof(argv[3]);
  double pt2y = atof(argv[4]);

  double result  = relAng(pt1x, pt1y, pt2x, pt2y);
  cout << "The angle from " << argv[1] << "," << argv[2];
  cout << "  to " << argv[3] << "," << argv[4];
  cout << "  is : " << result << endl;

  return(0);
}
