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

#include "ghostvehicle.h"
#include "vehicle.h"

namespace vns {

GhostVehicle::GhostVehicle(Vehicle* vehicle) : LaneObject(GhostT,vehicle->getWidth(),vehicle->getLength()){
    parent = vehicle;
}

GhostVehicle::~GhostVehicle(){
	parent = 0;
}

float GhostVehicle::getSpeed() const {
	return parent->getSpeed();
}

float GhostVehicle::getAccel() const {
	return parent->getAccel();
}

}
