project(alogToolsLib)

set( sources
    indexedAlogReader.cpp
    alogLineReader.cpp
    indexReader.cpp
    indexWriter.cpp
    recordTypes.h
    recordTypes.cpp
    utils.h
    utils.cpp

    FileNotFoundException.h
    FileNotFoundException.cpp
    VersionException.cpp
    VersionException.h
)

add_library(alogTools ${sources})