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

#ifndef VNS_NEWELL_H_
#define VNS_NEWELL_H_

#include "drivermodel.h"

namespace vns {

/**
 * G.F. Newell, A Simplified Car-following Theory: A Lower Order Model,
 * Transportation Research Part B, Vol 36, 2002
 *
 */

class Newell : public DriverModel {
public:
	Newell(const DriverInfo* driver);
	virtual ~Newell();

private:
    float v0;      // desired speed
    float s0;

public:
    inline float getDesiredSpeed() const {return v0; };
    float accel(const LaneObject* v, const LaneObject* fv=0);
    float accel(const LaneObject* v, float stopPos);
    float accel(const LaneObject* v, float dist, float speed, float accel);
    float updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel);

};

}

#endif
