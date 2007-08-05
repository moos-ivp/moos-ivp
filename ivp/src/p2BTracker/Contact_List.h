#ifndef CONTACT_LIST_HEADER
#define CONTACT_LIST_HEADER

#include <vector>
//#include "Contact.h"
#include "Measurement.h"

using namespace std;

class Contact_List
{
 public:

  Contact_List();
  ~Contact_List();

  int associate_bearing(Measurement*);
  int get_num_contacts();
  void update_contact(int,Measurement*);
  void get_Cinfo(int,CINFO*);

  double range_guess;
  int    min_points;

 protected:

  int num_contacts;

  vector<Contact> Contacts;

bool SolveTrack(int,double,double);
bool Add_Measurement(int,Measurement*);

};
#endif
