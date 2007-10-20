#ifndef __DoubleLinearInterp_h__
#define __DoubleLinearInterp_h__

#include "LUInterpBuffer.h"

class DoubleLinearInterp {
	typedef std::pair<double,double> val_type;

public:
	double operator()(const val_type &lo, const val_type &hi, double time)
	{
		val_type mid;
		mid.first = time;
		double dt = hi.first - lo.first;
		double alpha = 0.0;

		if(dt != 0.0) {
			alpha = (mid.first - lo.first) / dt;
		}

		mid.second = alpha*hi.second + (1-alpha)*lo.second;

		return mid.second;
	}
};

typedef LUInterpBuffer<double,double,DoubleLinearInterp> DoubleInterpBuffer;
typedef LUInterpBuffer<double,double,DoubleLinearInterp>::iterator DoubleInterpBufferIterator;

#endif /* __DoubleLinearInterp_h__ */

