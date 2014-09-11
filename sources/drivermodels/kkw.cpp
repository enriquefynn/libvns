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
#include "kkw.h"
#include "lane.h"
#include "vehicle.h"
#include "driverinfo.h"

namespace vns {

KKW::KKW(const DriverInfo* d) : DriverModel(d) {
	v0 = randomValue(d->v0_min,d->v0_max);
    k = randomValue(d->k_min,d->k_max);
    pb0 = randomValue(d->pb0_min,d->pb0_max);
    pb1 = randomValue(d->pb1_min,d->pb1_max);
    pa1 = randomValue(d->pa1_min,d->pa1_max);
    pa2 = randomValue(d->pa2_min,d->pa2_max);
    vp = randomValue(d->vp_min,d->vp_max);
}

KKW::~KKW(){}

float KKW::accel(const LaneObject* v, const LaneObject* fv){
    if (fv) {
    	return accel(v,fv->getRearPosition(),fv->getSpeed(),fv->getAccel());
    }
    return accel(v,vns::MAX_DOUBLE,v->getSpeed(),0.0);

}

float KKW::accel(const LaneObject* v, float stopPos) {
	return accel(v,stopPos,0.0,0.0);
}

float KKW::accel(const LaneObject* v, float pos, float speed, float){

    return 0;
}

float KKW::updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel){
	speed = (speed + dt * accel + 0.5);
	float advance = (lanePos + dt * speed + 0.5);
	return advance;
}

}
