// Pings2Solution.cpp: implementation of the CPings2Solution class.
//
//////////////////////////////////////////////////////////////////////

#include "Pings2Solution.h"
#include "MOOSGenLib/MOOSGenLibGlobalHelper.h"

#include <algorithm>
#include <functional>
#define ROUND(x) (x-floor(x)>=0.5 ? ceil(x) : floor(x))


#define DEFAULT_MIN_BASELINE 2.0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPings2Solution::CPings2Solution()
{

}

CPings2Solution::~CPings2Solution()
{

}

void CPings2Solution::Clear()
{
    m_X.clear();
    m_Y.clear();
    m_Yaw.clear();
    m_Pings.clear();
}

bool CPings2Solution::Solve( std::list<CPings2Solution::Solution> & SolutionSet)
{
    SolutionSet.clear();
  
    //we are going to ransac over possible solutions...

    //pick two pings
    //solve for 2 vehicle locations
    //score each with bust to other pings/locations

    int nPings = m_Pings.size();

    if(nPings<2)
        return MOOSFail("Waiting for more than 2 pings... \n");

    if(m_X.size()<3)
        return MOOSFail("Insufficent Pose information %s\n",MOOSHERE);

    if(m_Y.size()<3)
        return MOOSFail("Insufficent Pose information %s\n",MOOSHERE);

    int nTries = nPings*2; //fix me later

    for(int i = 1;i<nTries;i++)
    {
        int ndx1 = (int)ROUND(MOOSUniformRandom(0,nPings-1));
        int ndx2 = ndx1;
        while(ndx1==ndx2)
            ndx2 = (int)ROUND(MOOSUniformRandom(0,nPings-1));

        //now have two seperate pings
        double dfT1 = m_Pings[ndx1].first;
        double dfT2 = m_Pings[ndx2].first;
        double dfP1 = m_Pings[ndx1].second;
        double dfP2 = m_Pings[ndx2].second;

        XY_LOCATION Loc1(m_X(dfT1), m_Y(dfT1) ); 
        XY_LOCATION Loc2(m_X(dfT2), m_Y(dfT2) ); 

        XY_LOCATION Sol1,Sol2;
        


        if(Intersect(Loc1,dfP1,Loc2,dfP2,Sol1,Sol2))
        {


            //we have two possible solutions here
            //score all other ping w.r.t this:

            double dfSSE1 = 0;
            double dfSSE2 = 0;

            for(int j = 0;j<nPings;j++)
            {
                if(j==ndx1 || j==ndx2)
                    continue;

                double dfTQ = m_Pings[j].first;
                double dfRQ = m_Pings[j].second;
                double dfXQ = m_X(dfTQ);
                double dfYQ = m_Y(dfTQ);

                double dfError1 = hypot(dfXQ-Sol1.first,dfYQ-Sol1.second)-dfRQ;
                double dfError2 = hypot(dfXQ-Sol2.first,dfYQ-Sol2.second)-dfRQ;

                dfSSE1+=dfError1*dfError1;
                dfSSE2+=dfError2*dfError2;

            }
            
            //store the solutions
            Solution S1,S2;

            S1.m_Location = Sol1;
            S1.m_dfSSE = dfSSE1/nPings;
            SolutionSet.push_back(S1);

            S2.m_Location = Sol2;
            S2.m_dfSSE = dfSSE2/nPings;
            SolutionSet.push_back(S2);
        }
        else
        {
        }
    }

    //now sort the solutions
    SolutionSet.sort();
    

    //OK now we need to pool/quantise  the solutions
    std::list<Solution>::iterator p;
    std::map< std::pair<int,int>, double > T;
    std::map< std::pair<int,int>, double >::iterator q;
    double dfResolution = 1.0;
    for(p = SolutionSet.begin();p!= SolutionSet.end(); p++)
    {       
        int nX = (int)ROUND(p->m_Location.first/dfResolution);
        int nY= (int)ROUND(p->m_Location.second/dfResolution);
        std::pair<int,int> NDX( nX,nY);
        q =T.find(NDX); 

        if(q!=T.end())
        {
            q->second = q->second>p->m_dfSSE ? p->m_dfSSE : q->second;
        }
        else
        {
            T[NDX] = p->m_dfSSE;
        }
    }

    //now copy back into the solution set to return to user
    SolutionSet.clear();

//    MOOSTrace("%d unique solutions :\n",T.size());
    for(q = T.begin();q!= T.end(); q++)
    {
 /*       MOOSTrace("\t[X,Y] = {%f %f} Score = %f\n",
            q->first.first*dfResolution,
            q->first.second*dfResolution,
            q->second);
*/
        Solution S;
        S.m_Location = XY_LOCATION(q->first.first*dfResolution,
            q->first.second*dfResolution);
        S.m_dfSSE = q->second;
        SolutionSet.push_back(S);
    }

    SolutionSet.sort();





    return true;
}



bool CPings2Solution::Intersect(XY_LOCATION L1,
                            double dfR1,
                            XY_LOCATION L2,
                            double dfR2,
                            XY_LOCATION & Sol1,
                            XY_LOCATION & Sol2)
{

    double dfXv1 = L1.first;
    double dfXv2 = L2.first;

    double dfYv1 = L1.second;
    double dfYv2 = L2.second;


    double dfDX = dfXv2 - dfXv1;
    double dfDY = dfYv2 - dfYv1;

    double dfDP = hypot(dfDX,dfDY);

    if(dfDP<DEFAULT_MIN_BASELINE)
        return false;

    double dfgx = (dfDP*dfDP-dfR2*dfR2+dfR1*dfR1)/(2*dfDP);

    double dfgy = sqrt(dfR1*dfR1-dfgx*dfgx);

    

    double dfSX1 = dfXv1+(dfgx*dfDX+dfgy*dfDY)/dfDP;
    double dfSY1 = dfYv1+(dfgx*dfDY-dfgy*dfDX)/dfDP;


    double dfSX2 = dfXv1+(dfgx*dfDX-dfgy*dfDY)/dfDP;
    double dfSY2 = dfYv1+(dfgx*dfDY+dfgy*dfDX)/dfDP;


    Sol1.first = dfSX1;
    Sol1.second = dfSY1;

    Sol2.first = dfSX2;
    Sol2.second = dfSY2;

    return true;
}