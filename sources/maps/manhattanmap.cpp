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

#include "manhattanmap.h"
#include "roadnetwork.h"
#include "routes.h"

namespace vns {

ManhattanMap::ManhattanMap(int cols,int rows) :rows(rows),cols(cols){
	roadLength = 200;
	flanes = 1;
	blanes = 1;
	queueSize = 0;
	fparking = false;
	bparking = false;
}

ManhattanMap::~ManhattanMap() {

}


vns::RoadNetwork* ManhattanMap::createRoadNetwork(){
	vns::RoadNetwork* network = new vns::RoadNetwork();
	network->setAutomaticConnectivityGeneration(false);
	double space = roadLength;

	vns::Vector<vns::Vec> v;
	for(int32 y=0;y<rows;y++){
		for(int32 x=-1;x<cols;x++){
			v.clear();
			v << vns::Vec(x*space,y*space) << vns::Vec((x+1)*space,y*space);
			vns::PolyLine* line = new vns::PolyLine(v);
			vns::Road* road = network->createRoad(line,flanes,blanes);
			road->setRightParking(fparking);
			road->setLeftParking(bparking);
			road->setMaximumCarsInQueue(vns::Forward,queueSize);
			road->setMaximumCarsInQueue(vns::Backward,queueSize);
		}
	}
	for(int32 x=0;x<cols;x++){
		for(int32 y=-1;y<rows;y++){
			v.clear();
			v << vns::Vec(x*space,y*space) << vns::Vec(x*space,(y+1)*space);
			vns::PolyLine* line = new vns::PolyLine(v);
			vns::Road* road = network->createRoad(line,flanes,blanes);
			road->setRightParking(fparking);
			road->setLeftParking(bparking);
			road->setMaximumCarsInQueue(vns::Forward,queueSize);
			road->setMaximumCarsInQueue(vns::Backward,queueSize);
		}
	}

	network->generateRoadsConnectivity();
	network->setAutomaticConnectivityGeneration(true);
	return network;
}

vns::Routes* ManhattanMap::createRoutes(vns::RoadNetwork* network,int ncars,double simtime,VehicleType vType, const vns::DriverInfo& driver){
	vns::Routes* routes = new vns::Routes(network);

	int32 driverID = routes->registerDriver(driver);

	for (int i = 0; i < cols; i++) {
		int rid = (cols * rows + cols) + i * cols + i;
		vns::Path* path = new vns::Path();
		int l = 0;
		for (int y = l; y <= rows; y++) {
			path->appendNode(network->getRoad(rid + y ), vns::Forward);
		}
		double interval = simtime/double(ncars);
		int32 pathID = routes->addPath(path);
		routes->addRoute( pathID,interval,vType, driverID );
	}

	for (int i = 0; i < cols; i++) {
		int rid = (cols * rows + cols) + i * cols + i;
		int l = rows;
		vns::Path* path = new vns::Path();
		for (int y = l; y >= 0; y--) {
			path->appendNode(network->getRoad(rid + y ), vns::Backward);
		}
		double interval = simtime/double(ncars);
		int32 pathID = routes->addPath(path);
		routes->addRoute( pathID,interval,vType, driverID );
	}

	for (int l = 0; l < rows; l++) {
		int sid = l*cols+l;
		vns::Path* path = new vns::Path();
		for(int x=0;x<=cols;x++){
			path->appendNode(network->getRoad(sid+x),vns::Forward);
		}
		double interval = simtime/double(ncars);
		int32 pathID = routes->addPath(path);
		routes->addRoute( pathID,interval,vType, driverID );
	}

	for (int l = 0; l < rows; l++) {
		int sid = l*cols+l;
		vns::Path* path = new vns::Path();
		for(int x=cols;x>=0;x--){
			path->appendNode(network->getRoad(sid+x),vns::Backward);
		}
		double interval = simtime/double(ncars);
		int32 pathID = routes->addPath(path);
		routes->addRoute( pathID,interval,vType, driverID );
	}

	return routes;
}

}
