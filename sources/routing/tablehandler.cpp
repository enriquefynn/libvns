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

#include "tablehandler.h"

namespace vns {

TableHandler::TableHandler(const char* name) {
	table = 0;
	fileName = name;
}

TableHandler::TableHandler(MemBasedRoutingTable* tb) {
	VNS_ASSERT( tb ,"TableHandler::TableHandler","Invalid MemBasedRoutingTable!");
	table = tb;
}

TableHandler::~TableHandler() {
	delete table;
	table = 0;
}

Path* TableHandler::calculatePath(RoadNetwork* network,Road* r1, Way w1, Road* r2, Way w2,const VehicleType& ){
	if(network==0) return 0;
	if(r1==r2 && w1==w2) return 0;
	if(table==0){
		table = new MemBasedRoutingTable(network);
		table->load(fileName);
	}
	if( table->hasPath(r1,w1,r2,w2)){
		return table->getPath(r1,w1,r2,w2);
	}
	return 0;
}

}
