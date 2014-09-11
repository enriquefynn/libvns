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

#ifndef VNS_VFDM_H_
#define VNS_VFDM_H_

#include "drivermodel.h"

namespace vns {

/**
 * Full velocity difference model for a car-following theory
Rui Jiang, Qingsong Wu,* and Zuojin Zhu 2001

 *
 */

class FVDM : public DriverModel {
private:
    float v0;      // desired speed
    float s0;
    float tau;		// speed adaptation time
    float tWidth; 	// The transition width
    float beta; // form factor
    float lambda; // sensitivity

public:
	FVDM(const DriverInfo* driver);
	virtual ~FVDM();

	inline float getDesiredSpeed() const {return v0; };
    float accel(const LaneObject* v, const LaneObject* fv=0);
    float accel(const LaneObject* v, float stopPos);
    float accel(const LaneObject* v, float dist, float speed, float accel);
    float updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel);

};

}

#endif
