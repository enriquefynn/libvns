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
#include "newell.h"
#include "lane.h"
#include "vehicle.h"
#include "driverinfo.h"

namespace vns {

Newell::Newell(const DriverInfo* d) : DriverModel(d) {
	v0 = randomValue(d->v0_min,d->v0_max);
    s0 = randomValue(d->s0_min,d->s0_max); // safety distance
}

Newell::~Newell(){}

float Newell::accel(const LaneObject* v, const LaneObject* fv){
    if (fv) {
    	return accel(v,fv->getRearPosition(),fv->getSpeed(),fv->getAccel());
    }
    return accel(v,vns::MAX_FLOAT,v->getSpeed(),0.0);
}

float Newell::accel(const LaneObject* v, float stopPos) {
	return accel(v,stopPos,0.0,0.0);
}

float Newell::accel(const LaneObject* v, float pos, float , float){
	float s = pos - v->getFrontPosition();
	//float dv = v->getSpeed() - speed;
    float v0Local = vns_MIN(v0*malphaV0,speedLimit);

    float dtLocal = DT*malphaT;

    float vNew = vns_MIN(vns_MAX((s - s0) / dtLocal, 0), v0Local);
    return (vNew - v->getSpeed()) / dtLocal;
}

float Newell::updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel){
    if (speed < 0) {
        speed = 0;
    }
    float advance = speed * dt + accel * dt * dt;
    speed += dt * accel;
    lanePos += advance;
    if (speed < 0) {
        speed = 0;
        accel = 0;
    }
    return lanePos;
}

}
