/***********************************************************/
/*    NAME: Michael Benjamin                               */
/*    FILE: TupleTools.h                                   */
/*    DATE: Sep 15 2005                                    */
/***********************************************************/

#ifndef TUPLE_TOOLS_HEADER
#define TUPLE_TOOLS_HEADER

#include <vector>
#include "Tuple3D.h"
#include "IvPBox.h"
#include "PDMap.h"
#include "IvPDomain.h"

//std::vector<Tuple3D> tupelize(PDMap*, IvPDomain, std::vector<double>);


std::vector<Tuple3D> tupelize(const IvPBox*);
std::vector<Tuple3D> tupelize(const IvPBox*, IvPDomain);

std::vector<Tuple3D> tupelize(PDMap*);
std::vector<Tuple3D> tupelize(PDMap*, IvPDomain);

#endif
