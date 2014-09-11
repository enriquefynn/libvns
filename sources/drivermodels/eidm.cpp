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
#include "eidm.h"
#include "lane.h"
#include "vehicle.h"
#include "driverinfo.h"

#define H(a) ((a)==0?(0.5):(((a)<0)?(0):(1)))

namespace vns {

EIDM::EIDM(const DriverInfo* d) : DriverModel(d) {
	v0 = randomValue(d->v0_min,d->v0_max);
    a = randomValue(d->a_min,d->a_max);
    b = randomValue(d->b_min,d->b_max);
    T = randomValue(d->T_min,d->T_max); // desired time gap
    s0 = randomValue(d->s0_min,d->s0_max); // safety distance
    s1 = randomValue(d->s1_min,d->s1_max); // gap
    delta = randomValue(d->delta_min,d->delta_max);
    coolness = randomValue(d->coolness_min,d->coolness_max);
}

EIDM::~EIDM(){}

float EIDM::accel(const LaneObject* v, const LaneObject* fv){
	//updateMemory(DT,v->getSpeed(),v0);
	if( fv == 0 ){
		return accel(v,vns::MAX_DOUBLE,v->getSpeed(),0.0);
	}
	return accel(v,fv->getRearPosition(),fv->getSpeed(),fv->getAccel());
}

float EIDM::accel(const LaneObject* vehicle, float stopPos) {
	return accel(vehicle,stopPos,0.0,0.0);
}

float EIDM::accel(const LaneObject* vehicle, float pos, float vLead, float aLead){
	float v0Local = vns_MIN(malphaV0*v0,speedLimit);
	float aLocal = malphaA*a;
	float TLocal = malphaT*T;

	float v = vehicle->getSpeed();
    float dv = v - vLead;
    float s = pos - vehicle->getFrontPosition();

    float s_star = s0 + v*TLocal + s1 + (0.5*v*dv)/fsqrt(aLocal*b);
    if(s_star<s0) s_star = s0;
    float aIDM = aLocal*(1-fpow(v/v0Local,delta)-(s_star*s_star)/(s*s));

    // constant-acceleration heuristic (CAH)

    vLead = vns_MAX(vLead,0.00001);
    float aLeadRestricted = vns_MIN(aLead,aLocal);
    float dvp = v-vLead;
    float denomCAH = vLead*vLead - 2*s*aLeadRestricted;
    float aCAH = (vLead*dvp <= -2*s*aLeadRestricted)?(v*v*aLeadRestricted)/denomCAH : aLeadRestricted - 0.5*((dvp*dvp*H(dvp))/s);

    float aACC = (aIDM>=aCAH) ? aIDM : (1-coolness)*aIDM + coolness*(aCAH + b*tanh((aIDM-aCAH)/b));

    return aACC;
}

float EIDM::updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel){
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
