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

#include "mapfilters.h"
#include "roadnetwork.h"
#include "vector.h"

namespace vns {


void MapFilters::simplifyJunctions(vns::RoadNetwork* network){
	int i=0;
	while( i < network->getNumberOfJunctions() ){
		vns::Junction* junction = network->getJunction(i);
		if( junction->getNumberOfRoads() == 2 ){
			network->mergeRoads(junction);
		}else{
			i++;
		}
	}
}

void MapFilters::removeIslands(vns::RoadNetwork* network){
	int i=0;
	while( i < network->getNumberOfRoads() ){
		vns::Road* road = network->getRoad(i);
		if( road->getStartJunction() == 0 && road->getEndJunction() == 0 ){
			network->deleteRoad(road);
		}else{
			i++;
		}
	}
}

vns::RoadNetwork* MapFilters::generateSubNetwork(vns::RoadNetwork* network, int nRoads){
	vns::RoadNetwork* net = new vns::RoadNetwork();
	net->setOffset( network->getOffset() );
	net->setAutomaticConnectivityGeneration(false);
	for(int i=0;i<network->getNumberOfRoads() && i<nRoads;i++){
		vns::Road* rd = network->getRoad(i);
		vns::PolyLine* pol = rd->getPolyLine();
		vns::Vector<vns::Vec> pt;
		pt.resize(pol->getNumberOfPoints());
		for(int p=0;p<pol->getNumberOfPoints();p++){
			pt[p] = pol->getPoint(p);
		}
		net->createRoad(new vns::PolyLine(pt),rd->getNumberOfForwardLanes(),rd->getNumberOfBackwardLanes());
	}
	net->generateRoadsConnectivity();
	net->setAutomaticConnectivityGeneration(true);
	return net;
}

};
