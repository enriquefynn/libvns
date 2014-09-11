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

#include "core.h"
#include "pathstop.h"
#include <cstring>

namespace vns {

vns::PathStop::Type PathStop::fromTypeName(const char* typeName){
	if( strcmp(typeName,"leftparking") == 0 ) {
		return vns::PathStop::LeftParking;
	} else if( strcmp(typeName,"rightparking") == 0 ) {
		return vns::PathStop::RightParking;
	} else if( strcmp(typeName,"leftmostlane") == 0 ) {
		return vns::PathStop::LeftMostLane;
	} else if( strcmp(typeName,"rightmostlane") == 0 ) {
		return vns::PathStop::RightMostLane;
	} else if( strcmp(typeName,"lefttaxistand") == 0 ) {
		return vns::PathStop::LeftTaxiStand;
	} else if( strcmp(typeName,"righttaxistand") == 0 ) {
		return vns::PathStop::RightTaxiStand;
	}
	return vns::PathStop::RightMostLane;
}

const char* PathStop::fromType(vns::PathStop::Type type){
	switch(type){
	case LeftParking: return "leftparking";
	case RightParking: return "rightparking";
	case LeftMostLane: return "leftmostlane";
	case RightMostLane: return "rightmostlane";
	case LeftTaxiStand: return "lefttaxistand";
	case RightTaxiStand: return "righttaxistand";
	}
	return 0;
}

PathStop::PathStop(PathStop::Type t, float laneP, float d){
	type = t;
	lanePos = laneP;
	time = d;
};

void PathStop::write(DataStream& f, const PathStop* stop){
	f << int8(stop->type);
	f << stop->lanePos;
	f << stop->time;
};

PathStop* PathStop::read(DataStream& f){
	float position;
	float duration;
	int8 type;
	f >> type;
	f >> position;
	f >> duration;
	return new PathStop(PathStop::Type(type),position,duration);
}

}
