#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

#include "MOOS/libAlogTools/FileNotFoundException.h"
#include "MOOS/libAlogTools/recordTypes.h"
#include "MOOS/libAlogTools/indexWriter.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
void indexWriter::writeIndexFile(string alogIndexName)
{
    // sort the lines by timestamp and then write to index file
    sort(m_alogRecords.begin(), m_alogRecords.end());

    ofstream outfile(alogIndexName.c_str());
    if (!outfile.is_open())
    {
        // TODO: this is the wrong exception to throw!
        throw FileNotFoundException();
    }

    outfile << m_alogHeader;
    outfile << m_alogMsgList;
    outfile << m_alogSrcList;

    long recOffset = outfile.tellp();
    m_alogHeader.recsBegin = recOffset;

    // Go and rewrite header, now we know where the data begins
    outfile.seekp(0, ios_base::beg);
    outfile << m_alogHeader;
    outfile.seekp(recOffset);

    vector<aloglib::idxRec>::const_iterator it = m_alogRecords.begin();
    while (it != m_alogRecords.end())
    {
        const aloglib::idxRec &aline = *it;
        //printf("time: %f,  lBegin: %d,  lEnd: %d\n",aline.time,aline.lineBegin,aline.lineEnd);
        outfile << aline;
        ++it;
    }

    outfile.close();
}

////////////////////////////////////////////////////////////////////////////////
void indexWriter::parseAlogFile(string alogFileName)
{
    ifstream myfile(alogFileName.c_str());
    if (!myfile.is_open())
    {
        throw FileNotFoundException(alogFileName);
    }

    int numRecords = 0;
    while (!myfile.eof())
    {
        // Store byte offset to next line
        long int lineBegin = (long int) myfile.tellg();

        // Read the line in
        string line;
        getline(myfile, line);

        // Check for blank line
        size_t pos = line.find_first_not_of(" \t\r\n");
        bool blankLine = (pos == string::npos);

        if (!blankLine && !line.empty())
        {
            // Check for comment
            if (line.at(pos) == '%')
            {
                size_t logstartpos = line.find("LOGSTART");
                if (logstartpos != string::npos)
                {
                    size_t startTimePos = line.find_first_of("0123456789.");
                    if (startTimePos != string::npos)
                    {
                        double dfTime = atof(line.substr(startTimePos).c_str());
                        m_alogHeader.startTime = dfTime;
                    }
                }

                continue;
            }

            // ignore negative timestamps
            // isdigit checks for [0-9] so will fail on -x.x
            if (isdigit(line.at(pos)) == 0)
            {
                continue;
            }


            // Wrap a stream around the string, to allow easier parsing
            std::istringstream stm(line);

            double timeStamp;
            string varName;
            string srcName;
            stm >> timeStamp >> varName >> srcName;

            // Feed MOOS variable name into message list
            m_alogMsgList.insert(varName);

            // Feed MOOS source name into Source list
            m_alogSrcList.insert(srcName);

            long int lineEnd = (long int) myfile.tellg();

            aloglib::idxRec alogLine;
            alogLine.time = timeStamp;
            alogLine.lineBegin = lineBegin;
            alogLine.len = lineEnd - lineBegin;

            //printf("time: %f,  lineBegin: %ld,  len: %ld,  varName: %s\n", alogLine.time, alogLine.lineBegin, alogLine.len, varName.c_str() );
            m_alogRecords.push_back(alogLine);

            numRecords++;
        }
    }
    myfile.close();

    m_alogHeader.numRecs = numRecords;
}

