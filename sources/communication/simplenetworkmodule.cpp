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

#include "simplenetworkmodule.h"
#include "simulator.h"
#include "calendarscheduler.h"

namespace vns {

SimpleNetwork::SimpleNetwork(){
	onBoardUnits = new vns::HashTable<uint64, Vehicle*>(10000);
	scheduler = new CalendarScheduler(0.05,1000);
	communicationRange = 250;
}

SimpleNetwork::~SimpleNetwork(){
	onBoardUnits->clear();
	delete onBoardUnits;
	delete scheduler;
}

void SimpleNetwork::setCommunicationRange(double range){
	communicationRange = range;
}

double SimpleNetwork::getCommunicationRange(){
	return communicationRange;
}

void SimpleNetwork::send(Vehicle* sender,void* data){
	vns::HashTable<uint64, Vehicle*>::Iterator it(onBoardUnits);
	vns::Vec pos = sender->getPosition();
	while( it.isValid() ) {
		Vehicle* vehicle = it.value();
		if(pos.distanceToLessThan(vehicle->getPosition(),communicationRange)){
			onDataReceived(sender,vehicle,data);
		}
		it.next();
	}
	onSendFinish(data);
};

SimpleNetworkModule::SimpleNetworkModule() : NetworkModule(){
	network = 0;
}

SimpleNetworkModule::~SimpleNetworkModule(){

}

void SimpleNetwork::onSimulationStart(const Simulator* sim){
	time = sim->getSimulationTime();
	onBoardUnits->clear();
	scheduler->clearEvents();
}

void SimpleNetwork::onSimulationStep(const Simulator* sim){
	time = sim->getSimulationTime();;
	scheduler->invokeEvents(time,sim->getStepTime());
}

void SimpleNetwork::onSimulationStop( const Simulator* sim){
	time = sim->getSimulationTime();
	onBoardUnits->clear();
	scheduler->clearEvents();
}

void SimpleNetwork::onVehicleCreated(const Simulator*, Vehicle* vehicle){
	onBoardUnits->insert(vehicle->getID(),vehicle);
}

void SimpleNetwork::onVehicleRemoved(const Simulator*, Vehicle* vehicle){
	onBoardUnits->remove(vehicle->getID());
}

void SimpleNetworkModule::onSimulationStart(const Simulator* sim){
	VNS_ASSERT( network != 0 ,"SimpleNetworkModule","Error: You should add a network. (SimpleNetworkModule::setNetwork)");
	network->onSimulationStart(sim);
}

void SimpleNetworkModule::onSimulationStep(const Simulator* sim){
	network->onSimulationStep(sim);
}

void SimpleNetworkModule::onSimulationStop( const Simulator* sim){
	network->onSimulationStop(sim);
}

void SimpleNetworkModule::onVehicleCreated(const Simulator* sim, Vehicle* vehicle){
	lock.lock();
	network->onVehicleCreated(sim,vehicle);
	network->onInitialize(sim,vehicle);
	lock.unlock();
}

void SimpleNetworkModule::onVehicleRemoved(const Simulator* sim, Vehicle* vehicle){
	lock.lock();
	network->onVehicleRemoved(sim,vehicle);
	lock.unlock();
}

void SimpleNetworkModule::setNetwork(SimpleNetwork* net){
	network = net;
}

}
