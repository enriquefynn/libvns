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

#ifndef VNS_GIPPS_H_
#define VNS_GIPPS_H_

#include "drivermodel.h"

namespace vns {

/**
 * Gipps, P.G. 1981 A behavioural car-following model for computer simulation.
 * Transportation Research Board Part B, 15, 105-111
 */

class Gipps : public DriverModel {
public:
	Gipps(const DriverInfo* driver);
	virtual ~Gipps();

private:
    float v0;      // desired speed
    float a;       // acceleration
    float b;       // deceleration
    float s0;
    float T;       // desired safety time headway

public:
    inline float getDesiredSpeed() const {return v0; };
    float accel(const LaneObject* v, const LaneObject* fv=0);
    float accel(const LaneObject* v, float stopPos);
    float accel(const LaneObject* v, float dist, float speed, float accel);
    float updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel);

};

}

#endif
