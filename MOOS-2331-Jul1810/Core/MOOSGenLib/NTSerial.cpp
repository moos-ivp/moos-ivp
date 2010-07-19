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
//   This file is part of a  MOOS Core Component. 
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
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif


//    Serial.cpp - Implementation of the CNTSerial class
//
//    Copyright (C) 1999-2001 Ramon de Klein (R.de.Klein@iaf.nl)
//
// This program is free software; you can redistribute it and/ormodify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


//////////////////////////////////////////////////////////////////////
// Include the precompiled header

#define STRICT
#include <crtdbg.h>
#include <tchar.h>
#include <windows.h>


//////////////////////////////////////////////////////////////////////
// Include module headerfile

#include "NTSerial.h"


//////////////////////////////////////////////////////////////////////
// Disable warning C4127: conditional expression is constant, which
// is generated when using the _RPTF and _ASSERTE macros.

#pragma warning(disable: 4127)


//////////////////////////////////////////////////////////////////////
// Enable debug memory manager

#ifdef _DEBUG
static const char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Code

CNTSerial::CNTSerial ()
{
    // Reset data
    m_lLastError     = ERROR_SUCCESS;
    m_hFile          = 0;
    m_eEvent         = EEventNone;
    m_hevtOverlapped = 0;
}

CNTSerial::~CNTSerial ()
{
    // If the device is already closed,
    // then we don't need to do anything.
    if (m_hFile)
    {
        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::~CNTSerial - Serial port not closed\n");

        // Close implicitly
        ClosePort();
    }
}

CNTSerial::EPort CNTSerial::CheckPort (LPCTSTR lpszDevice)
{
    // Try to open the device
    HANDLE hFile = ::CreateFile(lpszDevice, 
                           GENERIC_READ|GENERIC_WRITE, 
                           0, 
                           0, 
                           OPEN_EXISTING, 
                           FILE_FLAG_OVERLAPPED, 
                           0);

    // Check if we could open the device
    if (hFile == INVALID_HANDLE_VALUE)
    {
        // Display error
        switch (::GetLastError())
        {
        case ERROR_FILE_NOT_FOUND:
            // The specified COM-port does not exist
            return EPortNotAvailable;

        case ERROR_ACCESS_DENIED:
            // The specified COM-port is in use
            return EPortInUse;

        default:
            // Something else is wrong
            return EPortUnknownError;
        }
    }

    // Close handle
    ::CloseHandle(hFile);

    // Port is available
    return EPortAvailable;
}

LONG CNTSerial::Open (LPCTSTR lpszDevice, DWORD dwInQueue, DWORD dwOutQueue)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the port isn't already opened
    if (m_hFile)
    {
        m_lLastError = ERROR_ALREADY_INITIALIZED;
        _RPTF0(_CRT_WARN,"CNTSerial::Open - Port already opened\n");
        return m_lLastError;
    }

    // Open the device
    m_hFile = ::CreateFile(lpszDevice,
                           GENERIC_READ|GENERIC_WRITE,
                           0,
                           0,
                           OPEN_EXISTING,
                           FILE_FLAG_OVERLAPPED,
                           0);
    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        // Reset file handle
        m_hFile = 0;

        // Display error
        m_lLastError = ::GetLastError();
        _RPTF0(_CRT_WARN, "CNTSerial::Open - Unable to open port\n");
        return m_lLastError;
    }

    // We cannot have an event handle yet
    _ASSERTE(m_hevtOverlapped == 0);

    // Create the event handle for internal overlapped operations (manual reset)
    m_hevtOverlapped = ::CreateEvent(0,true,false,0);
    if (m_hevtOverlapped == 0)
    {
        // Obtain the error information
        m_lLastError = ::GetLastError();
        _RPTF0(_CRT_WARN,"CNTSerial::Open - Unable to create event\n");

        // Close the port
        ::CloseHandle(m_hFile);
        m_hFile = 0;

        // Return the error
        return m_lLastError;
    }

    // Setup the COM-port
    if (!::SetupComm(m_hFile,dwInQueue,dwOutQueue))
    {
        // Display a warning
        long lLastError = ::GetLastError();
        _RPTF0(_CRT_WARN,"CNTSerial::Open - Unable to setup the COM-port\n");

        // Close the port
        ClosePort();

        // Save last error from SetupComm
        m_lLastError = lLastError;
        return m_lLastError;
    }

    // Setup the default communication mask
    SetMask();

    // Setup the device for default settings
    Setup();

    // Non-blocking reads is default
    SetupReadTimeouts(EReadTimeoutNonblocking);

    // Default is no handshaking
    SetupHandshaking(EHandshakeOff);

    // Return successful
    return m_lLastError;
}

LONG CNTSerial::ClosePort (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // If the device is already closed,
    // then we don't need to do anything.
    if (m_hFile == 0)
    {
        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::Close - Method called when device is not open\n");
        return m_lLastError;
    }

    // Free event handle
    ::CloseHandle(m_hevtOverlapped);
    m_hevtOverlapped = 0;

    // Close COM port
    ::CloseHandle(m_hFile);
    m_hFile = 0;

    // Return successful
    return m_lLastError;
}

LONG CNTSerial::Setup (EBaudrate eBaudrate, EDataBits eDataBits, EParity eParity, EStopBits eStopBits)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::Setup - Device is not opened\n");
        return m_lLastError;
    }

    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::Setup - Unable to obtain DCB information\n");
        return m_lLastError;
    }

    // Set the new data
    dcb.BaudRate = DWORD(eBaudrate);
    dcb.ByteSize = BYTE(eDataBits);
    dcb.Parity   = BYTE(eParity);
    dcb.StopBits = BYTE(eStopBits);

    // Determine if parity is used
    dcb.fParity  = (eParity != EParNone);

    // Set the new DCB structure
    if (!::SetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::Setup - Unable to set DCB information\n");
        return m_lLastError;
    }

    // Return successful
    return m_lLastError;
}

LONG CNTSerial::SetEventChar (BYTE bEventChar, bool fAdjustMask)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::SetEventChar - Device is not opened\n");
        return m_lLastError;
    }

    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::SetEventChar - Unable to obtain DCB information\n");
        return m_lLastError;
    }

    // Set the new event character
    dcb.EvtChar = char(bEventChar);

    // Adjust the event mask, to make sure the event will be received
    if (fAdjustMask)
    {
        // Enable 'receive event character' event.  Note that this
        // will generate an EEventNone if there is an asynchronous
        // WaitCommEvent pending.
        SetMask(GetEventMask() | EEventRcvEv);
    }

    // Set the new DCB structure
    if (!::SetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::SetEventChar - Unable to set DCB information\n");
        return m_lLastError;
    }

    // Return successful
    return m_lLastError;
}

LONG CNTSerial::SetMask (DWORD dwMask)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::SetMask - Device is not opened\n");
        return m_lLastError;
    }

    // Set the new mask. Note that this will generate an EEventNone
    // if there is an asynchronous WaitCommEvent pending.
    if (!::SetCommMask(m_hFile,dwMask))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::SetMask - Unable to set event mask\n");
        return m_lLastError;
    }

    // Return successful
    return m_lLastError;
}

LONG CNTSerial::WaitEvent (LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::WaitEvent - Device is not opened\n");
        return m_lLastError;
    }

    // Wait for the event to happen
    OVERLAPPED ovInternal;
    if (lpOverlapped == 0)
    {
        // Setup our own overlapped structure
        memset(&ovInternal,0,sizeof(ovInternal));
        ovInternal.hEvent = m_hevtOverlapped;

        // Use our internal overlapped structure
        lpOverlapped = &ovInternal;
    }

    // Make sure the overlapped structure isn't busy
    _ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

    // Wait for the COM event
    if (!::WaitCommEvent(m_hFile,LPDWORD(&m_eEvent),lpOverlapped))
    {
        // Set the internal error code
        long lLastError = ::GetLastError();

        // Overlapped operation in progress is not an actual error
        if (lLastError != ERROR_IO_PENDING)
        {
            // Save the error
            m_lLastError = lLastError;

            // Issue an error and quit
            _RPTF0(_CRT_WARN,"CNTSerial::WaitEvent - Unable to wait for COM event\n");
            return m_lLastError;
        }

        // We need to block if the client didn't specify an overlapped structure
        if (lpOverlapped == &ovInternal)
        {
            // Wait for the overlapped operation to complete
            switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
            {
            case WAIT_OBJECT_0:
                // The overlapped operation has completed
                break;

            case WAIT_TIMEOUT:
                // Cancel the I/O operation
                ::CancelIo(m_hFile);

                // The operation timed out. Set the internal error code and quit
                m_lLastError = ERROR_TIMEOUT;
                return m_lLastError;

            default:
                // Set the internal error code
                m_lLastError = ::GetLastError();

                // Issue an error and quit
                _RPTF0(_CRT_WARN,"CNTSerial::WaitEvent - Unable to wait until COM event has arrived\n");
                return m_lLastError;
            }
        }
    }
    else
    {
        // The operation completed immediatly. Just to be sure
        // we'll set the overlapped structure's event handle.
        ::SetEvent(lpOverlapped->hEvent);
    }

    // Return successfully
    return m_lLastError;
}


LONG CNTSerial::SetupHandshaking (EHandshake eHandshake)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::SetupHandshaking - Device is not opened\n");
        return m_lLastError;
    }

    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::SetupHandshaking - Unable to obtain DCB information\n");
        return m_lLastError;
    }

    // Set the handshaking flags
    switch (eHandshake)
    {
    case EHandshakeOff:
        dcb.fOutxCtsFlow = false;                    // Disable CTS monitoring
        dcb.fOutxDsrFlow = false;                    // Disable DSR monitoring
        dcb.fDtrControl = DTR_CONTROL_DISABLE;        // Disable DTR monitoring
        dcb.fOutX = false;                            // Disable XON/XOFF for transmission
        dcb.fInX = false;                            // Disable XON/XOFF for receiving
        dcb.fRtsControl = RTS_CONTROL_DISABLE;        // Disable RTS (Ready To Send)
        break;

    case EHandshakeHardware:
        dcb.fOutxCtsFlow = true;                    // Enable CTS monitoring
        dcb.fOutxDsrFlow = true;                    // Enable DSR monitoring
        dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;    // Enable DTR handshaking
        dcb.fOutX = false;                            // Disable XON/XOFF for transmission
        dcb.fInX = false;                            // Disable XON/XOFF for receiving
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;    // Enable RTS handshaking
        break;

    case EHandshakeSoftware:
        dcb.fOutxCtsFlow = false;                    // Disable CTS (Clear To Send)
        dcb.fOutxDsrFlow = false;                    // Disable DSR (Data Set Ready)
        dcb.fDtrControl = DTR_CONTROL_DISABLE;        // Disable DTR (Data Terminal Ready)
        dcb.fOutX = true;                            // Enable XON/XOFF for transmission
        dcb.fInX = true;                            // Enable XON/XOFF for receiving
        dcb.fRtsControl = RTS_CONTROL_DISABLE;        // Disable RTS (Ready To Send)
        break;

    default:
        // This shouldn't be possible
        _ASSERTE(false);
        m_lLastError = E_INVALIDARG;
        return m_lLastError;
    }

    // Set the new DCB structure
    if (!::SetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::SetupHandshaking - Unable to set DCB information\n");
        return m_lLastError;
    }

    // Return successful
    return m_lLastError;
}

LONG CNTSerial::SetupReadTimeouts (EReadTimeout eReadTimeout)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::SetupReadTimeouts - Device is not opened\n");
        return m_lLastError;
    }

    // Determine the time-outs
    COMMTIMEOUTS cto;
    if (!::GetCommTimeouts(m_hFile,&cto))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::SetupReadTimeouts - Unable to obtain timeout information\n");
        return m_lLastError;
    }

    // Set the new timeouts
    switch (eReadTimeout)
    {
    case EReadTimeoutBlocking:
        cto.ReadIntervalTimeout = 0;
        cto.ReadTotalTimeoutConstant = 0;
        cto.ReadTotalTimeoutMultiplier = 0;
        break;
    case EReadTimeoutNonblocking:
        cto.ReadIntervalTimeout = MAXDWORD;
        cto.ReadTotalTimeoutConstant = 0;
        cto.ReadTotalTimeoutMultiplier = 0;
        break;
    default:
        // This shouldn't be possible
        _ASSERTE(false);
        m_lLastError = E_INVALIDARG;
        return m_lLastError;
    }

    // Set the new DCB structure
    if (!::SetCommTimeouts(m_hFile,&cto))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::SetupReadTimeouts - Unable to set timeout information\n");
        return m_lLastError;
    }

    // Return successful
    return m_lLastError;
}

CNTSerial::EBaudrate CNTSerial::GetBaudrate (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::GetBaudrate - Device is not opened\n");
        return EBaudUnknown;
    }

    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::GetBaudrate - Unable to obtain DCB information\n");
        return EBaudUnknown;
    }

    // Return the appropriate baudrate
    return EBaudrate(dcb.BaudRate);
}

CNTSerial::EDataBits CNTSerial::GetDataBits (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::GetDataBits - Device is not opened\n");
        return EDataUnknown;
    }

    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::GetDataBits - Unable to obtain DCB information\n");
        return EDataUnknown;
    }

    // Return the appropriate bytesize
    return EDataBits(dcb.ByteSize);
}

CNTSerial::EParity CNTSerial::GetParity (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::GetParity - Device is not opened\n");
        return EParUnknown;
    }

    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::GetParity - Unable to obtain DCB information\n");
        return EParUnknown;
    }

    // Check if parity is used
    if (!dcb.fParity)
    {
        // No parity
        return EParNone;
    }

    // Return the appropriate parity setting
    return EParity(dcb.Parity);
}

CNTSerial::EStopBits CNTSerial::GetStopBits (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::GetStopBits - Device is not opened\n");
        return EStopUnknown;
    }

    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::GetStopBits - Unable to obtain DCB information\n");
        return EStopUnknown;
    }

    // Return the appropriate stopbits
    return EStopBits(dcb.StopBits);
}

DWORD CNTSerial::GetEventMask (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::GetEventMask - Device is not opened\n");
        return 0;
    }

    // Obtain the COM mask
    DWORD dwMask = 0;
    if (!::GetCommMask(m_hFile, &dwMask))
    {
        // Set the internal error code
        m_lLastError = ::GetLastError();

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::GetEventMask - Unable to get the event mask\n");
        return 0;
    }

    // Return the event mask
    return dwMask;
}

BYTE CNTSerial::GetEventChar (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::GetEventChar - Device is not opened\n");
        return 0;
    }

    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::GetEventChar - Unable to obtain DCB information\n");
        return 0;
    }

    // Set the new event character
    return BYTE(dcb.EvtChar);
}

CNTSerial::EHandshake CNTSerial::GetHandshaking (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::GetHandshaking - Device is not opened\n");
        return EHandshakeUnknown;
    }

    // Obtain the DCB structure for the device
    CDCB dcb;
    if (!::GetCommState(m_hFile,&dcb))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::GetHandshaking - Unable to obtain DCB information\n");
        return EHandshakeUnknown;
    }

    // Check if hardware handshaking is being used
    if ((dcb.fDtrControl == DTR_CONTROL_HANDSHAKE) && (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE))
        return EHandshakeHardware;

    // Check if software handshaking is being used
    if (dcb.fOutX && dcb.fInX)
        return EHandshakeSoftware;

    // No handshaking is being used
    return EHandshakeOff;
}

LONG CNTSerial::Write (const void* pData, size_t iLen, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
    // Overlapped operation should specify the pdwWritten variable
    _ASSERTE(!lpOverlapped || pdwWritten);

    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Use our own variable for read count
    DWORD dwWritten;
    if (pdwWritten == 0)
    {
        pdwWritten = &dwWritten;
    }

    // Reset the number of bytes written
    *pdwWritten = 0;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::Write - Device is not opened\n");
        return m_lLastError;
    }

    // Wait for the event to happen
    OVERLAPPED ovInternal;
    if (lpOverlapped == 0)
    {
        // Setup our own overlapped structure
        memset(&ovInternal,0,sizeof(ovInternal));
        ovInternal.hEvent = m_hevtOverlapped;

        // Use our internal overlapped structure
        lpOverlapped = &ovInternal;
    }

    // Make sure the overlapped structure isn't busy
    _ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

    // Write the data
    if (!::WriteFile(m_hFile,pData,iLen,pdwWritten,lpOverlapped))
    {
        // Set the internal error code
        long lLastError = ::GetLastError();

        // Overlapped operation in progress is not an actual error
        if (lLastError != ERROR_IO_PENDING)
        {
            // Save the error
            m_lLastError = lLastError;

            // Issue an error and quit
            _RPTF0(_CRT_WARN,"CNTSerial::Write - Unable to write the data\n");
            return m_lLastError;
        }

        // We need to block if the client didn't specify an overlapped structure
        if (lpOverlapped == &ovInternal)
        {
            // Wait for the overlapped operation to complete
            switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
            {
            case WAIT_OBJECT_0:
                // The overlapped operation has completed
                if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwWritten,FALSE))
                {
                    // Set the internal error code
                    m_lLastError = ::GetLastError();

                    _RPTF0(_CRT_WARN,"CNTSerial::Write - Overlapped completed without result\n");
                    return m_lLastError;
                }
                break;

            case WAIT_TIMEOUT:
                // Cancel the I/O operation
                ::CancelIo(m_hFile);

                // The operation timed out. Set the internal error code and quit
                m_lLastError = ERROR_TIMEOUT;
                return m_lLastError;

            default:
                // Set the internal error code
                m_lLastError = ::GetLastError();

                // Issue an error and quit
                _RPTF0(_CRT_WARN,"CNTSerial::Write - Unable to wait until data has been sent\n");
                return m_lLastError;
            }
        }
    }
    else
    {
        // The operation completed immediatly. Just to be sure
        // we'll set the overlapped structure's event handle.
        ::SetEvent(lpOverlapped->hEvent);
    }

    // Return successfully
    return m_lLastError;
}

LONG CNTSerial::Write (LPCSTR pString, DWORD* pdwWritten, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
    // Determine the length of the string
    return Write(pString,strlen(pString),pdwWritten,lpOverlapped,dwTimeout);
}

LONG CNTSerial::NTRead (void* pData, size_t iLen, DWORD* pdwRead, LPOVERLAPPED lpOverlapped, DWORD dwTimeout)
{
    // Overlapped operation should specify the pdwRead variable
    _ASSERTE(!lpOverlapped || pdwRead);

    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Use our own variable for read count
    DWORD dwRead;
    if (pdwRead == 0)
    {
        pdwRead = &dwRead;
    }

    // Reset the number of bytes read
    *pdwRead = 0;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::Read - Device is not opened\n");
        return m_lLastError;
    }

    // Wait for the event to happen
    OVERLAPPED ovInternal;
    if (lpOverlapped == 0)
    {
        // Setup our own overlapped structure
        memset(&ovInternal,0,sizeof(ovInternal));
        ovInternal.hEvent = m_hevtOverlapped;

        // Use our internal overlapped structure
        lpOverlapped = &ovInternal;
    }

    // Make sure the overlapped structure isn't busy
    _ASSERTE(HasOverlappedIoCompleted(lpOverlapped));

#ifdef _DEBUG
    // The debug version fills the entire data structure with
    // 0xDC bytes, to catch buffer errors as soon as possible.
    memset(pData,0xDC,iLen);
#endif
    
    // Read the data
    if (!::ReadFile(m_hFile,pData,iLen,pdwRead,lpOverlapped))
    {
        // Set the internal error code
        long lLastError = ::GetLastError();

        // Overlapped operation in progress is not an actual error
        if (lLastError != ERROR_IO_PENDING)
        {
            // Save the error
            m_lLastError = lLastError;

            // Issue an error and quit
            _RPTF0(_CRT_WARN,"CNTSerial::Read - Unable to read the data\n");
            return m_lLastError;
        }

        // We need to block if the client didn't specify an overlapped structure
        if (lpOverlapped == &ovInternal)
        {
            // Wait for the overlapped operation to complete
            switch (::WaitForSingleObject(lpOverlapped->hEvent,dwTimeout))
            {
            case WAIT_OBJECT_0:
                // The overlapped operation has completed
                if (!::GetOverlappedResult(m_hFile,lpOverlapped,pdwRead,FALSE))
                {
                    // Set the internal error code
                    m_lLastError = ::GetLastError();

                    _RPTF0(_CRT_WARN,"CNTSerial::Read - Overlapped completed without result\n");
                    return m_lLastError;
                }
                break;

            case WAIT_TIMEOUT:
                // Cancel the I/O operation
                ::CancelIo(m_hFile);

                // The operation timed out. Set the internal error code and quit
                m_lLastError = ERROR_TIMEOUT;
                return m_lLastError;

            default:
                // Set the internal error code
                m_lLastError = ::GetLastError();

                // Issue an error and quit
                _RPTF0(_CRT_WARN,"CNTSerial::Read - Unable to wait until data has been read\n");
                return m_lLastError;
            }
        }
    }
    else
    {
        // The operation completed immediatly. Just to be sure
        // we'll set the overlapped structure's event handle.
        ::SetEvent(lpOverlapped->hEvent);
    }

    // Return successfully
    return m_lLastError;
}

int CNTSerial::Flush()
{
    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::Read - Device is not opened\n");
        return m_lLastError;
    }

    if (!::PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_RXCLEAR))
    {
        // Set the internal error code
        m_lLastError = ::GetLastError();
        _RPTF0(_CRT_WARN,"CNTSerial::Read - Overlapped completed without result\n");
    }
    
    // Return successfully
    return m_lLastError;
}

CNTSerial::EEvent CNTSerial::GetEventType (void)
{
    // Obtain the event
    EEvent eEvent = m_eEvent;

    // Reset internal event type
    m_eEvent = EEventNone;

    // Return the current cause
    return eEvent;
}

CNTSerial::EError CNTSerial::GetError (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Check if the device is open
    if (m_hFile == 0)
    {
        // Set the internal error code
        m_lLastError = ERROR_INVALID_HANDLE;

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::GetError - Device is not opened\n");
        return EErrorUnknown;
    }

    // Obtain COM status
    DWORD dwErrors = 0;
    if (!::ClearCommError(m_hFile,&dwErrors,0))
    {
        // Set the internal error code
        m_lLastError = ::GetLastError();

        // Issue an error and quit
        _RPTF0(_CRT_WARN,"CNTSerial::GetError - Unable to obtain COM status\n");
        return EErrorUnknown;
    }

    // Return the error
    return EError(dwErrors);
}

bool CNTSerial::GetCTS (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Obtain the modem status
    DWORD dwModemStat = 0;
    if (!::GetCommModemStatus(m_hFile,&dwModemStat))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::GetCTS - Unable to obtain the modem status\n");
        return false;
    }

    // Determine if CTS is on
    return (dwModemStat & MS_CTS_ON) != 0;
}

bool CNTSerial::GetDSR (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Obtain the modem status
    DWORD dwModemStat = 0;
    if (!::GetCommModemStatus(m_hFile,&dwModemStat))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::GetDSR - Unable to obtain the modem status\n");
        return false;
    }

    // Determine if DSR is on
    return (dwModemStat & MS_DSR_ON) != 0;
}

bool CNTSerial::GetRing (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Obtain the modem status
    DWORD dwModemStat = 0;
    if (!::GetCommModemStatus(m_hFile,&dwModemStat))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::GetRing - Unable to obtain the modem status");
        return false;
    }

    // Determine if Ring is on
    return (dwModemStat & MS_RING_ON) != 0;
}

bool CNTSerial::GetRLSD (void)
{
    // Reset error state
    m_lLastError = ERROR_SUCCESS;

    // Obtain the modem status
    DWORD dwModemStat = 0;
    if (!::GetCommModemStatus(m_hFile,&dwModemStat))
    {
        // Obtain the error code
        m_lLastError = ::GetLastError();

        // Display a warning
        _RPTF0(_CRT_WARN,"CNTSerial::GetRLSD - Unable to obtain the modem status");
        return false;
    }

    // Determine if RLSD is on
    return (dwModemStat & MS_RLSD_ON) != 0;
}

