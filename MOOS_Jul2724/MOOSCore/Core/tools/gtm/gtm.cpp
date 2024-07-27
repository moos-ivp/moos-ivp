#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

const std::string kNoConsumer = "no consumer";
const std::string kNoProducer = "no producer";
const double kSamplePeriod = 3.0;

std::string app_name;

class GraphTheMOOS :  public CMOOSApp
{
public:
    GraphTheMOOS()
    {
        m_Comms.SetQuiet(true);
        SetQuiet(true);
    }
    bool OnConnectToServer()
    {
        m_Comms.EnableCommsStatusMonitoring(true);
        return true;
    }

    bool Iterate()
    {
        std::list<MOOS::ClientCommsStatus>  Statuses;
        std::list<MOOS::ClientCommsStatus>::iterator q;
        m_Comms.GetClientCommsStatuses(Statuses);

        for(q=Statuses.begin();q!= Statuses.end();++q)
        {
            statuses_[q->name_] = *q;
        }

        UpdateTopology();

        double time_expired = MOOS::Time()-CMOOSApp::GetAppStartTime();
        if(time_expired>kSamplePeriod)
        {
            PrintTopologyToFile();
            exit(0);
        }
        else
        {
            std::cerr<<"sampling....please  wait "<<std::setprecision(1)<<5.0-time_expired<< " seconds\r";
        }

        return true;

    }


    void UpdateTopology()
    {
        std::map<std::string,MOOS::ClientCommsStatus>::iterator q,p;

        //for every client that posted a status message...`
        for(q=statuses_.begin();q!= statuses_.end();++q)
        {
            MOOS::ClientCommsStatus & qStatus = (q->second);
            std::list<std::string>::iterator s;

            //skip our own subscriptions
            if(app_name==q->first)
                continue;

            //for each message published
            for( s = qStatus.publishes_.begin();s !=qStatus.publishes_.end();++s)
            {

                message & rmessage = known_messages_[*s];
                rmessage.producers_.insert(q->first);
            }


            for( s = qStatus.subscribes_.begin();s !=qStatus.subscribes_.end();++s)
            {
                message & rmessage = known_messages_[*s];
                rmessage.consumers_.insert(q->first);
            }
        }
    }


    bool IsIgnoredMessage(const std::string & name)
    {
        if(MOOSWildCmp("*_STATUS",name))
        {
            return true;
        }

        return false;
    }


    void PrintMessageRoute(std::ofstream &f,
                           const std::string & src,
                           const std::string &dest,
                           const std::string & name,
                           bool dotted,
                           bool ommit_leg_1)
    {

        if(IsIgnoredMessage(name))
            return;

        if(!ommit_leg_1)
        {
            f<<"\""<<src<<"\""
                << "->"
                <<"\""<< name<< "\""
                << "->"
                <<"\""<< dest<< "\""
                << (dotted ? "[style=dotted]" :   "[style=solid]")
                <<"\n";
        }
        else
        {
            f <<"\""<< name<< "\""
              << "->"
              <<"\""<< dest<< "\""
              << (dotted ? "[style=dotted]" :   "[style=solid]")
              <<"\n";
        }
    }

    void PrintTopologyToFile()
    {

        std::ofstream output_file("moos.dot");
        output_file<<"digraph{\n";
        output_file<<"size = \"10,10\"\n";

        std::cerr<<"Connectivity detected is as follows \n\n";

        std::map<std::string,MOOS::ClientCommsStatus>::iterator q,p;

        //for every client that posted a status message...`
        for(q=statuses_.begin();q!= statuses_.end();++q)
        {
            MOOS::ClientCommsStatus & qStatus = (q->second);
            std::list<std::string>::iterator s;
            std::set<std::string> links_made_already;

            //for each message published
            for( s = qStatus.publishes_.begin();s !=qStatus.publishes_.end();++s)
            {
                //find all the consumers...
                message & rmessage = known_messages_[*s];
                std::set<std::string>::iterator w;
                for(w = rmessage.consumers_.begin();
                    w!=rmessage.consumers_.end();
                    ++w)
                {
                    std::cerr<<q->first<<" sends "<<*s<<" to "<<*w<<"\n";

                    if(links_made_already.count(*s)==0)
                    {
                        links_made_already.insert(*s);

                        PrintMessageRoute(output_file,q->first,*w,*s,false,false);
                    }
                    else
                    {
                        PrintMessageRoute(output_file,q->first,*w,*s,false,true);

                    }
                }
            }
        }

        //here we look for the two kinds of orphans
        std::map<std::string, message>::iterator m;
        for(m = known_messages_.begin();
            m!= known_messages_.end();
            ++m)
        {
            message & rmessage = m->second;
            if(rmessage.consumers_.empty())
            {
                rmessage.has_no_consumer_ = true;
                std::set<std::string>::iterator w;
                for(w = rmessage.producers_.begin();
                    w!=rmessage.producers_.end();
                    w++)
                {
                    std::cerr<<MOOS::ConsoleColours::Red();
                    std::cerr<<*w<<" sends "<<m->first<<" to "<<kNoConsumer<<"\n";
                    std::cerr<<MOOS::ConsoleColours::reset();

                    PrintMessageRoute(output_file,*w,kNoConsumer,m->first,true,false);
                }
            }

            if(rmessage.producers_.empty())
            {
                rmessage.has_no_producer_ = true;
                std::set<std::string>::iterator w;
                for(w = rmessage.consumers_.begin();
                    w!=rmessage.consumers_.end();
                    w++)
                {
                    std::cerr<<MOOS::ConsoleColours::Red();
                    std::cerr<<*w<<" wants "<<m->first<<" from  "<<kNoProducer<<"\n";
                    std::cerr<<MOOS::ConsoleColours::reset();

                    PrintMessageRoute(output_file,kNoProducer,*w,m->first,true,false);
                }
            }

        }


        bool need_no_producer = false;
        bool need_no_consumer = false;

        std::map<std::string, message>::iterator mm;
        for(mm = known_messages_.begin();
            mm!= known_messages_.end();
            ++mm)
        {
            message & rmessage = mm->second;

            if(IsIgnoredMessage(mm->first))
                continue;

            std::string params;
            if(rmessage.has_no_consumer_){
                need_no_consumer= true;
                params = "[shape=box,color=red,style=dotted]";
            }
            else if(rmessage.has_no_producer_){
                need_no_producer= true;
                params = "[shape=box,color=red,style=dotted]";
            }
            else{
                params = "[shape=box,color=blue]";
            }

            output_file<<"\""<<mm->first<<"\""<<params<<"\n";
        }

        if(need_no_consumer){
            output_file<<"\""<<kNoConsumer<<"\" [style=filled,fillcolor=red]\n";
        }
        if(need_no_producer){
            output_file<<"\""<<kNoProducer<<"\" [style=filled,fillcolor=red]\n";
        }


        output_file<<"}\n";



    }

    void OnPrintHelpAndExit()
    {
        MOOSTrace("  capture MOOS comms topology to a .gv file ");
        MOOSTrace("\n\noptions:\n");
        MOOSTrace("  --pattern=<string>     : globbing pattern for client name\n");
        MOOSTrace("\n\nexample:\n");
        MOOSTrace("  ./gtm --pattern='foo*'\n");
        exit(0);
    }

    bool OnProcessCommandLine()
    {
        std::string t;
        if(m_CommandLineParser.GetVariable("--pattern",t))
            Wildcard_names_=MOOS::StringListToVector(t,",");
        else
            Wildcard_names_.push_back("*");
        return true;
    }
    bool OnStartUp()
    {
        SetAppFreq(1.0);
        return true;
    }

protected:
    std::vector<std::string> Wildcard_names_;

    std::map<std::string,MOOS::ClientCommsStatus> statuses_;

    //this is a message type
    struct message
    {
        message(){};
        explicit message(const std::string & s): name_(s),has_no_consumer_(false),has_no_producer_(false){}
        std::string name_;
        std::set<std::string> producers_;
        std::set<std::string> consumers_;
        bool has_no_consumer_;
        bool has_no_producer_;
    };

    //these are all the known messages.
    std::map<std::string, message> known_messages_;

};


int main(int argc , char* argv[])
{
    //here we do some command line parsing...
    MOOS::CommandLineParser P(argc,argv);

    //we may want many instances run from command line so lets guess
    //a random name. This is just to stop users having to specify
    //--moos_name at the command line lots and lots...
    std::stringstream ss;
    srand ( static_cast<unsigned int>( time(NULL)) );
    ss<<"gtm-"<< rand() %1024;
    app_name = ss.str();
    app_name = P.GetFreeParameter(1, app_name);



    GraphTheMOOS gtm;
    gtm.Run(app_name,argc,argv);
}
