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

#include "trafficlightsgenerator.h"

namespace vns {

TrafficLightsGenerator::TrafficLightsGenerator() {
	// TODO Auto-generated constructor stub

}

TrafficLightsGenerator::~TrafficLightsGenerator() {
	// TODO Auto-generated destructor stub
}

bool TrafficLightsGenerator::areTrafficLightsAllowed(vns::Junction* junction){
	int nr = junction->getNumberOfRoads();
	if (nr <= 2) return false;
	for (int r = 0; r < nr; r++) {
		vns::Road* road = junction->getRoad(r);
		if(road->getType()==vns::Road::Motorway || road->getType()==vns::Road::MotorwayLink
				|| road->getType()==vns::Road::Trunk || road->getType()==vns::Road::TrunkLink){
			return false;
		}
	}
	return true;
}


void TrafficLightsGenerator::generateTrafficLights(vns::RoadNetwork* network){
	int nj = network->getNumberOfJunctions();
	for(int i=0;i<nj;i++){
		vns::Junction* junction = network->getJunction(i);
		generateTrafficLights( junction );
	}
}

bool TrafficLightsGenerator::conflictsExist(vns::Lane* laneA,vns::Lane* laneB){
	vns::Vec a1 = laneA->getLastPoint();
	vns::Vec b1 = laneB->getLastPoint();
	for(int c=0; c < laneA->getNumberOfConnections(); c++){
		const vns::Road* roadA = laneA->getConnection(c);
		if( roadA == 0) continue;
		for(int la=0;la<roadA->getNumberOfLanes();la++){
			const vns::Lane* destLaneA = roadA->getLane(la);
			if(destLaneA->getStartJunction()!=laneA->getEndJunction()) continue;
			vns::Vec a2 = destLaneA->getFirstPoint();

			for(int k=0;k<laneB->getNumberOfConnections();k++){
				const vns::Road* roadB = laneB->getConnection(k);
				if( roadB == 0) continue;
				for(int lb=0;lb<roadB->getNumberOfLanes();lb++){
					const vns::Lane* destLaneB = roadB->getLane(lb);
					if(destLaneB->getStartJunction()!=laneB->getEndJunction()) continue;
					vns::Vec b2 = destLaneB->getFirstPoint();
					if(vns::intersects(a1,a2,b1,b2)){
						return true;
					}
				}
			}
		}

	}
	return false;
}

void TrafficLightsGenerator::insertEntryLanes(vns::Lane* lane, vns::Vector< vns::List<vns::Lane*> >& states){
	for( int s=0 ; s < states.size() ; s++ ){
		if( insert( lane, states[s] ) ){
			return;
		}
	}
	int n = states.size();
	states.resize(n+1);
	states[n].append( lane );
}

bool TrafficLightsGenerator::insert( vns::Lane* lane , vns::List<vns::Lane*>& lanes){
	for( vns::List<vns::Lane*>::Iterator i=lanes.begin(); i!=lanes.end(); i++ ){
		vns::Lane* ln = *i;
		if(ln->getRoad() == lane->getRoad()){
			continue;
		}
		if( conflictsExist(lane,ln) ){
			double diff = abs(lane->getLastPointDirection().angleDiffTo( -ln->getLastPointDirection() ));
			if(diff < 30 && lane->hasConnectionTo(ln->getRoad()) && ln->hasConnectionTo(lane->getRoad())){
				continue;
			}
			return false;

		}else{
			continue;

		}
	}
	lanes.append(lane);
	return true;
}


vns::TrafficLightController* TrafficLightsGenerator::generateTrafficLights(vns::Junction* junction){

	if( !areTrafficLightsAllowed(junction) ) return 0;

	vns::RoadNetwork* network = junction->getRoadNetwork();
	int nr = junction->getNumberOfRoads();
	vns::List<vns::Lane*> possibles;
	for (int r = 0; r < nr; r++) {
		vns::Road* road = junction->getRoad(r);
		for (int l = 0; l < road->getNumberOfLanes(); l++) {
			vns::Lane* lane = road->getLane(l);
			if (lane->getEndJunction() == junction) {
				possibles.append(lane);
			}
		}
	}

	// Remove lanes with no conflicts -> always Green

	vns::List<vns::Lane*>::Iterator it = possibles.begin();
	while (it != possibles.end()) {
		vns::Lane* lane = (*it);
		bool conflict = false;
		for(vns::List<vns::Lane*>::Iterator l = possibles.begin(); l!=possibles.end(); l++) {
			vns::Lane* ln = *l;
			if (ln == lane || ln->getRoad() == lane->getRoad())
				continue;
			if (conflictsExist(lane, ln)) {
				conflict = true;
				break;
			}
		}
		if (conflict == true)
			it++;
		else
			it = possibles.erase(it);
	}

	vns::Vector< vns::List<vns::Lane*> > entryLanes;
	while( !possibles.isEmpty() ) {
		vns::Lane* lane = possibles.takeFirst();
		insertEntryLanes(lane, entryLanes);
	}

	if (entryLanes.size() <= 1)
		return 0;

	// now we have all entry roads
	vns::FixedTimeTrafficLightController* c =
					dynamic_cast<vns::FixedTimeTrafficLightController*> (network->createTrafficLightController(
							vns::TrafficLightController::FixedTime));

	int offset = rand() % 20;
	if( rand()%2 == 0 ){
		offset = 25 + rand() % 20;
	}

	c->setTimeOffset(double(offset * 1.0));
	for (int k = 0; k < entryLanes.size(); k++) {
		vns::List<vns::Lane*> el = entryLanes.at(k);
		for(vns::List<vns::Lane*>::Iterator l = el.begin(); l!=el.end(); l++) {
			vns::Lane* lane = *l;
			c->installLight(lane);
		}
	}

	for (int k = 0; k < entryLanes.size(); k++) {
		vns::Vector<vns::Light> lights;
		for (int x = 0; x < entryLanes.size(); x++) {
			vns::List<vns::Lane*> el = entryLanes.at(x);
			for (int l = 0; l < el.size(); l++) {
				if (x == k) {
					lights.append(vns::GreenLight);
				} else {
					lights.append(vns::RedLight);
				}
			}
		}
		for (int l = 0; l < c->getNumberOfLights(); l++) {
			vns::Lane* lane = c->getLane(l);
			if (lights.at(l) == vns::GreenLight)
				continue;
			bool conflict = false;
			for (int z = 0; z < c->getNumberOfLights(); z++) {
				if (z == l)
					continue;
				if (lights.at(z) == vns::GreenLight) {
					vns::Lane* ln = c->getLane(z);
					if (lane->getRoad() != ln->getRoad() && conflictsExist(lane, ln)) {
						conflict = true;
						break;
					}
				}
			}
			if (conflict == false) {
				lights[l] = vns::GreenLight;
			}
		}
		c->addState(20.0, lights);
		lights.clear();

		for (int x = 0; x < entryLanes.size(); x++) {
			vns::List<vns::Lane*> el = entryLanes.at(x);
			for (int l = 0; l < el.size(); l++) {
				if (x == k) {
					lights.append(vns::YellowLight);
				} else {
					lights.append(vns::RedLight);
				}
			}
		}
		for (int l = 0; l < c->getNumberOfLights(); l++) {
			vns::Lane* lane = c->getLane(l);
			if (lights.at(l) == vns::YellowLight)
				continue;
			bool conflict = false;
			for (int z = 0; z < c->getNumberOfLights(); z++) {
				if (z == l)
					continue;
				if (lights.at(z) == vns::YellowLight) {
					vns::Lane* ln = c->getLane(z);
					if (lane->getRoad() != ln->getRoad() && conflictsExist(lane, ln)) {
						conflict = true;
						break;
					}
				}
			}
			if (conflict == false) {
				lights[l] = vns::YellowLight;
			}
		}
		c->addState(5.0, lights);
		lights.clear();
	}

	// Normalize lights
	for (int s = c->getNumberOfStates() - 2; s >= 0; s--) {
		vns::TrafficLightState* st = c->getState(s);
		vns::TrafficLightState* nst = c->getState(s + 1);
		for (int l = 0; l < c->getNumberOfLights(); l++) {
			if (st->getLight(l) == vns::YellowLight && nst->getLight(l)== vns::GreenLight) {
				st->setLight(l, vns::GreenLight);
			}
		}
	}
	return c;
}

}
