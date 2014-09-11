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
#include "traincarmodel.h"
#include "lane.h"
#include "vehicle.h"
#include "driverinfo.h"

namespace vns {

TrainCarModel::TrainCarModel() : DriverModel() {
    s0 = 0.5; // safety distance
}

TrainCarModel::~TrainCarModel(){}

float TrainCarModel::accel(const LaneObject* v, const LaneObject* fv){
	if(fv) return fv->getAccel();
	return v->getAccel();
}

float TrainCarModel::accel(const LaneObject* v, float) {
	return v->getAccel();
}

float TrainCarModel::accel(const LaneObject* v, float, float , float){
    return v->getAccel();
}

float TrainCarModel::updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel){
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
