/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Permute.cpp                                          */
/*    DATE: Feb 10th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "Permute.h"

using namespace std;

vector<int> permute(const vector<int>& old_permutation, int max_tries)
{
  vector<int> new_permutation = old_permutation;
  if(max_tries < 1)
    return(new_permutation);

  int i;
  int vsize = old_permutation.size();
  
  vector<bool> taken;
  for(i=0; i<vsize; i++)
    taken.push_back(false);

  int tries = 0;
  while(tries < max_tries) {

    // Generate a random permutation
    for(i=0; i<vsize; i++) {
      // Pick num in range [0 AMT-1], AMT is amt yet to be assigned
      int ix = rand() % (vsize-i);


      // Go thru ix number of elements in the list of "untaken" elements.
      bool done = false;
      while(!done) {
	if(ix > vsize)
	ix -= vsize;
	if(!taken[ix]) {
	  new_permutation[i] = old_permutation[ix];
	  taken[ix] = true;
	  done = true;
	}
	else
	  ix++;
      }
    }

    // Check if new_permutation is different *at-every-index* compared
    // to old_permutation.
    bool all_different = true;
    for(i=0; ((i<vsize) && all_different); i++) {
      if(new_permutation[i] == old_permutation[i])
	all_different = false;
    }

    if(all_different)
      return(new_permutation);
    else {
      // reset the taken array
      for(i=0; i<vsize; i++)
	taken[i] = false;
      tries++;
    }
  }
}





