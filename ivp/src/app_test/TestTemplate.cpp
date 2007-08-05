/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: TestTemplate.cpp                                     */
/*    DATE:                                                      */
/*****************************************************************/

#include "KObject.h"

int testTemplate(int argc, char *argv[])
{
  const int v = 7;

  KObject<v> kobject;
  kobject.print();
}
