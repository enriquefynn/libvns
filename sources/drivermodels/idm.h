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

#ifndef VNS_IDM_H_
#define VNS_IDM_H_

#include "drivermodel.h"

namespace vns {

/**
 * M. Treiber, A. Hennecke, and D. Helbing,
 * "Congested traffic states in empirical observations and microscopic simulations,"
 * Physical Review E, vol. 62, no. 2, p. 1805, 2000.
 */

class IDM : public DriverModel {
public:
	IDM(const DriverInfo* driver);
	virtual ~IDM();

private:
    float v0;      // desired speed (m/s)
    float a;       // maximum acceleration (m/s^2)
    float b;       // comfortable braking (m/s^2)
    float s0;      // bumper-to-bumper distance (m)
    float s1;		// gap (m)
    float T;       // safe time headway (s)

    float delta;	// acceleration exponent (m/s^2)

public:
    inline float getDesiredSpeed() const {return v0; };
    float accel(const LaneObject* v, const LaneObject* fv=0);
    float accel(const LaneObject* v, float stopPos);
    float accel(const LaneObject* v, float dist, float speed, float accel);
    float updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel);

};

}

#endif /* VNS_DRIVERMODEL_H_ */
