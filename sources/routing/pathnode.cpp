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

#include "pathnode.h"
#include "routes.h"
#include "roadnetwork.h"

namespace vns {

PathNode::PathNode(Road* road, Way way):road(road),way(way){
	next=0;
	prev=0;
}

PathNode::~PathNode(){
	clearStops();
	next=0;
	prev=0;
}

void PathNode::write(DataStream& f, const PathNode* node) {
	f << node->road->getID();
	f << bool(node->way == Forward ? true : false);
	f << (int32) node->stops.size();
	for (int32 i = 0; i < node->stops.size(); i++) {
		PathStop::write(f, node->stops.at(i));
	}
}

PathNode* PathNode::read(DataStream& f, Routes* routes) {
	int32 roadID;
	int32 n;
	bool forward;
	f >> roadID;
	f >> forward;
	Road* road = routes->getRoadNetwork()->getRoad(roadID);
	VNS_ASSERT( road, "Routes::load", "Invalid RoadNetwork!");
	PathNode* node = new PathNode(road, (forward ? Forward : Backward));
	f >> n;
	node->stops.resize(n);
	for (int32 i = 0; i < n; i++) {
		node->stops[i] = PathStop::read(f);
	}
	return node;
}

void PathNode::clearStops(){
	for(int32 i=0;i<stops.size();i++){
		delete stops[i];
	}
	stops.clear();
}

bool PathNode::addStop(PathStop::Type type, double position, float duration){
	if(position<road->getRoadStart(way) || position>road->getRoadEnd(way)){
		return false;
	}
	for(int32 i=0;i<stops.size();i++){
		if(stops.at(i)->getPosition()==position){
			return false;
		}
		if(stops.at(i)->getPosition()>position){
			PathStop* stop = new PathStop(type,position,duration);
			stops.insert(i,stop);
			return true;
		}
	}
	PathStop* stop = new PathStop(type,position,duration);
	stops.append(stop);
	return true;
}

}
