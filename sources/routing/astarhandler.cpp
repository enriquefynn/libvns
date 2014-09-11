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

#include "astarhandler.h"
#include "roadnetwork.h"
#include <queue>
#include "list.h"

namespace vns {

AStarHandler::AStarHandler(WeightFunction* f) {
	VNS_ASSERT( f != 0 ,"AStarHandler::AStarHandler","Null instance of WeightFunction!");
	function = f;
}

AStarHandler::~AStarHandler() {

}

Path* AStarHandler::calculatePath(RoadNetwork* network, Road* r1, Way w1, Road* r2, Way w2, const VehicleType& vType){
	if(r1==r2 && w1==w2) return 0;
	if(!r1->allowsVehicle(vType,w1) || !r2->allowsVehicle(vType,w2)) return 0;

	NR = network->getNumberOfRoads();
	visited.resize(NR*2);
	visited.fill(false,visited.size());

	std::priority_queue<AStarHandler::Node*,std::vector<AStarHandler::Node*>,AStarHandler::Comp> pq;

	Vec p1 = (w1 == Forward?r1->getPolyLine()->getLastPoint():r1->getPolyLine()->getFirstPoint());
	Vec p2 = (w2 == Forward?r2->getPolyLine()->getLastPoint():r2->getPolyLine()->getFirstPoint());

	vns::List<AStarHandler::Node*> mem;

	AStarHandler::Node* start = new AStarHandler::Node(r1,w1, function->getG(r1,w1), function->getH(p1,p2));
	mem.push_back( start );
	pq.push( start );

	while( !pq.empty() ){

		AStarHandler::Node* node = pq.top();
		pq.pop();

		int32 id = (node->way==vns::Forward?node->road->getID():NR+node->road->getID());

		if( !visited[id] ){
			if( node->road == r2 && node->way == w2 ){
				Path* path = createPath( node );
				while( !mem.isEmpty() ){
					AStarHandler::Node* node = mem.takeFirst();
					delete node;
				}
				return path;
			}else{
				Junction* junction = node->road->getEndJunction( node->way );
				if( junction ){
					for(int32 i=0;i<junction->getNumberOfRoads();i++){
						Road* nextRoad = junction->getRoad(i);
						Way way = (junction == nextRoad->getStartJunction()?Forward:Backward);
						if( nextRoad != node->road && function->isPossibleToCross(node->road,node->way,nextRoad,way,vType) ) {
							p1 = (junction == nextRoad->getStartJunction()?nextRoad->getPolyLine()->getLastPoint():nextRoad->getPolyLine()->getFirstPoint());
							AStarHandler::Node* nextNode = new AStarHandler::Node( nextRoad, way, node->g + function->getG(nextRoad,way), function->getH(p1,p2) , node );
							mem.push_back( nextNode );
							pq.push( nextNode );
						}
					}
				}
				visited[id] = true;
			}
		}
	}
	while( !mem.isEmpty() ){
		AStarHandler::Node* node = mem.takeFirst();
		delete node;
	}
	return 0;
}


Path* AStarHandler::createPath(const AStarHandler::Node* node){
	Path* path = new Path();
	while( node != 0 ){
		path->prependNode(node->road,node->way);
		node = node->parent;
	}
	if(path->getNumberOfNodes()==0){
		delete path;
		return 0;
	}
	return path;
}

}
