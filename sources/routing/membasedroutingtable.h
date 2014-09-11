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

#ifndef VNS_MEMBASEDROUTINGTABLE_H_
#define VNS_MEMBASEDROUTINGTABLE_H_

#include "core.h"
#include "routingtable.h"
#include "pathfinderhandler.h"

namespace vns {

class MemBasedRoutingTable : public RoutingTable {
private:

	int16 NR;
	int16 NR2;

	int16 **dist;
	int16 **path;

	int16 connection(int16 i ,int16 j,WeightFunction* function,const VehicleType& vType=vns::CAR);
	Path* getPath(int32 a,int32 b);
	Path* reconstructPath(int32 i,int32 j);
	Path* getPathByConnections(int32 i,int32 j);
	int16 getPrevious(int16 a, int16 j);

public:
	MemBasedRoutingTable(vns::RoadNetwork* network);
	virtual ~MemBasedRoutingTable();

	void computeAllPaths(WeightFunction* function,const VehicleType& vType=vns::CAR);

	void load(const char* fileName);
	void save(const char* fileName);
	void printDistTable();
	void printPathTable();
	vns::Path* getPath(vns::Road* src,vns::Way srcWay,vns::Road* dst,vns::Way dstWay);
	vns::Path* getPath(int32 srcID,vns::Way srcWay,int32 dstID,vns::Way dstWay);
	bool hasPath(vns::Road* src,vns::Way srcWay, vns::Road* dst,vns::Way dstWay);
	bool hasPath(int32 srcID,vns::Way srcWay,int32 dstID,vns::Way dstWay);
	int32 getWeight(int32 srcID,vns::Way srcWay,int32 dstID,vns::Way dstWay);
	int32 getWeight(vns::Road* src,vns::Way srcWay,vns::Road* dst,vns::Way dstWay);
	inline int32 getRawWeight(uint32 i,uint32 j){ return (int32)dist[i][j];};
	inline int32 getRawPath(uint32 i,uint32 j){ return (int32)path[i][j]; };
};

}

#endif /* VNS_MEMBASEDROUTINGTABLE_H_ */
