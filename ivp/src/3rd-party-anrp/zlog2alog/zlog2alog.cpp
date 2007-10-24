#include <iostream>
#include <string>
#include <map>

#include "esc_spc.h"

extern "C" {
#include <stdio.h>
#include <zlib.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
}

using namespace std;

int main(int argc, char *argv[])
{
	if(argc <= 2) {
		cerr << "usage: " << argv[0] << " [path/to/zlog] [ VAR1 VAR2 ... [VAR-n] | all ]" << endl;
		cerr << "writes the alog to standard output." << endl;
		return 0;
	}
	
	bool allEnabled = false;
	map<string,bool> envar;
	gzFile ifp;
	
	ifp = gzopen(argv[1], "rb");
	if(argc == 3 && string(argv[2]) == string("all")) {
		allEnabled = true;
	} else {
		for(int i=2; i<argc; i++) {
			envar[string(argv[i])] = true;
		}
	}

	double dbST, lST, lastHitTime, mtime;
	string mkey, msender, mtype, mcontent;
	
	char inp[80];
	gzgets(ifp, inp, 80);
	
	int mtmp[4];
	sscanf(inp, "zLogger 1 %i.%i %i.%06i", &(mtmp[0]), &(mtmp[1]),
	       &(mtmp[2]), &(mtmp[3]));
	
	dbST = double(mtmp[0]) + (double(mtmp[1]) / 1000000.0);
	lST  = double(mtmp[2]) + (double(mtmp[3]) / 1000000.0);
	
	printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"); // sheesh!
	printf("%%%% LOG FILE:       %s - converted by zlog2alog\n", argv[1]);
	printf("%%%% FILE OPENED ON  ---\n");
	printf("%%%% LOGSTART        %f\n", lST);
	printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
	
	while(!gzeof(ifp)) {
		char _tmp[32000];
		gzgets(ifp, _tmp, 32000);
		//if(gzeof(ifp)) continue;
		_tmp[strlen(_tmp)-1] = 0; // kill that newline! stupid pos
		string line = _tmp;
		
		switch(line[0]) {
			case 'H': // new time
				lastHitTime = atof((line.c_str())+1);
				break;
			case 'N': // new message .. doesn't mean much
				break;
			case 'V': // key (need to unesc)
				mkey = reverse_esc_spc(line.substr(1));
				break;
			case 'S': // sender (need to unesc)
				msender = reverse_esc_spc(line.substr(1));
				break;
			case 'W': // time offset sent at (relative to db time)
				mtime = atof((line.c_str())+1);
				break;
			case 'T': // type, D=double, S=string, U=undef
				mtype = line.substr(1);
				break;
			case 'C': // contents (need to unesc)
				mcontent = reverse_esc_spc(line.substr(1));

				// wee now to print it out
				if(allEnabled || envar.find(mkey) != envar.end())
					if(mtime > lST - dbST || 1)
						printf("%-15f %-20s %-10s %s\n", mtime, mkey.c_str(),
						       msender.c_str(), mcontent.c_str());
				break;
			default:
				cerr << "something isn't right here.. not a zlog?" << endl;
				break;
		}
	}

	gzclose(ifp);
}
