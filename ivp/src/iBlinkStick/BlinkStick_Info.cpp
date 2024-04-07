/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: MIT, Cambridge MA                                    */
/*   FILE: BlinkStick_Info.cpp                                  */
/*   DATE: April 7th, 2024                                      */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "BlinkStick_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The iBlinkStick app works with the BlinkStick LED device.     ");
  blk("  The color and state of the LED can be controlled by posting a ");
  blk("  message, parsed by iBlinkStick.                               ");
  blk("                                                                ");
  blk("  The device is controlled by a Python app, as well as a bash   ");
  blk("  script wrapper. This MOOS app interacts with the bash script  ");
  blk("  by making system calls to the bash script.                    ");
  blk("                                                                ");
  blk("  Requirements: BlinkStick device itself, the Python blinkstick ");
  blk("  package installed, and the qblink.sh bash script. The former  ");
  blk("  is provided by the company making the BlinkStick. The latter  ");
  blk("  is part of the pablo-common tree often on MIT RasPi computers ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: iBlinkStick file.moos [OPTIONS]                          ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch iBlinkStick with the given process name         ");
  blk("      rather than iBlinkStick.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of iBlinkStick.        ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("iBlinkStick Example MOOS Configuration                          ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = iBlinkStick                                     ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  qblink = QBLINK    // MOOS var accepting blink posts          ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blu("=============================================================== ");
  blu("iBlinkStick INTERFACE                                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  QBLINK = color=red, dim=50, time=10, delay=4                  ");
  blk("  QBLINK = color=pink, side0=false, side1=true                  ");
  blk("  QBLINK = brown                                                ");
  blk("  QBLINK = off                                                  ");
  blk("  QBLINK = blue, time=10                                        ");
  blk("                                                                ");
  blk("  colors: blue,green,red,yellow,white,purple,brown,pink,        ");
  blk("          orange,cyan,random,off                                ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  None                                                          ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("iBlinkStick", "gpl");
  exit(0);
}

