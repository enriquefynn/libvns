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

#include "fixedtimetrafficlightcontroller.h"
#include "roadnetwork.h"
#include "simulator.h"

namespace vns {

DataStream& operator <<(DataStream& f, FixedTimeTrafficLightController* controller){
	f << (int32)controller->lanes.size();
	for(int32 i=0;i<controller->lanes.size();i++){
		Lane* lane = controller->lanes.at(i);
		f << lane->getRoad()->getID();
		f << int8(lane->getID());
	}
	f << (int32)controller->states.size();
	for(int32 i=0;i<controller->states.size();i++){
		f << controller->states.at(i);
	}
	f << controller->timeOffset;
	return f;
}

DataStream& operator >>(DataStream& f, FixedTimeTrafficLightController* controller){
	int32 n;
	int32 roadID;
	int8 laneID;
	f >> n;
	controller->lanes.resize(n);
	for(int32 i=0;i<n;i++){
		f >> roadID;
		f >> laneID;
		Lane* lane = controller->getRoadNetwork()->getLane(roadID,laneID);
		if(lane){
			controller->lanes[i] = lane;
			controller->lanes[i]->setTrafficLightColor( OffLight );
		}
	}
	f >> n;
	controller->states.resize(n);
	for(int32 i=0;i<n;i++){
		controller->states[i] = new TrafficLightState();
		f >> controller->states[i];
	}
	f >> controller->timeOffset;
	return f;
}

FixedTimeTrafficLightController::FixedTimeTrafficLightController(int32 id,RoadNetwork* network) :
	TrafficLightController(id, TrafficLightController::FixedTime, network) {
	currentState = 0;
	timeOffset = 0;
}

FixedTimeTrafficLightController::~FixedTimeTrafficLightController() {
	clearStates();
}

bool FixedTimeTrafficLightController::hasLightInstalled(Lane* lane) const {
	int32 nl = lanes.size();
	for(int32 i=0;i<nl;i++){
		if( lanes.at(i) == lane ){
			return true;
		}
	}
	return false;
}

bool FixedTimeTrafficLightController::installLight(Lane* lane){
	VNS_ASSERT( getRoadNetwork()->isValidLane(lane) ,"FixedTimeTrafficLightController::installLight","Invalid Lane!");
	if( lane->getTrafficLightColor() != NoLight || hasLightInstalled(lane)){
		return false;
	}
	lanes.append( lane );
	lane->setTrafficLightColor( OffLight );
	int32 ns = states.size();
	for(int32 i=0;i<ns;i++){
		states.at(i)->lights.append(GreenLight);
	}
	return true;
}

void FixedTimeTrafficLightController::uninstallLight(Lane* lane){
	VNS_ASSERT(getRoadNetwork()->isValidLane(lane) ,"FixedTimeTrafficLightController::uninstallLight","Invalid Lane!");
	int32 nl = lanes.size();
	for (int32 i = 0; i < nl; i++) {
		if (lanes.at(i) == lane) {
			lane->setTrafficLightColor( NoLight );
			int32 ns = states.size();
			for(int32 s=0;s<ns;s++){
				states.at(s)->lights.remove(i);
			}
			lanes.remove(i);
			return;
		}
	}
}


void FixedTimeTrafficLightController::uninstallLights(){
	int32 nl = lanes.size();
	for(int32 i=0;i<nl;i++){
		if(lanes[i]){
			lanes[i]->setTrafficLightColor( NoLight );
		}
	}
	lanes.clear();
	int32 ns = states.size();
	for(int32 s=0;s<ns;s++){
		states.at(s)->lights.clear();
	}
}



int32 FixedTimeTrafficLightController::addState( float duration, const Vector<Light>& colors){
	VNS_ASSERT( colors.size() == getNumberOfLights() ,"TrafficLightController::addState","*** Invalid number of lights!");
	TrafficLightState* state = new TrafficLightState(duration,colors);
	states.append(state);
	return states.size()-1;
}

void FixedTimeTrafficLightController::removeState(int32 i){
	if(i>=0 && i<states.size()){
		TrafficLightState* state = states.at(i);
		delete state;
		states.remove(i);
	}
}

void FixedTimeTrafficLightController::clearStates(){
	int32 nl = states.size();
	for(int32 i=0;i<nl;i++){
		delete states[i];
	}
	states.clear();
}

void FixedTimeTrafficLightController::initialize(Simulator* sim){
	currentState = 0;
	if( states.size() == 0 ){
		for( int8 i=0 ; i < getNumberOfLights() ; i++ ){
			if(lanes[i]){
				lanes[i]->setTrafficLightColor( OffLight );
			}
		}
	}else{
		TrafficLightState* state = states.at( currentState );
		double t = timeOffset;
		while( t >= state->getDuration() ){
			t -= state->getDuration();
			currentState = (1 + currentState) % states.size();
			state = states.at(  currentState );
		}

		double comuteTime = sim->getSimulationTime() + (state->getDuration()-t);
		for( int8 i=0 ; i < state->getNumberOfLights() ; i++ ){
			if(lanes[i]){
				lanes[i]->setTrafficLightColor( state->getLight(i) );
			}
		}
		sim->getEventScheduler()->schedule(comuteTime,&FixedTimeTrafficLightController::update,this,sim);
	}
}

void FixedTimeTrafficLightController::update(Simulator* sim){
	currentState = (1 + currentState) % states.size();
	TrafficLightState* state = states.at(currentState);
	for( int8 i=0 ; i < state->getNumberOfLights() ; i++ ){
		if(lanes[i]){
			lanes[i]->setTrafficLightColor( state->getLight(i) );
		}
	}
	double comuteTime = sim->getSimulationTime()+state->getDuration();
	sim->getEventScheduler()->schedule(comuteTime,&FixedTimeTrafficLightController::update,this,sim);
}

}
