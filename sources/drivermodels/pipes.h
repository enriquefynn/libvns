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

#ifndef VNS_PIPES_H_
#define VNS_PIPES_H_

#include "drivermodel.h"

namespace vns {

/**
 *  Pipes, L.A. (1953), An Operational Analysis of Traffic Dynamics. Journal of Applied Physics, vol.24, no.1, 274-287.
 *
 */

class Pipes : public DriverModel {
public:
	Pipes(const DriverInfo* driver);
	virtual ~Pipes();

private:
    float v0;      // desired speed

public:
    inline float getDesiredSpeed() const {return v0; };
    float accel(const LaneObject* v, const LaneObject* fv=0);
    float accel(const LaneObject* v, float stopPos);
    float accel(const LaneObject* v, float dist, float speed, float accel);
    float updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel);

};

}

#endif
