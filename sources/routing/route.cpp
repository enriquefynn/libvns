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

#include "route.h"
#include "routes.h"

namespace vns {

Route::Route(Path* pth, double interval,VehicleType vType,int32 driverID,double start,double end){
	VNS_ASSERT( interval>0.0 ,"Route::Route","Period should be > 0!");
	path = pth;
	period = interval;
	vehicleType = vType;
	driver = driverID;
	startTime = start;
	endTime = end;
}

Route::~Route(){

}


void Route::write(DataStream& f, const Route* route ){
    f << int32(route->path->getID());
    f << route->period;
    f << (int8)route->vehicleType;
    f << route->driver;
    f << route->startTime;
    f << route->endTime;
}

vns::Route* Route::read(DataStream& f, Routes* routes ){
	int32 pathID;
	double period;
	int8 vType;
	int32 driverID = -1;
	double startTime;
	double endTime;
	f >> pathID;
	f >> period;
    f >> vType;
    f >> driverID;
    f >> startTime;
    f >> endTime;
    return new vns::Route(routes->getPath(pathID),period,vType,-1,startTime,endTime);
}

}
