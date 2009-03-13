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
// BluefinBatteryDriverV1.cpp: implementation of the CBluefinBatteryDriverV1 class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif

#include <MOOSGenLib/MOOSGenLib.h>
#include <strstream>
#include "BluefinBatteryDriverV1.h"


#define NORMAL_CELL_LOW  3.75
#define NORMAL_CELL_HIGH  4.2
#define NORMAL_PACK_LOW 8*NORMAL_CELL_LOW
#define NORMAL_PACK_HIGH 8*NORMAL_CELL_HIGH


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBluefinBatteryDriverV1::CBluefinBatteryDriverV1()
{
    m_Batteries[0].m_nPackAddress = 0x02;
    m_Batteries[1].m_nPackAddress = 0x05;

    m_Batteries[0].m_dfVoltage = -1;
    m_Batteries[0].m_dfVoltage = -1;


}

CBluefinBatteryDriverV1::~CBluefinBatteryDriverV1()
{
    Switch(false);

}

bool CBluefinBatteryDriverV1::Initialise()
{
    return Switch(true);
}


bool CBluefinBatteryDriverV1::GetData()
{
    //read from harware sensor here...
    if(!GetCellData(0))
    {
        MOOSTrace("Can't reach pack 0\n");
    }


    if(!GetCellData(1))
    {
        MOOSTrace("Can't reach pack 1\n");
    }

    if(!GetPackVoltage(0))
    {
        MOOSTrace("Can't reach pack 0\n");
    }

    if(!GetPackVoltage(1))
    {
        MOOSTrace("Can't reach pack 1\n");
    }

    m_Status.m_dfVoltage = GetMeanVoltage();

    return true;
}



bool CBluefinBatteryDriverV1::GetCellData(int nPack)
{
    int nAddr = m_Batteries[nPack].m_nPackAddress;

    BYTE TxContent[10];
    BYTE RxContent[10];

    TxContent[0] = nAddr&0xFF;
    TxContent[1] = 0x03;

    for(int i = 0; i<CELLS_PER_PACK;i++)
    {
        TxContent[2] = i&0xFF;
        if(SendAndRecieve(TxContent,3,RxContent,4))
        {
            int nLevel =( RxContent[1]<<8) + RxContent[2];
            double dfVoltage = nLevel*5.0/4095.0;
//            MOOSTrace("Voltage Pack[%d],Cell[%d] = %f \n",nPack,i+1,dfVoltage);

            m_Batteries[nPack].m_Cells[i].m_dfVoltage=dfVoltage;
        }
    }

    return true;
}

bool CBluefinBatteryDriverV1::SendAndRecieve(BYTE *pTx, int nTx, BYTE *pRx, int nRx)
{
    int nMsgLen = nTx+5;
    BYTE Msg[20];

    int nCheckSum = 0;
    for(int i = 0;i<nTx;i++)
    {
        nCheckSum^=pTx[i];
    }
    
    Msg[0] = 0xAA;
    Msg[1] = 0x00;
    memcpy(&Msg[2],pTx,nTx);
    Msg[2+nTx] = nCheckSum&0xFF;
    Msg[3+nTx] = 0xAA;
    Msg[4+nTx] = 0x01;

    m_pPort->Write((char*)Msg,nMsgLen);


    //note we seem to be getting an extra 0x00 at teh end of
    //some messages!!!
    int nReplyLen = nRx+6;
    BYTE Reply[100];

    int nRead = m_pPort->ReadNWithTimeOut((char*)Reply,nReplyLen,0.1);

    bool bSuccess = (nRead >0 && nRead>= nRx+2)?true:false;
    if(bSuccess)
    {
        memcpy(pRx,&Reply[2],nRx);
    }


    return bSuccess;
}

bool CBluefinBatteryDriverV1::SwitchPack(int nPack, bool bOn)
{
    int nAddr = m_Batteries[nPack].m_nPackAddress;


    /*
    Battery on:
                       addr type (forced operational)
                     |    |    |
    bat4:  0xAA 0x00 0x04 0x02 0x06 0xAA CRC 0x01
    bat5:  0xAA 0x00 0x05 0x02 0x06 0xAA CRC 0x01

    Battery off:
                     addr type (off)
                      |    |    |
    bat4:  0xAA 0x00 0x04 0x02 0x00 0xAA 0x01
    bat5:  0xAA 0x00 0x05 0x02 0x00 0xAA 0x01
    */

    BYTE TxContent[3];
    BYTE RxContent[2];

    TxContent[0] = nAddr&0xFF;
    TxContent[1] = 0x02;
    TxContent[2] = bOn? 0x06 : 0x00;

    if(SendAndRecieve(TxContent,3,RxContent,2))
    {
        if(RxContent[0]==0x0 && RxContent[1]==0x01)
            return true;
    }
    return false;
}

bool CBluefinBatteryDriverV1::GetPackStatus(int nPack)
{
    int nAddr = m_Batteries[nPack].m_nPackAddress;
    BYTE TxContent[2];
    BYTE RxContent[3];

    TxContent[0] = nAddr&0xFF;
    TxContent[1] = 0x01;

    if(SendAndRecieve(TxContent,2,RxContent,3))
    {
        BYTE Status = RxContent[1];
        if(Status&&0x6)
        {
            m_Batteries[nPack].m_eState = ON;
            MOOSTrace("Battery[%d] is ON\n",nPack);
        }
        else
        {
            m_Batteries[nPack].m_eState = OFF;
            MOOSTrace("Battery[%d] is OFF\n",nPack);
        }
    }

    return true;

}

bool CBluefinBatteryDriverV1::GetPackVoltage(int nPack)
{
    int nAddr = m_Batteries[nPack].m_nPackAddress;
    BYTE TxContent[3];
    BYTE RxContent[3];

    TxContent[0] = nAddr&0xFF;
    TxContent[1] = 0x06;
    TxContent[2] = 0x03;

    if(SendAndRecieve(TxContent,3,RxContent,3))
    {
        int nLevel =( RxContent[1]<<8) + RxContent[2];
        double dfVoltage = nLevel*60.0/4095.0;
        m_Batteries[nPack].m_dfVoltage = dfVoltage;
        //MOOSTrace("TOTAL Voltage Pack[%d] %f \n",nPack,dfVoltage);
        return true;
    }

    return false;
    
}

bool CBluefinBatteryDriverV1::IsError()
{
    //here we embedd the rules....
    m_sError = "";

    for(int nPack = 0;nPack<2;nPack++)
    {
    int nCell=0;
        for(nCell = 0; nCell<CELLS_PER_PACK;nCell++)
        {
            double dfVoltage = m_Batteries[nPack].m_Cells[nCell].m_dfVoltage;

            if(dfVoltage<NORMAL_CELL_LOW)
            {
                char sError[100];
                sprintf(sError,"Cell %d on pack %d is undervoltage %f",
                    nCell,
                    nPack,
                    dfVoltage);
                m_sError = sError;

                SwitchPack(nPack,false);

                return true;
            }

            if(dfVoltage>NORMAL_CELL_HIGH)
            {
                char sError[100];
                sprintf(sError,"Cell %d on pack %d is overvoltage %f (stop charging if applicable)",
                    nCell,
                    nPack,
                    dfVoltage);
                m_sError = sError;

                SwitchPack(nPack,false);

                return true;
            }
        }

        //now check total voltages!!!
        if(m_Batteries[nPack].m_dfVoltage<NORMAL_PACK_LOW)
        {
            char sError[100];
            sprintf(sError,"Pack %d is undervoltage %f ",
                nCell,
                nPack,
                m_Batteries[nPack].m_dfVoltage);
            m_sError = sError;

            SwitchPack(nPack,false);

            return true;
        }

        if(m_Batteries[nPack].m_dfVoltage>NORMAL_PACK_HIGH)
        {
            char sError[100];
            sprintf(sError,"Pack %d is overvoltage %f (stop charging if applicable)",
                nCell,
                nPack,
                m_Batteries[nPack].m_dfVoltage);
            m_sError = sError;

            SwitchPack(nPack,false);

            return true;
        }       
    }



    return false;

}

double CBluefinBatteryDriverV1::GetMeanVoltage()
{
    return (m_Batteries[0].m_dfVoltage+m_Batteries[0].m_dfVoltage)/2;
}

bool CBluefinBatteryDriverV1::Switch(bool bSwitch)
{
    if(!SwitchPack(0,bSwitch))
    {
        MOOSTrace("Can't reach pack 0\n");
    }

    if(!SwitchPack(1,bSwitch))
    {
        MOOSTrace("Can't reach pack 0\n");
    }

    GetPackStatus(0);
    GetPackStatus(1);
   
    return true;
}

string CBluefinBatteryDriverV1::GetCellsString()
{
    ostrstream os;

    for(int nPack = 0;nPack<2;nPack++)
    {
        os<<"PACK="<<nPack<<":";
        os<<"V="<<m_Batteries[nPack].m_dfVoltage<<",";
        os<<"Cells=[";

        for(int nCell = 0; nCell<CELLS_PER_PACK;nCell++)
        {
            os<<m_Batteries[nPack].m_Cells[nCell].m_dfVoltage<<",";
        }

        os<<"];";
    }

    os<<ends;

    string sResult = os.str();

    os.rdbuf()->freeze(0);

    return sResult;
}
