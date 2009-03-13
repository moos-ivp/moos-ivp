///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford 
//   University 2003-2005. email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS CORE Component. 
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
//   The XPC classes in MOOS are modified versions of the source provided 
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga 
//
//////////////////////////    END_GPL    //////////////////////////////////
#ifndef _XPCEndian
#define _XPCEndian

template<class OrigType> class XPCEndian
{
private:
    union EndianType
    {
        OrigType EndianValue;            // Endian value of original type
        char cCharList[sizeof(OrigType)];    // Byte representation of original type

        // Swap the bytes to derive the endian value
        void vEndianSwap()
        {
            char cTemp;
            int iSwap2 = 0;
            for (int iSwap1 = sizeof(OrigType)-1; iSwap1 >= sizeof(OrigType)/2; iSwap1--)
            {
                cTemp = cCharList[iSwap1];
                cCharList[iSwap1] = cCharList[iSwap2];
                cCharList[iSwap2] = cTemp;
                iSwap2++;
            }
        }
    };

    OrigType OriginalVal;    // Stores the data type in its original format
    EndianType EndianVal;     // Stores the data type in its endian format
public:
    // Overladed = operator used to store the value in its original and endian format
    XPCEndian &operator=(OrigType &_Val)
    {
        OriginalVal = _Val;
        EndianVal.EndianValue = _Val;
        EndianVal.vEndianSwap();
        return *this;
    }    

    // Sets the Value of the endian object and stores the value in its original and endia
    // format
    void vSetValue(OrigType _Val)
    {
        OriginalVal = _Val;
                EndianVal.EndianValue = _Val;
                EndianVal.vEndianSwap();
    }

    // Return the endian value
    OrigType getEndianValue() 
    { 
        return EndianVal.EndianValue;
    }

    // Return the original value
    OrigType getOriginalValue()
    {
        return OriginalVal;
    }
};

#endif
