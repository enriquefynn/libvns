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

#include "pathfinderhandler.h"
#include "core.h"
#include "roadnetwork.h"
#include "path.h"

namespace vns {

bool WeightFunction::isPossibleToCross(const Road* from, Way fromWay,const Road* to, Way toWay,const VehicleType& vType){
	if(!from->allowsVehicle(vType,fromWay) || !to->allowsVehicle(vType,toWay)) return false;
	if (from == to && fromWay != toWay) {
		if (from->canInvertWay(fromWay)) {
			return true;
		}
	}
	const vns::Junction* srcend = from->getEndJunction(fromWay);
	if (srcend == 0) return false;
	const vns::Junction* dststart = to->getStartJunction(toWay);
	if (dststart == 0) return false;
	if (srcend != dststart) {
		return false;
	}
	return from->hasConnectionTo(to);
}

double DistanceFunction::getG(const Road* road,Way){
	return road->getLength();
}

double DistanceFunction::getH(const Vec& p1, const Vec& p2){
	return p1.distanceTo(p2);
}

double TravelTimeFunction::getG(const Road* road, Way way){
	int32 nl = road->getNumberOfLanes();
	double speed = 1.0;
	for (int32 i = 0; i < nl; i++) {
		const Lane* ln = road->getLane(i);
		if (ln->getWay() == way && ln->getMaximumSpeed() > speed) {
			speed = ln->getMaximumSpeed();
		}
	}
	return road->getLength() / speed;
}

double TravelTimeFunction::getH(const Vec& p1, const Vec& p2){
	return p1.distanceTo(p2)/25.0;
}

}
