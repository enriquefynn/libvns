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

#include "lane.h"
#include "junction.h"
#include "roadnetwork.h"
#include "road.h"
#include "simulator.h"
#include "vehicle.h"

namespace vns {

Lane::Lane() {
    first = 0;
    last = 0;
    nObjects = 0;

	road = 0;
	id = -1;
    way = Forward;
    flags = 0;
    lightColor = NoLight;
    width = 3.0;
	laneStart = 0.0;
	laneEnd = 0.0;
    maximumSpeed = 50.0/3.6; // 50 Kmh;
    minimumSpeed = 0.0;
    laneSensors = 0;
    userdata = 0;
    nextLanes = new vns::HashTable<const Road*,Lane*>(8);

    exitVehicle = 0;
}

Lane::Lane(Road* rd, int8 idd, Way wy, int32 flgs , float w) {
    first = 0;
    last = 0;
    nObjects = 0;
    way = wy;
	road = rd;
	id = idd;
    lightColor = NoLight;
    width = w;
    flags = flgs;
    maximumSpeed = 50.0/3.6; // 50 Kmh;
    minimumSpeed = 0.0;
	laneStart = road->getRoadStart();
	laneEnd = road->getRoadEnd();
    laneSensors = 0;
    userdata = 0;
    nextLanes = new vns::HashTable<const Road*,Lane*>(8);

    exitVehicle = 0;
}


Lane* Lane::clone() const {
	Lane* lane = new Lane(road);
	lane->road = road;
	lane->way = way;
	lane->id = id;
    lane->flags = flags;
    lane->width = width;
    lane->laneStart = laneStart;
    lane->laneEnd = laneEnd;
    lane->maximumSpeed = maximumSpeed;
    lane->minimumSpeed = minimumSpeed;
    return lane;
}

Lane::~Lane(){
	nextLanes->clear();
	connections.clear();
    road = 0;
    lightColor = NoLight;
    clearLaneSensors();
    exitVehicle = 0;
}

bool Lane::addConnectionTo(const Road* nroad){
	if( nroad==0 ) return false;
	Junction* end = getEndJunction();
	if( end==0 ) return false;
	if( road==nroad ) return false;
	if( !end->isExitRoad(nroad) ) return false;
	int32 nr = connections.size();
	for(int32 i=0;i<nr;i++){
		if( nroad == connections.at(i) ) return true;
	}
	connections.append(nroad);
	return true;
}

bool Lane::removeConnectionTo(const Road* nroad){
	if(nroad==0) return false;
	return connections.removeItem(nroad);
}


bool Lane::hasConnectionTo(const Lane* lane) const {
	const vns::Road* nroad = lane->getRoad();
	return connections.contains( nroad );
}

bool Lane::hasConnectionTo( const Road* nroad) const {
	return connections.contains( nroad );
}

void Lane::clearConnections(){
	connections.clear();
}

float Lane::getLaneStartOffset() const {
	return laneStart - road->getRoadStart(way);
}

float Lane::getLaneEndOffset() const {
	return road->getRoadEnd(way) - laneEnd;
}

void Lane::setLaneStartOffset(float o){
	double pos = road->getRoadStart(way) + o;
	setLaneStart(pos);
}

void Lane::setLaneEndOffset(float o){
	double pos = road->getRoadEnd(way) - o;
	setLaneEnd(pos);
}

void Lane::setLaneStart(float pos){
	float roadStart = road->getRoadStart(way);
	float roadEnd = road->getRoadEnd(way);
	if( road->hasStartCrossWalk(way) && pos < (roadStart + VNS_ROAD_CROSSWALKWIDTH + 0.5)){
		laneStart = roadStart + VNS_ROAD_CROSSWALKWIDTH + 0.5;
	}else{
		if(pos > roadEnd || pos > laneEnd || pos < roadStart ){
			laneStart = roadStart;
		}else{
			laneStart = pos;
		}
	}
	getPositionPoint(firstPoint, firstPointDir, firstPointNormal, laneStart);
}

void Lane::setLaneEnd(float pos){
	float roadStart = road->getRoadStart(way);
	float roadEnd = road->getRoadEnd(way);
	if( road->hasEndCrossWalk(way) && pos > (roadEnd - VNS_ROAD_CROSSWALKWIDTH - 0.5)){
		laneEnd = roadEnd - VNS_ROAD_CROSSWALKWIDTH - 0.5;
	}else{
		if(pos < roadStart || pos < laneStart || pos > roadEnd ){
			laneEnd = roadEnd;
		}else{
			laneEnd = pos;
		}
	}
	getPositionPoint(lastPoint,lastPointDir, lastPointNormal, laneEnd);
}

void Lane::setWidth(float w){
	width = w;
	road->update();
}

void Lane::setRoadCenterOffset(float off){
	offset = off;
	// Refresh LaneEnd and LaneStart
	setLaneEnd( laneEnd );
	setLaneStart( laneStart  );
}

void Lane::getPositionPoint(Vec& pt, Vec& dir, Vec& perp, float pos) const {
	if( way == Forward ){
		road->getPolyLine()->pointDirFromFirstPoint(pt,dir,perp,pos,offset);
	}else{
		road->getPolyLine()->pointDirFromFirstPoint(pt,dir,perp,road->getLength()-pos,offset);
		dir = -dir;
		perp = -perp;
	}
}

Junction* Lane::getEndJunction(){
	return (way==Forward?road->endJunction:road->startJunction);
}

Junction* Lane::getStartJunction(){
	return (way==Forward?road->startJunction:road->endJunction);
}

const Junction* Lane::getEndJunction() const {
	return (way==Forward?road->endJunction:road->startJunction);
}

const Junction* Lane::getStartJunction() const {
	return (way==Forward?road->startJunction:road->endJunction);
}

RoadNetwork* Lane::getRoadNetwork(){
	return road->roadNetwork;
}

const RoadNetwork* Lane::getRoadNetwork() const {
	return road->roadNetwork;
}

Lane* Lane::getNextLaneTo(const Lane* destLane){
	if(id==-1) return road->lanes.at(0);
	if(id==-2) return road->lanes.at(road->lanes.size()-1);
	return road->lanes.at(id+(destLane->id>id?1:-1));
}

Lane* Lane::getDrivingLaneOnLeft(){
	if( way == Forward ){
		if(id==-1) return 0;
		if(id==-2) return road->lanes.at(road->lanes.size()-1);
		if(id==0) return 0;
		if(id>0){
			return road->lanes.at(id-1);
		}
	}else{
		if(id==-1)return road->lanes.at(0);
		if(id==-2)return 0;
		int32 nl = road->lanes.size();
		if(id==0) return 0;
		if(id<nl-1){
			return road->lanes.at(id+1);
		}
	}
	return 0;
}

Lane* Lane::getDrivingLaneOnRight(){
	if( way == Forward ){
		if(id==-1)return road->lanes.at(0);
		if(id==-2)return 0;
		int32 nl = road->lanes.size();
		if(id<nl-1){
			return road->lanes.at(id+1);
		}
	}else{
		if(id==-1)return 0;
		if(id==-2)return road->lanes.at(road->lanes.size()-1);
		if(id>0){
			return road->lanes.at(id-1);
		}
	}
	return 0;
}

Lane* Lane::getSameWayDrivingLaneOnLeft(){
	Lane* l = getDrivingLaneOnLeft();
	if(l && way == l->way && !l->hasFlag(vns::Lane::NoTrafficLane)){
		return l;
	}
	return 0;
}

Lane* Lane::getSameWayDrivingLaneOnRight(){
	Lane* r = getDrivingLaneOnRight();
	if(r && way == r->way && !r->hasFlag(vns::Lane::NoTrafficLane)){
		return r;
	}
	return 0;
}

Lane* Lane::getParkingLaneOnLeft(){
	if( way == Forward ){
		return road->leftParkingLane;
	}
	return road->rightParkingLane;
}

Lane* Lane::getParkingLaneOnRight(){
	if( way == Forward ){
		return road->rightParkingLane;
	}
	return road->leftParkingLane;
}


Vehicle* Lane::getFirstVehicle(){
	LaneObject* obj = first;
	while( obj ){
		if( obj->getObjectType() == vns::Object::VehicleT ){
			return dynamic_cast<Vehicle*> (obj);
		}
		obj = obj->getNextObject();
	}
	return 0;
}

Vehicle* Lane::getLastVehicle(){
	LaneObject* obj = last;
	while( obj ){
		if( obj->getObjectType() == vns::Object::VehicleT ){
			return dynamic_cast<Vehicle*> (obj);
		}
		obj = obj->getPreviousObject();
	}
	return 0;
}

const Vehicle* Lane::getFirstVehicle() const {
	const LaneObject* obj = first;
	while( obj ){
		if( obj->getObjectType() == vns::Object::VehicleT ){
			return dynamic_cast<const Vehicle*> (obj);
		}
		obj = obj->getNextObject();
	}
	return 0;
}

const Vehicle* Lane::getLastVehicle() const {
	const LaneObject* obj = last;
	while( obj ){
		if( obj->getObjectType() == vns::Object::VehicleT ){
			return dynamic_cast<const Vehicle*> (obj);
		}
		obj = obj->getPreviousObject();
	}
	return 0;
}

bool Lane::allowsVehicle(const VehicleType& ) const {
	if( hasFlag(NoTrafficLane) ) return false;
	return true;
}


void Lane::addLaneSensor(Trigger* trigger, float pos){
	LaneSensor* nt = new LaneSensor(trigger,pos);
	LaneSensor** prev = &laneSensors;
	LaneSensor* curr = laneSensors;
	while(curr && curr->lanePos < pos) {
		prev = &curr->next;
		curr = curr->next;
	}
	nt->next = curr;
	*prev = nt;
}

void Lane::removeLaneSensor(Trigger* trigger){
	LaneSensor** prev = &laneSensors;
	LaneSensor* curr = laneSensors;
	while(curr){
		if( curr->trigger == trigger ){
			*prev = curr->next;
			delete curr;
			break;
		}
		prev = &curr->next;
		curr = curr->next;
	}
}

void Lane::clearLaneSensors(){
	LaneSensor* tmp;
	LaneSensor* curr = laneSensors;
	while( curr ){
		tmp = curr;
		curr = curr->next;
		delete tmp;
	}
	laneSensors = 0;
}


double Lane::getFreeSpacePosition(float len) const {
	float gap = 1.0;
	LaneObject* prev = 0;
	LaneObject* next = 0;
	float start = laneStart+len*0.5+gap;
	float end = laneEnd-len*0.5-gap;
	float pos = vns::randomValue(start,end);
	objectsAround(pos, prev, next);

	float lp = (prev?prev->getFrontPosition()+len*0.5+gap:start);
	float ln = (next?next->getRearPosition()-len*0.5-gap:end);

	if( ln > lp ){
		return vns::randomValue(lp,ln);
	}
	LaneObject* obj = next;
	while(obj){
		lp = obj->getFrontPosition()+len*0.5+gap;
		ln = (obj->next?obj->next->getRearPosition()-len*0.5-gap:end);
		if( ln > lp ){
			return vns::randomValue(lp,ln);
		}
		obj = obj->next;
	}
	obj = prev;
	while(obj){
		lp = (obj->prev?obj->prev->getFrontPosition()+len*0.5+gap:start);
		ln = obj->getRearPosition()-len*0.5-gap;
		if( ln > lp ){
			return vns::randomValue(lp,ln);
		}
		obj = obj->prev;
	}
	return -1.f;
}

void Lane::objectsAround(float pos, LaneObject*& previous, LaneObject*& next) const {
	previous = 0;
	next = first;
	LaneObject* obj = first;
	while( obj && obj->lanePos <= pos ){
		previous = obj;
		next = obj->next;
		obj = obj->next;
	}
}


void Lane::addObject(LaneObject* obj, float pos) {
	if(obj->lane != 0) return;
	Vec perp;
	obj->prev = 0;
	obj->next = 0;
	obj->lanePos = pos;
	obj->lane = this;

	mlock.lock();
	LaneObject* curr = first;
	while( curr ){
		if( obj->lanePos <= curr->lanePos ){
			obj->next = curr;
			if( obj->prev ){
				obj->prev->next = obj;
			}else{
				first = obj;
			}
			curr->prev = obj;
			nObjects++;
			this->getPositionPoint(obj->pos,obj->dir,perp,obj->lanePos);
			mlock.unlock();
			return;
		}
		obj->prev = curr;
		curr = curr->next;
	}
	if(obj->prev){
		obj->prev->next = obj;
	}else{
		first = obj;
	}
	last = obj;
	nObjects++;
	this->getPositionPoint(obj->pos,obj->dir,perp,obj->lanePos);
	mlock.unlock();
}

/*
void Lane::updateObjectPosition(LaneObject* obj,float pos){
	obj->lock();
	obj->lanePos = pos;
	PolyLine* line = road->getPolyLine();
	float lpos = (way == Forward?pos:road->getLength()-pos);

	obj->lineIndex = line->getIndexPositionX(lpos,obj->lineIndex);

	int32 i = obj->lineIndex;
    Vec dp = line->getPoint(i+1)-line->getPoint(i);
    float a = (lpos-line->getT(i))/(line->getT(i+1)-line->getT(i));
    obj->dir = dp.normalized();
    Vec perp = (line->getNormal(i)*(1-a)+line->getNormal(i+1)*a).normalized();
    obj->pos = line->getPoint(i)+dp*a+perp*offset;

	if( way == Backward ){
		obj->dir = -obj->dir;
	}
	obj->unlock();
}*/

/*
 * first - 1 - 2 - last
 */

void Lane::removeObject(LaneObject* obj) {
	if(obj->lane != this) return;
	mlock.lock();
	if(obj->prev == 0){
		first = obj->next;
	}else{
		obj->prev->next = obj->next;
	}
	if(obj->next == 0){
		last = obj->prev;
	}else{
		obj->next->prev = obj->prev;
	}

    obj->lane = 0;
	obj->prev = 0;
	obj->next = 0;
	nObjects--;
	mlock.unlock();
}

uint32 Lane::simulationStep(Simulator* sim) {
	uint32 nVehicles = 0;
	LaneObject* obj = last;
	while( obj ){
		obj->simulationStep( sim );
		if(obj->getObjectType()==vns::Object::VehicleT){
			nVehicles++;
		}
        obj = obj->prev;
	}
	return nVehicles;
}

void Lane::updateStep(Simulator* sim) {
	LaneObject* obj = last;
	while( obj ){
		if( obj->simstep != sim->simstep ){
			obj->simstep = sim->simstep;
			obj->updateStep( sim );
			for(vns::List<Observer*>::Iterator obs=sim->observers.begin();obs!=sim->observers.end();obs++){
				(*obs)->onLaneObjectSimulationStep(sim,obj);
			}
		}
		Vehicle* vobj = Vehicle::fromObject(obj);
		obj = obj->prev;
		if(vobj && vobj->getState() == vns::Vehicle::Removed ){
			sim->deleteVehicle(vobj);
		}
	}
}

bool Lane::canEnter(const LaneObject* obj) const {
	if( first == 0 ) return true;
	float pos = getLaneStart() + obj->getLength()+first->getLength()*0.5+1.0;
	if( first->getLanePosition() >= pos ){
		return true;
	}
	float sp = first->getSpeed();
	float stops = 2.5;
	float v2 = 2*first->getAccel()*(pos-first->getLanePosition())+(sp*sp);
	if( v2 < stops*stops){
		return false;
	}
	return true;
}

bool Lane::canEnter(float length) const {
	if( first == 0 ) return true;
	if( (first->getRearPosition()-getLaneStart()) < length+2.0 ){
		return false;
	}
	return true;
}

DataStream& operator <<(DataStream&f, const Lane* lane) {
	f<<bool(lane->way==vns::Forward?true:false);
    f<<lane->flags;
    f<<lane->id;
    f<<lane->maximumSpeed;
    f<<lane->minimumSpeed;
    f<<lane->width;
    f<<lane->offset;
    f<<lane->laneStart;
    f<<lane->laneEnd;
    f<<lane->firstPoint;
    f<<lane->lastPoint;
    f<<lane->firstPointDir;
    f<<lane->lastPointDir;
    f<<lane->firstPointNormal;
    f<<lane->lastPointNormal;
    return f;
}

DataStream& operator >>(DataStream& f, Lane* lane) {
	bool way;
	f>>way;
	lane->way = (way==true?vns::Forward:vns::Backward);
    f>>lane->flags;
    f>>lane->id;
    f>>lane->maximumSpeed;
    f>>lane->minimumSpeed;
    f>>lane->width;
    f>>lane->offset;
    f>>lane->laneStart;
    f>>lane->laneEnd;
    f>>lane->firstPoint;
    f>>lane->lastPoint;
    f>>lane->firstPointDir;
    f>>lane->lastPointDir;
    f>>lane->firstPointNormal;
    f>>lane->lastPointNormal;

    return f;
}


}
