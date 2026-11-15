#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>

namespace {
int failures = 0;

void check(bool condition, const char* message)
{
    if(!condition) {
        std::cerr << message << '\n';
        ++failures;
    }
}

bool near(double actual, double expected, double tolerance)
{
    return std::isfinite(actual) && std::fabs(actual - expected) < tolerance;
}

struct Case {
    double originLat, originLon, lat, lon, east, north;
    const char* zone;
};
}

int main()
{
    // Independent WGS84 reference offsets, computed with PROJ's CLI:
    // proj -f %.8f +proj=utm +ellps=WGS84 +zone=<origin zone> [+south]
    // Project both (lon, lat) pairs in that projection, then subtract.
    // No PROJ dependency is needed to build or run this test.
    const Case cases[] = {
        {40.5, -71.98, 40.5, -72.02, -3390.088799, 115.386224, "19T"},
        {40.5, -72.02, 40.5, -71.98, 3390.088799, 115.386224, "18T"},
        {-33, -71.98, -33, -72.02, -3738.732362, -106.716829, "19H"},
        {0.02, 3, -0.02, 3, 0.000000, -4421.201846, "31N"},
        {-0.02, 3, 0.02, 3, 0.000000, 4421.201846, "31M"},
        {10, 179.98, 10, -179.98, 4389.511200, 39.947516, "60P"},
        {10, -179.98, 10, 179.98, -4389.511200, 39.947516, "1P"},
        {60, 3.02, 60, 2.98, -2224.967562, 202.524024, "32V"},
        {75, 9.02, 75, 8.98, -1150.129166, 116.764482, "33X"},
        {75, 20.98, 75, 21.02, 1150.129166, 116.764482, "33X"},
        {40.5, -71, 40.51, -70.99, 872.472913, 1090.934507, "19T"},
        {7.99, 3, 8.01, 3, 0.000000, 2211.030947, "31N"},
    };

    // Reuse the instance to exercise changes of zone and hemisphere on Initialise.
    CMOOSGeodesy geodesy;
    double north, east, lat, lon;
    check(!geodesy.LatLong2LocalUTM(40.5, -72.02, north, east),
          "Forward conversion should require an origin");
    check(!geodesy.UTM2LatLong(0, 0, lat, lon),
          "Inverse conversion should require an origin");
    for(const Case& c : cases) {
        const int previousFailures = failures;
        check(geodesy.Initialise(c.originLat, c.originLon), "Initialise failed");
        check(std::strcmp(geodesy.GetUTMZone(), c.zone) == 0, "Wrong origin zone");
        check(geodesy.LatLong2LocalUTM(c.originLat, c.originLon, north, east),
              "Origin conversion failed");
        check(near(north, 0, 1e-6) && near(east, 0, 1e-6), "Origin must map to zero");
        for(int repeat = 0; repeat < 2; ++repeat) {
            check(geodesy.LatLong2LocalUTM(c.lat, c.lon, north, east),
                  "Forward conversion failed");
            // The native truncated UTM series differs slightly from PROJ.
            check(near(east, c.east, 0.05) && near(north, c.north, 0.05),
                  "Offset differs from fixed-zone reference");
            check(near(geodesy.GetMetersEast(), east, 1e-6) &&
                  near(geodesy.GetMetersNorth(), north, 1e-6), "Wrong cached offset");
            check(geodesy.UTM2LatLong(east, north, lat, lon), "Round trip failed");
            check(near(lat, c.lat, 2e-8) && near(lon, c.lon, 2e-8),
                  "Round trip coordinates differ");
            check(std::strcmp(geodesy.GetUTMZone(), c.zone) == 0,
                  "Conversion changed origin zone");
        }
        // Also invert independently computed coordinates.
        check(geodesy.UTM2LatLong(c.east, c.north, lat, lon), "Reference inverse failed");
        check(near(lat, c.lat, 1e-6) && near(lon, c.lon, 1e-6),
              "Reference inverse coordinates differ");
        if(failures != previousFailures)
            std::cerr << "Failed case: origin " << c.originLat << ", " << c.originLon
                      << "; point " << c.lat << ", " << c.lon << '\n';
    }

    // At the UTM limits, the inverse's initial grid estimate can fall just
    // outside the latitude range even though the projected point is valid.
    const double boundaryLatitudes[] = {-80.0, 84.0};
    const double longitudeOffsets[] = {-0.01, 0.01};
    for(double boundaryLat : boundaryLatitudes) {
        check(geodesy.Initialise(boundaryLat, 3.0), "Boundary Initialise failed");
        for(double longitudeOffset : longitudeOffsets) {
            const double targetLon = 3.0 + longitudeOffset;
            check(geodesy.LatLong2LocalUTM(boundaryLat, targetLon, north, east),
                  "Boundary forward conversion failed");
            check(geodesy.UTM2LatLong(east, north, lat, lon),
                  "Boundary round trip failed");
            check(near(lat, boundaryLat, 2e-8) && near(lon, targetLon, 2e-8),
                  "Boundary round trip coordinates differ");
            check(lat >= -80.0 && lat <= 84.0, "Inverse latitude outside UTM limits");
        }
        const double outsideLat = boundaryLat + (boundaryLat > 0 ? 0.01 : -0.01);
        check(!geodesy.LatLong2LocalUTM(outsideLat, 3.0, north, east),
              "Out-of-range forward latitude accepted");
        check(!geodesy.UTM2LatLong(0.0, boundaryLat > 0 ? 1000.0 : -1000.0, lat, lon),
              "Out-of-range inverse latitude accepted");
    }

    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    check(!geodesy.Initialise(nan, 0), "NaN origin accepted");
    check(!geodesy.Initialise(0, inf), "Infinite origin accepted");
    check(!geodesy.LatLong2LocalUTM(nan, 0, north, east), "NaN latitude accepted");
    check(!geodesy.LatLong2LocalUTM(0, inf, north, east), "Infinite longitude accepted");
    check(!geodesy.UTM2LatLong(inf, 0, lat, lon), "Infinite offset accepted");
    check(!geodesy.UTM2LatLong(1e20, 1e20, lat, lon), "Unprojectable offset accepted");
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
