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

#ifndef VNS_MAPFILTERS_H_
#define VNS_MAPFILTERS_H_

namespace vns {

class RoadNetwork;

class MapFilters {
public:

	static void simplifyJunctions(RoadNetwork* network);
	static void removeIslands(RoadNetwork* network);
	static RoadNetwork* balancedNetwork(RoadNetwork* network);
	static RoadNetwork* generateSubNetwork(RoadNetwork* network, int nRoads);
};

}

#endif
