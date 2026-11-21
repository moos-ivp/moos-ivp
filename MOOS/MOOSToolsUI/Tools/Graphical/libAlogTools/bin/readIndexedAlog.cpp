#include <iostream>
#include <cstdio>
#include <string>

#include "alogTools/indexedAlogReader.h"

int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        printf("Usage: %s /path/to/filename.alog\n",argv[0]);
        exit(0);
    }

    indexedAlogReader myindexedAlogReader;
    std::string fName(argv[1]);
    if( myindexedAlogReader.Init( fName ) )
    {
        std::string line;
    
        printf("\n\n");
        printf("First three lines\n");
        
        myindexedAlogReader.GetNextLine(line);
        printf("\t%s\n",line.c_str());

        myindexedAlogReader.GetNextLine(line);
        printf("\t%s\n",line.c_str());

        myindexedAlogReader.GetNextLine(line);
        printf("\t%s\n",line.c_str());
        printf("\n\n");
        printf("Line 45678:\n");
        
        myindexedAlogReader.GetLine(45678, line);
        printf("\t%s\n",line.c_str());
        printf("\n\n");
    }
    else
    {
        printf("indexedAlogReader.Init(%s) failed...\n",fName.c_str());
    }
}
