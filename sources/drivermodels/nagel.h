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

#ifndef VNS_NAGEL_H_
#define VNS_NAGEL_H_

#include "drivermodel.h"

namespace vns {

/*! \cond PRIVATE */

/**
 * Nagel, K., Schrekenberg, M. (1992) A Cellular Automaton Model for Freeway Traffic,
Journal of Physics, France, Vol. I2, pp. 2221.
 *
 */

class Nagel : public DriverModel {
public:
	Nagel(const DriverInfo* driver);
	virtual ~Nagel();

    float v0;      // desired speed
    float pSlowDown;
    float pSlowToStart;

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
