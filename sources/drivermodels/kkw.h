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

#ifndef VNS_KKW_H_
#define VNS_KKW_H_

#include "drivermodel.h"

namespace vns {

/*! \cond PRIVATE */

class KKW : public DriverModel {
public:
	KKW(const DriverInfo* driver);
	virtual ~KKW();

private:
    float v0;      // desired speed
    float k;
    float pb0;
    float pb1;
    float pa1;
    float pa2;
    float vp;

public:
    inline float getDesiredSpeed() const {return v0; };
    float accel(const LaneObject* v, const LaneObject* fv=0);
    float accel(const LaneObject* v, float stopPos);
    float accel(const LaneObject* v, float dist, float speed, float accel);
    float updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel);

};

/*! \endcond */

}

#endif
