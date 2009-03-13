#include "MOOSGenLib/MOOSGenLibGlobalHelper.h"
#include "MOOSGenLib/MOOSAssert.h"
#include "MOOSLIB/XPCTcpSocket.h"
#include "MOOSLIB/MOOSCommClient.h"
#include "MOOSGenLib/MOOSLock.h"
#include "MOOSLIB/MOOSException.h"

#ifndef _WIN32
    #include <signal.h>
#endif

#include "HTTPConnection.h"
#include <sstream>

class CHTMLTag
{
private:
    std::ostringstream & m_Stream;
    std::string m_sTag;
    std::string m_sOptions;

public:
    CHTMLTag(std::ostringstream & sStream ,
        const std::string & sTag,
        const std::string & sOptions="",
        const std::string & sContents="",
        bool bNewLine=true):m_Stream(sStream),m_sTag(sTag)
    {
        Open(sStream,m_sTag,sOptions,sContents,bNewLine);
    }

    ~CHTMLTag()
    {
        Close(m_Stream,m_sTag);
    }

    static void Open(std::ostringstream & sStream,
        const std::string & sTag,
        const std::string & sOptions="",
        const std::string & sContents="", 
        bool bNewLine=false)
    {
        std::string sQ = bNewLine? "\r\n" : "";
        sStream<<"<"<<sTag<<" "<<sOptions<<">"<<sContents<<sQ;
    }

    static void Close(std::ostringstream & sStream,const std::string & sTag)
    {
        sStream<<"</"<<sTag<<">\r\n";
    }
    static std::string Print(const std::string & sTag,
        const std::string & sOptions="",
        const std::string & sContents="")
    {
        std::ostringstream  sStream;
        Open(sStream,sTag,sOptions,sContents);
        Close(sStream,sTag);
        return sStream.str();
    }

};


CHTTPConnection::CHTTPConnection(XPCTcpSocket * pSocket,
                                 CMOOSCommClient* pMOOSComms,
                                 CMOOSLock* pMOOSCommsLock):m_pSocket(pSocket),m_pMOOSComms(pMOOSComms),m_pMOOSCommsLock(pMOOSCommsLock)
{

}


bool CHTTPConnection::Run()
{
    m_ServeThread.Initialise(_CB,this);

    m_ServeThread.Start();

    return true;
}


bool CHTTPConnection::Serve()
{
 
    //ignore broken pipes as is standard for network apps
#ifndef _WIN32
    signal(SIGPIPE,SIG_IGN);
#endif
    
    try
    {
        if(ReadRequest())
        {
            if(!MakeWebPage())
            {
                SendFailureHeader();
            }
            else
            {
                SendHeader();
            }
            
            SendWebPage();
            
        }    
    }
    catch(XPCException e)
    {
        MOOSTrace("Exception caught in HTTP Connection server :\n%s\n",e.sGetException());
        UNUSED_PARAMETER(e);
    }
    
    //all done - clean up
    //strange behaviour on some combinations of clients and hosts
    //closing socket too quickly (and I mean with a few uS) prevents final read of page even though
    //XPCSocket reports all data is sent
    MOOSPause(100);

    m_pSocket->vCloseSocket();
    
    delete m_pSocket;
    
    return true;
}



bool CHTTPConnection::ReadRequest()
{    

    m_Request.Clean();

    //first line should always be obvious
    std::string sLine;
    if(!ReadLine(sLine))
        return false;

    std::string sWhat = MOOSChomp(sLine," ");
    std::string sPath = MOOSChomp(sLine," ");
    std::string sProtocol = MOOSChomp(sLine," ");

    //switch
    //get the file name
    //std::string sFile = sPath.substr(0,sPath.find_last_of("/"));

    //this is a Poke form
    std::string sFocus = MOOSChomp(sPath,"?");
    std::string sFormContents = sPath;

    //are we looking for a single variable?
    MOOSRemoveChars(sFocus,"/");
    if(!sFocus.empty())
    {
        m_sFocusVariable = sFocus;
    }
    else
    {
        m_sFocusVariable.clear();
    }
    

    if(!sFormContents.empty())
    {        
        HandlePoke(sFormContents);
    }

    //now read in rest of header lines
    do{
        //fetch a line but with speed as we know a request is being formed
        if(ReadLine(sLine))
        {
            //MOOSTrace("Rx: %s\n", sLine.c_str());
            if(!sLine.empty())
            {
                //we are in a header so parse a line
                m_Request.AddHeader(sLine);
            }
        }
        else
        {
            sLine.empty();
        }
    }while(sLine.size());

  

    return true;
}

void CHTTPConnection::SendLine(std::string sLine)
{
    sLine+="\r\n";
    int nSent = m_pSocket->iSendMessage((void*)(sLine.c_str()),sLine.size());
    if(nSent!=sLine.size())
	MOOSTrace("Failed tcp/ip send\n");
}

void CHTTPConnection::SendString(std::string sLine)
{
    m_pSocket->iSendMessage((void*)(sLine.c_str()),sLine.size());
}



bool CHTTPConnection::ReadLine(std::string & sLine)
{    
    sLine.clear();

    try
    {
        char t = '\0';
        while(1)
        {
            int nRead = m_pSocket->iRecieveMessage(&t,sizeof(t),0);
            if(nRead==1)
            {
                if(t!='\r')
                {
                    sLine+=t;
                }
                else
                {
                    break;
                }
            }
            if(nRead==0)
            {
                //time out or graceful closure
                return false;
            }
        }

    }
    catch(XPCException e)
    {
        UNUSED_PARAMETER(e);
        MOOSAssert(0);
    }
    
    MOOSRemoveChars(sLine,"\r\n");

    //MOOSTrace(sLine+"\n");
    return true;
}

bool CHTTPConnection::SendWebPage()
{   
   SendLine(m_sWebPage);

   return true;
}

char CharFromHex (std::string a)
{
    std::istringstream k (a);
    int Z;
    k >> std::hex >> Z;
    
    return char (Z); // cast to char and return
}

std::string Decode(std::string Text)
{
    
    std::string::size_type Pos;
    std::string Hex;
    while (std::string::npos != (Pos = Text.find('%')))
    {
        Hex = Text.substr(Pos + 1, 2);
        Text.replace(Pos, 3, 1, CharFromHex(Hex));
    }
    return Text;
    
}


bool CHTTPConnection::HandlePoke(std::string sPokeURL)
{
    //& seperates fields, + is space,
    std::map<std::string,std::string> Token2ValMap;
    while(!sPokeURL.empty())
    {
        std::string sPair =MOOSChomp(sPokeURL,"&");
        std::string sToken = MOOSChomp(sPair,"=");
        Token2ValMap[sToken] = sPair;
    }

    
    if(m_sFocusVariable.empty())
    {
        //this is a full webpage so the user shoudl have hit the Poke button
        if(Token2ValMap.find("VariableToPoke")==Token2ValMap.end())
            return false;
        if(Token2ValMap.find("DoPoke")==Token2ValMap.end())
            return false;

    }
    else
    {
        Token2ValMap["VariableToPoke"] =m_sFocusVariable;
    }

    if(Token2ValMap.find("NewValue")==Token2ValMap.end())
        return false;

    std::string sVar = Token2ValMap["VariableToPoke"];
    std::string sWhat = Token2ValMap["NewValue"];


    m_pMOOSCommsLock->Lock();
    {
        if(MOOSIsNumeric(sWhat))
        {
            m_pMOOSComms->Notify(sVar,atof(sWhat.c_str()));
        }
        else
        {
            m_pMOOSComms->Notify(sVar,Decode(sWhat));
        }
    }
    m_pMOOSCommsLock->UnLock();

    return true;
}


bool CHTTPConnection::MakeWebPage()
{
    //we can insert a quick check here to makes sure we aren't being
    //asked for things loke favicon.ico
    if(MOOSStrCmp(m_sFocusVariable, "favicon.ico"))
    {
        m_sWebPage = "Not Found";
        return false;
    }
        
    
    m_pMOOSCommsLock->Lock();

    try
    {
        if(!m_pMOOSComms->IsConnected())
            throw CMOOSException("No DB Connection");

        MOOSMSG_LIST MsgList;
        if(!m_pMOOSComms->ServerRequest("ALL",MsgList))
            throw CMOOSException("Failed ServerRequest");

        
        std::ostringstream wp;
        {
            {
                CHTMLTag HTML(wp,"HTML","style=\"font-family:arial\"");
                {
                    CHTMLTag Head(wp,"HEAD");
                    wp<<"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
                    //wp<<"<meta http-equiv=\"refresh\" content=\"0.5;URL=http://localhost:9080\">\n";
                    CHTMLTag Title(wp,"TITLE","","MOOSDB");
                }
                //body tag
                CHTMLTag BODY(wp,"BODY","BGCOLOR=white TEXT=black ");

                CHTMLTag CenteredTable(wp,"TABLE","width=\"100%\" height=\"100%\"");
                CHTMLTag CellRow(wp,"TR");
                CHTMLTag CellData(wp,"TD","valign=\"middle\" align=\"center\"");


                if(m_sFocusVariable.empty())
                {
                    //we have no focus variable eg localhost:9080/DB_TIME
                    //so show the whole load
                    BuildFullDBWebPageContents(wp,MsgList);
                }
                else
                {
                    //only interested in a single variable...
                    BuildSingleVariableWebPageContents(wp,MsgList);
                }
            }

            m_sWebPage = wp.str();

            //MOOSTrace(m_sWebPage);
        }

    }
    catch(CMOOSException e)
    {
        m_pMOOSCommsLock->UnLock();
        return false;
    }

    //success
    m_pMOOSCommsLock->UnLock();
    return true;


}

bool CHTTPConnection::BuildSingleVariableWebPageContents( std::ostringstream & wp,MOOSMSG_LIST & MsgList)
{
    // tell user what they are looking at
    wp<<CHTMLTag::Print("H1","ALIGN=middle",
        MOOSFormat("\"%s\" in MOOSDB : %s ",
        m_sFocusVariable.c_str(),
        m_pMOOSComms->GetDescription().c_str()));


    CMOOSMsg Msg;
    if(m_pMOOSComms->PeekMail(MsgList,m_sFocusVariable,Msg))
    {

        {
            CHTMLTag Table(wp,"TABLE","BORDER=1");

            //set up table headers...
            {
                CHTMLTag Row(wp,"TR");
                wp<<CHTMLTag::Print("TH","","Value");
                wp<<CHTMLTag::Print("TH","","Time");
                wp<<CHTMLTag::Print("TH","","Freq");
                wp<<CHTMLTag::Print("TH","","Source");
                wp<<CHTMLTag::Print("TH","","Community");
            }

            //data row
            {
                CHTMLTag Row(wp,"TR");
                {
                    CHTMLTag Form(wp,"FORM","action='/"+m_sFocusVariable+"'");

                    std::string sControl = MOOSFormat("<input name=NewValue value=%s>",Msg.GetAsString().c_str());
                    wp<<CHTMLTag::Print("TD","",sControl);
                }

                wp<<CHTMLTag::Print("TD","",MOOSFormat("%.3f",Msg.GetTime()));
                wp<<CHTMLTag::Print("TD","",MOOSFormat("%.3f",Msg.m_dfVal2));
                wp<<CHTMLTag::Print("TD","",Msg.GetSource());
                wp<<CHTMLTag::Print("TD","",Msg.GetCommunity());
            }
        }

        wp<<CHTMLTag::Print("p","","edit value box and press return to poke (set) this variable");  

    }
    else
    {
        wp<<CHTMLTag::Print("H2","ALIGN=middle,TEXT=red",MOOSFormat("The variable %s does not exist in this DB!",m_sFocusVariable.c_str()));
    }

    //add a refresh button
    wp<<"<p>\r\n";     


    wp<<CHTMLTag::Print("A","href = /","home");
    wp<<CHTMLTag::Print("A","href = /"+m_sFocusVariable,"refresh");
       

    return true;
}

bool CHTTPConnection::BuildFullDBWebPageContents( std::ostringstream & wp,MOOSMSG_LIST & MsgList)
{

    // tell user what they are looking at
    wp<<CHTMLTag::Print("H1","ALIGN=middle","Contents of MOOSDB : "+m_pMOOSComms->GetDescription());
    
    {
        CHTMLTag Table(wp,"TABLE","BORDER=1 WIDTH=\"80%\" ");

        //set up table headers...
        {
            CHTMLTag Row(wp,"TR");
            wp<<CHTMLTag::Print("TH","","Name");
            wp<<CHTMLTag::Print("TH","","Time");
            wp<<CHTMLTag::Print("TH","","Type");
            wp<<CHTMLTag::Print("TH","","Freq");
            wp<<CHTMLTag::Print("TH","","Source");
            wp<<CHTMLTag::Print("TH","","Community");
            wp<<CHTMLTag::Print("TH","","Value");



        }

        MOOSMSG_LIST::iterator q;

        for(q =  MsgList.begin(); q!=MsgList.end();q++)
        {
            CMOOSMsg & rMsg = *q;
            
            std::string sT;
            switch(rMsg.m_cDataType)
            {
                case MOOS_STRING:
                    sT = "$";
                    break;
                case MOOS_DOUBLE:
                    sT = "double";
                    break;
                case MOOS_NOT_SET:
                    sT = "pending";
                    break;
            }
          
            CHTMLTag Row(wp,"TR");
            wp<<CHTMLTag::Print("TD","valign=\"left\"",CHTMLTag::Print("A","href = /"+rMsg.GetName(),rMsg.GetName()));
            wp<<CHTMLTag::Print("TD","valign=\"left\"",MOOSFormat("%.3f",rMsg.GetTime()));
            wp<<CHTMLTag::Print("TD","valign=\"left\"",MOOSFormat("%s",sT.c_str() ) );
            wp<<CHTMLTag::Print("TD","valign=\"left\"",MOOSFormat("%.3f",rMsg.m_dfVal2));
            wp<<CHTMLTag::Print("TD","valign=\"left\"",rMsg.GetSource());
            wp<<CHTMLTag::Print("TD","valign=\"left\"",rMsg.GetCommunity());
            wp<<CHTMLTag::Print("TD","valign=\"left\" style=\"WORD-BREAK:BREAK-ALL\"",rMsg.GetAsString().c_str());

        }

    }


    wp<<CHTMLTag::Print("p","","Click on a name column for individual variable pages. <A href = /> refresh </A>");


    //make a Poke table
    {
        //wp<<CHTMLTag::Print("h2","","Poke");

        CHTMLTag Form(wp,"FORM","action='/'");
        {
            CHTMLTag Table(wp,"TABLE");
            {
                {
                    CHTMLTag Row(wp,"TR");                  
                    wp<<CHTMLTag::Print("TH","align=\"left\"","New Name");                    
                    wp<<CHTMLTag::Print("TH","align=\"left\"","New Value");
                }

                {
                    CHTMLTag Row(wp,"TR");                   
                    wp<<CHTMLTag::Print("TD","","<input name=VariableToPoke>");                    
                    wp<<CHTMLTag::Print("TD","","<input name=NewValue>");
                    wp<<CHTMLTag::Print("TD","","<input type=submit name=DoPoke value=Poke>");                  
                }                        
            }
        }

        wp<<CHTMLTag::Print("p","","Add a new MOOS variable or change an existing one");
    }

        
    return true;
}


bool CHTTPConnection::SendFailureHeader()
{
    SendLine("HTTP/1.1 404 Not Found");
    SendLine("NONE");
    SendLine("");
    
    return true;
}

bool CHTTPConnection::SendHeader()
{
      SendLine("HTTP/1.1 200 OK");
      SendLine("Content-Type: text/html; charset=ISO-8859-1");
      SendLine(MOOSFormat("Content-Length: %d",m_sWebPage.size()));
      SendLine("");

      return true;
}
bool CHTTPConnection::HasCompleted()
{
    return !m_ServeThread.IsThreadRunning();
}
