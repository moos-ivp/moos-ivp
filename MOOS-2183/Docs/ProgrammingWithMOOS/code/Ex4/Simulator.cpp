
#include "Simulator.h"
#include <math.h>

//default constructor
CSimulator::CSimulator()
{
}

//default (virtual) destructor
CSimulator::~CSimulator()
{
}

/** 
Called by base class whenever new mail has arrived.
Place your code for handling mail (notifications that something
has changed in the MOOSDB in this function

Parameters:
    NewMail :  std::list<CMOOSMsg> reference

Return values:
    return true if everything went OK
    return false if there was problem
**/
bool CSimulator::OnNewMail(MOOSMSG_LIST &NewMail)
{
    return true;
}

/**
called by the base class when the application has made contact with
the MOOSDB and a channel has been opened. Place code to specify what
notifications you want to receive here.
**/
bool CSimulator::OnConnectToServer()
{
    return true;
}

/** Called by the base class periodically. This is where you place code
which does the work of the application **/
bool CSimulator::Iterate()
{ 
    static int k = 0; // a simple counter to simulate a simulator -not and important detail...
    if(k++%10==0)
    {
    //simulate some brownina motion
    m_dfHeading+=MOOSWhiteNoise(0.1); 

    //publish the data (2nd param is a double so it will be forever double data...)
    std::string sVarName = m_sVehicleName+"_Heading";
    m_Comms.Notify(sVarName,m_dfHeading,MOOSTime());
    }
    if(k%35==0)
    {
    
    m_dfBatteryVoltage-=fabs(MOOSWhiteNoise(0.1));
    std::string sStatus = MOOSFormat("Status=%s,BatteryVoltage=%.2f,Bilge = %s",
                     m_dfBatteryVoltage>50.0? "Good":"Bad",
                     m_dfBatteryVoltage,
                     m_sBilge.c_str());

    //publish the data (2nd param is a std::string so it will be forever string data...)
    //note how name of variable is set by what was rad from configuration file
    std::string sVarName = m_sVehicleName+"_Status";
    m_Comms.Notify(m_sVehicleName,sStatus,MOOSTime());
    }
    return true;
}

/** called by the base class before the first ::Iterate is called. Place
startup code here - especially code whic reads configuration data from the 
mission file **/
bool CSimulator::OnStartUp()
{       
    //here we extract the vehicle name..
    m_sVehicleName = "UnNamed";
    if(!m_MissionReader.GetConfigurationParam("VehicleName",m_sVehicleName))
    MOOSTrace("Warning parameter \"VechicleName\" not specified. Using default of \"%s\"\n",m_sVehicleName.c_str());


    //here we extract a vector of doubles fom the configuration file
    std::vector<double> vInitialLocation(3,0.0);
    int nRows=vInitialLocation.size();
    int nCols = 1;
    if(!m_MissionReader.GetConfigurationParam("InitialLocation",vInitialLocation,nRows,nCols))
    MOOSTrace("Warning parameter \"InitialLocation\" not specified. Using default of \"%s\"\n",DoubleVector2String(vInitialLocation).c_str());
    
    //here we extrac a more compicated compound string parameter
    std::string sComplex;
    if(m_MissionReader.GetConfigurationParam("InitialConditions",sComplex))
    {
    //OK now we can suck out individual parameters from sComplex

    //what is the initial Bilge condition status?
    m_sBilge = "Off";
    MOOSValFromString(m_sBilge,sComplex,"Bilge");

    //what is the initial battery Voltage?
    m_dfBatteryVoltage = 100.0;
    MOOSValFromString(m_dfBatteryVoltage,sComplex,"BatteryVoltage");

    //what is the initial heading
    m_dfHeading = 0;
    MOOSValFromString(m_dfHeading,sComplex,"Heading");

    }
    else
    {
    //bad news - this one is compulsory for this application...
    return MOOSFail("no \"InitialConditions\" specified in mission file (compulsory)\n");
    }


    MOOSTrace("Verbose Summary:\n");
    MOOSTrace("\tVehicle is called : %s\n",m_sVehicleName.c_str());
    MOOSTrace("\tInitial Location is  : %s\n",DoubleVector2String(vInitialLocation).c_str());
    MOOSTrace("\tHeading is  : %f\n",m_dfHeading);
    MOOSTrace("\tBatteryVoltage is  : %s\n",m_sBilge.c_str());



    return true;
}
