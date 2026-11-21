/*
 * ktm.cpp
 *
 *  Created on: Feb 6, 2014
 *      Author: pnewman
 */

#include <string>
#include <map>

#include <cstring>
#include <stdexcept>
#include <iomanip>

#include "MOOS/libMOOS/Utils/CommandLineParser.h"
#include "MOOS/libMOOS/Utils/ThreadPrint.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include "MOOS/libMOOS/Comms/MulticastNode.h"
#include "MOOS/libMOOS/Comms/EndToEndAudit.h"

const int kFlusheveryN = 1;
const int64_t kDefaultIgnoreDelayBelow=3000;

const std::string kDefaultLogFile = "default.moos_audit.log";
const std::string kDefaultAuditDirectory = "moos_comms_audits";

std::string multicast_address = "224.1.1.8";
int multicast_port = 4000;
std::string name_pattern;
double kBandwidthFIRConstant = 0.01;


struct atm_stat{
    double average_bandwidth;
    std::string source;
    std::string destination;
    std::string name;
    int64_t last_write_time;
};
std::map<std::string, atm_stat> atm_stats;

bool sort_on_bw(const atm_stat & a, const atm_stat &b){
    return a.average_bandwidth>b.average_bandwidth;
}


std::string FormatBytes(int n) {
    std::stringstream ss;
    if (n == 0) {
        ss << n;
    } else if (n > (1 << 30)) {
        ss << (n >> 30) << " GB/s";
    } else if (n > (1 << 20)) {
        ss << (n >> 20) << " MB/s";
    } else if (n > (1 << 10)) {
        ss << (n >> 10) << " kB/s";
    } else {
        ss << " B/s";
    }

    return ss.str();
}


void PrintStatistics(){
    std::cerr<<"bandwidth by channel\n";

    std::list<atm_stat> stat_list;
    std::map<std::string ,atm_stat>::iterator p;
    for(p=atm_stats.begin();p!=atm_stats.end();++p){
        stat_list.push_back(p->second);
    }

    stat_list.sort(sort_on_bw);

    std::list<atm_stat>::iterator q;
    for(q=stat_list.begin();q!=stat_list.end();++q){
        std::stringstream ss;

        ss<<std::setw(8)<<FormatBytes(int(q->average_bandwidth))<<" ";
        ss<<std::setw(15)<<"\""<<q->name<<"\""
         <<" from "<<q->source
         <<" to "<<q->destination<<"\n";
        std::cerr<<ss.str();
    }
}

void AccummulateStatistic(MOOS::EndToEndAudit::MessageStatistic & ms){
    std::string key= ms.message_name+":"+ms.source_client+":"+ms.destination_client;
    std::map<std::string, atm_stat>::iterator q;
    q = atm_stats.find(key);
    if(q==atm_stats.end()){
        atm_stat new_statistic;
        new_statistic.name = ms.message_name;
        new_statistic.destination = ms.destination_client;
        new_statistic.source = ms.source_client;
        new_statistic.average_bandwidth = 0.0;
        new_statistic.last_write_time = ms.receive_time;
        atm_stats[key] = new_statistic;
    }else{
        atm_stat & stat=q->second;
        int64_t dt = ms.receive_time-stat.last_write_time+1;
        double point_estimate_bandwidth = (1e6*ms.message_size)/dt;
        stat.last_write_time = ms.receive_time;
        stat.average_bandwidth = (1.0-kBandwidthFIRConstant)*stat.average_bandwidth+
                kBandwidthFIRConstant*point_estimate_bandwidth;
    }


}



void PrintHelpAndExit()
{
    std::cerr<<"audit the MOOS (atm)\n";
    std::cerr<<"--channel=<address>     address to listen  for audit comms on \n";
    std::cerr<<"--port=<int>            port to listen on \n";
    std::cerr<<"--ignore_delay_below_us ignore delays less than this in uS\n";
    std::cerr<<"--log                   log statistics\n";
    std::cerr<<"--log_directory=<dir>   log directory";
    std::cerr<<"--log_name=<name>       log file name";

    exit(0);
}


void MakeLoggingDirectory(const std::string & logging_directory_name){
    if(!MOOSCreateDirectory(logging_directory_name)){
        std::cerr<<"failed to create audit directory "<<logging_directory_name<<"\n";
        exit(-1);
    }
}

void OpenLogFile(std::ofstream & ofs,
                 const std::string & logging_directory,
                 std::string & logfile_name){
    MakeLoggingDirectory(logging_directory);

    std::string log_file_path;
    if(logfile_name.empty()){
        log_file_path = logging_directory+"/"+MOOSGetTimeStampString()+".moos_audit.log";
    }else{
        log_file_path = logging_directory+"/"+logfile_name;
    }
    ofs.open(log_file_path.c_str(),std::ios::ate | std::ios::out);
    if(!ofs){
        std::cerr<<"failed to open log file "<<log_file_path<<"\n";
        exit(-1);
    }

    std::cerr<<"appending audit data to to "<<log_file_path<<"\n";
}


void PrettyPrint(std::ostream & output_stream,const MOOS::EndToEndAudit::MessageStatistic & ms){
    static double last = ms.source_time;
    output_stream<<std::left<<std::setw(7)<<std::setprecision(12)<<ms.source_time-last<<" ";
    output_stream<<std::left<<std::setw(7)<<std::setprecision(4);
    output_stream<<(ms.receive_time-ms.source_time)/1000.0<<" ms delay ";
    output_stream<<ms.source_client<<" |----  "<<ms.message_name<<"["<<ms.message_size<<"]  ----> "<<ms.destination_client<<std::endl;
    last = ms.source_time;
}

void LogToFile(std::ofstream & output_stream,const MOOS::EndToEndAudit::MessageStatistic & ms){

    output_stream<<std::left<<std::setw(15);
    output_stream<<ms.receive_time-ms.source_time<<",";

    output_stream<<std::left<<std::setw(15);
    output_stream<<ms.message_name<<",";

    output_stream<<std::left<<std::setw(15);
    output_stream<<ms.source_client<<",";

    output_stream<<std::left<<std::setw(15);
    output_stream<<ms.destination_client<<",";

    output_stream<<std::left<<std::setw(20);
    output_stream<<ms.receive_time<<",";

    output_stream<<std::left<<std::setw(15);
    output_stream<<ms.cpu_load<<"\n";

    static int lines_logged = 0;
    if(lines_logged++%kFlusheveryN==0){
        output_stream.flush();
        std::cerr<<"\r"<<std::left<<std::setw(15)
                <<lines_logged<<" audit records written";
    }

}


void WriteHeader(std::ofstream & output_stream){
    output_stream<<std::left<<std::setw(15);
    output_stream<<"%delay(uS)"<<",";

    output_stream<<std::left<<std::setw(15);
    output_stream<<"name"<<",";

    output_stream<<std::left<<std::setw(15);
    output_stream<<"src"<<",";

    output_stream<<std::left<<std::setw(15);
    output_stream<<"dest"<<",";

    output_stream<<std::left<<std::setw(20);
    output_stream<<"rx_time"<<",";

    output_stream<<std::left<<std::setw(15);
    output_stream<<"cpu_load"<<"\n";
}


bool ShouldBeLogged(const MOOS::EndToEndAudit::MessageStatistic & message_statistic,
                    int64_t ignore_delays_below){

    int64_t delta = message_statistic.receive_time-message_statistic.source_time;

    if(delta<ignore_delays_below)
        return false;

    return true;
}

int main(int argc, char *argv[])
{

    MOOS::CommandLineParser P(argc,argv);

    if(P.GetFlag("--help","-h"))
        PrintHelpAndExit();

    //sort out logging!
    std::string logging_directory = kDefaultAuditDirectory;
    std::string log_file_name=kDefaultLogFile;
    P.GetVariable("--log_directory",logging_directory);
    P.GetVariable("--log_name",log_file_name);
    bool logging = P.GetFlag("--log");
    std::ofstream output_stream;
    if(logging){
        OpenLogFile(output_stream,
                    logging_directory,
                    log_file_name);

        WriteHeader(output_stream);
    }


    P.GetVariable("--channel",multicast_address);
    P.GetVariable("--port",multicast_port);
    P.GetVariable("--name",name_pattern);

    int ignore_delay_below_us = kDefaultIgnoreDelayBelow;
    P.GetVariable("--ignore_delay_below_us",ignore_delay_below_us);

    MOOS::MulticastNode multicast_listener;
    multicast_listener.Configure(multicast_address,multicast_port,1);
    multicast_listener.Run(false,true);

    int number_messages_rxed = 0;
    int number_messages_logged = 0;
    double last_display_time = MOOSTime();
    double average_latency_us = 0;
    double alpha = 0.01;
    while(1)
    {

        std::string sReply;

        if(multicast_listener.Read(sReply,1000)){
            MOOS::EndToEndAudit::MessageStatistic ms;
            ms.FromString(sReply);

            number_messages_rxed++;
            int64_t latency = ms.receive_time-ms.source_time;
            average_latency_us=alpha*latency+(1.0-alpha)*average_latency_us;

            if(ShouldBeLogged(ms,ignore_delay_below_us)){
                if(logging){
                    LogToFile(output_stream,ms);
                    number_messages_logged++;
                }else{
                    PrettyPrint(std::cerr,ms);
                }
            }

            AccummulateStatistic(ms);
        }

        if(MOOSTime()-last_display_time>3.0){
            last_display_time = MOOSTime();
            std::cerr<<MOOSFormat("processed %.8d messages and logged %.6d. "
                                  "Mean delay %.2fuS\n",
                                  number_messages_rxed,
                                  number_messages_logged,
                                  average_latency_us);

            PrintStatistics();
        }


    }

    return 0;


}

