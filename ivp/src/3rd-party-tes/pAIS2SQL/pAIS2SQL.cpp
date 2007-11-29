// t. schneider tes@mit.edu 11.29.07
// laboratory for autonomous marine sensing systems
// massachusetts institute of technology 
// 
// this is pAIS2SQL.cpp 
//
// see the readme file within this directory for information
// pertaining to usage and purpose of this script.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software.  If not, see <http://www.gnu.org/licenses/>.

#include "pAIS2SQL.h"

CpAIS2SQL::CpAIS2SQL()
{
}
CpAIS2SQL::~CpAIS2SQL()
{
}

// OnNewMail: called when new mail (previously registered for)
// has arrived.
bool CpAIS2SQL::OnNewMail(MOOSMSG_LIST &NewMail)
{
	return true;
}

// OnConnectToServer: called when connection to the MOOSDB is
// complete
bool CpAIS2SQL::OnConnectToServer()
{
	return true;
}

// Iterate: called AppTick times per second
bool CpAIS2SQL::Iterate()
{ 
	return true;
}

// OnStartUp: called when program is run
bool CpAIS2SQL::OnStartUp()
{       
	return true;
}
