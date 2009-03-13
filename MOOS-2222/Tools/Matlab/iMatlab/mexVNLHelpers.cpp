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
#include "mexVNLHelpers.h"


bool  Matlab2Double(double & dfVal,const mxArray * pMLA)
{
    if(!mxIsDouble(pMLA))
        return false;

    dfVal = mxGetScalar(pMLA);
    return true;
}

bool  Matlab2String(std::string & sStr,const mxArray * pMLA)
{
    /* Input must be a string. */
    if ( mxIsChar(pMLA) != 1)
    {
        return false;
    }
    
    /* Input must be a row vector. */
    if (mxGetM(pMLA)!=1)
    {
        mexPrintf("Input must be a row vector.");
        return false;
    }
    
    /* Get the length of the input string. */
    int buflen = (mxGetM(pMLA) * mxGetN(pMLA)) + 1;
    
    /* Allocate memory for input and output strings. */
    void * input_buf=mxCalloc(buflen, sizeof(char));
    
    /* Copy the string data from prhs[0] into a C string 
    * input_ buf.
    * If the string array contains several rows, they are copied,
    * one column at a time, into one long string array.
    */
    int status = mxGetString(pMLA, (char*)input_buf, buflen);
    
    if(status!=0)
    {
        mexErrMsgTxt("Bad String extraction.");
        return false;
    }
    
    //yay!
    sStr  = std::string ((char*)input_buf);
    
    return true;
    
                
}
#ifdef HAVE_VNL
bool MatrixMatlab2VectorVNL(const mxArray * pMLA,vnl_vector<double> & V)
{
    int nRows = mxGetM(pMLA);
    int nCols = mxGetN(pMLA);
    
    if(! ((nCols==1)^(nRows==1)) )
    {
        mexPrintf("MatrixMatlab2VectorVNL::Error pMLA is not a vector\n");
        return false;
    }
    
    
    int nEls = nRows*nCols;
    if(V.size()!=nEls)
    {
        V.set_size(nEls);
    }
    
    double *pData = mxGetPr(pMLA);
    
    for(int i = 0;i<nEls;i++)
        V[i] = pData[i];
    
    
    return true;
}

bool MatrixMatlab2MatrixVNL(const mxArray * pMLA,vnl_matrix<double> & M)
{
    int nRows = mxGetM(pMLA);
    int nCols = mxGetN(pMLA);
    
    //    mexPrintf("Matlab::Matrix is %d by %d \n", nRows,nCols);
    
    
    if(M.rows()!=nRows || M.cols()!=nCols)
    {
        M.set_size(nRows,nCols);
    }
    
    //  mexPrintf("VNL::Matrix is %d by %d \n", M.rows(),M.cols());
    
    double *pData = mxGetPr(pMLA);
    for(int i = 0;i<nRows;i++)
    {
        for(int j = 0;j<nCols;j++)
        {    
            double dfVal = pData[ (j)*nRows+i];
            M(i,j) = dfVal;
            //            mexPrintf("VNL::M[%d][%d] = %g\n",i,j,dfVal);
        }
    }
    
    //    mexPrintf("VNL Matrix is %d by %d and e(0,0) is %f\n", M.rows(),M.cols(),M(0,0));
    
    return true;
}

bool MatrixVNL2MatrixMatlab( mxArray * & pMLA,const vnl_matrix<double> & M)
{
    int nRows = M.rows();
    int nCols = M.cols();
    
    pMLA  = mxCreateDoubleMatrix(M.rows(), M.cols(), mxREAL);
    double *pData = mxGetPr(pMLA);
    
    for(int i = 0;i<nRows;i++)
    {
        for(int j = 0;j<nCols;j++)
        {    
            pData[ (j)*nRows+i] = M(i,j);
        }
    }
    
    return true;
}

bool VectorVNL2MatrixMatlab( mxArray * & pMLA,const vnl_vector<double> & V)
{
    int nRows = V.size();
    
    pMLA  = mxCreateDoubleMatrix(V.size(), 1, mxREAL);
    double *pData = mxGetPr(pMLA);
    for(int i = 0;i<nRows;i++)
    {
        pData[i] = V[i];
    }
    
    return true;
}
#endif
