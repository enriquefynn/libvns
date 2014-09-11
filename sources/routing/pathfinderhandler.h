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

#ifndef VNS_PATHFINDERHANDLER_H_
#define VNS_PATHFINDERHANDLER_H_

#include "core.h"
#include "path.h"

namespace vns {

class Vec;
class Road;
class RoadNetwork;

class WeightFunction {
public:
	virtual ~WeightFunction(){};
	virtual double getG(const Road* road,Way way) = 0;
	virtual double getH(const Vec& p1, const Vec& p2) = 0;
	virtual bool isPossibleToCross(const Road* from, Way fromWay,const Road* to, Way toWay,const VehicleType& type=vns::CAR);
};

class DistanceFunction : public WeightFunction {
public:
	virtual double getG(const Road* road,Way way);
	virtual double getH(const Vec& p1, const Vec& p2);
};

class TravelTimeFunction : public WeightFunction {
public:
	virtual double getG(const Road* road, Way way);
	virtual double getH(const Vec& p1, const Vec& p2);
};


class PathFinderHandler {
public:
	virtual ~PathFinderHandler(){};
	virtual Path* calculatePath(RoadNetwork* network, Road* from, Way fromWay, Road* to, Way toWay, const VehicleType& type=vns::CAR) = 0;
};


}

#endif /* PATHFINDERHANDLER_H_ */
