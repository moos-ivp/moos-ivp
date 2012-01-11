#include <MOOSLIB/MOOSApp.h>
#include <iostream>


//a very simple example on using MOOS's binary data transport.
// to run as a publisher pushing 1MB of data:
//   ./binary_comms_example 1000000
//
// to run a a consumer of this data:
//  ./binary_comms_example
//


class CMessager: public CMOOSApp
{
public:
    CMessager():_nSize(-1),_bPublisher(false){};


    bool OnNewMail(MOOSMSG_LIST & Mail)
    {

        for(MOOSMSG_LIST::iterator q = Mail.begin();q!=Mail.end();q++)
            q->Trace();
        return true;
    }
    bool Iterate()
    {
        if(_bPublisher &&_nSize>0)
        {
            //make a big vector and send it...
            std::vector<char> VastMessage(_nSize);
            m_Comms.Notify("BigThing",(VastMessage.data()),_nSize);
        }
        return true;
    }
    bool OnConnectToServer()
    {
        if(!_bPublisher)
            m_Comms.Register("BigThing",0);
        return true;
    }

   bool OnStartup()
   {
       if(!_bPublisher)
           m_Comms.Register("BigThing",0);
   }



    void MakePublisher(int nSize)
    {
        _nSize = nSize;
        _bPublisher = true;
        std::cout<<"will publish "<<_nSize<<" bytes"<<std::endl;
    }

protected:
    int _nSize;
    bool _bPublisher;
};


int main(int argc, char * argv[])
{
    CMessager M;

    std::string sMyName = "binary_tester";

    if(argc>1)
    {
        M.MakePublisher(atoi(argv[1]) );
        sMyName+="_publisher";
    }
    else
    {
        sMyName+="_consumer";
        std::cout<<"I will subscribed to BigThing\n";
    }

    M.Run(sMyName.c_str(),"mission.moos");
}
