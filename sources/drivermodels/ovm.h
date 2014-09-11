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

#ifndef VNS_OVM_H_
#define VNS_OVM_H_

#include "drivermodel.h"

namespace vns {

/**
 * Bando, M., K. Hasebe, A. Nakayama, A. Shibata,
and Y. Sugiyama (1995). Dynamical Model of
Traffic Congestion and Numerical Simulation.
Physical Reviews E, 51(2), 1035-1042.
 *
 */

class OVM : public DriverModel {
private:
    float v0;      // desired speed
    float s0;
    float tau;		// speed adaptation time
    float tWidth; 	// The transition width
    float beta; // form factor
    float lambda; // sensitivity

public:
	OVM(const DriverInfo* driver);
	virtual ~OVM();

	inline float getDesiredSpeed() const {return v0; };
    float accel(const LaneObject* v, const LaneObject* fv=0);
    float accel(const LaneObject* v, float stopPos);
    float accel(const LaneObject* v, float dist, float speed, float accel);
    float updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel);

};

}

#endif
