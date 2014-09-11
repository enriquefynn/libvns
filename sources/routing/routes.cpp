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

#include "routes.h"
#include "vehiclefactory.h"
#include "routesfile.h"
#include "drivers.h"

namespace vns {

Routes::Routes(RoadNetwork* net){
	VNS_ASSERT( net ,"Routes::Routes","Invalid RoadNetwork!");
	network = net;

	defaultDrivers.resize(6);
	defaultDrivers[vns::CAR] = new CarDriver();
	defaultDrivers[vns::BIKE] = new BikeDriver();
	defaultDrivers[vns::TAXI] = new TaxiDriver();
	defaultDrivers[vns::BUS] = new BusDriver();
	defaultDrivers[vns::TRUCK] = new TruckDriver();
	defaultDrivers[vns::TRAIN] = new TrainDriver();
}

Routes::Routes(RoadNetwork* net,const char* fileName){
	VNS_ASSERT( net ,"Routes::Routes","Invalid RoadNetwork!");
	network = net;

	defaultDrivers.resize(6);
	defaultDrivers[vns::CAR] = new CarDriver();
	defaultDrivers[vns::BIKE] = new BikeDriver();
	defaultDrivers[vns::TAXI] = new TaxiDriver();
	defaultDrivers[vns::BUS] = new BusDriver();
	defaultDrivers[vns::TRUCK] = new TruckDriver();
	defaultDrivers[vns::TRAIN] = new TrainDriver();

	load(fileName);
}

Routes::~Routes(){
    clear();
	for(int32 i=0;i<defaultDrivers.size();i++){
		delete defaultDrivers.at(i);
	}
	defaultDrivers.clear();
}

DataStream& operator <<(DataStream&f, const Routes* routes) {
    f << (int32)routes->drivers.size();
    for (int32 i=0; i<routes->drivers.size(); ++i){
    	DriverInfo::write(f, routes->drivers.at(i) );
    }
    f << (int32)routes->paths.size();
    for (int32 i=0; i<routes->paths.size(); ++i){
    	Path::write(f, routes->paths.at(i) );
    }
    f << (int32)routes->routes.size();
    for (int32 i=0; i<routes->routes.size(); ++i){
    	Route::write(f, routes->routes.at(i) );
    }
    f << (int32)routes->scheduledRoutes.size();
    for (int32 i=0; i<routes->scheduledRoutes.size(); ++i){
    	ScheduledRoute::write(f, routes->scheduledRoutes.at(i) );
    }
    return f;
}

DataStream& operator >>(DataStream& f, Routes* routes) {
	int32 n;
	f >> n;
	routes->drivers.resize(n);
    for (int32 i=0; i<n; i++){
    	routes->drivers[i] = vns::DriverInfo::read( f );
    }
	f >> n;
	routes->paths.resize(n);
    for (int32 i=0; i<n; i++){
    	routes->paths[i] = vns::Path::read( f , routes);
    	routes->paths[i]->id = i;
    }
	f >> n;
    routes->routes.resize(n);
    for (int32 i=0; i<n; i++){
    	routes->routes[i] = Route::read( f , routes );
    }
    f >> n;
    routes->scheduledRoutes.resize(n);
    for (int32 i=0; i<n; i++){
    	routes->scheduledRoutes[i] = ScheduledRoute::read( f , routes);
    }
    return f;
}


int32 Routes::registerDriver(const DriverInfo& driver){
	int32 n = drivers.size();
	drivers.resize(n+1);
	drivers[n] = new DriverInfo(driver);
	return n;
}

int32 Routes::addPath(vns::Path* path){
	VNS_ASSERT( path!=0 ,"Routes::addPath","Error: 'path' is NULL");
	int32 n = paths.size();
	paths.resize(n+1);
	paths[n] = path;
	path->id = n;
	return n;
}

int32 Routes::addScheduledRoute(int32 pathID,double depart,VehicleType vType,int32 driverID){
	VNS_ASSERT( pathID>=0 && pathID<paths.size() ,"Routes::addScheduledRoute","Invalid 'pathID'");
	VNS_ASSERT( driverID>=-1 && driverID<drivers.size() ,"Routes::addScheduledRoute","Invalid 'driverID'");

	vns::Path* path = paths.at(pathID);
	vns::ScheduledRoute* nroute = new ScheduledRoute(path,depart,vType,driverID);
	int32 n = scheduledRoutes.size();
	for(int32 i=0;i<n;i++){
		vns::ScheduledRoute* route = scheduledRoutes.at(i);
		if( nroute->scheduledTime < route->scheduledTime){
			scheduledRoutes.insert(i,nroute);
			return i;
		}
	}
	scheduledRoutes.insert(n,nroute);
	return n;
}

void Routes::deleteScheduledRoute(int32 i){
	VNS_ASSERT( i>=0 && i<scheduledRoutes.size() ,"Routes::removeScheduledRoute","Invalid index!");
	delete scheduledRoutes.at(i);
	scheduledRoutes.remove(i);
}

void Routes::deleteRoute(int32 i){
	VNS_ASSERT( i>=0 && i<routes.size() ,"Routes::deleteRoute","Invalid index!");
	delete routes.at(i);
	routes.remove(i);
}

int32 Routes::addRoute(int32 pathID, double interval,VehicleType vType,int32 driverID,double startTime,double endTime){
	VNS_ASSERT( pathID>=0 && pathID<paths.size() ,"Routes::addRoute","Invalid 'pathID'");
	VNS_ASSERT( driverID>=-1 && driverID<drivers.size() ,"Routes::addRoute","Invalid 'driverID'");

	vns::Path* path = paths.at(pathID);
	vns::Route* route = new vns::Route(path,interval,vType,driverID,startTime,endTime);
	int32 n = routes.size();
	routes.append( route );
	return n;
}

void Routes::clearRoutes(){
	for(int32 i=0;i<routes.size();i++){
		delete routes.at(i);
	}
    routes.clear();
}

void Routes::clearScheduledRoutes(){
	for(int32 i=0;i<scheduledRoutes.size();i++){
		delete scheduledRoutes.at(i);
	}
	scheduledRoutes.clear();
}

void Routes::clear(){
	for(int32 i=0;i<drivers.size();i++){
		delete drivers.at(i);
	}
	for(int32 i=0;i<paths.size();i++){
		delete paths.at(i);
	}
	drivers.clear();
	paths.clear();
	clearRoutes();
	clearScheduledRoutes();
}


bool Routes::save(const char* fileName) {
	vns::RoutesFile file;
	return file.save(this,fileName);
}


bool Routes::load(const char* fileName) {
	vns::RoutesFile file;
	return file.load(this,fileName);
}

}
