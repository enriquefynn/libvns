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

#ifndef VNS_MANHATTANMAP_H_
#define VNS_MANHATTANMAP_H_

#include "core.h"
#include "list.h"
#include "drivers.h"

namespace vns {

class RoadNetwork;
class Routes;

class ManhattanMap {
public:
	ManhattanMap(int rows,int cols);
	virtual ~ManhattanMap();

	inline void setRoadLength(float length){roadLength=length;};
	inline void setNumberOfForwardLanes(int n){flanes=n;};
	inline void setNumberOfBackwardLanes(int n){blanes=n;};
	inline void setForwardParkingLanes(bool parking){ fparking = parking; };
	inline void setBackwardParkingLanes(bool parking){ bparking = parking; };
	inline void setQueueSize(uint32 size){ queueSize = size; };

	RoadNetwork* createRoadNetwork();
	Routes* createRoutes(RoadNetwork* network,int ncars,double simtime,VehicleType vType=vns::CAR,const DriverInfo& driver=CarDriver());

private:
	uint32 queueSize;
	bool fparking;
	bool bparking;
	float roadLength;
	int flanes;
	int blanes;
	int rows;
	int cols;
};

}

#endif
