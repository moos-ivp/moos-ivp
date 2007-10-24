/*****************************************************************************/
// Filename: UnpackFunctorBase.h
/*****************************************************************************/
// Description: This class is a base class for unpacking functors.
//
// Constraints: None.
//
// Author:      Matthew S. Davis (msd4@psu.edu)
//
// Date:        07Jul2004
/*****************************************************************************/
// Penn State University Applied Research Laboratory Proprietary Software
// (c) Copyright 2004 The Pennsylvania State University
// This comment block may not be removed
/*****************************************************************************/
#ifndef UNPACK_FUNCTOR_BASE_H_INCLUDED
#define UNPACK_FUNCTOR_BASE_H_INCLUDED

/**
* This class is the superclass of the unpack functors.
*/
class CUnpackFunctorBase
{
public:
	/**
	* Overload the () operator so the class behaves like a function.
	*/
	virtual void operator() (void* destinationData, void* packedBuffer) const = 0;
}; /* end CUnpackFunctorBase */
#endif
