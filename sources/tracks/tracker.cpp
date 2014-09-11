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

#include "tracker.h"
#include "simulator.h"
#include "vehicle.h"

namespace vns {

Tracker::Tracker() : Observer() {
	vehicles = new vns::HashTable<Vehicle*,Track*>(10000);
	stepInterval = 1.0;
	nextStep = 0;
}

Tracker::~Tracker() {

}

void Tracker::onVehicleRemoved(const Simulator* , Vehicle* vehicle){
	Track* track = vehicles->value(vehicle,0);
	vehicles->remove(vehicle);
	if( track ){
		tracks.append( track );
	}
}

void Tracker::onVehicleInjected(const Simulator* , Vehicle* vehicle){
	Track* track = new Track();
	vehicles->insert(vehicle,track);
}

void Tracker::onSimulationStart(const Simulator* sim){
	nextStep = sim->getSimulationTime() + stepInterval;
}

void Tracker::onSimulationStop(const Simulator* ){
	vns::HashTable<Vehicle*,Track*>::Iterator it(vehicles);
	while(it.isValid()){
		Track* track = it.value();
		tracks.append( track );
		it.next();
	}
	vehicles->clear();
}

void Tracker::onSimulationStep(const Simulator* sim){
	double t = sim->getSimulationTime();
	if(t>nextStep){
		vns::HashTable<Vehicle*,Track*>::Iterator it(vehicles);
		while(it.isValid()){
			Vehicle* vehicle = it.key();
			Track* track = it.value();
			Vec pos = vehicle->getPosition();
			Vec dir = vehicle->getDirection();
			track->append(Tracker::Position(pos,dir,uint32(t)));
			it.next();
		}
		nextStep = t + stepInterval;
	}
}

void Tracker::clear(){
	List<Track*>::Iterator it = tracks.begin();
	while( it!= tracks.end() ){
		Track* track = *it;
		it = tracks.erase(it);
		delete track;
	}
}

}
