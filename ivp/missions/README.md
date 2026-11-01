
* Overview

The missions in this folder represent a progression of mission and
launch script sophistication starting with the very basic (s1_alph),
culminating with m1_alpha.  The m1_alpha mission is representative of
nearly all missions. Most missions will have additional components and
parameters in addition to those found in the m1_alpha mission, but
m1_alpha can be considered a baseline.


* s1_alpha

A single MOOS community for both the shoreside and vehicle applications.
A single vehicle traverses a set of waypoints and then returns home.


* u1_alpha

Two MOOS communities are used, one for the shoreside and one for the vehicle.
Different from s1_alpha since it uses two communities. Nearly all missions
hereafter will have a single shoreside and one or more vehicles.

In this mission the pShare app is configured directly by naming a few
variables to be shared in each direction.


* u2_alpha

A second vehicle is added. The pShare configuration is still directly configured,
in part to show how untenable it will be to be configuring pShare directly.
The second vehicle traverse a home-plate pattern shifted 20 meters south-east
from the first vehicle.

The scheme of (shoreside) VAR_ALL and VAR_VNAME to (vehicle) VAR pShare mapping
is introduced.

The launch.sh script will now invoke two launch_vehicle.sh scripts, with the
latter script augmented to accept --vname=alpha/bravo


* u3_alpha

Same two vehicle home-plate pattern, with same variables shared in both directions

Here, uFldNodeBroker, uFldShoreBroker and pHostInfo are introduced as enablers to
ease in pShare configuration. This mission is still fixed with two vehicles
This will set us up for the the next mission that
allows N vehicles to be launched.







* m1_alpha

Introducing meta and targ files




