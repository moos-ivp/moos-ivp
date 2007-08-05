#ifndef __SimplePID_h__
#define __SimplePID_h__

class SimplePID {
public:
	SimplePID() {
		pGain = iGain = dGain = 0;
		iMin = iMax = 0;
		iState = dState = 0;
	}
	~SimplePID() {}

	void SetGains(double pG, double iG, double dG) {
		pGain = pG; iGain = iG; dGain = dG;
	}

	void SetMaxMinIntegrator(double i, double a) {
		iMin = i; iMax = a;
	}

	double Run(double error, double position) {
		double pTerm, dTerm, iTerm;

		pTerm = pGain * error;
		iState += error;

		if(iState > iMax) iState = iMax;
		if(iState < iMin) iState = iMin;

		iTerm = iGain * iState;
		dTerm = dGain * (position - dState);
		
		dState = position;
		return pTerm + iTerm - dTerm;
	}

private:
	double pGain, iGain, dGain;
	double iMin, iMax;
	
	double dState;
	double iState;
};

#endif
