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

#include "trafficlightcontroller.h"
#include "fixedtimetrafficlightcontroller.h"
#include "pretimedtrafficlightcontroller.h"
#include "roadnetwork.h"

namespace vns {


TrafficLightState::TrafficLightState(){};
TrafficLightState::TrafficLightState(float t, const Vector<Light>& colors){
	duration = t;
	lights.resize( colors.size() );
	for(int8 i=0;i<colors.size();i++){
		lights[i] = colors[i];
	}
}

TrafficLightState::~TrafficLightState(){
	lights.clear();
}

Light TrafficLightState::getLight(int8 i) const {
	return lights.at(i);
}

void TrafficLightState::setLight(int8 i,Light light){
	lights[i] = light;
}

int8 TrafficLightState::getNumberOfLights() const {
	return lights.size();
}

TrafficLightController::TrafficLightController(int32 id, TrafficLightController::Type type, RoadNetwork* network):type(type),id(id),network(network){}

TrafficLightController::~TrafficLightController() {}

DataStream& operator <<(DataStream& f, const Light& color){
	f << (int8)color;
	return f;
}

DataStream& operator >>(DataStream& f, Light& color){
	int8 c;
	f >> c;
	color = Light(c);
	return f;
}

DataStream& operator <<(DataStream& f, const TrafficLightState* state){
	f << state->duration;
	f << state->lights;
	return f;
}

DataStream& operator >>(DataStream& f, TrafficLightState* state){
	f >> state->duration;
	f >> state->lights;
	return f;
}

void TrafficLightController::write(DataStream& f,TrafficLightController* controller){
	f << int8(controller->type);
	f << controller->id;
	switch( controller->type ){
	case FixedTime:
		f << dynamic_cast<FixedTimeTrafficLightController*>(controller);
		break;
	case PreTimed:
		f << dynamic_cast<PreTimedTrafficLightController*>(controller);
		break;
	}
}

TrafficLightController* TrafficLightController::read(DataStream& f,RoadNetwork* network){
	int8 type;
	int32 id;
	f >> type;
	f >> id;
	switch( TrafficLightController::Type(type) ){
	case FixedTime:{
		FixedTimeTrafficLightController* controller = new FixedTimeTrafficLightController(id,network);
		f >> controller;
		return controller;
	}
	case PreTimed:{
		PreTimedTrafficLightController* controller = new PreTimedTrafficLightController(id,network);
		f >> controller;
		return controller;
	}
	default:
		return 0;
		break;
	}
	return 0;
}

}
