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

#ifndef VNS_TABLEHANDLER_H_
#define VNS_TABLEHANDLER_H_

#include "core.h"
#include "roadnetwork.h"
#include "path.h"
#include "pathfinderhandler.h"
#include "membasedroutingtable.h"

namespace vns {

class TableHandler : public PathFinderHandler {

private:

	const char* fileName;
	MemBasedRoutingTable* table;

public:
	TableHandler(const char* filename);
	TableHandler(MemBasedRoutingTable* tb);
	virtual ~TableHandler();

	Path* calculatePath(RoadNetwork* network, Road* from, Way wayFrom, Road* to, Way wayTo,const VehicleType& type=vns::CAR);
};

}

#endif
