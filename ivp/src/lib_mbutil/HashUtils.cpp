/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: HashUtils.cpp                                        */
/*    DATE: Nov 2nd, 2022                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/


#include <iostream>
#include <sys/time.h>
#include <ctime>
#include "MBUtils.h"
#include "HashUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: missionHash()
//   Example: 221103K-FOXY-WEED


string missionHash()
{
  string hash = getCurrYear();
  hash += getCurrMonth();
  hash += getCurrDay();
  hash += "-";
  hash += getCurrHour();
  hash += getCurrMinute();
  hash += hashAlphaUpper(1);
  hash += "-";
  hash += toupper(randomWord(adjectives4()));
  hash += "-";

  vector<string> svector1 = nouns4();
  vector<string> svector2 = names4();
  vector<string> svector = mergeVectors(svector1, svector2);
  hash += toupper(randomWord(svector));

  return(hash);
}

//---------------------------------------------------------
// Procedure: missionHashShort(string)
//   Example: 221103-0834K-FOXY-WEED  ==> FOXY-WEED
//   Example: mhash=240121-1540N-LIVE-GULL,utc=170528.23 ==> LIVE-GULL 

string missionHashShort(string full)
{
  string mhash = full;
  if(strContains(full, "mhash="))
    mhash = tokStringParse(full, "mhash", ',', '=');
  
  biteString(mhash, '-');
  biteString(mhash, '-');
  
  return(mhash);
}


//---------------------------------------------------------
// Procedure: missionHashLShort(string)
//   Example: 221103-0834K-FOXY-WEED  ==> K-FOXY-WEED
//   Example: mhash=240121-1540N-LIVE-GULL,utc=170528.23 ==> N-LIVE-GULL 

string missionHashLShort(string full)
{
  string mhash = full;
  if(strContains(full, "mhash="))
    mhash = tokStringParse(full, "mhash", ',', '=');

  string lchar;
  if(mhash.length() > 12)
    lchar = mhash.at(11);
  
  biteString(mhash, '-');
  biteString(mhash, '-');

  mhash = lchar + "-" + mhash;
  
  return(mhash);
}


//---------------------------------------------------------
// Procedure: missionHashUTC(string)
//   Example: mhash=250121-1122S-GLAD-ARLO,utc=1737476534.76 ==> 1737476534.76

double missionHashUTC(string full)
{
  string utc_str = tokStringParse(full, "utc", ',', '=');
  double utc_dbl = atof(utc_str.c_str());

  return(utc_dbl);
}


//---------------------------------------------------------
// Procedure: zHash()
//   Example: 241103-0834-SANIBEL-88

string zHash()
{
  string zhash = getCurrYear();
  zhash += getCurrMonth();
  zhash += getCurrDay();
  zhash += "-";
  zhash += getCurrHour();
  zhash += getCurrMinute();
  zhash += "-";
  zhash += toupper(randomWord(us_cities7()));
  zhash += "-";

  string two_digits_00_99 = intToString(rand() % 100);
  if(two_digits_00_99.length() == 1)
    two_digits_00_99 = "0" + two_digits_00_99;
  zhash += two_digits_00_99;
  
  return(zhash);
}

//---------------------------------------------------------
// Procedure: zHashShort()
//   Example: 241103-0834-SANIBEL-88  --> SANIBEL-88

string zHash(string full)
{
  string two_digits_00_99 = rbiteString(full, '-');
  string city = rbiteString(full, '-');
  
  if((two_digits_00_99 == "") || (city == ""))
    return("");
      
  string zhash_short = city + "-" + two_digits_00_99;
  
  return(zhash_short);
}


//---------------------------------------------------------
// Procedure: hashAlphaNum()

string hashAlphaNum(unsigned int len)
{
  string pchars="0123456789";
  pchars += "abcdefghijklmnpqrstuvwxyz";
  pchars += "ABCDEFGHIJKLMNPQRSTUVWXYZ";

  unsigned int pamt = pchars.size();

  string hash_str;
  
  for(unsigned int i=0; i<len; i++) {
    int randix = rand() % pamt;
    hash_str += pchars.at(randix);
  }

  return(hash_str);
}

//---------------------------------------------------------
// Procedure: hashAlphaUpper()

string hashAlphaUpper(unsigned int len)
{
  string pchars="ABCDEFGHIJKLMNPQRSTUVWXYZ";

  unsigned int pamt = pchars.size();

  string hash_str;
  
  for(unsigned int i=0; i<len; i++) {
    int randix = rand() % pamt;
    hash_str += pchars.at(randix);
  }

  return(hash_str);
}


//---------------------------------------------------------
// Procedure: randomWord()

string randomWord(const vector<string>& words)
{
  if(words.size() == 0)
    return("NULL");

  unsigned int pamt = words.size();
  int randix = rand() % pamt;
  string hash_str = words.at(randix);

  return(hash_str);
}

//---------------------------------------------------------
// Procedure: getCurrYear()

string getCurrYear(bool full)
{
  time_t ttime = time(0);
  tm *local_time = localtime(&ttime);

  int year = 1900 + local_time->tm_year;
  if(!full)
    year = year % 1000;

  string year_str = intToString(year);
  if(!full && (year_str.length() == 1))
    year_str = "0" + year_str;
      
  return(year_str);
}

//---------------------------------------------------------
// Procedure: getCurrMonth()

string getCurrMonth()
{
  time_t ttime = time(0);
  tm *local_time = localtime(&ttime);

  int month = 1 + local_time->tm_mon;

  string month_str = intToString(month);
  if(month_str.length() == 1)
    month_str = "0" + month_str;
      
  return(month_str);
}

//---------------------------------------------------------
// Procedure: getCurrDay()

string getCurrDay()
{
  time_t ttime = time(0);
  tm *local_time = localtime(&ttime);

  int day = local_time->tm_mday;

  string day_str = intToString(day);
  if(day_str.length() == 1)
    day_str = "0" + day_str;
      
  return(day_str);
}

//---------------------------------------------------------
// Procedure: getCurrHour()

string getCurrHour()
{
  time_t ttime = time(0);
  tm *local_time = localtime(&ttime);

  int hour = local_time->tm_hour;

  string hour_str = intToString(hour);
  if(hour_str.length() == 1)
    hour_str = "0" + hour_str;
      
  return(hour_str);
}

//---------------------------------------------------------
// Procedure: getCurrMinute()

string getCurrMinute()
{
  time_t ttime = time(0);
  tm *local_time = localtime(&ttime);

  int minute = local_time->tm_min;

  string minute_str = intToString(minute);
  if(minute_str.length() == 1)
    minute_str = "0" + minute_str;
      
  return(minute_str);
}


//---------------------------------------------------------
// Procedure: getCurrSeconds()

string getCurrSeconds()
{
  time_t ttime = time(0);
  tm *local_time = localtime(&ttime);

  int seconds = local_time->tm_sec;

  string seconds_str = intToString(seconds);
  if(seconds_str.length() == 1)
    seconds_str = "0" + seconds_str;
      
  return(seconds_str);
}


//---------------------------------------------------------
// Procedure: getCurrSecsUTC()

string getCurrSecsUTC()
{
  time_t ttime = time(0);
  tm *local_time = localtime(&ttime);

  long utc_secs = local_time->tm_gmtoff;

  string utc_str = doubleToString(utc_secs,3);

  return(utc_str);
}


//---------------------------------------------------------
// Procedure: getCurrTimeUTC()

double getCurrTimeUTC()
{
  double dfT = 0.0;
  struct timeval TimeVal;
  
  if(gettimeofday(&TimeVal, NULL)==0) {
    dfT = TimeVal.tv_sec+TimeVal.tv_usec/1000000.0;
  }
  return(dfT);
}


//---------------------------------------------------------
// Procedure: getCurrTime()

string getCurrTime()
{
  string time = getCurrHour();
  time += ":" + getCurrMinute();
  time += ":" + getCurrSeconds();

  return(time);
}

//---------------------------------------------------------
// Procedure: getCurrDate()

string getCurrDate()
{
  string date = getCurrYear(true);
  date += ":" + getCurrMonth();
  date += ":" + getCurrDay();

  return(date);
}



//----------------------------------------------------------------
// Procedure: adjectives4()

vector<string> adjectives4()
{
  vector<string> adj;

  adj.push_back("Able");  adj.push_back("Avid");
  adj.push_back("Back");  adj.push_back("Bald");
  adj.push_back("Bare");  adj.push_back("Bass");
  adj.push_back("Blue");  adj.push_back("Bold");
  adj.push_back("Bony");  adj.push_back("Busy");
  adj.push_back("Calm");  adj.push_back("Cagy");
  adj.push_back("Chic");  adj.push_back("Cold");
  adj.push_back("Cool");  adj.push_back("Cozy");
  adj.push_back("Curt");  adj.push_back("Cute");
  adj.push_back("Damp");  adj.push_back("Daft");
  adj.push_back("Dark");  adj.push_back("Darn");
  adj.push_back("Dead");  adj.push_back("Deaf");
  adj.push_back("Dear");  adj.push_back("Deep");
  adj.push_back("Drab");  adj.push_back("Dual");
  adj.push_back("Dull");  adj.push_back("Dumb");
  adj.push_back("Easy");  adj.push_back("Edgy");
  adj.push_back("Epic");  adj.push_back("Evil");
  adj.push_back("Fair");  adj.push_back("Fast");
  adj.push_back("Fine");  adj.push_back("Firm");
  adj.push_back("Flat");  adj.push_back("Fond");
  adj.push_back("Foul");  adj.push_back("Foxy");
  adj.push_back("Free");  adj.push_back("Full");
  adj.push_back("Glad");  adj.push_back("Glib");
  adj.push_back("Good");  adj.push_back("Gold");
  adj.push_back("Grey");  adj.push_back("Grim");
  adj.push_back("Half");  adj.push_back("Hard");
  adj.push_back("Head");  adj.push_back("High");
  adj.push_back("Holy");  adj.push_back("Huge");
  adj.push_back("Idle");  adj.push_back("Inky");
  adj.push_back("Just");  adj.push_back("Keen");
  adj.push_back("Kind");  adj.push_back("Last");
  adj.push_back("Late");  adj.push_back("Lazy");
  adj.push_back("Like");  adj.push_back("Limp");
  adj.push_back("Lite");  adj.push_back("Live");
  adj.push_back("Lone");  adj.push_back("Long");
  adj.push_back("Loud");  adj.push_back("Lush");
  adj.push_back("Main");  adj.push_back("Male");
  adj.push_back("Mass");  adj.push_back("Mean");
  adj.push_back("Mega");  adj.push_back("Mere");
  adj.push_back("Meek");  adj.push_back("Mild");
  adj.push_back("Nude");  adj.push_back("Numb");
  adj.push_back("Near");  adj.push_back("Neat");
  adj.push_back("Next");  adj.push_back("Nice");
  adj.push_back("Oily");  adj.push_back("Okay");
  adj.push_back("Only");  adj.push_back("Oozy");
  adj.push_back("Open");  adj.push_back("Oval");
  adj.push_back("Pale");  adj.push_back("Past");
  adj.push_back("Pink");  adj.push_back("Prim");
  adj.push_back("Poor");  adj.push_back("Posh");
  adj.push_back("Pure");  adj.push_back("Rare");
  adj.push_back("Real");  adj.push_back("Rear");
  adj.push_back("Rich");  adj.push_back("Ripe");
  adj.push_back("Rude");  adj.push_back("Ruby");
  adj.push_back("Safe");  adj.push_back("Same");
  adj.push_back("Sexy");  adj.push_back("Sick");
  adj.push_back("Slim");  adj.push_back("Slow");
  adj.push_back("Soft");  adj.push_back("Sole");
  adj.push_back("Sore");  adj.push_back("Sour");
  adj.push_back("Sure");  adj.push_back("Tall");
  adj.push_back("Tame");  adj.push_back("Then");
  adj.push_back("Thin");  adj.push_back("Tidy");
  adj.push_back("Tiny");  adj.push_back("Tory");
  adj.push_back("True");  adj.push_back("Ugly");
  adj.push_back("Vain");  adj.push_back("Vast");
  adj.push_back("Very");  adj.push_back("Vice");
  adj.push_back("Vile");  adj.push_back("Warm");
  adj.push_back("Wary");  adj.push_back("Wavy");
  adj.push_back("Waxy");  adj.push_back("Weak");
  adj.push_back("Wide");  adj.push_back("Wild");
  adj.push_back("Wise");  adj.push_back("Zany");
  adj.push_back("Zero");

  adj.push_back("Jims");
  adj.push_back("Bobs");
  adj.push_back("Hals");
  adj.push_back("Neds");
  adj.push_back("Teds");
  adj.push_back("Bens");
  adj.push_back("Gamy");
  adj.push_back("Misc");
  adj.push_back("Plus");
  adj.push_back("Puny");
  adj.push_back("Malt");
  adj.push_back("Lewd");
  adj.push_back("Iron");
  adj.push_back("Iced");
  adj.push_back("Hind");
  adj.push_back("Dank");
  adj.push_back("Gory");
  adj.push_back("Glum");
  adj.push_back("Hazy");
  adj.push_back("Four");
  adj.push_back("Five");
  adj.push_back("Nine");
  return(adj);
}



//----------------------------------------------------------------
// Procedure: nouns4()

vector<string> nouns4()
{
  vector<string> nouns;

  nouns.push_back("Ache");  nouns.push_back("Area");
  nouns.push_back("Army");  nouns.push_back("Axis");
  nouns.push_back("Baby");  nouns.push_back("Back");
  nouns.push_back("Ball");  nouns.push_back("Band");
  nouns.push_back("Bang");  nouns.push_back("Bank");
  nouns.push_back("Base");  nouns.push_back("Beak");
  nouns.push_back("Beer");  nouns.push_back("Bean");
  nouns.push_back("Bear");  nouns.push_back("Blot");
  nouns.push_back("Bill");  nouns.push_back("Body");
  nouns.push_back("Boar");  nouns.push_back("Book");
  nouns.push_back("Boon");  nouns.push_back("Bunk");
  nouns.push_back("Bull");  nouns.push_back("Brew");
  nouns.push_back("Butt");  nouns.push_back("Cake");
  nouns.push_back("Call");  nouns.push_back("Calf");
  nouns.push_back("Card");  nouns.push_back("Carp");
  nouns.push_back("Chip");  nouns.push_back("Crab");
  nouns.push_back("Crib");  nouns.push_back("Clam");
  nouns.push_back("Care");  nouns.push_back("Case");
  nouns.push_back("Cash");  nouns.push_back("City");
  nouns.push_back("Claw");  nouns.push_back("Clip");
  nouns.push_back("Club");  nouns.push_back("Coil");
  nouns.push_back("Code");  nouns.push_back("Corn");
  nouns.push_back("Core");  nouns.push_back("Colt");
  nouns.push_back("Cost");  nouns.push_back("Comb");
  nouns.push_back("Dart");  nouns.push_back("Date");
  nouns.push_back("Data");  nouns.push_back("Deal");
  nouns.push_back("Dear");  nouns.push_back("Dime");
  nouns.push_back("Dish");  nouns.push_back("Door");
  nouns.push_back("Dose");  nouns.push_back("Dope");
  nouns.push_back("Duet");  nouns.push_back("Duct");
  nouns.push_back("Drip");  nouns.push_back("Duty");
  nouns.push_back("East");  nouns.push_back("Edge");
  nouns.push_back("Ebay");  nouns.push_back("Eddy");
  nouns.push_back("Face");  nouns.push_back("Fawn");
  nouns.push_back("Fact");  nouns.push_back("Farm");
  nouns.push_back("Fear");  nouns.push_back("File");
  nouns.push_back("Flan");  nouns.push_back("Film");
  nouns.push_back("Fire");  nouns.push_back("Firm");
  nouns.push_back("Fish");  nouns.push_back("Flop");
  nouns.push_back("Font");  nouns.push_back("Food");
  nouns.push_back("Fool");  nouns.push_back("Foal");
  nouns.push_back("Foot");  nouns.push_back("Form");
  nouns.push_back("Fund");  nouns.push_back("Game");
  nouns.push_back("Girl");  nouns.push_back("Goal");
  nouns.push_back("Glee");  nouns.push_back("Goto");
  nouns.push_back("Goat");  nouns.push_back("Goby");
  nouns.push_back("Gold");  nouns.push_back("Gulp");
  nouns.push_back("Grub");  nouns.push_back("Guru");
  nouns.push_back("Gull");  nouns.push_back("Grog");
  nouns.push_back("Gate");  nouns.push_back("Hack");
  nouns.push_back("Hair");  nouns.push_back("Half");
  nouns.push_back("Harp");  nouns.push_back("Hall");
  nouns.push_back("Halo");  nouns.push_back("Hand");
  nouns.push_back("Head");  nouns.push_back("Heap");
  nouns.push_back("Hemp");  nouns.push_back("Helm");
  nouns.push_back("Help");  nouns.push_back("Hill");
  nouns.push_back("Hoax");  nouns.push_back("Home");
  nouns.push_back("Hope");  nouns.push_back("Hose");
  nouns.push_back("Host");  nouns.push_back("Hour");
  nouns.push_back("Hymm");  nouns.push_back("Idea");
  nouns.push_back("Idol");  nouns.push_back("Ipod");
  nouns.push_back("Imac");  nouns.push_back("Isle");
  nouns.push_back("Java");  nouns.push_back("Jolt");
  nouns.push_back("Joey");  nouns.push_back("Jpeg");
  nouns.push_back("Judo");  nouns.push_back("Kind");
  nouns.push_back("Kiwi");  nouns.push_back("Kale");
  nouns.push_back("King");  nouns.push_back("Knob");
  nouns.push_back("Lack");  nouns.push_back("Lady");
  nouns.push_back("Land");  nouns.push_back("Left");
  nouns.push_back("Life");  nouns.push_back("Lion");
  nouns.push_back("Lime");  nouns.push_back("Line");
  nouns.push_back("Link");  nouns.push_back("List");
  nouns.push_back("Lisp");  nouns.push_back("Look");
  nouns.push_back("Loot");  nouns.push_back("Lock");
  nouns.push_back("Logo");  nouns.push_back("Loop");
  nouns.push_back("Lord");  nouns.push_back("Loss");
  nouns.push_back("Love");  nouns.push_back("Mail");
  nouns.push_back("Mark");  nouns.push_back("Mask");
  nouns.push_back("Mass");  nouns.push_back("Menu");
  nouns.push_back("Mime");  nouns.push_back("Mini");
  nouns.push_back("Mako");  nouns.push_back("Mall");
  nouns.push_back("Mary");  nouns.push_back("Memo");
  nouns.push_back("Menu");  nouns.push_back("Moth");
  nouns.push_back("Mule");  nouns.push_back("Mind");
  nouns.push_back("Milk");  nouns.push_back("Mint");
  nouns.push_back("Mold");  nouns.push_back("Mink");
  nouns.push_back("Miss");  nouns.push_back("Mood");
  nouns.push_back("Move");  nouns.push_back("Mole");
  nouns.push_back("Name");  nouns.push_back("Need");
  nouns.push_back("News");  nouns.push_back("Node");
  nouns.push_back("Note");  nouns.push_back("Nova");
  nouns.push_back("Null");  nouns.push_back("Oval");
  nouns.push_back("Orca");  nouns.push_back("Page");
  nouns.push_back("Pain");  nouns.push_back("Pair");
  nouns.push_back("Park");  nouns.push_back("Part");
  nouns.push_back("Past");  nouns.push_back("Path");
  nouns.push_back("Paul");  nouns.push_back("Peso");
  nouns.push_back("Pike");  nouns.push_back("Plan");
  nouns.push_back("Play");  nouns.push_back("Plum");
  nouns.push_back("Ploy");  nouns.push_back("Prop");
  nouns.push_back("Pony");  nouns.push_back("Prom");
  nouns.push_back("Post");  nouns.push_back("Port");
  nouns.push_back("Pool");  nouns.push_back("Prey");
  nouns.push_back("Pump");  nouns.push_back("Race");
  nouns.push_back("Reno");  nouns.push_back("Rome");
  nouns.push_back("Rain");  nouns.push_back("Raft");
  nouns.push_back("Rake");  nouns.push_back("Rank");
  nouns.push_back("Rest");  nouns.push_back("Rice");
  nouns.push_back("Rise");  nouns.push_back("Risk");
  nouns.push_back("Ritz");  nouns.push_back("Road");
  nouns.push_back("Rock");  nouns.push_back("Rust");
  nouns.push_back("Role");  nouns.push_back("Room");
  nouns.push_back("Root");  nouns.push_back("Rule");
  nouns.push_back("Rump");  nouns.push_back("Sale");
  nouns.push_back("Scan");  nouns.push_back("Seat");
  nouns.push_back("Seal");  nouns.push_back("Seam");
  nouns.push_back("Sham");  nouns.push_back("Shin");
  nouns.push_back("Shop");  nouns.push_back("Show");
  nouns.push_back("Side");  nouns.push_back("Sign");
  nouns.push_back("Site");  nouns.push_back("Size");
  nouns.push_back("Skin");  nouns.push_back("Slum");
  nouns.push_back("Slug");  nouns.push_back("Snag");
  nouns.push_back("Stud");  nouns.push_back("Sort");
  nouns.push_back("Soda");  nouns.push_back("Sole");
  nouns.push_back("Solo");  nouns.push_back("Soup");
  nouns.push_back("Song");  nouns.push_back("Spam");
  nouns.push_back("Spat");  nouns.push_back("Star");
  nouns.push_back("Stop");  nouns.push_back("Step");
  nouns.push_back("Task");  nouns.push_back("Tack");
  nouns.push_back("Tape");  nouns.push_back("Tart");
  nouns.push_back("Team");  nouns.push_back("Tech");
  nouns.push_back("Term");  nouns.push_back("Test");
  nouns.push_back("Text");  nouns.push_back("Time");
  nouns.push_back("Toad");  nouns.push_back("Tour");
  nouns.push_back("Town");  nouns.push_back("Tone");
  nouns.push_back("Tree");  nouns.push_back("Tray");
  nouns.push_back("Turn");  nouns.push_back("Tuna");
  nouns.push_back("Tubb");  nouns.push_back("Tent");
  nouns.push_back("Tusk");  nouns.push_back("Twig");
  nouns.push_back("Twin");  nouns.push_back("Twit");
  nouns.push_back("Type");  nouns.push_back("Unit");
  nouns.push_back("User");  nouns.push_back("Vice");
  nouns.push_back("View");  nouns.push_back("Vent");
  nouns.push_back("Wall");  nouns.push_back("Wand");
  nouns.push_back("Warp");  nouns.push_back("Wasp");
  nouns.push_back("Week");  nouns.push_back("Weed");
  nouns.push_back("West");  nouns.push_back("Wick");
  nouns.push_back("Whim");  nouns.push_back("Wife");
  nouns.push_back("Will");  nouns.push_back("Wiki");
  nouns.push_back("Wimp");  nouns.push_back("Wind");
  nouns.push_back("Wine");  nouns.push_back("Wood");
  nouns.push_back("Wolf");  nouns.push_back("Word");
  nouns.push_back("Work");  nouns.push_back("Year");
  nouns.push_back("Zest");  nouns.push_back("Zeta");

  nouns.push_back("Norm");
  nouns.push_back("NoNo");
  nouns.push_back("Legs");
  nouns.push_back("Eggs");
  nouns.push_back("Jugs");
  nouns.push_back("Jaws");
  nouns.push_back("Joys");
  nouns.push_back("Days");
  nouns.push_back("Ways");
  nouns.push_back("Jets");
  nouns.push_back("Beds");
  nouns.push_back("Bags");
  nouns.push_back("Ants");
  nouns.push_back("Orks");
  nouns.push_back("Paws");
  nouns.push_back("Pics");
  nouns.push_back("Rugs");
  nouns.push_back("Umps");
  nouns.push_back("Odds");
  nouns.push_back("Pose");
  nouns.push_back("Perk");
  nouns.push_back("Puff");
  nouns.push_back("Quad");
  nouns.push_back("Shag");
  nouns.push_back("Hash");
  nouns.push_back("Keep");
  nouns.push_back("Tick");
  nouns.push_back("Wins");
  nouns.push_back("Webs");
  nouns.push_back("Rows");
  nouns.push_back("Ribs");
  nouns.push_back("Rats");
  nouns.push_back("Figs");
  nouns.push_back("Guts");
  nouns.push_back("Elms");
  nouns.push_back("Dogs");
  nouns.push_back("Duds");
  nouns.push_back("Bars");
  nouns.push_back("Bees");
  nouns.push_back("Zits");
  nouns.push_back("Zigs");
  nouns.push_back("Plug");
  nouns.push_back("Info");
  nouns.push_back("Moos");
  nouns.push_back("Ward");
  nouns.push_back("Void");
  nouns.push_back("Tail");
  nouns.push_back("Trig");
  nouns.push_back("Slab");
  nouns.push_back("Skew");
  nouns.push_back("Salt");
  nouns.push_back("Shot");
  nouns.push_back("Addr");

  return(nouns);
}
  
//----------------------------------------------------------------
// Procedure: names4()

vector<string> names4()
{
  vector<string> names;

  names.push_back("Abel");  names.push_back("Adam");
  names.push_back("Adan");  names.push_back("Addy");
  names.push_back("Aden");  names.push_back("Ajax");
  names.push_back("Alan");  names.push_back("Aldo");
  names.push_back("Alec");  names.push_back("Alex");
  names.push_back("Alto");  names.push_back("Alva");
  names.push_back("Ames");  names.push_back("Amir");
  names.push_back("Amos");  names.push_back("Andy");
  names.push_back("Ares");  names.push_back("Arlo");
  names.push_back("Aven");  names.push_back("Axel");
  names.push_back("Ayan");  names.push_back("Aziz");
  names.push_back("Bart");  names.push_back("Beau");
  names.push_back("Beck");  names.push_back("Bert");
  names.push_back("Beto");  names.push_back("Bill");
  names.push_back("Bing");  names.push_back("Bird");
  names.push_back("Blue");  names.push_back("Bolt");
  names.push_back("Bond");  names.push_back("Bose");
  names.push_back("Boss");  names.push_back("Boyd");
  names.push_back("Brad");  names.push_back("Bram");
  names.push_back("Bran");  names.push_back("Bret");
  names.push_back("Brio");  names.push_back("Buck");
  names.push_back("Budd");  names.push_back("Burk");
  names.push_back("Burl");  names.push_back("Burr");
  names.push_back("Burt");  names.push_back("Bush");
  names.push_back("Cade");  names.push_back("Cage");
  names.push_back("Cain");  names.push_back("Carl");
  names.push_back("Cary");  names.push_back("Case");
  names.push_back("Cato");  names.push_back("Chad");
  names.push_back("Chaz");  names.push_back("Chet");
  names.push_back("Chip");  names.push_back("Clay");
  names.push_back("Clem");  names.push_back("Cobb");
  names.push_back("Coby");  names.push_back("Codi");
  names.push_back("Cody");  names.push_back("Cole");
  names.push_back("Colt");  names.push_back("Cory");
  names.push_back("Cree");  names.push_back("Crew");
  names.push_back("Cris");  names.push_back("Cruz");
  names.push_back("Curt");  names.push_back("Dale");
  names.push_back("Dane");  names.push_back("Dash");
  names.push_back("Dave");  names.push_back("Davy");
  names.push_back("Dean");  names.push_back("Deon");
  names.push_back("Desi");  names.push_back("Dino");
  names.push_back("Dion");  names.push_back("Doug");
  names.push_back("Drew");  names.push_back("Duke");
  names.push_back("Earl");  names.push_back("Eddy");
  names.push_back("Eden");  names.push_back("Edge");
  names.push_back("Elby");  names.push_back("Elio");
  names.push_back("Elmo");  names.push_back("Elon");
  names.push_back("Emil");  names.push_back("Emir");
  names.push_back("Emry");  names.push_back("Enzo");
  names.push_back("Epic");  names.push_back("Eric");
  names.push_back("Erik");  names.push_back("Erle");
  names.push_back("Eros");  names.push_back("Evan");
  names.push_back("Ezra");  names.push_back("Finn");
  names.push_back("Ford");  names.push_back("Fred");
  names.push_back("Gabe");  names.push_back("Gael");
  names.push_back("Gage");  names.push_back("Gary");
  names.push_back("Gene");  names.push_back("Gibb");
  names.push_back("Glen");  names.push_back("Gray");
  names.push_back("Grey");  names.push_back("Guss");
  names.push_back("Hale");  names.push_back("Hank");
  names.push_back("Hans");  names.push_back("Hawk");
  names.push_back("Hays");  names.push_back("Herb");
  names.push_back("Holt");  names.push_back("Hoyt");
  names.push_back("Huck");  names.push_back("Huey");
  names.push_back("Hugh");  names.push_back("Hugo");
  names.push_back("Hunt");  names.push_back("Hyde");
  names.push_back("Iago");  names.push_back("Igor");
  names.push_back("Iker");  names.push_back("Indy");
  names.push_back("Ivan");  names.push_back("Iver");
  names.push_back("Izzy");  names.push_back("Jack");
  names.push_back("Jair");  names.push_back("Jake");
  names.push_back("Jase");  names.push_back("Jaxx");
  names.push_back("Jazz");  names.push_back("Jean");
  names.push_back("Jeff");  names.push_back("Jess");
  names.push_back("Jett");  names.push_back("Joah");
  names.push_back("Jody");  names.push_back("Joel");
  names.push_back("Joey");  names.push_back("John");
  names.push_back("Jose");  names.push_back("Josh");
  names.push_back("Juan");  names.push_back("Judd");
  names.push_back("Jude");  names.push_back("Kade");
  names.push_back("Kale");  names.push_back("Kane");
  names.push_back("Karl");  names.push_back("Kato");
  names.push_back("Kent");  names.push_back("Kerr");
  names.push_back("Khan");  names.push_back("Kian");
  names.push_back("Kiel");  names.push_back("Kiev");
  names.push_back("King");  names.push_back("Kipp");
  names.push_back("Kirk");  names.push_back("Knox");
  names.push_back("Kobe");  names.push_back("Koda");
  names.push_back("Kody");  names.push_back("Kole");
  names.push_back("Krew");  names.push_back("Kurt");
  names.push_back("Kyle");  names.push_back("Kylo");
  names.push_back("Kyro");  names.push_back("Lake");
  names.push_back("Lane");  names.push_back("Lars");
  names.push_back("Leaf");  names.push_back("Leon");
  names.push_back("Levi");  names.push_back("Levy");
  names.push_back("Liam");  names.push_back("Link");
  names.push_back("Loki");  names.push_back("Luca");
  names.push_back("Ludo");  names.push_back("Luis");
  names.push_back("Luka");  names.push_back("Luke");
  names.push_back("Lyle");  names.push_back("Lynx");
  names.push_back("Mace");  names.push_back("Mack");
  names.push_back("Marc");  names.push_back("Mark");
  names.push_back("Mars");  names.push_back("Matt");
  names.push_back("Mell");  names.push_back("Merl");
  names.push_back("Mica");  names.push_back("Mick");
  names.push_back("Mike");  names.push_back("Milo");
  names.push_back("Mitt");  names.push_back("Mont");
  names.push_back("Moss");  names.push_back("Musa");
  names.push_back("Mylo");  names.push_back("Nash");
  names.push_back("Nate");  names.push_back("Navy");
  names.push_back("Neil");  names.push_back("Neon");
  names.push_back("Nero");  names.push_back("Newt");
  names.push_back("Nial");  names.push_back("Nick");
  names.push_back("Nico");  names.push_back("Niko");
  names.push_back("Nile");  names.push_back("Nilo");
  names.push_back("Noah");  names.push_back("Noel");
  names.push_back("Noon");  names.push_back("Nova");
  names.push_back("Nyle");  names.push_back("Odin");
  names.push_back("Olaf");  names.push_back("Omar");
  names.push_back("Omni");  names.push_back("Onyx");
  names.push_back("Opus");  names.push_back("Orlo");
  names.push_back("Otho");  names.push_back("Otis");
  names.push_back("Otto");  names.push_back("Owen");
  names.push_back("Paco");  names.push_back("Park");
  names.push_back("Paul");  names.push_back("Penn");
  names.push_back("Pete");  names.push_back("Phil");
  names.push_back("Pike");  names.push_back("Poet");
  names.push_back("Polk");  names.push_back("Quil");
  names.push_back("Quin");  names.push_back("Rain");
  names.push_back("Rand");  names.push_back("Raul");
  names.push_back("Reed");  names.push_back("Reef");
  names.push_back("Reid");  names.push_back("Reif");
  names.push_back("Remi");  names.push_back("Remy");
  names.push_back("Rene");  names.push_back("Rhys");
  names.push_back("Rice");  names.push_back("Rich");
  names.push_back("Rick");  names.push_back("Rico");
  names.push_back("Riot");  names.push_back("Roan");
  names.push_back("Robb");  names.push_back("Rock");
  names.push_back("Rolf");  names.push_back("Rome");
  names.push_back("Rook");  names.push_back("Rory");
  names.push_back("Ross");  names.push_back("Rowe");
  names.push_back("Rudy");  names.push_back("Rush");
  names.push_back("Russ");  names.push_back("Ryan");
  names.push_back("Sage");  names.push_back("Saul");
  names.push_back("Sean");  names.push_back("Seth");
  names.push_back("Shaw");  names.push_back("Shep");
  names.push_back("Skip");  names.push_back("Solo");
  names.push_back("Stan");  names.push_back("Sven");
  names.push_back("Taft");  names.push_back("Tate");
  names.push_back("Teal");  names.push_back("Thad");
  names.push_back("Theo");  names.push_back("Thom");
  names.push_back("Thor");  names.push_back("Tito");
  names.push_back("Toby");  names.push_back("Todd");
  names.push_back("Tony");  names.push_back("Tosh");
  names.push_back("Trey");  names.push_back("Troy");
  names.push_back("Tuan");  names.push_back("Tuck");
  names.push_back("Uley");  names.push_back("Utah");
  names.push_back("Vern");  names.push_back("Vick");
  names.push_back("Vito");  names.push_back("Vlad");
  names.push_back("Voss");  names.push_back("Wade");
  names.push_back("Walt");  names.push_back("Webb");
  names.push_back("West");  names.push_back("Will");
  names.push_back("Whit");  names.push_back("Witt");
  names.push_back("Wolf");  names.push_back("Wynn");
  names.push_back("Xavi");  names.push_back("Xyan");
  names.push_back("Yale");  names.push_back("York");
  names.push_back("Yuri");  names.push_back("Zach");
  names.push_back("Zack");  names.push_back("Zaid");
  names.push_back("Zain");  names.push_back("Zale");
  names.push_back("Zane");  names.push_back("Zeke");
  names.push_back("Zeus");  names.push_back("Zion");
  names.push_back("Zuma");

  return(names);
}

//----------------------------------------------------------------
// Procedure: intcountries7()

vector<string> intcountries7()
{
  vector<string> countries;

  countries.push_back("Albania");  countries.push_back("Algeria");
  countries.push_back("Anahola");  countries.push_back("Andorra");
  countries.push_back("Antwerp");  countries.push_back("Armenia");
  countries.push_back("Austria");  countries.push_back("Bahamas");
  countries.push_back("Bahrain");  countries.push_back("Belarus");
  countries.push_back("Belgium");  countries.push_back("Bolivia");
  countries.push_back("Bonifay");  countries.push_back("Burundi");
  countries.push_back("Chimayo");  countries.push_back("Comoros");
  countries.push_back("Croatia");  countries.push_back("Denmark");
  countries.push_back("Dixmore");  countries.push_back("Dresden");
  countries.push_back("Ecuador");  countries.push_back("Eritrea");
  countries.push_back("Estonia");  countries.push_back("Finland");
  countries.push_back("Georgia");  countries.push_back("Germany");
  countries.push_back("Grenada");  countries.push_back("Hungary");
  countries.push_back("Iceland");  countries.push_back("Ireland");
  countries.push_back("Jamaica");  countries.push_back("Lebanon");
  countries.push_back("Lesotho");  countries.push_back("Liberia");
  countries.push_back("Moldova");  countries.push_back("Morocco");
  countries.push_back("Namibia");  countries.push_back("Nigeria");
  countries.push_back("Romania");  countries.push_back("Senegal");
  countries.push_back("Somalia");  countries.push_back("Tunisia");
  countries.push_back("Ukraine");  countries.push_back("Uruguay");
  countries.push_back("Vanuatu");  countries.push_back("Vietnam");

  return(countries);
}
  
//----------------------------------------------------------------
// Procedure: us_cities7()

vector<string> us_cities7()
{
  vector<string> cities;

  cities.push_back("Airmont");  cities.push_back("Alabama");
  cities.push_back("Alachua");  cities.push_back("Almedia");
  cities.push_back("Amherst");  cities.push_back("Andover");
  cities.push_back("Andrews");  cities.push_back("Antlers");
  cities.push_back("Arcadia");  cities.push_back("Ardmore");
  cities.push_back("Arizona");  cities.push_back("Ashburn");
  cities.push_back("Ashland");  cities.push_back("Atlanta");
  cities.push_back("Attalla");  cities.push_back("Atwater");
  cities.push_back("Augusta");  cities.push_back("Aviston");
  cities.push_back("Babbitt");  cities.push_back("Babylon");
  cities.push_back("Baldwin");  cities.push_back("Baraboo");
  cities.push_back("Batavia");  cities.push_back("Bayport");
  cities.push_back("Baywood");  cities.push_back("Beckett");
  cities.push_back("Bedford");  cities.push_back("Beecher");
  cities.push_back("Belding");  cities.push_back("Belfair");
  cities.push_back("Belford");  cities.push_back("Belmont");
  cities.push_back("Belview");  cities.push_back("Belzoni");
  cities.push_back("Berkley");  cities.push_back("Berwick");
  cities.push_back("Beverly");  cities.push_back("Bigwell");
  cities.push_back("Bigfoot");  cities.push_back("Bigfork");
  cities.push_back("Blakley");  cities.push_back("Blawnox");
  cities.push_back("Bristol");  cities.push_back("Bolivar");
  cities.push_back("Bodfish");  cities.push_back("Bolinas");
  cities.push_back("Bolivar");  cities.push_back("Bonanza");
  cities.push_back("Boswell");  cities.push_back("Boulder");
  cities.push_back("Bourbon");  cities.push_back("Buffalo");
  cities.push_back("Bullard");  cities.push_back("Burbank");
  cities.push_back("Byhalia");  cities.push_back("Calhoun");
  cities.push_back("Califon");  cities.push_back("Calumet");
  cities.push_back("Cambria");  cities.push_back("Carlton");
  cities.push_back("Carlyle");  cities.push_back("citieson");
  cities.push_back("Carabou");  cities.push_back("Cascade");
  cities.push_back("Celeron");  cities.push_back("Chaplin");
  cities.push_back("Chapman");  cities.push_back("Chatham");
  cities.push_back("Chelsea");  cities.push_back("Chester");
  cities.push_back("Chicago");  cities.push_back("Chinook");
  cities.push_back("Cibecue");  cities.push_back("Clarion");
  cities.push_back("Clayton");  cities.push_back("Clemson");
  cities.push_back("Clifton");  cities.push_back("Cochran");
  cities.push_back("Colbert");  cities.push_back("Collins");
  cities.push_back("Colwich");  cities.push_back("Compton");
  cities.push_back("Concord");  cities.push_back("Cookson");
  cities.push_back("Coosada");  cities.push_back("Cordova");
  cities.push_back("Corydon");  cities.push_back("Cowpens");
  cities.push_back("Corinth");  cities.push_back("Corning");
  cities.push_back("Cornell");  cities.push_back("Cresson");
  cities.push_back("Crivitz");  cities.push_back("Crocker");
  cities.push_back("Crystal");  cities.push_back("Cushing");
  cities.push_back("Cypress");  cities.push_back("Dalhart");
  cities.push_back("Danbury");  cities.push_back("Danvers");
  cities.push_back("Davison");  cities.push_back("Delevan");
  cities.push_back("Deltona");  cities.push_back("Derwood");
  cities.push_back("Detroit");  cities.push_back("Dickson");
  cities.push_back("Dotsero");  cities.push_back("Drayton");
  cities.push_back("Dubuque");  cities.push_back("Dunkirk");
  cities.push_back("Eastman");  cities.push_back("Electra");
  cities.push_back("Elkhart");  cities.push_back("Elkhorn");
  cities.push_back("Elkland");  cities.push_back("Emerson");
  cities.push_back("Emporia");  cities.push_back("Eustace");
  cities.push_back("Everett");  cities.push_back("Fairfax");
  cities.push_back("Fairlea");  cities.push_back("Farwell");
  cities.push_back("Fayette");  cities.push_back("Flippin");
  cities.push_back("Florala");  cities.push_back("Florida");
  cities.push_back("Fontana");  cities.push_back("Forrest");
  cities.push_back("Forsyth");  cities.push_back("Fordyce");
  cities.push_back("Fosston");  cities.push_back("Foxfire");
  cities.push_back("Fremont");  cities.push_back("Gadsden");
  cities.push_back("Gallant");  cities.push_back("Gambier");
  cities.push_back("Gardena");  cities.push_back("Gardner");
  cities.push_back("Garwood");  cities.push_back("Garland");
  cities.push_back("Gateway");  cities.push_back("Genesee");
  cities.push_back("Gholson");  cities.push_back("Gifford");
  cities.push_back("Gilbert");  cities.push_back("Gillett");
  cities.push_back("Glasgow");  cities.push_back("Glencoe");
  cities.push_back("Glidden");  cities.push_back("Glyndon");
  cities.push_back("Goodhue");  cities.push_back("Goodman");
  cities.push_back("Grafton");  cities.push_back("Grabill");
  cities.push_back("Granger");  cities.push_back("Grayson");
  cities.push_back("Greenup");  cities.push_back("Gridley");
  cities.push_back("Guntown");  cities.push_back("Guthrie");
  cities.push_back("Hageman");  cities.push_back("Hamburg");
  cities.push_back("Hampton");  cities.push_back("Hanover");
  cities.push_back("Hancock");  cities.push_back("Hammond");
  cities.push_back("Harmony");  cities.push_back("Haskins");
  cities.push_back("Hatboro");  cities.push_back("Hayward");
  cities.push_back("Hemlock");  cities.push_back("Herlong");
  cities.push_back("Hickman");  cities.push_back("Hideout");
  cities.push_back("Hilbert");  cities.push_back("Hildale");
  cities.push_back("Hoboken");  cities.push_back("Holgate");
  cities.push_back("Honaker");  cities.push_back("Hopkins");
  cities.push_back("Houston");  cities.push_back("Hubbard");
  cities.push_back("Huguley");  cities.push_back("Ipswich");
  cities.push_back("Indiana");  cities.push_back("Ironton");
  cities.push_back("Irrigon");  cities.push_back("Jackpot");
  cities.push_back("Jackson");  cities.push_back("Jarrell");
  cities.push_back("Jellico");  cities.push_back("Jenison");
  cities.push_back("Jericho");  cities.push_back("Jolivue");
  cities.push_back("Justice");  cities.push_back("Keasbey");
  cities.push_back("Kellogg");  cities.push_back("Kempner");
  cities.push_back("Kendall");  cities.push_back("Killian");
  cities.push_back("Kimball");  cities.push_back("Kincaid");
  cities.push_back("Kindred");  cities.push_back("Kingman");
  cities.push_back("Kraemer");  cities.push_back("LaBelle");
  cities.push_back("Lacombe");  cities.push_back("Ladonia");
  cities.push_back("Lambert");  cities.push_back("Lansing");
  cities.push_back("Langley");  cities.push_back("Langdon");
  cities.push_back("Laporte");  cities.push_back("Laverne");
  cities.push_back("Layhill");  cities.push_back("Lathrop");
  cities.push_back("Leipsic");  cities.push_back("Lepanto");
  cities.push_back("Liberty");  cities.push_back("Lindale");
  cities.push_back("Lindsay");  cities.push_back("Livonia");
  cities.push_back("Loretto");  cities.push_back("Macopin");
  cities.push_back("Madison");  cities.push_back("Magalia");
  cities.push_back("Mallory");  cities.push_back("Malvern");
  cities.push_back("Mammoth");  cities.push_back("Manheim");
  cities.push_back("Mansura");  cities.push_back("Moravia");
  cities.push_back("Markham");  cities.push_back("Mayfair");
  cities.push_back("Maynard");  cities.push_back("Maywood");
  cities.push_back("Maytown");  cities.push_back("Medford");
  cities.push_back("Melrose");  cities.push_back("Memphis");
  cities.push_back("Menahga");  cities.push_back("Merengo");
  cities.push_back("Midland");  cities.push_back("Milford");
  cities.push_back("Mincora");  cities.push_back("Mission");
  cities.push_back("Moville");  cities.push_back("Montana");
  cities.push_back("Munhall");  cities.push_back("Munford");
  cities.push_back("Muscoda");  cities.push_back("Mustang");
  cities.push_back("Nahunta");  cities.push_back("Narrows");
  cities.push_back("Natalia");  cities.push_back("Navassa");
  cities.push_back("Needham");  cities.push_back("Needles");
  cities.push_back("Nekoosa");  cities.push_back("Newaygo");
  cities.push_back("Newport");  cities.push_back("Newkirk");
  cities.push_back("Norvelt");  cities.push_back("Norwood");
  cities.push_back("NewYork");  cities.push_back("Niantic");
  cities.push_back("Niagara");  cities.push_back("Nokomis");
  cities.push_back("Norfolk");  cities.push_back("Norwalk");
  cities.push_back("Norwich");  cities.push_back("Oakboro");
  cities.push_back("Oakdale");  cities.push_back("Oakland");
  cities.push_back("Oakwood");  cities.push_back("Oberlin");
  cities.push_back("Ontario");  cities.push_back("Oolitic");
  cities.push_back("Oologah");  cities.push_back("Orlando");
  cities.push_back("Orlinda");  cities.push_back("Orleans");
  cities.push_back("Osborne");  cities.push_back("Othello");
  cities.push_back("Pacheco");  cities.push_back("Palatka");
  cities.push_back("Palmero");  cities.push_back("Palouse");
  cities.push_back("Panacea");  cities.push_back("Pandora");
  cities.push_back("Parsons");  cities.push_back("Pawling");
  cities.push_back("Payette");  cities.push_back("Peabody");
  cities.push_back("Pearson");  cities.push_back("Peebles");
  cities.push_back("Pendell");  cities.push_back("Penhook");
  cities.push_back("Penrose");  cities.push_back("Peralta");
  cities.push_back("Phoenix");  cities.push_back("Pickett");
  cities.push_back("Piketon");  cities.push_back("Plummer");
  cities.push_back("Polkton");  cities.push_back("Pomeroy");
  cities.push_back("Pontiac");  cities.push_back("Portage");
  cities.push_back("Portola");  cities.push_back("Potsdam");
  cities.push_back("Preston");  cities.push_back("Primera");
  cities.push_back("Proctor");  cities.push_back("Quinlan");
  cities.push_back("Quitman");  cities.push_back("Ragland");
  cities.push_back("Raleigh");  cities.push_back("Rainier");
  cities.push_back("Ralston");  cities.push_back("Rangley");
  cities.push_back("Ravenna");  cities.push_back("Raymond");
  cities.push_back("Raymore");  cities.push_back("Reading");
  cities.push_back("Redland");  cities.push_back("Redmond");
  cities.push_back("Roanoke");  cities.push_back("Robbins");
  cities.push_back("Rosebud");  cities.push_back("Roswell");
  cities.push_back("Roxboro");  cities.push_back("Roxbury");
  cities.push_back("Salinas");  cities.push_back("Sanibel");
  cities.push_back("Sanborn");  cities.push_back("Saranac");
  cities.push_back("Satanta");  cities.push_back("Scandia");
  cities.push_back("Scanlon");  cities.push_back("Seaford");
  cities.push_back("Seattle");  cities.push_back("Sebring");
  cities.push_back("Seville");  cities.push_back("Seymour");
  cities.push_back("Shawnee");  cities.push_back("Shelton");
  cities.push_back("Skagway");  cities.push_back("Slinger");
  cities.push_back("Slocumb");  cities.push_back("Soledad");
  cities.push_back("Spencer");  cities.push_back("Spokane");
  cities.push_back("Spooner");  cities.push_back("Stayton");
  cities.push_back("Stepney");  cities.push_back("Sturgis");
  cities.push_back("Stryker");  cities.push_back("Sultana");
  cities.push_back("Suncook");  cities.push_back("Sundown");
  cities.push_back("Swansea");  cities.push_back("Suffolk");
  cities.push_back("Talmage");  cities.push_back("Tangent");
  cities.push_back("Tarboro");  cities.push_back("Taunton");
  cities.push_back("Telford");  cities.push_back("Temelec");
  cities.push_back("Tesuque");  cities.push_back("Thermal");
  cities.push_back("Topanga");  cities.push_back("Tonopah");
  cities.push_back("Toppers");  cities.push_back("Tornado");
  cities.push_back("Toronto");  cities.push_back("Towanda");
  cities.push_back("Tremont");  cities.push_back("Trenton");
  cities.push_back("Trinity");  cities.push_back("Tyndall");
  cities.push_back("Wabasso");  cities.push_back("Wakeman");
  cities.push_back("Waldorf");  cities.push_back("Wallace");
  cities.push_back("Walpole");  cities.push_back("Waltham");
  cities.push_back("Wapello");  cities.push_back("Warwick");
  cities.push_back("Wathena");  cities.push_back("Watkins");
  cities.push_back("Wautoma");  cities.push_back("Waverly");
  cities.push_back("Wayland");  cities.push_back("Webster");
  cities.push_back("Weidman");  cities.push_back("Wendell");
  cities.push_back("Wetumka");  cities.push_back("Wiggins");
  cities.push_back("Wilhoit");  cities.push_back("Willard");
  cities.push_back("Windsor");  cities.push_back("Winsted");
  cities.push_back("Wingate");  cities.push_back("Winlock");
  cities.push_back("Winslow");  cities.push_back("Woodway");
  cities.push_back("Woodall");  cities.push_back("Wooster");
  cities.push_back("Wyoming");  cities.push_back("Ulysses");
  cities.push_back("Valdese");  cities.push_back("Varnell");
  cities.push_back("Yachats");  cities.push_back("Yamhill");
  cities.push_back("Yardley");  cities.push_back("Yonkers");
  cities.push_back("Zebulon");  cities.push_back("Zeigler");

  return(cities);
}
//----------------------------------------------------------------
// Procedure: intcities7()

vector<string> intcities7()
{
  vector<string> cities;

  cities.push_back("Belfast");  cities.push_back("Beijing");
  cities.push_back("Jakarta");  cities.push_back("Kampala");
  cities.push_back("Karachi");  cities.push_back("Marigot");
  cities.push_back("Nairobi");  cities.push_back("SanJose");
  cities.push_back("SanJuan");  cities.push_back("TelAviv");
  cities.push_back("Tripoli");  cities.push_back("Vilnius");

  return(cities);
}
