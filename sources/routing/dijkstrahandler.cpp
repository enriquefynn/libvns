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

#include "dijkstrahandler.h"
#include "roadnetwork.h"
#include <queue>

namespace vns {

DijkstraHandler::DijkstraHandler(WeightFunction* f) {
	VNS_ASSERT( f != 0 ,"DijkstraHandler::DijkstraHandler","Null instance of WeightFunction!");
	function = f;
}

DijkstraHandler::~DijkstraHandler() {

}

Path* DijkstraHandler::calculatePath(RoadNetwork* network,Road* r1, Way w1, Road* r2, Way w2, const VehicleType& vType){
	if(r1==r2 && w1==w2) return 0;
	if(!r1->allowsVehicle(vType,w1) || !r2->allowsVehicle(vType,w2)) return 0;

	std::priority_queue<DijkstraHandler::Node,std::vector<DijkstraHandler::Node>,DijkstraHandler::Comp> pq;

	DijkstraHandler::Node start(r1, w1, 0);
	pq.push( start );

	NR = network->getNumberOfRoads();
	dist.resize(NR*2);
	prev.resize(NR*2);

	dist.fill(vns::MAX_DOUBLE,dist.size());


	int32 id = (start.way==vns::Forward?start.road->getID():NR+start.road->getID());
	dist[id] = 0.f;
	prev[id] = -1;

	while( !pq.empty() ){
		DijkstraHandler::Node node = pq.top();
		pq.pop();

		double d = node.w;

		int idcurr = (node.way==vns::Forward?node.road->getID():NR+node.road->getID());
		double dcurr = dist[idcurr];

		if(node.road == r2 && node.way == w2){
			return createPath(network,idcurr);
		}
		if( d <= dcurr ){
			vns::Junction* junction = node.road->getEndJunction(node.way);
			if (junction) {
				for (int32 i = 0; i < junction->getNumberOfRoads(); i++) {
					vns::Road* nextRoad = junction->getRoad(i);
					vns::Way way = (junction== nextRoad->getStartJunction() ? vns::Forward: vns::Backward);
					if (nextRoad != node.road && function->isPossibleToCross(node.road,node.way,nextRoad,way,vType)) {
						int idnext = (way==vns::Forward?nextRoad->getID():NR+nextRoad->getID());
						double tmp = dcurr + function->getG(nextRoad,way);
						if(  tmp < dist[idnext] ) {
							dist[idnext] = tmp;
							prev[idnext] = idcurr;
							pq.push( DijkstraHandler::Node(nextRoad, way, tmp) );
						}
					}
				}
			}
		}
	}
	return 0;
}

vns::Path* DijkstraHandler::createPath(RoadNetwork* network,int32 id){
	vns::Path* path = new vns::Path();
	while( prev[id] != -1){
		vns::Way way = (id<NR?vns::Forward:vns::Backward);
		int32 rid = (id<NR?id:id-NR);
		path->prependNode(network->getRoad(rid),way);
		id = prev[id];
	}
	vns::Way way = (id<NR?vns::Forward:vns::Backward);
	int32 rid = (id<NR?id:id-NR);
	if(rid>=0 && rid<NR){
		path->prependNode(network->getRoad(rid),way);
	}
	if(path->getNumberOfNodes()==0){
		delete path;
		return 0;
	}
	return path;
}

}
