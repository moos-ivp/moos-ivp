#include "ACTable.h"
#include "XYPoint.h"

int main()
{
  ACTable table(1);
  table.addCell("installed IVP");
  XYPoint point(1.0, 2.0);
  return (table.getTableOutput().empty() || !point.valid()) ? 1 : 0;
}
