///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman and others
//   at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Instrument. 
//        
//   This program is free software; you can redistribute it and/or 
//   modify it under the terms of the GNU General Public License as 
//   published by the Free Software Foundation; either version 2 of the 
//   License, or (at your option) any later version. 
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
//   General Public License for more details. 
//            
//   You should have received a copy of the GNU General Public License 
//   along with this program; if not, write to the Free Software 
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
//   02111-1307, USA. 
//
//////////////////////////    END_GPL    //////////////////////////////////
#include <mexVNLHelpers.h>
#include <string>
#include <map>

#include <fstream>





/** A sensor port */

#ifdef _WIN32
    CMOOSNTSerialPort gPort;
#else
    CMOOSLinuxSerialPort gPort;
#endif

/** a MOOS Connection **/

CMOOSCommClient gComms;

// a parameter holding class
class Param
{

public:
    enum Type
    {
        STR,
        DBL,
        VEC,
        MAT,
        UNK,
    };
    double dfVal;
    std::string sVal;
    Matrix M;
    Vector V;
    Type m_eType;
    Param()
    {
        m_eType=UNK;
        dfVal = -1;
    }
    std::string Str()
    {
        switch(m_eType)
        {
        case STR:
            return "Str: "+sVal;
        case DBL:
            return MOOSFormat("Dbl: %.3f",dfVal);
        case VEC: 
            return MOOSFormat("Vec: %d",V.rows());
        case MAT:
            return MOOSFormat("Mat: %dx%d",M.rows(),M.cols());
        case UNK:
            return MOOSFormat("NOT SET! ");
        }
    }

};
typedef  std::map<std::string,Param> ARGMAP;

ARGMAP gArgMap;



bool Initialise(const mxArray *prhs[], int nrhs)
{
    mexPrintf("*********** iMatlab Initialising ***********\n");
    mexPrintf("* A box of MOOS accesories                 *\n");
    mexPrintf("* P. Newman                                *\n");
    mexPrintf("* Oxford 2005                              *\n");
    mexPrintf("********************************************\n");



    if(prhs!=NULL)
    {
        for(int i = 1;i<nrhs;i+=2)
        {
            if(i<nrhs-1)
            {
                std::string sParam;
                double dfVal;
                if(!Matlab2String(sParam,prhs[i]))
                {
                    mexErrMsgTxt("Incorrect param value pair (not a string)");
                }

                const mxArray *p = prhs[i+1];
                Matrix M;
                Param NewParam;
                NewParam.Type = Param::UNK;
                switch(mxGetClassID(p))
                {
                case mxCHAR_CLASS:
                    {
                        if(Matlab2String(sParam,p))
                        {
                            NewParam.Type=Param::STR;
                        }
                    }
                    break
                case mxDOUBLE_CLASS:
                    if(MatrixMatlab2MatrixVNL(p,NewParam.M))
                    {
                        if(M.rows()==1 && M.cols()==1)
                        {
                            NewParam.Type = Param::DBL;
                            NewParam.dfVal = M[0][0];
                            //a scalar
                        }
                        else if (M.rows()==1 || M.cols()==1)
                        {
                            NewParam.Type = Param::VEC;
                            //a vector
                            if(M.rows()==1)
                            {
                                NewParam.V = M.transpose().column(0).as_vector();
                            }
                            if(M.cols()==1)
                            {
                                NewParam.V = M.column(0).as_vector();
                            }
                        }
                        else
                        {
                            NewParam.Type = Param::MAT;
                            NewParam.M = M;
                        }
                    }

                    break;
                default:
                    break;
                }

                //did we parse it OK?
                if(NewParam.Type==Param::UNK)
                {
                    mexPrintf("PROBLEM : can't parse parameter value %s\n",sParam.c_str());
                }
                else
                {
                    ARGMAP::iterator p = ArgMap.find(sParam);

                    if(p==ArgMap.end())
                    {                        
                        mexPrintf("warning no such parameter exists:  %s\n",sParam.c_str());
                    }
                    else
                    {
                        *(p->second) = NewParam;
                    }
                }
            }
        }
    }


    ARGMAP::iterator p;

    for(p = ArgMap.begin();p!=ArgMap.end();p++)
    {
        mexPrintf("Property %-25s  %s\n",p->first.c_str(),(p->second).Str().c_str());
    }


    bInitialised = true;

    return bInitialised;
}


void PrintHelp()
{

    std::ifstream HelpFile("OX2DLS.help");

    if(HelpFile.is_open())
    {
        while(!HelpFile.eof())
        {
            char Line[1024];
            HelpFile.getline(Line,sizeof(Line));
            mexPrintf("%s\n",Line);
        }
    }
    else
    {
        mexPrintf("help file \"OX2DLS.help\" not found\n");
    }

}

/** this is the matlab gateway function **/
/** function ; **/
void mexFunction(int nlhs,
                 mxArray *plhs[],
                 int nrhs,
                 const mxArray *prhs[])
{

    //local variables
    vnl_matrix<double> Sxy;
    std::string sCmd;

    //lets begin by parsing the input (RHS)
    if(nrhs<1)
    {
        PrintHelp();
        return;
    }
    
    //first parameter is always a string  command
    if(!Matlab2String(sCmd,prhs[0]))
    {
        mexErrMsgTxt("Param 1 (cmd) must be a string ");
    }


    // MOOS_MAIL_FETCH returns String
    // MOOS_MAIL_SEND
    // SERIAL_READ_ASCII


    if(sCmd=="INITIALISE")
    {
        Initialise(prhs,nrhs);
    }
    else if(sCmd=="RUN")
    {

        switch(nrhs)
        {
            case 2:    
            {
                if( !mxIsDouble(prhs[1]) || mxIsComplex(prhs[1])  ) 
                {
                    mexErrMsgTxt("Param 2 vehicle must be a 2 by N  scan in sensor centric x-y space");
                }                
                
                MatrixMatlab2MatrixVNL(prhs[1],Sxy);

                int nPoints = Sxy.columns();
                if(nPoints<2)
                {
                    mexErrMsgTxt("Need at least two points to extract a segment!\n");
                }

                double * pX = new double[nPoints];
                double * pY = new double[nPoints];

                for(int i = 0;i<nPoints;i++)
                {
                    pX[i] = Sxy(0,i);
                    pY[i] = Sxy(1,i);
                }



                if(!bInitialised)
                {
                    mexPrintf("Initialising with default settings...\n");
                    Initialise(NULL,0);
                }

                std::vector<CSegment2D> ResultSegments;
                if(TheRansacGadget.Run(pX,pY,nPoints,ResultSegments))
                {
                    vnl_matrix<double> Results(ResultSegments.size(),4);

                    mexPrintf("Found %d segments\n",ResultSegments.size());
                    for(int i = 0;i<ResultSegments.size();i++)
                    {
                        mexPrintf("S[%d] ->  %s",i,ResultSegments[i].Describe().c_str());
                        Results(i,0) = ResultSegments[i].x1;
                        Results(i,1) = ResultSegments[i].y1;
                        Results(i,2) = ResultSegments[i].x2;
                        Results(i,3) = ResultSegments[i].y2;
                    }


                    //the return result is a matrix of x1 y1 x2 y2 line segments
                    MatrixVNL2MatrixMatlab(plhs[0],Results);

                }

                delete pX;
                delete pY;

            }
            break;

            
            
            default:
                mexErrMsgTxt(" \"RUN\" needs a sacn as a second parameter\n");
        }
    }
    else
    {
        mexErrMsgTxt("Unknown command string\n");
    }
    

}
