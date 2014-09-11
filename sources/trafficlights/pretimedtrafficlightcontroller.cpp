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

#include "pretimedtrafficlightcontroller.h"
#include "roadnetwork.h"
#include "simulator.h"

namespace vns {

DataStream& operator <<(DataStream& f, const PreTimedState* state){
	f << state->time;
	f << (int32)state->states.size();
	for(int32 i=0;i<state->states.size();i++){
		f << state->states.at(i);
	}
	return f;
}

DataStream& operator >>(DataStream& f, PreTimedState* state){
	f >> state->time;
	int32 n;
	f >> n;
	state->states.resize(n);
	for(int32 i=0;i<n;i++){
		state->states[i] = new TrafficLightState();
		f >> state->states[i];
	}
	return f;
}

DataStream& operator <<(DataStream& f, PreTimedTrafficLightController* controller){
	f << (int32)controller->lanes.size();
	for(int8 i=0;i<controller->lanes.size();i++){
		Lane* lane = controller->lanes.at(i);
		f << lane->getRoad()->getID();
		f << lane->getID();
	}
	f << (int32)controller->preTimedStates.size();
	for(int32 i=0;i<controller->preTimedStates.size();i++){
		f << controller->preTimedStates.at(i);
	}
	return f;
}

DataStream& operator >>(DataStream& f, PreTimedTrafficLightController* controller){
	int32 n;
	int32 roadID;
	int32 laneID;
	f >> n;
	controller->lanes.resize(n);
	for(int8 i=0;i<n;i++){
		f >> roadID;
		f >> laneID;
		Lane* lane = controller->getRoadNetwork()->getLane(roadID,laneID);
		if(lane){
			controller->lanes[i] = lane;
			controller->lanes[i]->setTrafficLightColor( OffLight );
		}
	}
	f >> n;
	controller->preTimedStates.resize(n);
	for(int32 i=0;i<n;i++){
		controller->preTimedStates[i] = new PreTimedState(controller);
		f >> controller->preTimedStates[i];
	}
	return f;
}

PreTimedState::PreTimedState(PreTimedTrafficLightController* ctl){
	controller = ctl;
}

PreTimedState::PreTimedState(double t,PreTimedTrafficLightController* ctl){
	time = t;
	controller = ctl;
}

PreTimedState::~PreTimedState(){
	int32 nl = states.size();
	for(int32 i=0;i<nl;i++){
		delete states[i];
	}
	states.clear();
}

int8 PreTimedState::addState(float duration, const Vector<Light>& lights){
	VNS_ASSERT( lights.size() == controller->getNumberOfLights() ,"PreTimedState::addState","*** Invalid number of lights!");
	TrafficLightState* state = new TrafficLightState(duration,lights);
	states.append( state );
	return states.size()-1;
}

void PreTimedState::initialize(Simulator*){
	currentState = 0;
	if( states.size() == 0 ){
		for( int32 i=0 ; i < controller->lanes.size() ; i++ ){
			if(controller->lanes[i]){
				controller->lanes[i]->setTrafficLightColor( OffLight );
			}
		}
	}else{
		TrafficLightState* state = states.at( currentState );
		comuteTime = time + state->getDuration();
		for( int32 i=0 ; i < state->getNumberOfLights() ; i++ ){
			if(controller->lanes[i]){
				controller->lanes[i]->setTrafficLightColor( state->getLight(i) );
			}
		}
	}
}

void PreTimedState::update(Simulator*){
	if( states.size() == 0 ){
		for( int32 i=0 ; i < controller->lanes.size() ; i++ ){
			if(controller->lanes[i]){
				controller->lanes[i]->setTrafficLightColor( OffLight );
			}
		}
	}else{
		if( time >= comuteTime ){
			currentState = (1 + currentState) % states.size();
			TrafficLightState* state = states.at(currentState);
			comuteTime = time + state->getDuration();
			for( int32 i=0 ; i < state->getNumberOfLights() ; i++ ){
				if(controller->lanes[i]){
					controller->lanes[i]->setTrafficLightColor( state->getLight(i) );
				}
			}
		}
	}
}

PreTimedTrafficLightController::PreTimedTrafficLightController(int32 id,RoadNetwork* network) :
	TrafficLightController(id, TrafficLightController::PreTimed, network) {}

PreTimedTrafficLightController::~PreTimedTrafficLightController() {
	uninstallLights();
	int32 ns = preTimedStates.size();
	for(int32 i=0;i<ns;i++){
		if(preTimedStates[i]){
			delete preTimedStates[i];
			preTimedStates[i] = 0;
		}
	}
	preTimedStates.clear();
}

bool PreTimedTrafficLightController::hasLightInstalled(Lane* lane){
	int32 nl = lanes.size();
	for(int32 i=0;i<nl;i++){
		if( lanes.at(i) == lane ){
			return true;
		}
	}
	return false;
}

bool PreTimedTrafficLightController::installLight(Lane* lane){
	VNS_ASSERT( getRoadNetwork()->isValidLane(lane) ,"PreTimedTrafficLightController::installLight","Invalid Lane!");
	if( lane->getTrafficLightColor() != NoLight || hasLightInstalled(lane)){
		return false;
	}
	lanes.append( lane );
	lane->setTrafficLightColor( OffLight );
	return true;
}

void PreTimedTrafficLightController::uninstallLight(Lane* lane){
	VNS_ASSERT( getRoadNetwork()->isValidLane(lane) ,"PreTimedTrafficLightController::uninstallLight","Invalid Lane!");
	int32 nl = lanes.size();
	for (int32 i = 0; i < nl; i++) {
		if (lanes.at(i) == lane) {
			lane->setTrafficLightColor( NoLight );
			lanes.remove(i);
			return;
		}
	}
}

void PreTimedTrafficLightController::installLights(const Vector<Lane*>& ls){
	for(int32 i=0;i< ls.size();i++){
		VNS_ASSERT( getRoadNetwork()->isValidLane(ls.at(i)) ,"PreTimedTrafficLightController::installLights","Invalid Lane!");
	}

	int32 nl = lanes.size();
	for(int32 i=0;i<nl;i++){
		if(lanes[i]){
			lanes[i]->setTrafficLightColor( NoLight );
		}
	}

	lanes.resize( ls.size() );
	nl = lanes.size();
	for(int32 i=0;i<nl;i++){
		lanes[i] = ls.at(i);
		lanes[i]->setTrafficLightColor( OffLight );
	}
}

void PreTimedTrafficLightController::uninstallLights(){
	int32 nl = lanes.size();
	for(int32 i=0;i<nl;i++){
		if(lanes[i]){
			lanes[i]->setTrafficLightColor( NoLight );
		}
	}
	lanes.clear();
}


PreTimedState* PreTimedTrafficLightController::newPreTimedState( double scheduledTimed ){
	int32 ns = preTimedStates.size();
	for(int32 i=0;i<ns;i++){
		if( scheduledTimed == preTimedStates[i]->getScheduledTime() ){
			return 0;
		}
		if( scheduledTimed < preTimedStates[i]->getScheduledTime() ){
			PreTimedState* state = new PreTimedState(scheduledTimed,this);
			preTimedStates.insert( i, state );
			return state;
		}
	}
	PreTimedState* state = new PreTimedState(scheduledTimed,this);
	preTimedStates.append( state );
	return state;
}

void PreTimedTrafficLightController::removePreTimedState(PreTimedState* state){
	int32 ns = preTimedStates.size();
	for(int32 i=0;i<ns;i++){
		if( state == preTimedStates[i] ){
			delete state;
			state = 0;
			preTimedStates.remove(i);
			return;
		}
	}
}

void PreTimedTrafficLightController::removePreTimedState(int32 i){
	if(i>=0 && i<preTimedStates.size()){
		delete preTimedStates[i];
		preTimedStates.remove(i);
	}
}


void PreTimedTrafficLightController::initialize(Simulator* sim){
	currentState = -1;
	if( preTimedStates.size() == 0 ){
		for( int32 i=0 ; i < getNumberOfLights(); i++ ){
			if(lanes[i]){
				lanes[i]->setTrafficLightColor( OffLight );
			}
		}
	}else{
		PreTimedState* state = preTimedStates.at( 0 );
		if( sim->getSimulationTime() > state->getScheduledTime() ){
			currentState = 0;
			state->initialize(sim);
		}else{
			for( int32 i=0 ; i < getNumberOfLights(); i++ ){
				if(lanes[i]){
					lanes[i]->setTrafficLightColor( OffLight );
				}
			}
		}
		//double comuteTime = sim->getSimulationTime()+state->getDuration();
		//sim->getEventScheduler()->schedule(comuteTime,&FixedTimeTrafficLightController::update,this,sim);
	}

}

void PreTimedTrafficLightController::update(Simulator* sim){
	if( preTimedStates.size() == 0 ){
		for( int32 i=0 ; i < getNumberOfLights(); i++ ){
			if(lanes[i]){
				lanes[i]->setTrafficLightColor( OffLight );
			}
		}
	}else{
		if(currentState<0){
			PreTimedState* state = preTimedStates.at( 0 );
			if( sim->getSimulationTime() > state->getScheduledTime() ){
				currentState = 0;
				state->initialize(sim);
			}else{
				for( int32 i=0 ; i < getNumberOfLights(); i++ ){
					if(lanes[i]){
						lanes[i]->setTrafficLightColor( OffLight );
					}
				}
			}
			return;
		}
		if( currentState < preTimedStates.size()-1){
			PreTimedState* nextState = preTimedStates.at( currentState + 1 );
			if( sim->getSimulationTime() > nextState->getScheduledTime() ){
				currentState++;
				nextState->initialize(sim);
			}else{
				PreTimedState* state = preTimedStates.at( currentState );
				state->update(sim);
			}
		}else{
			PreTimedState* state = preTimedStates.at( currentState );
			state->update(sim);
		}
	}
}

}
