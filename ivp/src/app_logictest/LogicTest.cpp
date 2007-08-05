/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: LogicTest.cpp                                        */
/*    DATE: October 14th 2006                                    */
/*****************************************************************/

#include <iostream>
#include "LogicTest.h"
#include "LogicUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: test_AAA()

void LogicTest::test_AAA()
{
  return;
  this->resetGroup("AAA");
  string s = "(B <= \"Apple\") and ((K != 99) or (J >= 0))";
  //string s = "(B <= \"Apple\") and (J >= 0)";

  ParseNode *parse_node = new ParseNode(s);
  parse_node->recursiveParse();
  syntacticTest(parse_node);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("J", "hello");
  parse_node->recursiveSetVarVal("K", 98);
  semanticTest(parse_node, true);

  cout << endl;
  exit(0);
}

//-----------------------------------------------------------------
// Procedure: test_A()

void LogicTest::test_A()
{
  this->resetGroup("A");
  string s = "(B < $(TMP))";

  ParseNode *parse_node = new ParseNode(s);
  parse_node->recursiveParse();
  syntacticTest(parse_node);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", 144);
  parse_node->recursiveSetVarVal("TMP", 500);
  semanticTest(parse_node, true);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", 600);
  parse_node->recursiveSetVarVal("TMP", 500);
  semanticTest(parse_node, false);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", 500);
  parse_node->recursiveSetVarVal("TMP", 500);
  semanticTest(parse_node, false);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", 144);
  semanticTest(parse_node, false);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("TMP", 144);
  semanticTest(parse_node, false);
  cout << endl;
}

//-----------------------------------------------------------------
// Procedure: test_B()

void LogicTest::test_B()
{
  this->resetGroup("B");
  string s = "(B < 23)";

  ParseNode *parse_node = new ParseNode(s);
  parse_node->recursiveParse();
  syntacticTest(parse_node);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", 144);
  semanticTest(parse_node, false);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", 6);
  semanticTest(parse_node, true);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", 23);
  semanticTest(parse_node, false);
  cout << endl;
}

//-----------------------------------------------------------------
// Procedure: test_C()

void LogicTest::test_C()
{
  this->resetGroup("C");
  string s = "(B != \"Apple\")";

  ParseNode *parse_node = new ParseNode(s);
  parse_node->recursiveParse();
  syntacticTest(parse_node);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", 144);
  semanticTest(parse_node, false);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Apple");
  semanticTest(parse_node, false);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "apple");
  semanticTest(parse_node, true);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  semanticTest(parse_node, false);
  cout << endl;
}

//-----------------------------------------------------------------
// Procedure: test_D()

void LogicTest::test_D()
{
  this->resetGroup("D");
  string s = "(B = \"Apple\")";

  ParseNode *parse_node = new ParseNode(s);
  parse_node->recursiveParse();
  syntacticTest(parse_node);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", 144);
  semanticTest(parse_node, false);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Apple");
  semanticTest(parse_node, true);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "apple");
  semanticTest(parse_node, false);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  semanticTest(parse_node, false);
  cout << endl;
}

//-----------------------------------------------------------------
// Procedure: test_E()

void LogicTest::test_E()
{
  this->resetGroup("E");
  string s = "(B <= \"Apple\")";

  ParseNode *parse_node = new ParseNode(s);
  parse_node->recursiveParse();
  syntacticTest(parse_node);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  semanticTest(parse_node, true);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Apple");
  semanticTest(parse_node, true);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "apple");
  semanticTest(parse_node, false);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Zappa");
  semanticTest(parse_node, false);

  cout << endl;
}

//-----------------------------------------------------------------
// Procedure: test_F()

void LogicTest::test_F()
{
  this->resetGroup("F");
  string s = "(B <= \"Apple\") and ((K != 99) or (J >= 0))";
  //string s = "(B <= \"Apple\") and (J >= 0)";

  ParseNode *parse_node = new ParseNode(s);
  parse_node->recursiveParse();
  syntacticTest(parse_node);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("K", 23);
  parse_node->recursiveSetVarVal("J", 23);
  semanticTest(parse_node, true);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("J", 23);
  semanticTest(parse_node, true);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  semanticTest(parse_node, false);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("J", "hello");
  semanticTest(parse_node, false);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("J", -44);
  semanticTest(parse_node, false);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("J", "hello");
  parse_node->recursiveSetVarVal("K", 98);
  semanticTest(parse_node, true);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("J", "hello");
  parse_node->recursiveSetVarVal("K", "98");
  semanticTest(parse_node, true);

  cout << endl;
}

//-----------------------------------------------------------------
// Procedure: test_G()

void LogicTest::test_G()
{
  this->resetGroup("G");
  string s = "!((B <= \"Apple\") and ((K != 99) or (J >= 0)))";

  ParseNode *parse_node = new ParseNode(s);
  parse_node->recursiveParse();
  syntacticTest(parse_node);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("K", 23);
  parse_node->recursiveSetVarVal("J", 23);
  semanticTest(parse_node, false);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("J", 23);
  semanticTest(parse_node, false);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  semanticTest(parse_node, true);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("J", "hello");
  semanticTest(parse_node, true);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("J", -44);
  semanticTest(parse_node, true);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("J", "hello");
  parse_node->recursiveSetVarVal("K", 98);
  semanticTest(parse_node, false);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", "Abc");
  parse_node->recursiveSetVarVal("J", "hello");
  parse_node->recursiveSetVarVal("K", "98");
  semanticTest(parse_node, false);

  cout << endl;
}

//-----------------------------------------------------------------
// Procedure: test_H()

void LogicTest::test_H()
{
  this->resetGroup("H");
  string s = "(B=2) or (C=3)";

  ParseNode *parse_node = new ParseNode(s);
  parse_node->recursiveParse();
  syntacticTest(parse_node);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("A", 1);
  parse_node->recursiveSetVarVal("B", 2);
  parse_node->recursiveSetVarVal("C", 3);
  semanticTest(parse_node, true);

}


//-----------------------------------------------------------------
// Procedure: test_Z()

void LogicTest::test_Z()
{
  this->resetGroup("Z");
  string s = "(A = 5) or (B < $(TMP))";
  
  ParseNode *parse_node = new ParseNode(s);
  parse_node->recursiveParse();
  syntacticTest(parse_node);

  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("A", 22);
  parse_node->recursiveSetVarVal("B", 144);
  parse_node->recursiveSetVarVal("TMP", 143);
  semanticTest(parse_node, false);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("A", 5);
  parse_node->recursiveSetVarVal("B", 144);
  parse_node->recursiveSetVarVal("TMP", 143);
  semanticTest(parse_node, true);
  // ------------------------------------------
  // TMP is undefined but that should be tolerated
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("A", 5);
  parse_node->recursiveSetVarVal("B", 144);
  semanticTest(parse_node, true);
  // ------------------------------------------
  // A is undefined but that should be tolerated
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("B", 144);
  parse_node->recursiveSetVarVal("TMP", 500);
  semanticTest(parse_node, true);
  // ------------------------------------------
  // B is undefined but that should be tolerated
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("A", 5);
  parse_node->recursiveSetVarVal("TMP", 500);
  semanticTest(parse_node, true);
  // ------------------------------------------
  parse_node->recursiveClearVarVal();
  parse_node->recursiveSetVarVal("A", 22);
  parse_node->recursiveSetVarVal("B", 142);
  parse_node->recursiveSetVarVal("TMP", 143);
  semanticTest(parse_node, true);
}

//-----------------------------------------------------------------
// Procedure: syntacticTest()

void LogicTest::syntacticTest(ParseNode* parse_node)
{
  bool ok_syntax =  parse_node->recursiveSyntaxCheck();
  if(!ok_syntax) {
    cout << "Test #" << m_group << m_index << ": ";
    cout << "  FAILED (syntax error) !!" << endl;
    parse_node->print();
    exit(0);
  }
}

//-----------------------------------------------------------------
// Procedure: semanticTest()

bool LogicTest::semanticTest(ParseNode* parse_node, 
			     bool expected_result)
{
  if(!parse_node)
    return(false);

  cout << "Test #" << m_group << m_index << ": ";
  m_index++;
  
  bool result = parse_node->recursiveEvaluate();
  if(result == expected_result) {
    cout << "PASSED" << endl;
  }
  else {
    cout << "FAILED    !!!!!!!!!" << endl;
    parse_node->print();
  }
  
 
}

