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
#include "idm.h"
#include "lane.h"
#include "vehicle.h"
#include "driverinfo.h"

namespace vns {

IDM::IDM(const DriverInfo* d) : DriverModel(d) {
	v0 = randomValue(d->v0_min,d->v0_max);
    a = randomValue(d->a_min,d->a_max);
    b = randomValue(d->b_min,d->b_max);
    T = randomValue(d->T_min,d->T_max); // desired time gap
    s0 = randomValue(d->s0_min,d->s0_max); // safety distance
    s1 = randomValue(d->s1_min,d->s1_max); // gap
    delta = 4.0;
}

IDM::~IDM(){}

float IDM::accel(const LaneObject* v, const LaneObject* fv){
	updateMemory(DT,v->getSpeed(),v0);
	if( fv == 0 ){
		float mA = malphaA * a;
		float mV0 = vns_MIN(v0*malphaV0,speedLimit);
	    return mA*(1.0-fpow(v->getSpeed()/mV0,delta));
	}

	return accel(v,fv->getRearPosition(),fv->getSpeed(),fv->getAccel());


	/*
	if( fv == 0 ){
	    return a*(1-pow(v->getSpeed()/v0,delta));
	}

    float s_star = s0 + v->getSpeed()*(T + dv/(2*sqrtab));
    if(s_star<s0) s_star = s0;
    float r = a*(1-pow(v->getSpeed()/v0,delta)-(s_star*s_star)/(s*s));
    return r;*/

    // treat special case of v0=0 (standing obstacle)

}

float IDM::accel(const LaneObject* v, float stopPos) {
	updateMemory(DT,v->getSpeed(),v0);
	return accel(v,stopPos,0.0,0.0);
}

float IDM::accel(const LaneObject* v, float pos, float speed, float){

	float mA = malphaA * a;
	float mT = malphaT * T;
	float mV0 = vns_MIN(v0*malphaV0,speedLimit);

    float dv = v->getSpeed() - speed;
    float s = pos - v->getFrontPosition();
    float s_star = s0 + mT * v->getSpeed() + s1 * fsqrt((v->getSpeed() + 0.0001) / mV0) + (0.5 * v->getSpeed() * dv)/fsqrt(mA * b);
    if( s_star < s0 ) s_star = s0;

    float aIDM = mA * (1.0 - fpow( v->getSpeed()/mV0 , delta ) - ((s_star / s) * (s_star / s)));

    return aIDM;
}

float IDM::updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel){
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
