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

#ifndef VNS_ROUTINGTABLE_H_
#define VNS_ROUTINGTABLE_H_

#include "core.h"
#include "path.h"

namespace vns {

class RoadNetwork;
class Road;

class RoutingTable {
protected:

	vns::RoadNetwork* network;

public:
	RoutingTable(vns::RoadNetwork* net){ network = net; };
	virtual ~RoutingTable(){};

	virtual vns::Path* getPath(vns::Road* src,vns::Way srcWay,vns::Road* dst,vns::Way dstWay) = 0;
	virtual vns::Path* getPath(int32 srcID,vns::Way srcWay,int32 dstID,vns::Way dstWay) = 0;
	virtual bool hasPath(vns::Road* src,vns::Way srcWay, vns::Road* dst,vns::Way dstWay) = 0;
	virtual bool hasPath(int32 srcID,vns::Way srcWay,int32 dstID,vns::Way dstWay) = 0;
	virtual int32 getWeight(int32 srcID,vns::Way srcWay,int32 dstID,vns::Way dstWay) = 0;
	virtual int32 getWeight(vns::Road* src,vns::Way srcWay,vns::Road* dst,vns::Way dstWay) = 0;
	virtual int32 getRawWeight(uint32 i,uint32 j) = 0;
	virtual int32 getRawPath(uint32 i,uint32 j) = 0;
	inline vns::RoadNetwork* getRoadNetwork(){ return network; };
};

}


#endif /* VNS_ROUTINGTABLE_H_ */
