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

#ifndef VNS_TRAINCARMODEL_H_
#define VNS_TRAINCARMODEL_H_

#include "drivermodel.h"

namespace vns {

/*! \cond PRIVATE */

class TrainCarModel : public DriverModel {
public:
	TrainCarModel();
	virtual ~TrainCarModel();

private:
    float s0;

public:
    inline float getDesiredSpeed() const {return 0; };
    float accel(const LaneObject* v, const LaneObject* fv=0);
    float accel(const LaneObject* v, float stopPos);
    float accel(const LaneObject* v, float dist, float speed, float accel);
    float updatePositionAndSpeed(float dt,float lanePos,float &speed,float &accel);
    inline bool isLaneChangeAllowed(){ return false; };
};

/*! \endcond */

}

#endif
