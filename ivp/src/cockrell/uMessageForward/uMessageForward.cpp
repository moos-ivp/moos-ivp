// Kevin Cockrell March 2007. 
// This program "forwards" messages from from AUV to another via the gateway computer.
// This program registers for NAFCON_MESSAGES3, NAFCON_MESSAGES4 and NAFCON_MESSAGES5
// It double checks to see that NAFCON_MESSAGESX came from a vehicle with SourcePlatformId=X
// by searching the contents of the NAFCON_MESSAGE for it's source ID. 

// It then creates two new nafcon messages, with SourcePlatformId=0,
// DestinationPlatformId=[(3 and 4) OR (3 and 5) OR (4 and 5)], 
 
// Note that this is hardwired for the following: 3 --> [4 and 5],
// 4 --> [3 and 5], 5 --> [3 and 4]
 
#include <iostream>
#include <sstream>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "uMessageForward.h"

using namespace std;


CuMessageForward::CuMessageForward()
{
 //Constructor

  debug = 1; //Used to control some MOOSTrace and .notify things that are
		//used for debugging

}

CuMessageForward::~CuMessageForward()
{
}

bool CuMessageForward::OnStartUp()
{
   
  return(true);
}

bool CuMessageForward::OnConnectToServer()
{
 
  m_Comms.Register("NAFCON_MESSAGES3",0);
  m_Comms.Register("NAFCON_MESSAGES4",0);
  m_Comms.Register("NAFCON_MESSAGES5",0);
  return(true);
}

bool CuMessageForward::Iterate()
{
//We don't do anything in here because the only time we need to do something is when we recieve a message.
  return(true);
}


//-------------------------------------------------------------
// Procedure: OnNewMail

bool CuMessageForward::OnNewMail(MOOSMSG_LIST &NewMail)

{
  MOOSMSG_LIST::iterator p;

  for(p = NewMail.begin(); p != NewMail.end(); p++) {
    CMOOSMsg &Msg = *p;

   if(Msg.m_sKey == "NAFCON_MESSAGES3")
      {
	if(debug)
		MOOSTrace("Received NAFCON_MESSAGES3\n");
	
	nafcon_messages3 = Msg.m_sVal;
	change_dest_id(nafcon_messages3,3);
      }
    else if(Msg.m_sKey == "NAFCON_MESSAGES4")
      {	
	if(debug)
		MOOSTrace("Received NAFCON_MESSAGES4\n");
        
	nafcon_messages4 = Msg.m_sVal;
	change_dest_id(nafcon_messages4,4);
      }
    
   else if(Msg.m_sKey == "NAFCON_MESSAGES5")
      {	
	if(debug)
		MOOSTrace("Received NAFCON_MESSAGES5\n");
        
	nafcon_messages5 = Msg.m_sVal;
	change_dest_id(nafcon_messages5,5);
      }

  }
  return(true);
}

void CuMessageForward::change_dest_id(std::string nafcon_message, int which_message)
{
	int which_vehicle = 0; //This will be set to the src_id of the vehicle that sent the message
	std::string temp1;
	std::string temp2;
	temp2 = nafcon_message;


	//First figure out which vehicle it came from. 
	MOOSChomp(temp2,"SourcePlatformId=");
	temp1 = MOOSChomp(temp2,",");
	

	
	if (temp1 == "3")
	{
		if(debug)
			MOOSTrace("Came from vehicle with src ID 3\n");
		which_vehicle = 3;
	}	
	if (temp1 == "4")
	{	
		if(debug)
			MOOSTrace("Came from vehicle with src ID 4\n");
		which_vehicle = 4;
	}
	if (temp1 == "5")
	{	
		if(debug)
			MOOSTrace("Came from vehicle with src ID 5\n");
		which_vehicle = 5;
	}

	if (debug)
	{
		MOOSTrace("which_vehicle = %d\n",which_vehicle);
		MOOSTrace("which_message = %d\n",which_message);
	}

	if(which_vehicle != which_message)
		MOOSTrace("SourcePlatformId inside NAFCON_MESSAGES does not correspond. Problem?");
	
	//Create a new message with the dest_id changed

	int index_dest_id = 0;	
	index_dest_id = nafcon_message.find("DestinationPlatformId=",1) + 22; //The 22 was found "experimentally"
	//it has something to do with the indexing of strings, and what .find returns. 

	int index_source_id = 0;	
	index_source_id = nafcon_message.find("SourcePlatformId=",1) + 17; //The 17 was found "experimentally"
	//it has something to do with the indexing of strings, and what .find returns. 

	//int index_node_specific_4 = 0;	
	//index_node_specific_4 = nafcon_message.find("NodeSpecificInfo4=",1) + 18; 

	if(debug)
	{
		MOOSTrace("index_dest_id = %d\n",index_dest_id);
		MOOSTrace("index_source_id = %d\n",index_source_id);
		//MOOSTrace("index_node_specific_4 = %d\n",index_node_specific_4);
	
	}

	
	
	std::string new_message_a = nafcon_message;
	std::string new_message_b = nafcon_message;

	if (which_vehicle == 3)
	{
		if(debug)
			MOOSTrace("Creating new messages (3)...\n");
		//Change DestinationPlatformId 
		new_message_a.replace(index_dest_id,1,"4");	
		new_message_b.replace(index_dest_id,1,"5");
		
		//Change SourcePlatformId 
		new_message_a.replace(index_source_id,1,"0");	
		new_message_b.replace(index_source_id,1,"0"); 

		//Change NodeSpecificInfo4 
		//new_message_a.replace(index_node_specific_4,1,"3");	
		//new_message_b.replace(index_node_specific_4,1,"3"); 
	
	

	}

	if (which_vehicle == 4)
	{
		if(debug)
			MOOSTrace("Creating new messages (4)...\n");	

		//Change DestinationPlatformId 
		new_message_a.replace(index_dest_id,1,"3");	
		new_message_b.replace(index_dest_id,1,"5");  
  
		//Change SourcePlatformId 
		new_message_a.replace(index_source_id,1,"0");	
		new_message_b.replace(index_source_id,1,"0"); 

		//Change NodeSpecificInfo5 
		//new_message_a.replace(index_node_specific_4,1,"4");	
		//new_message_b.replace(index_node_specific_4,1,"4"); 

	
	}

	if (which_vehicle == 5)
	{
		if(debug)
			MOOSTrace("Creating new messages (5)...\n");
		//Change DestinationPlatformId 
		new_message_a.replace(index_dest_id,1,"3");	
		new_message_b.replace(index_dest_id,1,"4"); 

		//Change SourcePlatformId 
		new_message_a.replace(index_source_id,1,"0");	
		new_message_b.replace(index_source_id,1,"0");  

		//Change NodeSpecificInfo4 
		//new_message_a.replace(index_node_specific_4,1,"5");	
		//new_message_b.replace(index_node_specific_4,1,"5"); 

	  
	}
	


	//Publish the new one
	if(which_message == 3)
	{
        	m_Comms.Notify("PLUSNET_MESSAGES4",new_message_a);
		m_Comms.Notify("PLUSNET_MESSAGES5",new_message_b);
		if(debug)
 			MOOSTrace("New Messages Have Been Published (3)\n\n\n");
	}
	if(which_message == 4)
	{
        	m_Comms.Notify("PLUSNET_MESSAGES3",new_message_a);
		m_Comms.Notify("PLUSNET_MESSAGES5",new_message_b);
		if(debug)
 			MOOSTrace("New Messages Have Been Published (4)\n\n\n");
	}
	if(which_message == 5)
	{
        	m_Comms.Notify("PLUSNET_MESSAGES3",new_message_a);
		m_Comms.Notify("PLUSNET_MESSAGES4",new_message_b);
		if(debug)
 			MOOSTrace("New Messages Have Been Published (5)\n\n\n");
	}


	
}

