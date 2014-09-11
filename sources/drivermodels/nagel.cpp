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

#include "maths.h"
#include "nagel.h"
#include "lane.h"
#include "vehicle.h"
#include "driverinfo.h"

namespace vns {

Nagel::Nagel(const DriverInfo* d) : DriverModel(d) {
	v0 = randomValue(d->v0_min,d->v0_max);
	pSlowDown = randomValue(d->pSlowDown_min,d->pSlowDown_max);
	pSlowToStart = randomValue(d->pSlowToStart_min,d->pSlowToStart_max);
}

Nagel::~Nagel(){}

float Nagel::accel(const LaneObject* v, const LaneObject* fv){
    if (fv) {
    	return accel(v,fv->getRearPosition(),fv->getSpeed(),fv->getAccel());
    }
    return accel(v,vns::MAX_DOUBLE,v->getSpeed(),0.0);

}

float Nagel::accel(const LaneObject* v, float stopPos) {
	return accel(v,stopPos,0.0,0.0);
}

float Nagel::accel(const LaneObject* vehicle, float pos, float , float){
    return 0;
}

float Nagel::updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel){
	speed = (speed + dt * accel + 0.5);
	float advance = (lanePos + dt * speed + 0.5);
	return advance;
}

}
