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

#include "roadnetwork.h"
#include "mapfile.h"
#include <algorithm>

namespace vns {

RoadNetwork::RoadNetwork(){
	automaticConnectivityGeneration = true;
}

RoadNetwork::RoadNetwork(const char* file){
	automaticConnectivityGeneration = true;
	VNS_ASSERT( load(file) ,"Map File","Invalid file!");
}

RoadNetwork::~RoadNetwork(){
	clear();
}

bool RoadNetwork::save(const char* fileName) {
	vns::MapFile mapfile;
	return mapfile.save(this,fileName);
}

bool RoadNetwork::load(const char* fileName) {
	vns::MapFile mapfile;
	return mapfile.load(this,fileName);
}

void RoadNetwork::clear() {
	junctionsHash.clear();
	extents.clear();
    int32 nj = junctions.size();
    for (int32 i=0; i<nj; ++i)
        if (junctions[i])
            delete junctions[i];
    junctions.clear();
    int32 nr = roads.size();
    for (int i=0; i<nr; ++i)
        if (roads[i])
            delete roads[i];
    roads.clear();
    clearTrafficLights();
}



Road* RoadNetwork::createRoad(PolyLine* pol,int32 fl,int32 rl){
	VNS_ASSERT( fl>0 || rl>0,"RoadNetwork::createRoad","Road must have at least 1 lane!");
	Road* road = new Road(this,vns::Road::Undefined,roads.size(),pol);
	roads.append(road);
	for(int32 i=0;i<fl;i++){
		road->createLane(Forward);
	}
	for(int32 i=0;i<rl;i++){
		road->createLane(Backward);
	}
	if(automaticConnectivityGeneration){
		generateConnectivityOnRoadCreation(road );
	}
	extents.addBBox(road->getBBox());
	return road;
}

Road* RoadNetwork::createRoad(const Vector<Vec>& points, Road* road){
	Road* newRoad = road->clone();
	newRoad->setPoints( points );
	newRoad->id = roads.size();
	roads.append(newRoad);
	newRoad->update();
	if(automaticConnectivityGeneration){
		generateConnectivityOnRoadCreation( newRoad );
	}
	extents.addBBox(road->getBBox());
	return newRoad;
}

void RoadNetwork::generateConnectivityOnRoadCreation(Road* road){
	Junction* junction = getJunction( road->getFirstPoint() );
	if(junction){
		junction->addRoad( road );
	}else{
		int32 nr = roads.size();
		for(int32 i=0;i<nr;i++){
			Road* croad = roads[i];
			if( croad == road ) continue;
			if( road->getFirstPoint() == croad->getFirstPoint() ){
				Junction* junction = newJunction( road->getFirstPoint() );
				junction->addRoad(road);
				junction->addRoad(croad);
				break;
			}
			if( road->getFirstPoint() == croad->getLastPoint() ){
				Junction* junction = newJunction( road->getFirstPoint() );
				junction->addRoad(road);
				junction->addRoad(croad);
				break;
			}
		}
	}
	junction = getJunction( road->getLastPoint() );
	if(junction){
		junction->addRoad(road);
	}else{
		int32 nr = roads.size();
		for(int32 i=0;i<nr;i++){
			Road* croad = roads[i];
			if( croad == road ) continue;
			if( road->getLastPoint() == croad->getFirstPoint() ){
				Junction* junction = newJunction( road->getLastPoint() );
				junction->addRoad(road);
				junction->addRoad(croad);
				break;
			}
			if( road->getLastPoint() == croad->getLastPoint() ){
				Junction* junction = newJunction( road->getLastPoint() );
				junction->addRoad(road);
				junction->addRoad(croad);
				break;
			}
		}
	}
}

void RoadNetwork::generateRoadsConnectivity(){
	// Clear All Junctions
	int32 nj = junctions.size();
	for(int32 i=0;i<nj;i++){
		delete junctions[i];
	}
	junctions.clear();

	vns::HashTable<Vec, vns::List<Road*> > js(4096);
	int32 nr = roads.size();
	for(int32 i=0;i<nr;i++){
		Road* road = roads[i];
		js[road->getFirstPoint()].append(road);
		js[road->getLastPoint()].append(road);
	}

	vns::HashTable<Vec, vns::List<Road*> >::Iterator it(&js);
	while(it.isValid()){
		Vec p = it.key();
		vns::List<Road*>& values = it.value();
		if(values.size()>1){
			Junction* junction = newJunction( p );
			for(vns::List<Road*>::Iterator k = values.begin();k!=values.end();k++){
				junction->addRoad( *k );
			}
		}
		it.next();
	}
}

Road* RoadNetwork::getRoad(const vns::Vec& pt) {
    int32 nr = roads.size();
    for(int32 i=0; i<nr; i++){
        Road* road = roads[i];
        if(road->contains(pt)){
        	return road;
        }
    }
    return 0;
}

Road* RoadNetwork::getNearestRoad(const Vec& pt,int8 roadType) {
    int32 nr = roads.size();
    double nd=vns::MAX_DOUBLE;
    Road* nroad = 0;
    for(int32 i=0; i<nr; i++){
        Road* road = roads[i];
        if(roadType==-1 || road->getType()==roadType){
        	double d = road->getPolyLine()->distanceTo(pt);
        	if (d<nd) {
        		nd = d;
        		nroad = road;
        	}
        }
    }
    return nroad;
}

Road* RoadNetwork::getPointAndRoadWithin(Vec& pt, double radius, int32& pointIndex, int32 exceptRoad) {
    int32 nr = roads.size();
    for(int32 i=0; i<nr; ++i){
        Road* r = roads[i];
        if(r->getBBox().contains(pt) && r->getID() != exceptRoad ){
        	int32 pi = r->getPolyLine()->getPointWithin(pt,radius);
        	if(pi>=0){
        		pointIndex = pi;
        		return r;
        	}
        }
    }
    pointIndex = -1;
    return 0;
}

Lane* RoadNetwork::getLane(int32 roadID, int8 laneID){
	if(roadID>=0 && roadID<roads.size()){
		Road* road = roads.at(roadID);
		if(laneID>=0 && laneID<road->lanes.size()){
			return road->lanes.at(laneID);
		}
	}
	return 0;
}

/*

void RoadNetwork::addTrafficLights(TrafficLightsInformation tlinfo){

	// remove previous controller
	for( int32 i=0 ; i < tlinfo.controllers.size() ; i++){
		TLController& controller_info = tlinfo.controllers[i];
		TrafficLightController* controller = new TrafficLightController(i);
		// Creating lights
		for(int l=0; l < controller_info.lights.size(); l++){
			TLLight& light = controller_info.lights[l];
			controller->lightColors[light.id] = NoLight;
			for(int k=0;k<light.lanes.size();k++){
				TLLane& tllane = light.lanes[k];
				Q_ASSERT_X( tllane.road_no>=0 && tllane.road_no<roads.size() ,"TrafficLights","*** light on an invalid Road");
				Road* road = roads[tllane.road_no];
				Q_ASSERT_X( tllane.lane_no>=0 && tllane.lane_no<road->getNumberOfLanes() ,"TrafficLights","*** light on an invalid Lane");
				Lane* lane = road->getLane( tllane.lane_no );
				lane->lightColor = &controller->lightColors[light.id];
			}
		}
		// Adding states
		for(int l=0;l<controller_info.states.size();l++){
			controller->states.append(controller_info.states[l]);
		}
		// Creating Sensors
		for(int l=0; l < controller_info.sensors.size(); l++){
			TLSensor& sensor = controller_info.sensors[l];
			//qDebug() << "Roadid: " << sensor.road_no;
			Q_ASSERT_X( sensor.road_no>=0 && sensor.road_no<roads.size() ,"TrafficLightSensors","*** light on an invalid Road");
			Road* road = roads[sensor.road_no];
			Q_ASSERT_X( sensor.lane_no>=0 && sensor.lane_no<roads.size() ,"TrafficLightSensors","*** light on an invalid Lane");
			//Lane& lane = road.lanes[sensor.lane_no];

			TrafficLightSensor::SensorType type = (TrafficLightSensor::SensorType)sensor.type;

			TrafficLightSensor *trafficLightSensor = new TrafficLightSensor(sensor.id, type, 2.0, road, sensor.lane_pos,true);

			if(sensor.type == TrafficLightSensor::S_Counter){
				controller->counters[sensor.id] = trafficLightSensor;
			}else{
				controller->sensors[sensor.id] = trafficLightSensor;
			}

			road->addTrigger( trafficLightSensor );

		}

		trafficLights.append( controller );
	}
}
*/

TrafficLightController* RoadNetwork::createTrafficLightController(TrafficLightController::Type type){
	TrafficLightController* controller;
	uint32 id = trafficLights.size();
	switch(type){
	case TrafficLightController::FixedTime:
		controller = new FixedTimeTrafficLightController(id,this);
		break;
	case TrafficLightController::PreTimed:
		controller = new PreTimedTrafficLightController(id,this);
		break;
	default:
		controller = 0;
		break;
	}

	if(controller){
		trafficLights.append( controller );
	}
	return controller;
}

const TrafficLightController* RoadNetwork::getTrafficLightController(int32 id) const {
	if(id>=0 && id < trafficLights.size()){
		return trafficLights.at(id);
	}
	return 0;
}

TrafficLightController* RoadNetwork::getTrafficLightController(int32 id){
	if(id>=0 && id < trafficLights.size()){
		return trafficLights.at(id);
	}
	return 0;
}

void RoadNetwork::removeTrafficLightController(int32 id){
	if(id>=0 && id < trafficLights.size()){
		TrafficLightController* controller = trafficLights.at(id);
		controller->uninstallLights();
		delete controller;
		trafficLights.remove(id);
		int32 nl = trafficLights.size();
		for(int32 i=0;i<nl;i++){
			trafficLights[i]->id = i;
		}
	}
}

void RoadNetwork::removeTrafficLightController(TrafficLightController* controller){
	if(controller){
		removeTrafficLightController( controller->getID() );
	}
}

void RoadNetwork::clearTrafficLights(){
	for(int32 i=0;i<trafficLights.size();i++){
		trafficLights[i]->uninstallLights();
		delete trafficLights[i];
	}
	trafficLights.clear();
}


Junction* RoadNetwork::newJunction(const Vec& v){
	Junction* junction = getJunction(v);
	if(junction == 0){
		junction = new Junction(this, junctions.size() , v);
		junctions.append( junction );
		junctionsHash.insert( v, junction );
	}
	return junction;
}

bool RoadNetwork::join(Junction* junctionA, Junction* junctionB){
	if( !isValidJunction(junctionA) || !isValidJunction(junctionB) ){
		return false;
	}
	if( !junctionA->getBBox().intersects( junctionB->getBBox() ) ){
		return false;
	}
	while( junctionA->getNumberOfRoads() > 0 ){
		Road* road = junctionA->getRoad(0);
		junctionA->removeRoadByIndex(0);
		junctionB->addRoad(road);
	}
	deleteJunction( junctionA->getID() );
	return true;
}

bool RoadNetwork::join(Road* road, Junction* junction){
	if( !isValidRoad(road) || !isValidJunction(junction) ){
		return false;
	}
	if( road->getFirstPoint() == junction->getPosition() && road->startJunction == 0){
		junction->addRoad(road);
		return true;
	}else{
		if( road->getLastPoint() == junction->getPosition() && road->endJunction == 0){
			junction->addRoad(road);
			return true;
		}
	}
	return false;
}


bool RoadNetwork::join(Road* roadA, Road* roadB){
	if( !isValidRoad(roadA) || !isValidRoad(roadB) ){
		return false;
	}
	if( roadA->getLastPoint() == roadB->getLastPoint() ){
		if( roadA->endJunction && roadB->endJunction ){
			if(roadA->endJunction == roadB->endJunction){
				return false;
			}
			join( roadA->endJunction, roadB->endJunction );
		}else{
			if( roadA->endJunction && !roadB->endJunction ){
				roadA->endJunction->addRoad( roadB );
			}else{
				if( !roadA->endJunction && roadB->endJunction){
					roadB->endJunction->addRoad( roadA );
				}else{
					Junction* junction = newJunction( roadA->getLastPoint() );
					junction->addRoad(roadA);
					junction->addRoad(roadB);
				}
			}
		}
		return true;
	}else{
		if( roadA->getLastPoint() == roadB->getFirstPoint() ){
			if( roadA->endJunction && roadB->startJunction ){
				if(roadA->endJunction == roadB->startJunction){
					return false;
				}
				join( roadA->endJunction, roadB->startJunction );
			}else{
				if( roadA->endJunction && !roadB->startJunction ){
					roadA->endJunction->addRoad( roadB );
				}else{
					if( !roadA->endJunction && roadB->startJunction){
						roadB->startJunction->addRoad( roadA );
					}else{
						Junction* junction = newJunction( roadA->getLastPoint() );
						junction->addRoad(roadA);
						junction->addRoad(roadB);
					}
				}
			}
			return true;

		}else{
			if( roadA->getFirstPoint() == roadB->getLastPoint() ){
				if( roadA->startJunction && roadB->endJunction ){
					if(roadA->startJunction == roadB->endJunction){
						return false;
					}
					join( roadA->startJunction, roadB->endJunction );
				}else{
					if( roadA->startJunction && !roadB->endJunction ){
						roadA->startJunction->addRoad( roadB );
					}else{
						if( !roadA->startJunction && roadB->endJunction){
							roadB->endJunction->addRoad( roadA );
						}else{
							Junction* junction = newJunction( roadA->getFirstPoint() );
							junction->addRoad(roadA);
							junction->addRoad(roadB);
						}
					}
				}
				return true;
			}else{
				if( roadA->getFirstPoint() == roadB->getFirstPoint() ){
					if( roadA->startJunction && roadB->startJunction ){
						if(roadA->startJunction == roadB->startJunction){
							return false;
						}
						join( roadA->startJunction, roadB->startJunction );
					}else{
						if( roadA->startJunction && !roadB->startJunction ){
							roadA->startJunction->addRoad( roadB );
						}else{
							if( !roadA->startJunction && roadB->startJunction){
								roadB->startJunction->addRoad( roadA );
							}else{
								Junction* junction = newJunction( roadA->getFirstPoint() );
								junction->addRoad(roadA);
								junction->addRoad(roadB);
							}
						}
					}
					return true;
				}
			}
		}
	}
	return false;
}
/*
 *
 * 0--1---2----->3 [] 0-----1---------->2
 *
 * 3<--2---1------0 [] 0-----1---------->2
 *
 */


Road* RoadNetwork::mergeRoads(Junction* junction){
	if( isValidJunction(junction) && junction->getNumberOfRoads() == 2){
		Road* ra = junction->getRoad(0);
		Road* rb = junction->getRoad(1);

		Junction* junctionB = (rb->startJunction==junction?rb->getEndJunction():rb->getStartJunction());

		if( ra->endJunction == junction ){
			junction->removeRoad(ra);
			if( rb->startJunction == junction ){
				for(int32 i=1;i<rb->getNumberOfPoints();i++){
					ra->insertPoint(ra->getNumberOfPoints(), rb->getPoint(i));
				}
			}else{
				for(int32 i=rb->getNumberOfPoints()-2;i>=0;i--){
					ra->insertPoint(ra->getNumberOfPoints(), rb->getPoint(i));
				}
			}
		}else{
			junction->removeRoad(ra);
			if( rb->startJunction == junction ){
				for(int32 i=1;i<rb->getNumberOfPoints();i++){
					ra->insertPoint(0,rb->getPoint(i));
				}
			}else{
				for(int32 i=rb->getNumberOfPoints()-2;i>=0;i--){
					ra->insertPoint(0,rb->getPoint(i));
				}
			}
		}

		junction->removeRoad(rb);
		deleteJunction( junction->getID() );

		if(junctionB){
			junctionB->removeRoad(rb);
		}
		deleteRoad( rb );

		if(junctionB){
			junctionB->addRoad(ra);
		}

		return ra;
	}
	return 0;
}

Junction* RoadNetwork::splitRoad(Road* road, int32 ni){
	if( !isValidRoad(road) || ni<=0 || ni>=road->getNumberOfPoints()-1) return 0;

	int32 np = road->getNumberOfPoints();
	Vec v = road->getPoint(ni);

	Vector<Vec> points1;
	for(int32 i=0;i<=ni;i++){
		points1.append( road->getPoint(i) );
	}

	Vector<Vec> points2;
	for(int32 i=ni;i<np;i++){
		points2.append( road->getPoint(i) );
	}

	Junction* endJunction = road->getEndJunction();
	if(endJunction){
		endJunction->removeRoad(road);
	}

	// Road A
	road->setPoints(points1);
	// Road B
	Road* newRoad = road->clone();
	newRoad->setPoints( points2 );
	newRoad->id = roads.size();
	roads.append(newRoad);
	newRoad->update();

	// Middle Junction
	Junction* middleJunction = newJunction(v);
	middleJunction->addRoad(road);
	middleJunction->addRoad(newRoad);
	if(endJunction){
		endJunction->addRoad( newRoad );
	}
	return middleJunction;
}

void RoadNetwork::disconnect(Road* road, Junction* junction){
	if( isValidRoad(road) && isValidJunction(junction) ){
		if( road->startJunction == junction ){
			junction->removeRoad(road);
			Vec p = road->getFirstPoint()-(road->getFirstPointDirection()*2.0);
			road->setPoint(0,p);
		}else{
			if( road->endJunction == junction ){
				junction->removeRoad(road);
				Vec p = road->getLastPoint()-(road->getLastPointDirection()*2.0);
				road->setPoint( road->getNumberOfPoints()-1, p);
			}
		}
	}
	if( junction->getNumberOfRoads() < 2){
		deleteJunction( junction->getID() );
	}
}

void RoadNetwork::disconnectAll(Junction* junction){
	if( isValidJunction( junction ) ){
		while( junction->getNumberOfRoads() > 0 ){
			Road* road = junction->getRoad(0);
			if( road->startJunction == junction ){
				junction->removeRoad(road);
				Vec p = road->getFirstPoint()-(road->getFirstPointDirection()*2.0);
				road->setPoint(0,p);
			}else{
				if( road->endJunction == junction ){
					junction->removeRoad(road);
					Vec p = road->getLastPoint()-(road->getLastPointDirection()*2.0);
					road->setPoint( road->getNumberOfPoints()-1, p);
				}
			}
		}
		deleteJunction( junction->getID() );
	}
}

void RoadNetwork::deleteJunction(int32 ni){
	if(ni<0 || ni>=junctions.size()) return;
	delete junctions[ni];
	junctions.remove(ni);
	int32 nr = junctions.size();
	for(int32 i=ni;i<nr;i++){
		junctions[i]->id = i;
	}
}



void RoadNetwork::moveToFront(Road* road){
	if(isValidRoad(road)){
		int32 ni = road->getID();
		int32 nr = roads.size()-1;
		for(int32 i=ni;i<nr;i++){
			roads[i] = roads[i+1];
			roads[i]->id = i;
		}
		roads[nr] = road;
		road->id = nr;
	}
}

bool RoadNetwork::isValidRoad(const Road* road){
	if(road){
		if( road->id>=0 && road->id < roads.size() ){
			return roads[ road->id ] == road;
		}
	}
	return false;
};

bool RoadNetwork::isValidJunction(const Junction* junction){
	if(junction){
		if(junction->id >= 0 && junction->id < junctions.size() ){
			return junctions[ junction->id ] == junction;
		}
	}
	return false;
};

bool RoadNetwork::isValidLane(const Lane* lane){
	if(lane){
		const Road* road = lane->getRoad();
		if( isValidRoad(road) && lane->id>=0 && lane->id<road->lanes.size()){
			return road->lanes[ lane->id ] == lane;
		}
	}
	return false;
}

void RoadNetwork::deleteRoad(Road* road){
	if( isValidRoad(road) ){
		Vector<int32> deletes;
		deletes.append(road->getID());
		deleteRoads( deletes );
	}
}

void RoadNetwork::deleteRoad(int32 ni){
	if(ni>=0 && ni<roads.size()){
		Vector<int32> deletes;
		deletes.append( ni );
		deleteRoads( deletes );
	}
}

void RoadNetwork::deleteRoads(Vector<int32>& roadIDs){

	std::sort(roadIDs.begin(),roadIDs.end());


	/*for(int32 i=nids-1;i>=0;i--){
		int32 ni = roadIDs[i];
		if(ni>=0 && ni<roads.size()){
			Road* road = roads[ni];
			Junction* j1 = road->getStartJunction();
			if(j1){
				j1->removeRoad( road );
			}
			Junction* j2 = road->getEndJunction();
			if(j2){
				j2->removeRoad( road );
			}
		}
	}*/

	// Remove Roads
	int32 nids = roadIDs.size();
	for(int32 i=nids-1;i>=0;i--){
		int32 ni = roadIDs[i];
		if( ni >= 0 && ni < roads.size() ){
			delete roads[ni];
			roads.remove(ni);
		}
	}
	int32 nr = roads.size();
	for(int32 i=0;i<nr;i++){
		roads[i]->id = i;
	}

}


Junction* RoadNetwork::getJunction(const Vec& p){
	if(junctionsHash.contains(p)){
		return junctionsHash.value(p);
	}
	return 0;
}

bool RoadNetwork::hasJunction(const Vec& p){
	return junctionsHash.contains(p);
}

Junction* RoadNetwork::getJunction(const BBox& bb){
	int32 nj = junctions.size();
	for(int32 i=0;i<nj;i++){
		Junction* junction = junctions[i];
		if( junction->bb.intersects(bb) ){
			return junction;
		}
	}
	return 0;
}

void RoadNetwork::computeLaneConnections(Junction* junction, Road* roadA, Road* roadB){
    vns::Vector<Lane*> currs;
    int32 nl = roadA->getNumberOfLanes();
	for(int32 i=0;i<nl;i++){
		Lane* ln = roadA->getLane(i);
		if( !ln->hasFlag(Lane::NoTrafficLane) && ln->getEndJunction() == junction && ln->hasConnectionTo(roadB) ){
		    if( roadA->getEndJunction() == junction) currs.append(ln);
		    else currs.insert(0,ln);
		}
	}
    vns::Vector<Lane*> nexts;
    int32 nnl = roadB->getNumberOfLanes();
    for(int32 i=0;i<nnl;i++){
		Lane* ln = roadB->getLane(i);
		if( !ln->hasFlag(Lane::NoTrafficLane) && ln->getStartJunction() == junction ){
		    if( roadB->getStartJunction() == junction) nexts.append(ln);
		    else nexts.insert(0,ln);
		}
	}

    nl = currs.size();
    nnl = nexts.size();

    if( nl == 0 || nnl == 0 ){
    	return;
    }

    Road* leftRoad = junction->getLeftRoad(roadA);
    //bool leftentry = false;
    while( leftRoad != roadB ){
    	if( junction->isEntryRoad(leftRoad) ){
    		//leftentry = true;
    		break;
    	}
    	leftRoad = junction->getLeftEntryRoad(leftRoad);
    }
	Road* rightRoad = junction->getRightRoad(roadA);
    bool rightentry = false;
    while( rightRoad != roadB ){
    	if( junction->isEntryRoad(rightRoad) ){
    		rightentry = true;
    		break;
    	}
    	rightRoad = junction->getRightEntryRoad(rightRoad);
    }
    // clear
	for(int32 i=0;i<nl;i++){
		Lane* ln = currs.at(i);
		ln->nextLanes->remove(roadB);
	}

    if( junction->getRightRoad(roadA) == roadB && junction->getLeftRoad(roadA) == roadB){
    	double d0 = abs(currs.at(0)->getLastPoint().distanceTo(nexts.at(0)->getFirstPoint()));
    	double d1 = abs(currs.at(nl-1)->getLastPoint().distanceTo(nexts.at(nnl-1)->getFirstPoint()));
    	if( d0<=d1 ){
    		for(int32 i=0;i<nl;i++){
    			int32 ni = vns_MIN(i,nnl-1);
    			Lane* ln = currs.at(i);
    			Lane* nln = nexts.at(ni);
    			ln->nextLanes->insert(roadB,nln);
    		}
    	}else{
    		for(int32 i=0;i<nl;i++){
    			int32 ni = vns_MIN(i,nnl-1);
    			Lane* ln = currs.at(nl-i-1);
    			Lane* nln = nexts.at(nnl-ni-1);
    			ln->nextLanes->insert(roadB,nln);
    		}
    	}
		return;
    }

	if( rightentry ){
		for(int32 i=0;i<nl;i++){
			int32 ni = vns_MIN(i,nnl-1);
			Lane* ln = currs.at(i);
			Lane* nln = nexts.at(ni);
			ln->nextLanes->insert(roadB,nln);
		}
	}else{
		for(int32 i=0;i<nl;i++){
			int32 ni = vns_MIN(i,nnl-1);
			Lane* ln = currs.at(nl-i-1);
			Lane* nln = nexts.at(nnl-ni-1);
			ln->nextLanes->insert(roadB,nln);
		}
	}
}

DataStream& operator <<(DataStream&f, const RoadNetwork* network) {
    f << network->extents;
    f << network->offset;
	/*
	 * Junctions
	 */
	f << int32(network->junctions.size());
    for(int32 i=0; i < network->junctions.size(); ++i){
    	f << network->junctions[i];
    }
    /*
     * Roads
     */
	f << int32(network->roads.size());
    for(int32 i=0; i < network->roads.size(); ++i){
    	f << network->roads[i];
    }
    /*
     * Lane Connections
     */
    for(int32 i=0; i < network->roads.size(); ++i){
    	Road* road = network->roads[i];
        for(int32 l=0; l < road->lanes.size(); ++l){
        	Lane* lane = road->lanes[l];
        	f << int32(lane->connections.size());
        	for(int32 i=0;i<lane->connections.size();i++){
        		const Road* croad = lane->connections.at(i);
        		f << int32(croad->getID());
        	}
        }
    }

    /*
     * Traffic Lights
     */

	f << int32(network->trafficLights.size());
    for(int32 i=0; i < network->trafficLights.size(); ++i){
    	TrafficLightController::write( f, network->trafficLights.at(i) );
    }
    return f;
}

DataStream& operator>>(DataStream& f, RoadNetwork* network) {
	network->clear();

    f >> network->extents;
    f >> network->offset;

    /*
     * READ JUNCTIONS
     */
    int32 n;
    f >> n;
    network->junctions.resize(n);
    for (int32 i=0; i<n; ++i) {
        network->junctions[i] = new Junction( network );
        f>>network->junctions[i];
    }
    /*
     * READ ROADS
     */
    f>>n;
    network->roads.resize(n);
    for (int32 i=0; i<n; ++i){
    	network->roads[i] = new Road( network );
    	f>>network->roads[i];
    }
    /*
     * Lane Connections
     */
    for(int32 i=0; i < network->roads.size(); ++i){
    	Road* road = network->roads[i];
        for(int32 l=0; l < road->lanes.size(); ++l){
        	Lane* lane = road->lanes[l];
            int32 n,roadID;
            f >> n;
        	for(int32 i=0;i<n;i++){
        		f >> roadID;
        		Road* croad = network->roads[roadID];
        		lane->connections.append(croad);
        	}
        }
    }

    /*
     * Traffic Lights
     */

    f>>n;
    network->trafficLights.resize(n);
    for (int32 i=0; i<n; ++i){
    	network->trafficLights[i] = TrafficLightController::read(f,network);
    }
    /*
     * READ EXTENTS
     */


    int32 nj = network->junctions.size();
    for(int32 i=0;i<nj;i++){
    	Junction* junction = network->junctions[i];
    	junction->updateJunction();
    	junction->updateNextLanes();
    }

  /*
    network->extents.clear();
    for(int32 i=0; i < network->roads.size(); ++i){
    	Road* road = network->roads[i];
    	//road->updateConnectionsTODO();
    	network->extents.addBBox( road->getBBox() );
    }*/

    return f;
}


void RoadNetwork::balance(){
	if(roads.size()==0) return;
	vns::List<vns::Road*> queue;
	int32 rid=0;
	vns::Vector<vns::Road*> newRoads(roads.size());
	for(int i=0;i<roads.size();i++){
		if(roads[i]==0) continue;
		queue.append(roads[i]);
		roads[i] = 0;
		while(!queue.isEmpty()){
			vns::Road* road = queue.takeFirst();
			newRoads[rid++] = road;
			vns::Junction* end = road->getEndJunction();
			if(end){
				for(int r=0;r<end->getNumberOfRoads();r++){
					vns::Road* rd = end->getRoad(r);
					if(roads[rd->getID()]!=0){
						queue.append(rd);
						roads[rd->getID()] = 0;
					}
				}
			}
			vns::Junction* start = road->getStartJunction();
			if(start){
				for(int r=0;r<start->getNumberOfRoads();r++){
					vns::Road* rd = start->getRoad(r);
					if(roads[rd->getID()]!=0){
						queue.append(rd);
						roads[rd->getID()] = 0;
					}
				}
			}
		}
	}
	for(int i=0;i<newRoads.size();i++){
		roads[i] = newRoads[i];
		roads[i]->id = i;
	}
}


}
