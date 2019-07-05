/*
 * Share.h
 *
 *  Created on: Aug 26, 2012
 *      Author: pnewman
 */

#ifndef MOOS_ESSENTIAL_SHARE_H_
#define MOOS_ESSENTIAL_SHARE_H_

/*
 *
 */
namespace MOOS {

class Share {
public:
	Share();
	virtual ~Share();
	int Run(int argc,char * argv[]);

private:
	class Impl;
	Impl* _Impl;
};

}

#endif /* MOOS_ESSENTIAL_SHARE_H_ */
