/*
 *  Zipper.h
 *  MOOS
 *
 *  Created by pnewman on 20/08/2009.
 *  Copyright 2009 P Newman. All rights reserved.
 *
 */

#ifndef CZIPPERH
#define CZIPPERH

#include <MOOSGenLib/MOOSThread.h>
#include <string>
#include <MOOSGenLib/MOOSLock.h>

/*!
    @class   CZipper
    @abstract    Lauches a thread to write strings to a compressed (zipped file)
    @discussion  Uses the zlib library to write strings to file. Compressions is done in a background thread
*/

class CZipper
	{
	public:
		
		/*!
		 @function     Start
		 @abstract   Start the zipper specifying name of file. 
		 @discussion Start the zipper specifying name of file, a .gz will be added
		 @param	sFileNameBase  the base name of the compressed file. e.g t.txt will become t.txt.gz
		 */
		bool Start(const std::string sFileBaseName);
		
		/*!
		 @function Stop
		 @abstract   Stop zipping and terminate the compressed file correctly
		 @discussion  Stop zipping and terminate the compressed file correctly, blocking call
		 */
		bool Stop();
		
		/*!
		 @function IsRunning
		 @abstract   returns true if Zipper is active
		 @discussion returns true if Zipper is active
		 */

		bool IsRunning();
		
		/*!
		 @function   Push
		 @abstract   Push a string onto the 
		 @discussion Add a string to teh background threads work. This will eventually be added
		 @param sStr  the string which should be ashoved into the compressed file
		 */		
		bool Push(const std::string & sStr);


		//worker function
		bool DoZipLogging();
		
	protected:

		CMOOSLock   m_Lock;
		CMOOSThread m_Thread;
		
		
		std::list<std::string> m_ZipBuffer;
		std::string m_sFileName;
		
	};

#endif

