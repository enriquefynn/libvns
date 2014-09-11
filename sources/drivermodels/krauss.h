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

#ifndef VNS_KRAUSS_H_
#define VNS_KRAUSS_H_

#include "drivermodel.h"

namespace vns {

/**
 * Krauss, S. (1997) Microscopic Modeling of Traffic Flow: Investigation of Collision Free
Vehicle Dynamics, Ph.D. Thesis, University of Cologne, Cologne, Germany.
 *
 */

class Krauss : public DriverModel {
public:
	Krauss(const DriverInfo* driver);
	virtual ~Krauss();

private:
    float v0;      // desired speed
    float a;       // acceleration
    float b;       // deceleration
    float s0;
    float epsilon;

public:
    inline float getDesiredSpeed() const {return v0; };
    float accel(const LaneObject* v, const LaneObject* fv=0);
    float accel(const LaneObject* v, float stopPos);
    float accel(const LaneObject* v, float dist, float speed, float accel);
    float updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel);

};

}

#endif
