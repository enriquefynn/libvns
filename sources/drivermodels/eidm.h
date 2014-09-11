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

#ifndef VNS_EIDM_H_
#define VNS_EIDM_H_

#include "drivermodel.h"

namespace vns {

/**
 * Reference:
 * Arne Kesting, Martin Treiber, Dirk Helbing,
 * "Enhanced Intelligent Driver Model to access the impact of driving strategies on traffic capacity"
 * Philosophical Transactions of the Royal Society A 368, 4585-4605 (2010)
 *
 */

class EIDM : public DriverModel {
public:
	EIDM(const DriverInfo* driver);
	virtual ~EIDM();

private:
    float T;       // safe time headway (s)
    float s0;      // bumper-to-bumper distance (m)
    float s1;
    float v0;      // desired speed (m/s)
    float a;       // maximum acceleration (m/s^2)
    float b;       // comfortable braking (m/s^2)
    float delta;     // acceleration exponent (m/s^2)
    float coolness; // The coolness. coolness=0: acc1=IIDM (without constant-acceleration heuristic, CAH), coolness=1 CAH factor in range [0, 1]


public:
    inline float getDesiredSpeed() const {return v0; };
    float accel(const LaneObject* v, const LaneObject* fv=0);
    float accel(const LaneObject* v, float stopPos);
    float accel(const LaneObject* v, float dist, float speed, float accel);
    float updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel);

};

}

#endif
