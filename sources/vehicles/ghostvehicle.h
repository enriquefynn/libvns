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

#ifndef VNS_GHOSTVEHICLE_H_
#define VNS_GHOSTVEHICLE_H_

#include "laneobject.h"
#include "core.h"

namespace vns {

/*! \cond PRIVATE */

class Vehicle;

class GhostVehicle : public LaneObject {
private:
    Vehicle *parent;

    GhostVehicle(Vehicle* vehicle);
    ~GhostVehicle();

    void updatePosition(float d){lanePos+=d;};

    friend class Vehicle;

public:
    float getAccel() const;
    float getSpeed() const;
};

/*! \endcond */

}

#endif /* GHOSTVEHICLE_H_ */
