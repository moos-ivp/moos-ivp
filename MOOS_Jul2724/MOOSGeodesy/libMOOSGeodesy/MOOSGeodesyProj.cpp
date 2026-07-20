///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite
//
//   A suit of Applications and Libraries for Mobile Robotics Research
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and
//   Oxford University.
//
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford
//   University 2003-2005. email: pnewman@robots.ox.ac.uk.
//
//   This file is part of a MOOS Core Component.
//
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of the
//   License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//   General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////

#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "MOOSGeodesyBackend.h"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace {

typedef void* ProjProjection;
typedef ProjProjection (*ProjInitPlus)(const char*);
typedef int (*ProjTransform)(ProjProjection, ProjProjection, long, int,
                             double*, double*, double*);
typedef char* (*ProjStrError)(int);
typedef void (*ProjFree)(ProjProjection);

std::string ParentDirectory(const std::string& path)
{
    std::string::size_type slash = path.find_last_of("/\\");
    return slash == std::string::npos ? std::string() : path.substr(0, slash);
}

#ifndef _WIN32
std::string MOOSGeodesyLibraryDirectory()
{
    Dl_info info;
    if(dladdr(reinterpret_cast<void*>(&MOOSGeodesyLibraryDirectory), &info) &&
       info.dli_fname)
        return ParentDirectory(info.dli_fname);
    return std::string();
}
#else
std::string MOOSGeodesyLibraryDirectory()
{
    HMODULE module = NULL;
    if(!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          reinterpret_cast<LPCSTR>(&MOOSGeodesyLibraryDirectory),
                          &module))
        return std::string();

    char path[MAX_PATH];
    DWORD length = GetModuleFileNameA(module, path, MAX_PATH);
    if(length == 0 || length == MAX_PATH)
        return std::string();
    return ParentDirectory(std::string(path, length));
}
#endif

class DynamicProj
{
public:
    DynamicProj()
      : m_handle(0), m_initPlus(0), m_transform(0), m_strError(0), m_free(0)
    {
    }

    ~DynamicProj()
    {
        Close();
    }

    bool Load()
    {
        if(m_handle)
            return true;

        std::vector<std::string> candidates;
        const char* configuredPath = std::getenv("MOOS_PROJ_LIBRARY");
        if(configuredPath && configuredPath[0]) {
            candidates.push_back(configuredPath);
        } else {
            AddBundledCandidates(candidates);
            AddSystemCandidates(candidates);
        }

        for(std::vector<std::string>::const_iterator candidate = candidates.begin();
            candidate != candidates.end(); ++candidate) {
            if(Open(*candidate) && LoadSymbols()) {
                m_path = *candidate;
                return true;
            }
            Close();
        }

        std::stringstream message;
        message << "Unable to load a Proj library with the legacy Proj 5 API";
        if(configuredPath && configuredPath[0])
            message << " from MOOS_PROJ_LIBRARY=" << configuredPath;
        else
            message << "; set MOOS_PROJ_LIBRARY to the library path";
        m_error = message.str();
        return false;
    }

    ProjProjection InitPlus(const char* definition) const
    {
        return m_initPlus(definition);
    }

    int Transform(ProjProjection source, ProjProjection destination,
                  double* x, double* y) const
    {
        return m_transform(source, destination, 1, 1, x, y, 0);
    }

    const char* StrError(int error) const
    {
        return m_strError(error);
    }

    void Free(ProjProjection projection) const
    {
        if(projection)
            m_free(projection);
    }

    const std::string& Error() const
    {
        return m_error;
    }

private:
#ifdef _WIN32
    typedef HMODULE LibraryHandle;
#else
    typedef void* LibraryHandle;
#endif

    DynamicProj(const DynamicProj&);
    DynamicProj& operator=(const DynamicProj&);

    void AddBundledCandidates(std::vector<std::string>& candidates) const
    {
        const std::string directory = MOOSGeodesyLibraryDirectory();
        if(directory.empty())
            return;

#ifdef _WIN32
        const char* names[] = {"proj_5_2.dll", "proj.dll", "libproj.dll"};
#elif defined(__APPLE__)
        const char* names[] = {"libproj.14.dylib", "libproj.dylib"};
#else
        const char* names[] = {"libproj.so.14", "libproj.so"};
#endif
        const size_t count = sizeof(names) / sizeof(names[0]);
        for(size_t i = 0; i < count; ++i) {
            candidates.push_back(directory + "/" + names[i]);
#ifdef _WIN32
            candidates.push_back(directory + "/../../proj-5.2.0/bin/" + names[i]);
#else
            candidates.push_back(directory + "/../../proj-5.2.0/lib/" + names[i]);
#endif
        }
    }

    void AddSystemCandidates(std::vector<std::string>& candidates) const
    {
#ifdef _WIN32
        candidates.push_back("proj_5_2.dll");
        candidates.push_back("proj.dll");
        candidates.push_back("libproj.dll");
#elif defined(__APPLE__)
        candidates.push_back("libproj.14.dylib");
        candidates.push_back("libproj.dylib");
#else
        candidates.push_back("libproj.so.14");
        candidates.push_back("libproj.so");
#endif
    }

    bool Open(const std::string& path)
    {
#ifdef _WIN32
        m_handle = LoadLibraryA(path.c_str());
#else
        m_handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
        return m_handle != 0;
    }

    void* Symbol(const char* name)
    {
#ifdef _WIN32
        return reinterpret_cast<void*>(GetProcAddress(m_handle, name));
#else
        return dlsym(m_handle, name);
#endif
    }

    bool LoadSymbols()
    {
        m_initPlus = reinterpret_cast<ProjInitPlus>(Symbol("pj_init_plus"));
        m_transform = reinterpret_cast<ProjTransform>(Symbol("pj_transform"));
        m_strError = reinterpret_cast<ProjStrError>(Symbol("pj_strerrno"));
        m_free = reinterpret_cast<ProjFree>(Symbol("pj_free"));
        return m_initPlus && m_transform && m_strError && m_free;
    }

    void Close()
    {
        if(m_handle) {
#ifdef _WIN32
            FreeLibrary(m_handle);
#else
            dlclose(m_handle);
#endif
        }
        m_handle = 0;
        m_initPlus = 0;
        m_transform = 0;
        m_strError = 0;
        m_free = 0;
        m_path.clear();
    }

    LibraryHandle m_handle;
    ProjInitPlus m_initPlus;
    ProjTransform m_transform;
    ProjStrError m_strError;
    ProjFree m_free;
    std::string m_path;
    std::string m_error;
};

}

class CMOOSGeodesyProj : public CMOOSGeodesyBackend
{
public:
    CMOOSGeodesyProj()
      : m_utm(0), m_latlong(0)
    {
    }

    ~CMOOSGeodesyProj()
    {
        Reset();
    }

    CMOOSGeodesyBackend* Clone() const
    {
        CMOOSGeodesyProj* copy = new CMOOSGeodesyProj;
        if(m_utm && m_latlong &&
           !copy->InitialiseProjections(m_utmDefinition)) {
            delete copy;
            return new CMOOSGeodesyProj;
        }
        return copy;
    }

    bool Initialise(CMOOSGeodesy& geodesy, double lat, double lon)
    {
        geodesy.SetRefEllipsoid(23);
        geodesy.SetOriginLatitude(lat);
        geodesy.SetOriginLongitude(lon);

        char utmZone[4];
        double ignoredNorth = 0.0;
        double ignoredEast = 0.0;
        geodesy.LLtoUTM(geodesy.m_iRefEllipsoid, lat, lon,
                        ignoredNorth, ignoredEast, utmZone);

        std::stringstream definition;
        definition << "+proj=utm +ellps=WGS84 +zone=" << std::atoi(utmZone);
        if(lat < 0)
            definition << " +south";

        if(!InitialiseProjections(definition.str()))
            return false;

        double north = lat * deg2rad;
        double east = lon * deg2rad;
        int error = m_api.Transform(m_latlong, m_utm, &east, &north);
        if(error) {
            std::cerr << "Failed to transform datum, reason: "
                      << m_api.StrError(error) << std::endl;
            Reset();
            return false;
        }

        geodesy.SetOriginNorthing(north);
        geodesy.SetOriginEasting(east);
        geodesy.SetUTMZone(utmZone);
        geodesy.m_bSTEP_AFTER_INIT = true;
        return true;
    }

    bool LatLong2LocalUTM(CMOOSGeodesy& geodesy,
                          double lat, double lon,
                          double& metersNorth, double& metersEast)
    {
        metersNorth = std::numeric_limits<double>::quiet_NaN();
        metersEast = std::numeric_limits<double>::quiet_NaN();
        if(!m_latlong || !m_utm) {
            std::cerr << "Must call Initialise before calling LatLong2LocalUTM"
                      << std::endl;
            return false;
        }

        double north = lat * deg2rad;
        double east = lon * deg2rad;
        int error = m_api.Transform(m_latlong, m_utm, &east, &north);
        if(error) {
            std::cerr << "Failed to transform (lat,lon) = (" << lat << ","
                      << lon << "), reason: " << m_api.StrError(error)
                      << std::endl;
            return false;
        }

        metersNorth = north - geodesy.GetOriginNorthing();
        metersEast = east - geodesy.GetOriginEasting();
        return true;
    }

    bool UTM2LatLong(CMOOSGeodesy& geodesy,
                     double x, double y, double& lat, double& lon)
    {
        lat = std::numeric_limits<double>::quiet_NaN();
        lon = std::numeric_limits<double>::quiet_NaN();
        if(!m_latlong || !m_utm) {
            std::cerr << "Must call Initialise before calling UTM2LatLong"
                      << std::endl;
            return false;
        }

        double absoluteX = x + geodesy.GetOriginEasting();
        double absoluteY = y + geodesy.GetOriginNorthing();
        int error = m_api.Transform(m_utm, m_latlong,
                                    &absoluteX, &absoluteY);
        if(error) {
            std::cerr << "Failed to transform (x,y) = (" << x << "," << y
                      << "), reason: " << m_api.StrError(error) << std::endl;
            return false;
        }

        lat = absoluteY * rad2deg;
        lon = absoluteX * rad2deg;
        return true;
    }

    bool IsProj() const
    {
        return true;
    }

private:
    bool InitialiseProjections(const std::string& utmDefinition)
    {
        Reset();
        if(!m_api.Load()) {
            std::cerr << m_api.Error() << std::endl;
            return false;
        }

        m_utm = m_api.InitPlus(utmDefinition.c_str());
        if(!m_utm) {
            std::cerr << "Failed to initialise UTM projection" << std::endl;
            return false;
        }

        m_latlong = m_api.InitPlus("+proj=latlong +ellps=WGS84");
        if(!m_latlong) {
            std::cerr << "Failed to initialise latitude/longitude projection"
                      << std::endl;
            Reset();
            return false;
        }

        m_utmDefinition = utmDefinition;
        return true;
    }

    void Reset()
    {
        m_api.Free(m_utm);
        m_api.Free(m_latlong);
        m_utm = 0;
        m_latlong = 0;
        m_utmDefinition.clear();
    }

    DynamicProj m_api;
    ProjProjection m_utm;
    ProjProjection m_latlong;
    std::string m_utmDefinition;
};

CMOOSGeodesyBackend* CreateMOOSGeodesyProj()
{
    return new CMOOSGeodesyProj;
}
