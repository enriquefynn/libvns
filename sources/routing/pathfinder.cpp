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

#include "pathfinder.h"
#include "roadnetwork.h"

namespace vns {

PathFinder::PathFinder(RoadNetwork* net, PathFinderHandler* hlr) {
	VNS_ASSERT( net ,"PathFinder::PathFinder","network == 0.");
	VNS_ASSERT( hlr ,"PathFinder::PathFinder","handler == 0.");
	handler = hlr;
	network = net;
}

PathFinder::~PathFinder() {}

void PathFinder::setHandler(PathFinderHandler* hlr){
	VNS_ASSERT( hlr ,"PathFinder::setHandler","handler == 0!");
	handler = hlr;
}

Path* PathFinder::findPath(Road* from, Way w1, Road* to, Way w2, VehicleType vehicleType){
	if( !network->isValidRoad(from) ) return 0;
	if( !network->isValidRoad(to) ) return 0;
	return handler->calculatePath(network, from, w1, to, w2, vehicleType);
}

}
