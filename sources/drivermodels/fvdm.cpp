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
#include "fvdm.h"
#include "lane.h"
#include "vehicle.h"
#include "driverinfo.h"

namespace vns {

FVDM::FVDM(const DriverInfo* d) : DriverModel(d) {
	v0 = randomValue(d->v0_min,d->v0_max);
    s0 = randomValue(d->s0_min,d->s0_max); // safety distance
    beta = randomValue(d->beta_min,d->beta_max);
    tau = randomValue(d->tau_min,d->tau_max);
    tWidth = randomValue(d->tWidth_min,d->tWidth_max);
    lambda = randomValue(d->lambda_min,d->lambda_max);
}

FVDM::~FVDM(){}

float FVDM::accel(const LaneObject* v, const LaneObject* fv){
    if (fv) {
    	return accel(v,fv->getRearPosition(),fv->getSpeed(),fv->getAccel());
    }
    return accel(v,vns::MAX_FLOAT,0.0,0.0);
}

float FVDM::accel(const LaneObject* v, float stopPos){
	return accel(v,stopPos,0.0,0.0);
}

float FVDM::accel(const LaneObject* v, float pos, float speed, float){
	float s = pos - v->getFrontPosition();
	float dv = v->getSpeed() - speed;
    float v0Local = vns_MIN(v0*malphaV0,speedLimit);

    /*
    float smallValue = 1e-7;
    float betaLoc = beta;
    float tWidthLoc = vns_MAX(1e-6, tWidth*malphaT);*/

    // 0=fullVD original, 1=fullVD,secBased, 2=threePhase.


    	// original VDIFF model, OVM: lambda == 0
    	// triangular OVM function
    float T = beta; // "time headway"
    float vOptimal = vns_MAX(vns_MIN((s - s0) / T, v0Local), 0.0);

    return (vOptimal - v->getSpeed()) / tau - lambda * dv;


    // "Three-phase" OVM function
    /*
    float diffT = 0.0 * pow(vns_MAX(1 - v->getSpeed() / v0Local, 0.0001), 0.5);
    float Tmin = tWidthLoc + diffT; // minimum time headway
    float Tmax = betaLoc + diffT; // maximum time headway
    float Tdyn = (s - s0) / vns_MAX(v->getSpeed(), smallValue );
    float vOptimal = (Tdyn > Tmax) ? vns_MIN((s - s0) / Tmax, v0Local) : (Tdyn > Tmin) ? vns_MIN(v->getSpeed(), v0Local) : (Tdyn > 0) ? vns_MIN((s - s0) / Tmin, v0Local) : 0;

    return (vOptimal - v->getSpeed()) / tau - lambda * v->getSpeed() * dv / vns_MAX(s - 1.0 * s0, smallValue );
*/
}


float FVDM::updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel){
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
