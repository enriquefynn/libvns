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
#include "pipes.h"
#include "lane.h"
#include "vehicle.h"
#include "driverinfo.h"

namespace vns {

Pipes::Pipes(const DriverInfo* d) : DriverModel(d) {
	v0 = randomValue(d->v0_min,d->v0_max);
}

Pipes::~Pipes(){}

float Pipes::accel(const LaneObject* v, const LaneObject* fv){
    if (fv) {
    	return accel(v,fv->getRearPosition(),fv->getSpeed(),fv->getAccel());
    }
    return accel(v,vns::MAX_FLOAT,v->getSpeed(),0.0);
}

float Pipes::accel(const LaneObject* v, float stopPos) {
	return accel(v,stopPos,0.0,0.0);
}

float Pipes::accel(const LaneObject* v, float pos, float , float){
	float s = pos - v->getFrontPosition();
    float v0Local = vns_MIN(v0*malphaV0,speedLimit);
    float dtLocal = DT*malphaT;
    float D = v->getLength()*(1+v->getSpeed()/4.472);

    float vNew = vns_MIN(vns_MAX((s - D) / dtLocal, 0), v0Local);
    return (vNew - v->getSpeed()) / dtLocal;
}

float Pipes::updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel){
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
