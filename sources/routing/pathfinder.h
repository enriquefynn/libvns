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

#ifndef VNS_PATHFINDER_H_
#define VNS_PATHFINDER_H_

#include "core.h"
#include "pathfinderhandler.h"
#include "vector.h"

namespace vns {

class PathFinder {

private:
	PathFinderHandler* handler;
	RoadNetwork* network;

public:
	PathFinder(RoadNetwork* network, PathFinderHandler* handler);
	virtual ~PathFinder();

	void setHandler(PathFinderHandler* handler);

	Path* findPath(Road* from, Way fromWay, Road* to, Way toWay, VehicleType vehicleType=vns::CAR);
};

}

#endif /* PATHFINDER_H_ */
