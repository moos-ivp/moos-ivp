/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: LogicTest.h                                          */
/*    DATE: October 14th 2006                                    */
/*****************************************************************/

#ifndef LOGIC_TEST_HEADER
#define LOGIC_TEST_HEADER

#include <string>
#include <vector>
#include "ParseNode.h"

class LogicTest {

public:
  LogicTest() {m_index=0;};
  virtual ~LogicTest() {};

  void resetGroup(std::string s) {m_group=s; m_index=0;};

  void test_AAA();
  void test_A();
  void test_B();
  void test_C();
  void test_D();
  void test_E();
  void test_F();
  void test_G();
  void test_H();
  void test_I() {};
  void test_Z();
  
  void syntacticTest(ParseNode*);
  bool semanticTest(ParseNode*, bool);

 protected:
  std::string  m_group;
  int          m_index;

};
#endif







