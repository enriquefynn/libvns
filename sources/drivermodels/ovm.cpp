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
#include "ovm.h"
#include "lane.h"
#include "vehicle.h"
#include "driverinfo.h"

namespace vns {

OVM::OVM(const DriverInfo* d) : DriverModel(d) {
	v0 = randomValue(d->v0_min,d->v0_max);
    s0 = randomValue(d->s0_min,d->s0_max); // safety distance
    beta = randomValue(d->beta_min,d->beta_max);
    tau = randomValue(d->tau_min,d->tau_max);
    tWidth = randomValue(d->tWidth_min,d->tWidth_max);
    lambda = randomValue(d->lambda_min,d->lambda_max);
}

OVM::~OVM(){}

float OVM::accel(const LaneObject* v, const LaneObject* fv){
    if (fv) {
    	return accel(v,fv->getRearPosition(),fv->getSpeed(),fv->getAccel());
    }
    return accel(v,vns::MAX_FLOAT,0.0,0.0);
}

float OVM::accel(const LaneObject* v, float stopPos){
	return accel(v,stopPos,0.0,0.0);
}

float OVM::accel(const LaneObject* v, float pos, float speed, float){
	float s = pos - v->getFrontPosition();
	float dv = v->getSpeed() - speed;
    float v0Local = vns_MIN(v0*malphaV0,speedLimit);

    float betaLoc = beta;//*malphaT;
    float tWidthLoc = vns_MAX(1e-6, tWidth*malphaT);

    // 0=fullVD original, 1=fullVD,secBased, 2=threePhase.
    /*
    if(variant == 0){
    	// original VDIFF model, OVM: lambda == 0
        // standard OVM function (Bando model)
        // scale OVM/VDIFF so that v0 represents actual desired speed
    	float v0Prev = v0Local / (1.0 + tanh(betaLoc));
        vOptimal = vns_MAX(v0Prev * (tanh((s - s0) / tWidthLoc - betaLoc) - tanh(-betaLoc)), 0.0);

        return (vOptimal - v->getSpeed()) / tau - lambda * dv;
    }*/

    // standard OVM function (Bando model)
    // scale OVM/VDIFF so that v0 represents actual desired speed
    float v0Prev = v0Local / (1.0 + tanh(betaLoc));
    float vOptimal = vns_MAX(v0Prev * (tanh((s - s0) / tWidthLoc - betaLoc) - tanh(-betaLoc)), 0.0);

    return (vOptimal - v->getSpeed()) / tau - lambda * ((dv > 0) ? dv : 0);

}


float OVM::updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel){
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
