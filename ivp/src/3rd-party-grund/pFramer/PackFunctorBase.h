/*****************************************************************************/
// Filename: PackFunctorBase.h
/*****************************************************************************/
// Description: This class is a base class for packing functors.
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
#ifndef PACK_FUNCTOR_BASE_H_INCLUDED
#define PACK_FUNCTOR_BASE_H_INCLUDED

/**
* This class is the superclass of the pack functors.
*/
class CPackFunctorBase
{
public:
	/**
	* Overload the () operator so the class behaves like a function.
	*/
	virtual void operator() (void* packedBuffer, void* sourceData) const = 0;
}; /* end CPackFunctorBase */
#endif
