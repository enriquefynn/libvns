/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2011-2012 Ricardo Fernandes - All Rights Reserved       *
 *   Email: rjf@dcc.fc.up.pt or rikardojfernandez@gmail.com                *
 *                                                                         *
 *   This file is part of VNS - Vehicular Networks Simulator.              *
 *                                                                         *
 *   For information about the licensing and copyright of this code        *
 *   please contact the author.                                            *
 *                                                                         *
 ***************************************************************************/

#include "scheduledroute.h"
#include "routes.h"

namespace vns {

ScheduledRoute::ScheduledRoute(Path* pth, double schTime,VehicleType vType,int32 driverID){
	path = pth;
	scheduledTime = schTime;
	vehicleType = vType;
	driver = driverID;
};

ScheduledRoute::~ScheduledRoute(){};

void ScheduledRoute::write(DataStream& f, ScheduledRoute* route){
	f << (int32)route->path->getID();
    f << route->scheduledTime;
    f << (int8)route->vehicleType;
    f << route->driver;
}

vns::ScheduledRoute* ScheduledRoute::read(DataStream& f, Routes* routes){
	int32 pathID;
	double scheduledTime;
	int8 vType;
	int32 driverID = -1;
	f >> pathID;
    f >> scheduledTime;
    f >> vType;
    f >> driverID;
    return new vns::ScheduledRoute(routes->getPath(pathID),scheduledTime,vType,-1);
}

}
