///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt  This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
//
//   The XPC classes in MOOS are modified versions of the source provided 
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga 
//

#ifndef _XPCEndian
#define _XPCEndian

#include <algorithm>

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
            std::reverse(cCharList, cCharList + sizeof(OrigType));
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
