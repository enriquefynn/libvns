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

#include <omp.h>
#include <time.h>
#include "routesgenerator.h"
#include "core.h"
#include "astarhandler.h"
#include "drivers.h"

namespace vns {

RoutesGenerator::RoutesGenerator(vns::RoadNetwork* network) : network(network) {
	function = new vns::TravelTimeFunction();
	handler = defaultHandler = new vns::AStarHandler(function);
	interval = 2.0;
	vehicleType = vns::CAR;
	driver = CarDriver();
}

RoutesGenerator::~RoutesGenerator() {
	delete defaultHandler;
	delete function;
}

void RoutesGenerator::setGenerationInterval(float interv){
	interval = interv;
}

void RoutesGenerator::setVehicleType(const VehicleType& type){
	vehicleType = type;
}

void RoutesGenerator::setSourceRoads(vns::RoutesGenerator::Type type){
	sourceType = type;
}

void RoutesGenerator::setSinkRoads(vns::RoutesGenerator::Type type){
	sinkType = type;
}

void RoutesGenerator::setHandler(vns::PathFinderHandler* finderHandler){
	if(finderHandler){
		handler = finderHandler;
	}else{
		handler = defaultHandler;
	}
}

vns::Routes* RoutesGenerator::generate(int numberOfRoutes){
	vns::Routes* routes = new vns::Routes(network);
	generate(routes,numberOfRoutes);
	return routes;
}

void RoutesGenerator::generate(vns::Routes* routes,int numberOfRoutes){
	updateSourcesAndSinks();
	if(sources.size()==0 || sinks.size()==0) return;

	int32 driverID = routes->registerDriver(driver);
	//vns::Lock lock;
	//#pragma omp parallel for
	for(int i=0;i<numberOfRoutes;i++){
		// calcular
		vns::RoutesGenerator::Node src = sources.at(rand()%sources.size());
		vns::RoutesGenerator::Node dst = sinks.at(rand()%sinks.size());
		vns::Path* path = handler->calculatePath(network,src.road,src.way,dst.road,dst.way,vehicleType);
		if( path ){
			//lock.lock();
			int32 pathID = routes->addPath(path);
			routes->addRoute( pathID, interval, vehicleType, driverID );
			//lock.unlock();
		}
	}
}


void RoutesGenerator::updateSourcesAndSinks(){
	sources.clear();
	sinks.clear();
	for(int i=0;i<network->getNumberOfRoads();i++){
		vns::Road* road = network->getRoad(i);
		if( road->hasLanes(vns::Forward) ){
			if(sinkType==vns::RoutesGenerator::ALL
					|| (sinkType==vns::RoutesGenerator::PARKING && road->hasParking(vns::Forward))
					|| (sinkType==vns::RoutesGenerator::NOJUNCTION && road->getEndJunction(vns::Forward)==0)
					|| (sinkType==vns::RoutesGenerator::PARKING_OR_NOJUNCTION && (road->hasParking(vns::Forward) || road->getEndJunction(vns::Forward)==0))
			){
				sinks.append( vns::RoutesGenerator::Node(road,vns::Forward) );
			}
			if(sourceType==vns::RoutesGenerator::ALL
					|| (sourceType==vns::RoutesGenerator::PARKING && road->hasParking(vns::Forward))
					|| (sourceType==vns::RoutesGenerator::NOJUNCTION && road->getStartJunction(vns::Forward)==0)
					|| (sourceType==vns::RoutesGenerator::PARKING_OR_NOJUNCTION && (road->hasParking(vns::Forward) || road->getStartJunction(vns::Forward)==0))
			){
				sources.append( Node(road,vns::Forward) );
			}
		}
		if( road->hasLanes(vns::Backward) ){
			if(sinkType==vns::RoutesGenerator::ALL
					|| (sinkType==vns::RoutesGenerator::PARKING && road->hasParking(vns::Backward))
					|| (sinkType==vns::RoutesGenerator::NOJUNCTION && road->getEndJunction(vns::Backward)==0)
					|| (sinkType==vns::RoutesGenerator::PARKING_OR_NOJUNCTION && (road->hasParking(vns::Backward) || road->getEndJunction(vns::Backward)==0))
			){
				sinks.append( vns::RoutesGenerator::Node(road,vns::Backward) );
			}
			if(sourceType==vns::RoutesGenerator::ALL
					|| (sourceType==vns::RoutesGenerator::PARKING && road->hasParking(vns::Backward))
					|| (sourceType==vns::RoutesGenerator::NOJUNCTION && road->getStartJunction(vns::Backward)==0)
					|| (sourceType==vns::RoutesGenerator::PARKING_OR_NOJUNCTION && (road->hasParking(vns::Backward) || road->getStartJunction(vns::Backward)==0))
			){
				sources.append( Node(road,vns::Backward) );
			}
		}
	}
}


}
