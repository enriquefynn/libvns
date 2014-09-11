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

#include "trafficgenerationmodel.h"
#include "simulator.h"
#include "calendarscheduler.h"

namespace vns {


TrafficGenerationModel::TrafficGenerationModel( Routes* rts ) {
	routes = rts;
	genFunction = defaultGenFunction = new NegativeExponentialDistribution();
	scheduler = new CalendarScheduler(0.05,1000);
}

TrafficGenerationModel::~TrafficGenerationModel(){
	delete defaultGenFunction;
}

void TrafficGenerationModel::setGenerationFunction(GenerationFunction* function){
	if(function){
		genFunction = function;
	}else{
		genFunction = defaultGenFunction;
	}
}


void TrafficGenerationModel::onSimulationStart(Simulator* sim){
	if(routes==0) return;
	int32 nr = routes->getNumberOfRoutes();
	for(int i=0;i<nr;i++){
		Route* rt = routes->getRoute(i);
		vns::Path* path = rt->getPath();
		if(path->getNumberOfNodes()>0){
			//vns::Road* road = path->getFirstNode()->getRoad();
			double T = genFunction->calcT(rt->getStartTime(),rt->getInterval());
			//road->getEventScheduler()->schedule(T,&TrafficGenerationModel::addRoute,this,sim,rt);
			scheduler->schedule(T,&TrafficGenerationModel::addRoute,this,sim,rt);
		}
	}
	nr = routes->getNumberOfScheduledRoutes();
	for(int i=0;i<nr;i++){
		vns::ScheduledRoute* rt = routes->getScheduledRoute(i);
		vns::Path* path = rt->getPath();
		if(path->getNumberOfNodes()>0){
			//vns::Road* road = path->getFirstNode()->getRoad();
			//road->getEventScheduler()->schedule(rt->getScheduledTime(),&TrafficGenerationModel::addScheduledRoute,this,sim,rt);
			scheduler->schedule(rt->getScheduledTime(),&TrafficGenerationModel::addScheduledRoute,this,sim,rt);
		}
	}
}

void TrafficGenerationModel::addScheduledRoute(Simulator* sim, ScheduledRoute* route){
    Path* path = route->getPath();
    vns::Path::Iterator it = path->getFirstNodeIterator();
    VehicleType vtype = route->getVehicleType();
    int32 driverID = route->getDriver();
    if(driverID>=0){
    	sim->createVehicle( vtype, routes->getDriver(driverID), it );
    }else{
    	sim->createVehicle( vtype, routes->getDefaultDriver(vtype), it );
    }
}

void TrafficGenerationModel::addRoute(Simulator* sim, Route* route){
    Path* path = route->getPath();
    vns::Path::Iterator it = path->getFirstNodeIterator();
    VehicleType vtype = route->getVehicleType();
    int32 driverID = route->getDriver();
    if(driverID>=0){
    	sim->createVehicle( vtype, routes->getDriver(driverID), it );
    }else{
    	sim->createVehicle( vtype, routes->getDefaultDriver(vtype), it );
    }
	//vns::Road* road = path->getFirstNode()->getRoad();
	double T = genFunction->calcT(sim->getSimulationTime(),route->getInterval());
	scheduler->schedule(T,&TrafficGenerationModel::addRoute,this,sim,route);
	//road->getEventScheduler()->schedule(T,&TrafficGenerationModel::addRoute,this,sim,route);
}

void TrafficGenerationModel::onSimulationStep( Simulator* sim ){
	double t = sim->getSimulationTime();
	scheduler->invokeEvents(t,Simulator::DT);
}

void TrafficGenerationModel::onSimulationFinished( Simulator* ){
	scheduler->clearEvents();
}


}
