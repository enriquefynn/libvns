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

#ifndef TRAFFICLIGHTSGENERATOR_H_
#define TRAFFICLIGHTSGENERATOR_H_

#include "vector.h"
#include "trafficlightcontroller.h"
#include "roadnetwork.h"

namespace vns {

class TrafficLightsGenerator {
public:
	TrafficLightsGenerator();
	virtual ~TrafficLightsGenerator();

	void generateTrafficLights(vns::RoadNetwork* network);
	vns::TrafficLightController* generateTrafficLights(vns::Junction* junction);

private:
	bool areTrafficLightsAllowed(vns::Junction* junction);
	bool conflictsExist(vns::Lane* laneA,vns::Lane* laneB);
	void insertEntryLanes(vns::Lane* lane, vns::Vector< vns::List<vns::Lane*> >& states);
	bool insert( vns::Lane* lane , vns::List<vns::Lane*>& lanes);
};

}

#endif /* TRAFFICLIGHTSGENERATOR_H_ */
