#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>

#include "MOOS/libAlogTools/indexWriter.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
void processAlogFile( std::string alogFileName )
{
    indexWriter idxWriter;
    
    idxWriter.parseAlogFile(alogFileName);
    idxWriter.writeIndexFile( alogFileName + string( ".idx" ) );
}

int main(int argc, char**argv)
{
    if( argc < 2 )
    {
        printf("Usage: %s /path/to/filename.alog\n",argv[0]);
        exit(0);
    }

    processAlogFile( argv[1] );
}
 
