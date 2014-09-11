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
#include "gipps.h"
#include "lane.h"
#include "vehicle.h"
#include "driverinfo.h"

namespace vns {

Gipps::Gipps(const DriverInfo* d) : DriverModel(d) {
	v0 = randomValue(d->v0_min,d->v0_max);
    a = randomValue(d->a_min,d->a_max);
    b = randomValue(d->b_min,d->b_max);
    T = randomValue(d->T_min,d->T_max); // desired time gap
    s0 = randomValue(d->s0_min,d->s0_max); // safety distance
}

Gipps::~Gipps(){}

float Gipps::accel(const LaneObject* v, const LaneObject* fv){
    if (fv) {
    	return accel(v,fv->getRearPosition(),fv->getSpeed(),fv->getAccel());
    }
    return accel(v,vns::MAX_FLOAT,v->getSpeed(),0.0);
}

float Gipps::accel(const LaneObject* v, float stopPos){
	return accel(v,stopPos,0.0,0.0);
}

float Gipps::accel(const LaneObject* v, float pos, float speed, float){
	float s = pos - v->getFrontPosition();
	float dv = v->getSpeed() - speed;
    float v0Local = vns_MIN(v0*malphaV0,speedLimit);
    float TLocal = DT*malphaT;

    float vp = v->getSpeed() - dv;
    // safe speed
    float vSafe = -b * TLocal + fsqrt(b * b * TLocal * TLocal + vp * vp + 2 * b * vns_MAX(s - s0, 0.0));
    float vNew = vns_MIN(vSafe, vns_MIN(v->getSpeed() + a * TLocal, v0Local));
    return (vNew - v->getSpeed()) / TLocal;
}

float Gipps::updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel){
    if (speed < 0) {
        speed = 0;
    }
    float advance = (accel * dt >= -speed) ? speed * dt + 0.5 * accel * dt * dt : -0.5 * speed * speed / accel;
    lanePos += advance;
    speed += dt * accel;
    if (speed < 0) {
        speed = 0;
        accel = 0;
    }
    return lanePos;
}

}
