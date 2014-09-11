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

#include <cstring>
#include "lane.h"
#include "road.h"
#include "roadnetwork.h"
#include "vehicle.h"
#include "simulator.h"
#include "calendarscheduler.h"

namespace vns {

const char* Road::fromType(vns::Road::Type type){
	switch(type){
	case Undefined: return "undefined";
	case Motorway: return "motorway";
	case MotorwayLink: return "motorwaylink";
	case Street: return "street";
	case Trunk: return "trunk";
	case TrunkLink: return "trunklink";
	case Primary: return "primary";
	case Residential: return "residential";
	case BusGuideway: return "busguideway";
	case Service: return "service";
	case Parking: return "parking";
	case Fuel: return "fuel";
	case Toll: return "toll";
	case Footway: return "footway";
	case Cicleway: return "cicleway";
	case Steps: return "steps";
	case Railway: return "railway";
	case Monorail: return "monorail";
	}
	return "undefined";
}

vns::Road::Type Road::fromTypeName(const char* name){
	if(strcmp(name,"undefined")==0) return Undefined;
	if(strcmp(name,"motorway")==0) return Motorway;
	if(strcmp(name,"motorwaylink")==0) return MotorwayLink;
	if(strcmp(name,"street")==0) return Street;
	if(strcmp(name,"trunk")==0) return Trunk;
	if(strcmp(name,"trunklink")==0) return TrunkLink;
	if(strcmp(name,"primary")==0) return Primary;
	if(strcmp(name,"residential")==0) return Residential;
	if(strcmp(name,"service")==0) return Service;
	if(strcmp(name,"parking")==0) return Parking;
	if(strcmp(name,"fuel")==0) return Fuel;
	if(strcmp(name,"toll")==0) return Toll;
	if(strcmp(name,"footway")==0) return Footway;
	if(strcmp(name,"cicleway")==0) return Cicleway;
	if(strcmp(name,"steps")==0) return Steps;
	if(strcmp(name,"railway")==0) return Railway;
	if(strcmp(name,"monorail")==0) return Monorail;
	if(strcmp(name,"busguideway")==0) return BusGuideway;
	return Undefined;
}

Road::Road(RoadNetwork* net) {
	userdata = 0;
	roadNetwork = net;
	type = Undefined;
    id = -1;
    startJunction = 0;
    endJunction = 0;
    width = 0.0;
    offset = 0.0;
    roadStart = 0.0;
    roadEnd = 0.0;
    leftParkingLane = 0;
    rightParkingLane = 0;
    startCrossWalk = false;
    endCrossWalk = false;
    polyline = 0;
    maximumForwardQueue = 0;
    maximumBackwardQueue = 0;

    scheduler = new CalendarScheduler(0.1,10);
}

Road::Road(RoadNetwork* net, vns::Road::Type ty, int32 idd, PolyLine* p)  {
	VNS_ASSERT( p->size() > 1 ,"Road Constructor","PolyLine must have more than 1 point");
	userdata = 0;
	type = ty;
    id = idd;
    roadNetwork = net;
    polyline = p;

    startJunction = 0;
    endJunction = 0;
    width = 0.0;
    offset = 0.0;
    roadStart = 0.0;
    roadEnd = polyline->getLength();
    leftParkingLane = 0;
    rightParkingLane = 0;
    startCrossWalk = false;
    endCrossWalk = false;
    maximumForwardQueue = 0;
    maximumBackwardQueue = 0;

    scheduler = new CalendarScheduler(0.1,10);
}


Road* Road::clone() const {
	Road* road = new Road(roadNetwork);
	road->type = type;
    road->id = -1;
    road->startJunction = 0;
    road->endJunction = 0;
    road->width = width;
    road->offset = offset;
    road->roadStart = roadStart;
    road->roadEnd = roadEnd;
    road->maximumForwardQueue = maximumForwardQueue;
    road->maximumBackwardQueue = maximumBackwardQueue;
    if(leftParkingLane != 0){
    	road->leftParkingLane = leftParkingLane->clone();
    	road->leftParkingLane->road = road;
    	road->leftParkingLane->id = -1;
    }else{
		road->leftParkingLane = 0;
	}
	if(rightParkingLane != 0){
		road->rightParkingLane = rightParkingLane->clone();
		road->rightParkingLane->road = road;
		road->rightParkingLane->id = -2;
	}else{
		road->rightParkingLane = 0;
	}
    road->startCrossWalk = startCrossWalk;
    road->endCrossWalk = endCrossWalk;
    road->bb = bb;
    road->polyline = polyline->clone();

    int32 n = lanes.size();
    road->lanes.resize( n );
    for(int32 i=0; i<n; ++i){
    	road->lanes[i] = lanes[i]->clone();
    	road->lanes[i]->road = road;
    	road->lanes[i]->id = i;
    }
    road->update();
    return road;
}

Road::~Road(){
	Junction* junction = startJunction;
	if( junction ){
		junction->removeRoad(this);
		if (junction->getNumberOfRoads() == 0) {
			roadNetwork->deleteJunction(junction->getID());
		}
	}
	junction = endJunction;
	if( junction ){
		junction->removeRoad(this);
		if (junction->getNumberOfRoads() == 0) {
			roadNetwork->deleteJunction(junction->getID());
		}
	}

	int32 nl = lanes.size();
	for(int32 i=0;i<nl;i++){
		delete lanes[i];
	}
	lanes.clear();
	if(leftParkingLane){ delete leftParkingLane; leftParkingLane = 0;}
	if(rightParkingLane){ delete rightParkingLane; rightParkingLane = 0;}
	if(polyline){ delete polyline; }
	roadNetwork = 0;
	delete scheduler;
}

void Road::update(){
    int32 nl = lanes.size();
    width = 0;
    for(int32 i=0; i<nl; ++i) {
        Lane* lane = lanes[i];
        lane->id = i;
        lane->road = this;
        width += lane->getWidth();
    }

    float off = width*0.5+offset;
    if(leftParkingLane){
    	leftParkingLane->setRoadCenterOffset(off+leftParkingLane->width*0.5);
    	leftParkingLane->way = lanes[0]->way;
    }
    for (int32 i=0; i<nl; ++i) {
    	Lane* lane = lanes[i];
    	lane->setRoadCenterOffset( off - lane->width*0.5 );
        off -= lane->width;
    }
    if(rightParkingLane){
    	rightParkingLane->setRoadCenterOffset(off-rightParkingLane->width*0.5);
    	rightParkingLane->way = lanes[nl-1]->way;
    }

    /*
     *
     * Calculate Road BBOX
     */

    bb.clear();
    int32 np = polyline->size();
    for(int32 i=0; i<np ; i++){
    	const Vec p = polyline->getPoint(i);
    	//bb.addPoint( p );
    	const Vec n = polyline->getNormal(i);
    	double wd = width*0.5;
        bb.addPoint( p+(wd*n) );
        bb.addPoint( p-(wd*n) );
    }

    if(endJunction){
    	endJunction->updateJunction();
    }else{
    	setRoadEnd(polyline->getLength());
    }
    if(startJunction){
    	startJunction->updateJunction();
    }else{
    	setRoadStart(0.0);
    }
}

float Road::getWidth(vns::Way way) const {
	float w = 0;
    int32 nl = lanes.size();
    if(way == vns::Forward){
        for (int32 i=nl-1; i>=0; i--) {
        	Lane* l = lanes[i];
            if( l->way != way ) break;
            w += l->getWidth();
        }
    }else{
        for (int32 i=0; i<nl; i++) {
        	Lane* l = lanes[i];
            if( l->way != way ) break;
            w += l->getWidth();
        }
    }
    return w;
};


int8 Road::getNumberOfForwardLanes() const {
    int8 c = 0;
    int32 nl = lanes.size();
    for (int32 i=0; i<nl; ++i) {
    	Lane* l = lanes[i];
        if( l->getWay() == vns::Forward ){
            c++;
        }
    }
    return c;
}

int8 Road::getNumberOfBackwardLanes() const {
    int8 c = 0;
    int32 nl = lanes.size();
    for (int32 i=0; i<nl; ++i) {
    	Lane* l = lanes[i];
        if( l->getWay() == vns::Backward ){
            c++;
        }
    }
    return c;
}


BitSet Road::getLanesInWay(Way way) const {
    BitSet res;
    int32 nl = lanes.size();
    for (int32 i=0; i<nl; ++i) {
    	Lane* l = lanes[i];
        if( l->getWay() == way ){
            res.add(i);
        }
    }
    return res;
}

BitSet Road::getLanesConnectingTo( const Road* destRoad) const {
    BitSet res;
	int32 nl = lanes.size();
	for (int32 i=0; i<nl; ++i) {
		Lane* lane = lanes[i];
		if( lane->hasConnectionTo(destRoad) ){
			res.add(i);
		}
	}
    return res;
}

Lane* Road::createLane(Way way, float w){
	int32 pos = lanes.size();
	if(way == Backward){
		pos = 0;
	}
	Lane* lane = new Lane(this,pos,way,Lane::ChangeLaneToRight|Lane::ChangeLaneToLeft|Lane::Cars|Lane::Buses|Lane::Bikes|Lane::Trucks,w);
	lanes.insert(pos,lane);
	update();
	updateConnections();
	return lane;
}

void Road::removeLane(int8 id){
	int32 n = lanes.size();
	if( n>1 && id>=0 && id<n ){
		Lane* lane = lanes[id];
		lanes.remove(id);
		delete lane;
		update();
		updateConnections();
	}
}

bool Road::hasParking(vns::Way way){
	if( leftParkingLane && leftParkingLane->getWay()==way) return true;
	if( rightParkingLane && rightParkingLane->getWay()==way) return true;
	return false;
}

void Road::setLeftParking(bool parking){
	if(parking){
		if(leftParkingLane == 0){
			if( getLength() < VSN_MIN_ROAD_LENGTH_FOR_PARKING){
				return;
			}
			leftParkingLane = new Lane(this,-1,lanes[0]->way,Lane::ParkingLane,2.5);
		}
	}else{
		if(leftParkingLane){
			delete leftParkingLane;
			leftParkingLane = 0;
		}
	}
	update();
}

void Road::setRightParking(bool parking){
	if(parking){
		if(rightParkingLane == 0){
			if( getLength() < VSN_MIN_ROAD_LENGTH_FOR_PARKING){
				return;
			}
			int32 nl = lanes.size();
			rightParkingLane = new Lane(this,-2,lanes[nl-1]->way,Lane::ParkingLane,2.5);
		}
	}else{
		if(rightParkingLane){
			delete rightParkingLane;
			rightParkingLane = 0;
		}
	}
	update();
}

void Road::setEndCrossWalk(bool crossWalk){
	if(crossWalk == endCrossWalk) return;
	endCrossWalk = crossWalk;
	// UPDATE ROAD END
	setRoadEnd(roadEnd);
}

void Road::setStartCrossWalk(bool crossWalk){
	if(crossWalk == startCrossWalk) return;
	startCrossWalk = crossWalk;
	// UPDATE ROAD START
	setRoadStart(roadStart);
}

void Road::setRoadEnd(float end){
	if( end < roadStart || end > polyline->getLength() ){
		roadEnd = polyline->getLength();
	}else{
		roadEnd = end;
	}
	int32 nl = lanes.size();
	for(int32 i=0; i<nl; ++i){
		Lane* lane = lanes[i];
		if(lane->getWay() == Forward){
			lane->setLaneEnd( end );
		}else{
			lane->setLaneStart( polyline->getLength() - end );
		}
	}
	if(leftParkingLane){
		if(leftParkingLane->getWay() == Forward){
			leftParkingLane->setLaneEnd( end );
		}else{
			leftParkingLane->setLaneStart( polyline->getLength() - end );
		}
	}
	if(rightParkingLane){
		if(rightParkingLane->getWay() == Forward){
			rightParkingLane->setLaneEnd( end );
		}else{
			rightParkingLane->setLaneStart( polyline->getLength() - end );
		}
	}
}

void Road::setRoadStart(float start){
	if( start > roadEnd || start < 0 ){
		roadStart = 0;
	}else{
		roadStart = start;
	}
	int32 nl = lanes.size();
	for (int32 i=0; i<nl; ++i) {
		Lane* lane = lanes[i];
		if(lane->getWay() == Forward){
			lane->setLaneStart( start );
		}else{
			lane->setLaneEnd( polyline->getLength() - start );
		}
	}
	if(leftParkingLane){
		if(leftParkingLane->getWay() == Forward){
			leftParkingLane->setLaneStart( start );
		}else{
			leftParkingLane->setLaneEnd( polyline->getLength() - start );
		}
	}
	if(rightParkingLane){
		if(rightParkingLane->getWay() == Forward){
			rightParkingLane->setLaneStart( start );
		}else{
			rightParkingLane->setLaneEnd( polyline->getLength() - start );
		}
	}
}

Vec Road::getLastPointDirection(Way way) const {
	if(way == Forward){
		return polyline->getLastPointDirection();
	}
	return -polyline->getFirstPointDirection();
}

Vec Road::getFirstPointDirection(Way way) const {
	if(way == Forward){
		return polyline->getFirstPointDirection();
	}
	return -polyline->getLastPointDirection();
}

Vec Road::getFirstPointDirection(const Junction* junction) const {
	if( startJunction == junction ){
		return polyline->getFirstPointDirection();
	}
	return -polyline->getLastPointDirection();
}

Vec Road::getLastPointDirection(const Junction* junction) const {
	if( endJunction == junction ){
		return polyline->getLastPointDirection();
	}
	return -polyline->getFirstPointDirection();
}

const Vec& Road::getFirstPoint(Way way) const {
	if(way == Forward){
		return polyline->getPoint(0);
	}
	return polyline->getPoint( polyline->size()-1 );
}

const Vec& Road::getLastPoint(Way way) const {
	if(way == Forward){
		return polyline->getPoint( polyline->size()-1 );
	}
	return polyline->getPoint( 0 );
}

bool Road::contains(const vns::Vec& pt) const {
	if(bb.contains(pt)){
		return polyline->distanceToPointIsLessThan(pt,width*0.5);
	}
	return false;
}


void Road::getStartPoint(Vec& pt, Vec& dir, Vec& perp, Way way) const {
	if( way == Forward ){
		polyline->pointDirFromFirstPoint(pt,dir,perp,roadStart,offset);
	}else{
		polyline->pointDirFromFirstPoint(pt,dir,perp,roadEnd,offset);
		dir = -dir;
		perp = -perp;
	}
}

void Road::getEndPoint(Vec& pt, Vec& dir, Vec& perp, Way way) const {
	if( way == Forward ){
		polyline->pointDirFromFirstPoint(pt,dir,perp,roadEnd,offset);
	}else{
		polyline->pointDirFromFirstPoint(pt,dir,perp,roadStart,offset);
		dir = -dir;
		perp = -perp;
	}
}

void Road::getPositionPoint(Vec& pt, Vec& dir, Vec& perp, double pos, Way way) const {
	if( way == Forward ){
		polyline->pointDirFromFirstPoint(pt,dir,perp,pos,offset);
	}else{
		polyline->pointDirFromFirstPoint(pt,dir,perp,polyline->getLength()-pos,offset);
		dir = -dir;
		perp = -perp;
	}
}


// ALTERAR JUNCTIONS
void Road::setPoint(int32 i, const Vec& p){
	if(i>0 && i<polyline->size()-1){
		polyline->updatePoint(i,p.getX(),p.getY(),p.getZ());
		update();
		return;
	}
	if(i==0){
		if(startJunction){
			startJunction->updateCenter(p.getX(),p.getY(),p.getZ());
		}else{
			polyline->updatePoint(i,p.getX(),p.getY(),p.getZ());
			update();
		}
		return;
	}
	if(i==polyline->size()-1){
		if(endJunction){
			endJunction->updateCenter(p.getX(),p.getY(),p.getZ());
		}else{
			polyline->updatePoint(i,p.getX(),p.getY(),p.getZ());
			update();
		}
	}
}

// ALTERAR JUNCTIONS
bool Road::insertPoint(int32 i, Vec point){
	if((i<=0 && startJunction) || (i>=getNumberOfPoints() && endJunction) ) return false;
	polyline->insertPoint(i,point);
	update();
	return true;
}

// ALTERAR JUNCTIONS
bool Road::removePoint(int32 i){
	if(i<0 || i>getNumberOfPoints()-1 || getNumberOfPoints()<=2) return false;
	if( (i==0 && startJunction) || (i==getNumberOfPoints()-1 && endJunction) ) return false;
	polyline->removePoint(i);
	update();
	return true;
}

void Road::setPoints(const Vector<Vec>& points){
	polyline->setPoints(points);
	update();
}



void Road::updateConnections(){
	if(endJunction){
		endJunction->updateConnections();
	}
	if(startJunction){
		startJunction->updateConnections();
	}
}

bool Road::hasConnectionTo(const Road* road) const {
	int32 nl = lanes.size();
	for(int32 i=0;i<nl;i++){
		if(lanes.at(i)->hasConnectionTo(road)){
			return true;
		}
	}
	return false;
}

bool Road::hasConnectionTo(vns::Way way,const Road* road) const {
	int32 nl = lanes.size();
	for(int32 i=0;i<nl;i++){
		if(lanes.at(i)->getWay()==way && lanes.at(i)->hasConnectionTo(road)){
			return true;
		}
	}
	return false;
}

bool Road::hasConnectionTo(const Lane* lane) const {
	int32 nl = lanes.size();
	for(int32 i=0;i<nl;i++){
		if(lanes.at(i)->hasConnectionTo(lane)){
			return true;
		}
	}
	return false;
}

void Road::clearConnections(Way way){
	int32 nl = lanes.size();
	for(int32 l=0;l<nl;l++){
		Lane* lane = lanes[l];
		if( lane->getWay() == way ){
			lane->clearConnections();
		}
	}
}

void Road::clearAllConnections(){
	int32 nl = lanes.size();
	for(int32 l=0;l<nl;l++){
		Lane* lane = lanes[l];
		lane->clearConnections();
	}
}

void Road::setMaximumSpeed(float speed){
	int32 nl = lanes.size();
	for(int32 l=0;l<nl;l++){
		Lane* lane = lanes[l];
		lane->setMaximumSpeed( speed );
	}
}
void Road::setMinimumSpeed(float speed){
	int32 nl = lanes.size();
	for(int32 l=0;l<nl;l++){
		Lane* lane = lanes[l];
		lane->setMinimumSpeed( speed );
	}
}

void Road::setMaximumCarsInQueue(vns::Way way, uint32 v){
	if(way==Forward){
		maximumForwardQueue = v;
	}else{
		maximumBackwardQueue = v;
	}
}

bool Road::hasRoomInQueue(Way way){
	if(way==Forward){
		if(forwardQueue.size()==0 || (uint32)forwardQueue.size()<maximumForwardQueue){
			return true;
		}
	}else{
		if(backwardQueue.size()==0 || (uint32)backwardQueue.size()<maximumBackwardQueue){
			return true;
		}
	}
	return false;
}

void Road::addVehicleToQueue(Way way, Vehicle* vehicle){
	if(way==Forward){
		forwardQueue.append(vehicle);
	}else{
		backwardQueue.append(vehicle);
	}
}

bool Road::isSourceRoad(Way way) const {
	if( !hasLanes(way) ) return false;
	if( way == Forward){
		if( startJunction == 0 ) return true;
	}else{
		if( endJunction == 0 ) return true;
	}
	return (rightParkingLane && rightParkingLane->way == way) || (leftParkingLane && leftParkingLane->way == way);
}

bool Road::isSinkRoad(Way way) const {
	if( !hasLanes(way) ) return false;
	if( way == Forward){
		if( endJunction == 0 ) return true;
	}else{
		if( startJunction == 0 ) return true;
	}
	return (rightParkingLane && rightParkingLane->way == way) || (leftParkingLane && leftParkingLane->way == way);
}

Lane* Road::chooseLaneForVehicle(Way way, Vehicle* vehicle, double& lanePos){
    /*
     * Test Parking Lanes
     */
	float length = vehicle->getLength();

	vns::Lane* right = getRightParking(way);
	if( right ){
		float pos = right->getFreeSpacePosition(length);
		if( pos>=0 ){
			lanePos = pos;
			return right;
		}
	}
	vns::Lane* left = getLeftParking(way);
	if( left ){
		float pos = left->getFreeSpacePosition(length);
		if( pos>=0 ){
			lanePos = pos;
			return left;
		}
	}
    //TODO: Why this if?!
	//if( getStartJunction(way)==0 ) 
    {
	    vns::Road* nroad = 0;
	    vns::Path::Iterator it = vehicle->getPathIterator();
	    if( it.hasNext() ){
	    	nroad = it.getNextRoad();
	    }
		vns::Lane* bestLane = 0;
		float dToNext = vns::MAX_FLOAT;
		for (int8 l = 0; l < lanes.size(); l++) {
			vns::Lane* ln = lanes.at(l);
			if( ln->way == way ){
				if( nroad==0 || ln->hasConnectionTo(nroad) ) {
					LaneObject* obj = ln->firstObject();
					if(obj){
						if(bestLane==0 || obj->getLanePosition() > dToNext){
							dToNext = obj->getLanePosition();
							bestLane = ln;
						}
					}else{
						dToNext = vns::MAX_FLOAT;
						bestLane = ln;
					}
				}
			}
		}
		if( bestLane ){
			const LaneObject* f = bestLane->getFirstObject();
			if (f) {
				vehicle->lanePos = bestLane->getLaneStart();
				vehicle->speed = f->getSpeed();
				if (vehicle->getFrontPosition() >= f->getRearPosition()) {
					return 0;
				}
				double acc = vehicle->getAccelTo(f);
				if (acc < f->getAccel()) {
					return 0;
				}
			}
			lanePos = bestLane->getLaneStart();
			return bestLane;
		}
	}
	return 0;
}

bool Road::canInvertWay(vns::Way way) const {
	vns::Way otherWay = (way==vns::Forward?vns::Backward:vns::Forward);
	if( hasLanes(way) && hasLanes(otherWay)){
		if(way==Forward){
			int i = lanes.size()-1;
			while(i>0){
				if(lanes[i-1]->getWay()==vns::Backward){
					if(lanes[i]->hasFlag(vns::Lane::NoTrafficLane) || lanes[i-1]->hasFlag(vns::Lane::NoTrafficLane)){
						return false;
					}
					if(lanes[i]->hasFlag(vns::Lane::ChangeLaneToLeft)) return true;
					return false;
				}
				i--;
			}
		}else{
			int i = 0;
			while(i<lanes.size()-1){
				if(lanes[i+1]->getWay()==vns::Forward){
					if(lanes[i+1]->hasFlag(vns::Lane::NoTrafficLane) || lanes[i]->hasFlag(vns::Lane::NoTrafficLane)){
						return false;
					}
					if(lanes[i]->hasFlag(vns::Lane::ChangeLaneToLeft)) return true;
					return false;
				}
				i++;
			}
		}
	}
	return false;
}


void Road::updateConnectionsTODO(){
	Junction* end = this->endJunction;
	if( end == 0 ) return;
	for(int i=0;i<end->getNumberOfRoads();i++){
		Road* nroad = end->getRoad(i);
		if(nroad == this) continue;
		int a = 0;
		int n = this->getNumberOfLanes();
		for(int l=0;l<n;l++){
			Lane* ln = this->getLane(l);
			if(ln->hasConnectionTo(nroad)) a++;
		}
		if(a<=nroad->getNumberOfLanes()) continue;
		int l = n-1;
		while(a>nroad->getNumberOfLanes()){
			Lane* ln = getLane(l);
			ln->removeConnectionTo(nroad);
			l--;a--;
		}
		/*
		vns::Vec pt = nroad->getFirstPoint();
		double d1 = this->getLane(0)->getLastPoint().distanceTo(pt);
		double d2 = this->getLane(n-1)->getLastPoint().distanceTo(pt);
		if(d1>d2){
			int l = n-1;
			while(a>nroad->getNumberOfLanes()){
				Lane* ln = getLane(l);
				ln->removeConnectionTo(nroad);
				l--;a--;
			}
		}else{
			int l = 0;
			while(a>nroad->getNumberOfLanes()){
				Lane* ln = getLane(l);
				ln->removeConnectionTo(nroad);
				l++;a--;
			}
		}*/
	}
}

void Road::simulationStart(vns::Simulator*){
	nextVehicleID = 0;
	forwardQueue.clear();
	backwardQueue.clear();
	scheduler->clearEvents();
}

uint32 Road::simulationStep(Simulator* sim){

	double t = sim->getSimulationTime();
	scheduler->invokeEvents(t,Simulator::DT);

	uint32 nVehicles = 0;
	int32 nl = lanes.size();
	for(int32 l=0;l<nl;l++){
		vns::Lane* lane = lanes.at(l);
		if( lane->getNumberOfObjects()>0 ){
			nVehicles += lane->simulationStep( sim );
		}
	}
	if(leftParkingLane){
		nVehicles += leftParkingLane->simulationStep(sim);
	}
	if(rightParkingLane){
		nVehicles += rightParkingLane->simulationStep(sim);
	}
	return nVehicles;
}

void Road::updateStep(vns::Simulator* sim){
	int8 nl = lanes.size();
	for(int8 l=0;l<nl;l++){
		lanes.at(l)->updateStep( sim );
	}
	if(leftParkingLane) leftParkingLane->updateStep(sim);
	if(rightParkingLane) rightParkingLane->updateStep(sim);

    while( !forwardQueue.isEmpty() ){
    	double lanePos;
    	Vehicle* v = forwardQueue.front();
    	Lane* lane = chooseLaneForVehicle(vns::Forward,v, lanePos);
    	if(lane){
    		sim->addVehicle(v, lane, lanePos);
    	    for(vns::List<Observer*>::Iterator obs=sim->observers.begin();obs!=sim->observers.end();obs++){
    		    (*obs)->onVehicleInjected(sim,v);
    	    }
    		forwardQueue.pop_front();
    	}else{
    		break;
    	}
    }

    while( !backwardQueue.isEmpty() ){
    	double lanePos;
    	Vehicle* v = backwardQueue.front();
    	Lane* lane = chooseLaneForVehicle(vns::Backward,v, lanePos);
    	if(lane){
    		sim->addVehicle(v, lane, lanePos);
    	    for(vns::List<Observer*>::Iterator obs=sim->observers.begin();obs!=sim->observers.end();obs++){
    		    (*obs)->onVehicleInjected(sim,v);
    	    }
    	    backwardQueue.pop_front();
    	}else{
    		break;
    	}
    }

}

void Road::clearQueuedVehicles(){
    while( !backwardQueue.isEmpty() ){
    	Vehicle* v = backwardQueue.takeFirst();
    	delete v;
    }
    while( !forwardQueue.isEmpty() ){
    	Vehicle* v = forwardQueue.takeFirst();
    	delete v;
    }
}


bool Road::allowsVehicle(const VehicleType& type,const vns::Way& way) const {
	for(int8 i=0;i<lanes.size();i++){
		vns::Lane* ln = lanes.at(i);
		if(ln->way==way && ln->allowsVehicle(type) ) return true;
	}
	return false;
}


DataStream& operator <<(DataStream&f, const Road* road) {
	f<<int8(road->type);
    f<<road->id;
    f<<road->width;
    f<<road->offset;
    f<<road->roadStart;
    f<<road->roadEnd;
    bool left = false;
    if( road->leftParkingLane != 0){
    	left = true;
    	f << left;
    	f << road->leftParkingLane;
    }else{
    	f << left;
    }
    bool right = false;
    if( road->rightParkingLane != 0){
    	right = true;
    	f << right;
    	f << road->rightParkingLane;
    }else{
    	f << right;
    }
    f<<road->startCrossWalk;
    f<<road->endCrossWalk;
    f<<road->bb;
    f<<road->polyline;
    /*
     * Lanes
     */
    f<<(int32)road->lanes.size();
    for(int32 i=0;i<road->lanes.size();i++){
    	f << road->lanes.at(i);
    }

    /*
     * Start and End Junctions
     */
    f<<(int32)(road->startJunction?road->startJunction->id:-1);
    f<<(int32)(road->endJunction?road->endJunction->id:-1);

    return f;
}

DataStream& operator >>(DataStream& f, Road* road) {
    bool parking;
    int8 roadType;
	f>>roadType;
	road->type = vns::Road::Type(roadType);
    f>>road->id;
    f>>road->width;
    f>>road->offset;
    f>>road->roadStart;
    f>>road->roadEnd;
    f>>parking;
    if( parking == true){
    	road->leftParkingLane = new Lane(road,-1);
    	f>>road->leftParkingLane;
    	road->leftParkingLane->id = -1;
    }else{
    	road->leftParkingLane = 0;
    }
    f>>parking;
    if( parking == true){
    	road->rightParkingLane = new Lane(road,-2);
    	f>>road->rightParkingLane;
    	road->rightParkingLane->id = -2;
    }else{
    	road->rightParkingLane = 0;
    }
    f>>road->startCrossWalk;
    f>>road->endCrossWalk;
    f>>road->bb;

    /* Read PolyLine*/
    road->polyline = new PolyLine();
    f>>road->polyline;
    VNS_ASSERT( road->polyline->size() > 1 ,"Road","PolyLine must have more than 1 point");

    /*
     * Lanes
     */
    int32 n;
    f>>n;
    road->lanes.resize(n);
    for (int32 i=0; i<n; ++i){
    	road->lanes[i] = new Lane(road,i);
    	f>>road->lanes[i];
    	road->lanes[i]->id = i;
    }

    /*
     * Start and End Junctions
     */

    int32 jid;
    f>>jid;
    if(jid>=0 && jid<road->roadNetwork->getNumberOfJunctions()){
    	road->startJunction = road->roadNetwork->getJunction(jid);
    	road->startJunction->roads.append(road);
    }else{
    	road->startJunction = 0;
    }
    f>>jid;
    if(jid>=0 && jid<road->roadNetwork->getNumberOfJunctions()){
    	road->endJunction = road->roadNetwork->getJunction(jid);
    	road->endJunction->roads.append(road);
    }else{
    	road->endJunction = 0;
    }

    return f;
}


}

