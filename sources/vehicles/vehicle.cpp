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


#include "vehicle.h"
#include "simulator.h"
#include "road.h"
#include "lane.h"
#include "roadnetwork.h"
#include "trigger.h"
#include "ghostvehicle.h"

#define OPTIONAL_LANE_THINKTIME 3.0
#define LANE_CHANGE_GAP 2.0
#define JUNCTION_LOOKUP 20.0

namespace vns {

Vehicle::Vehicle(uint64 vid,VehicleType vtype, VehicleModel vmodel, const Path::Iterator& pathit, float width, float length, DriverModel* driver) : LaneObject(VehicleT, width, length) {
    id = vid;
	userdata = 0;
	networkNode = 0;
	path = pathit;
	vehicleType = vtype;
	vehicleModel = vmodel;
	model = driver;
    speed = 0.0;
    accel = 0.0;

    simstep = -1;
    stateFunction = 0;
	nextLane = 0;
    prevLane = 0;
	dirToNextLane = 0.0;
    ghost = new GhostVehicle(this);
    usingTrajectory = false;
    hasPriority = 0;
	yield = false;
	crossing = false;
    tmpPath = 0;
	INeedNewRoute = false;
	numberOfPassengers = 0;
	limitOfPassengers = 5;
	invertWay = 0;
	laneToChange = 0;
	laneStopPosition = vns::MAX_FLOAT;
	currentLaneDecision = DriverModel::None;
	/* Think times */
	optionalLaneChangeThinkTime = 0;
    nextThinkTime = 0;
}

Vehicle::~Vehicle(){
	while(!modules.isEmpty()){
		VehicleModule* module = modules.takeFirst();
		delete module;
	}
	if(ghost->lane){
		ghost->lane->removeObject(ghost);
	}
	delete ghost;
	delete model;
	if(tmpPath){ delete tmpPath; }
}

void Vehicle::simulationStep( Simulator* sim ) {
    if(sim->time >= nextThinkTime){
    	nextThinkTime = sim->time + vns::DriverModel::DT;
    	(this->*checkFunction)( sim );
    	newAccel = (this->*accelFunction)( sim );
    }
    runModules(sim);
}

void Vehicle::updateStep( Simulator* sim ) {
    accel = newAccel;

	brakeLights = (accel < -1 || speed<1.0 ? true: false);
	float oldPos = lanePos;
	lanePos = model->updatePositionAndSpeed(vns::Simulator::DT,lanePos,speed,accel);

	if (next &&  gapTo(lanePos,next) < 0.2 ) { // do not hit next car!!!
		speed = next->getSpeed();
		lanePos = oldPos;
	}

	(this->*stateFunction)( sim );

    /* Update Sensors */
    if(sensorIterator.isValid()){
    	Trigger* tr = sensorIterator.getTrigger();
    	float trpos = sensorIterator.getPosition();
    	float pi = oldPos+(length*0.5);
    	float pf = lanePos+(length*0.5);
    	float rearSensorPosition = trpos - tr->getLength()*0.5;
    	float frontSensorPosition = trpos + tr->getLength()*0.5;
    	if(pi < rearSensorPosition && pf >= rearSensorPosition){
    		tr->enter(this, sim->time - (pf-rearSensorPosition)/speed);
    	}
    	pi -= length;
    	pf -= length;
    	if(pi < frontSensorPosition && pf >= frontSensorPosition){
    		tr->leave(this, sim->time - (pf-frontSensorPosition)/speed);
    		sensorIterator.next();
    	}
    }

    if(ghost->getLane()) ghost->updatePosition( lanePos-oldPos );
    if(usingTrajectory) {
        float t = vns_MAX(vns_MIN(1.0-((trajEnd-lanePos)/trajectory.length()),1.f),0.f);
        trajectory.interpolate(t, pos, dir);
        dir.normalize();
        if(lanePos>trajEnd){
        	pos = pos+dir*(lanePos-trajEnd);
        }
    } else {
    	Vec perp;
    	lane->getPositionPoint(pos,dir,perp,lanePos);
    }
    if(pos.x()==0 && pos.y()==0){
    	fprintf(stderr,"MERDA %d %lf %d %d\n",state,lanePos,lane==0,usingTrajectory);
    }
}

void Vehicle::initialize( Simulator* sim ) {
	currentLaneDecision = DriverModel::None;
	laneStopPosition = vns::MAX_FLOAT;

	INeedNewRoute = false;
	junctionEnteringTime = vns::MAX_DOUBLE;
	usingTrajectory = false;
	invertWay = false;
	laneToChange = 0;
    nextThinkTime = -1;
    optionalLaneChangeThinkTime = -1;

    if(lane->hasFlag(Lane::ParkingLane)){
    	/* entering from a parking lane */
    	speed = 0.0;
    	accel = 0;
    	/* In parking lane but with a route ahead */
		Lane* r = lane->getDrivingLaneOnRight();
		if( r ){
			currentLaneDecision = DriverModel::MandatoryChangeToRight;
		}else{
			Lane* l = lane->getDrivingLaneOnLeft();
			if( l ){
				currentLaneDecision = DriverModel::MandatoryChangeToLeft;
			}else{
				fprintf(stderr,"ERROR: Vehicle tries to leave a parking lane but there are no drive lanes!\n");
				exit(1);
			}
		}
        state = Vehicle::LeavingParking;
    	stateFunction = &Vehicle::leavingParking;
    	accelFunction = &Vehicle::acc_leavingParking;
    	checkFunction = &Vehicle::check_leavingParking;
    }else{
    	double v = vns_MIN(model->getDesiredSpeed(),lane->getMaximumSpeed());
    	if(next){
    		speed = vns_MIN( v , next->getSpeed() );
    	}else{
    		speed = v;
    	}
    	accel = model->accel(this,next);
        enterRoad( sim );
        state = Vehicle::MovingInLane;
    	stateFunction = &Vehicle::movingInLane;
    	accelFunction = &Vehicle::acc_movingInLane;
    	checkFunction = &Vehicle::check_movingInLane;
    }
}


/*
 * Check
 */

void Vehicle::check_leavingParking(Simulator*){
	if(laneToChange) return;
	switch (currentLaneDecision) {
	case DriverModel::MandatoryChangeToLeft: {
		Lane* left = lane->getSameWayDrivingLaneOnLeft();
		if (left && model->isSafeToChangeLane(this, left)) {
			laneToChange = left;
		}
		break;
	}
	case DriverModel::MandatoryChangeToRight: {
		Lane* right = lane->getSameWayDrivingLaneOnRight();
		if (right && model->isSafeToChangeLane(this, right)) {
			laneToChange = right;
		}
		break;
	}
	default:
		break;
	}
}

void Vehicle::check_enteringParking(Simulator*){

}

void Vehicle::check_movingInLane(Simulator* sim) {
	float laneEnd = lane->getLaneEnd();
	float fpos = getFrontPosition();
    if( fpos >= laneEnd - 10){
		if( path.hasNext() && nextLane == 0) {
			nextLane = getBestNextLane();
			if (nextLane == 0) {
				VNS_Debug << "WTF? no NextLane\n";
			}
			yield = false;
			crossing = false;
			laneToChange = 0;
			currentLaneDecision = DriverModel::None;
		}
	} else {
		switch (currentLaneDecision) {
		case DriverModel::MandatoryChangeToLeft: {
			Lane* left = lane->getSameWayDrivingLaneOnLeft();
			if (left && model->isSafeToChangeLane(this, left)) {
				laneToChange = left;
				return;
			}
			return;
		}
		case DriverModel::MandatoryChangeToRight: {
			Lane* right = lane->getSameWayDrivingLaneOnRight();
			if (right && model->isSafeToChangeLane(this, right)) {
				laneToChange = right;
				return;
			}
			return;
		}
		default:
			break;
		}

		/* If we have a stop there is no need to check optional lane changes */
		if (stops.isValid()) {
			return;
		}

		if (sim->time > optionalLaneChangeThinkTime && fpos < laneEnd - 10) {
			laneToChange = considerLaneChanges(sim);
			optionalLaneChangeThinkTime = sim->time + OPTIONAL_LANE_THINKTIME;
		}

	}

}

void Vehicle::check_changingLane(Simulator* ) {

}

void Vehicle::check_stoppedInLane(Simulator* ){

}

void Vehicle::check_movingInJunction(Simulator* ) {

}

void Vehicle::check_changingWay(Simulator* ) {

}

/*
 * State Methods
 */

void Vehicle::leavingParking(Simulator* sim){
	if( laneToChange ){
		changeToLane( sim, laneToChange );
	}
}

void Vehicle::enteringParking(Simulator*){
	if( !path.hasNext() ){
		state = Vehicle::Removed;
	}
}

void Vehicle::movingInLane(Simulator* sim) {
	/* First check if we need to change lane */
	if( laneToChange ){
		changeToLane( sim, laneToChange );
		return;
	}

	float laneEnd = lane->getLaneEnd();
	float fpos = getFrontPosition();

	/* if needs to stop ahead */
	if( stops.isValid() ){
		if( fpos >= stops.getPosition()-5.0){
			fprintf(stderr,"%lf %lf\n",fpos,stops.getPosition());
			endStopTime = sim->time + stops.getDuration();
			state = Vehicle::Stopped;
			stateFunction = &Vehicle::stoppedInLane;
			accelFunction = &Vehicle::acc_stoppedInLane;
			checkFunction = &Vehicle::check_stoppedInLane;
			for(vns::List<Observer*>::Iterator obs=sim->observers.begin();obs!=sim->observers.end();obs++){
				(*obs)->onEnterRouteStop(sim,this);
			}
		}
		return;
	}

	/* if is approaching an intersection */
    if( fpos > laneEnd - JUNCTION_LOOKUP){
    	if (nextLane) {
			if( fpos >= laneEnd ) {
				crossToRoad(sim,nextLane);
				return;
			}
		}else{
			if( lanePos >= laneEnd ){
				if(path.hasNext()){
					VNS_Debug << "OUT----" << getID() << " " << lane->getRoad()->getID() << " " << (lane->getWay()==vns::Forward?"forward":"backward");
					VNS_Debug << "\nStops?" << stops.isValid() <<" " << path.getLastRoad()->getID() << "\n";
				}
				state = Removed;
				return;
			}
		}
    }
}

void Vehicle::changingLane(Simulator* sim) {
    if( lanePos >= trajEnd ) {
    	usingTrajectory = false;
        sim->removeObjectFromLane(ghost);
        state = Vehicle::MovingInLane;
		stateFunction = &Vehicle::movingInLane;
		accelFunction = &Vehicle::acc_movingInLane;
		checkFunction = &Vehicle::check_movingInLane;
		//movingInLane( sim );
    }
}

void Vehicle::stoppedInLane(Simulator* sim){
	if( sim->time > endStopTime ){
		stops.next();
		for(vns::List<Observer*>::Iterator obs=sim->observers.begin();obs!=sim->observers.end();obs++){
			(*obs)->onLeaveRouteStop(sim,this);
		}
		currentLaneDecision = model->checkForMandatoryLaneChanges(this);

        state = Vehicle::MovingInLane;
        stateFunction = &Vehicle::movingInLane;
        accelFunction = &Vehicle::acc_movingInLane;
        checkFunction = &Vehicle::check_movingInLane;
        //movingInLane(sim);
	}
}

void Vehicle::movingInJunction(Simulator*) {
/*	if(prevLane->exitVehicle == this){
		if( getRearPosition() > (lane->getLaneStart()-trajectory.length()) ){
			prevLane->exitVehicle = 0;
		}
	}
*/
    if( lanePos > lane->getLaneStart() ) {
        usingTrajectory = false;
        state = Vehicle::MovingInLane;
		stateFunction = &Vehicle::movingInLane;
        accelFunction = &Vehicle::acc_movingInLane;
        checkFunction = &Vehicle::check_movingInLane;
		//movingInLane( sim );
    }
}

void Vehicle::changingWay(Simulator* sim) {
    if( lanePos >= trajEnd ) {
    	usingTrajectory = false;
        sim->removeObjectFromLane(ghost);
        state = Vehicle::MovingInLane;
		stateFunction = &Vehicle::movingInLane;
		accelFunction = &Vehicle::acc_movingInLane;
		checkFunction = &Vehicle::check_movingInLane;
		//movingInLane( sim );
    }
}



/*
 * Accel Methods
 */

float Vehicle::acc_leavingParking(Simulator* sim){
	/* vehicles waiting to leave a parking lane */
	speed = 0;
	return 0;
}

float Vehicle::acc_enteringParking(Simulator* sim){
	/* vehicles entering a parking lane */
	return model->accel( this );
}

float Vehicle::acc_movingInLane(Simulator* sim){
	float stopPos = vns::MAX_FLOAT;
	float fpos = getFrontPosition();

	if( currentLaneDecision == DriverModel::MandatoryChangeToRight || currentLaneDecision == DriverModel::MandatoryChangeToLeft ){
		//stopPos = lane->getLaneEnd()-JUNCTION_LOOKUP;
	}
	if(stops.isValid()){
		stopPos = stops.getPosition();
	}

	if( next ){
		junctionEnteringTime = sim->getSimulationTime();
		if( stopPos < next->getRearPosition() ){
			return model->accel( this, stopPos );
		}
		return model->accel( this , next );
	}else{
		if( stopPos < lane->getLaneEnd() ){
			junctionEnteringTime = sim->getSimulationTime();
			return model->accel( this, stopPos);
		}
	}

	if( !path.hasNext() ) {
		return model->accel( this );
	}

	// IF i don't have vehicle in front or stops, look to the junction
	if( fpos>lane->getLaneEnd()-JUNCTION_LOOKUP){
		// Approching junction
		Junction* junction = lane->getEndJunction();
		if( junction ){
			return accelToJunction( sim, junction );
		}
	}
	junctionEnteringTime = sim->getSimulationTime();
	return model->accel( this );
}

float Vehicle::acc_changingLane(Simulator* sim){
	float stopPos = vns::MAX_FLOAT;
	float fpos = getFrontPosition();

	/* check possible stops ahead */

	if( next ){
		if( stopPos < next->getRearPosition() ){
			return model->accel( this, stopPos );
		}
		return model->accel( this , next );
	}else{
		if( stopPos < lane->getLaneEnd() ){
			return model->accel( this, stopPos);
		}
	}

	return model->accel( this );
}

float Vehicle::acc_stoppedInLane(Simulator* sim){
	/* vehicles stopped in lane */
	speed = 0;
	return 0;
}

float Vehicle::acc_movingInJunction(Simulator* sim) {
	float acc;
	if( next ){
		acc = model->accel( this , next );
	}else{
		acc = model->accel(this);
	}

	const Junction* junction = lane->getStartJunction();
	int8 nr = junction->getNumberOfRoads();
	for (int8 i = 0; i < nr; i++) {
		const Road* road = junction->getRoad(i);
		int8 nl = road->getNumberOfLanes();
		for (int8 l = 0; l < nl; l++) {
			const Lane* ln = road->getLane(l);
			if( ln == lane ) continue;
			if( ln->getStartJunction() == junction ){
				/* cars leaving the junction */
				const Vehicle* v = ln->getFirstVehicle();
				if(v==0) continue;
				if( v->getRearPosition() < ln->getLaneStart()-5 ){
					acc = vns_MIN( acc, acceltoVehicle(v) );
				}
			}
		}
	}
	return acc;
}

float Vehicle::acc_changingWay(Simulator* sim) {
	float stopPos = vns::MAX_FLOAT;
	float fpos = getFrontPosition();

	/* check possible stops ahead */

	if( next ){
		if( stopPos < next->getRearPosition() ){
			return model->accel( this, stopPos );
		}
		return model->accel( this , next );
	}else{
		if( stopPos < lane->getLaneEnd() ){
			return model->accel( this, stopPos);
		}
	}

	return model->accel( this );
}

float Vehicle::accelToJunction(Simulator* , Junction* junction){
	/*if(next && next->type==Object::GhostT){
		if(getSpeed()<3.0 && next->getSpeed()<3.0 && getFrontPosition()<lane->getLaneEnd()){
			junctionEnteringTime = sim->getSimulationTime();
			yield = true;
			crossing = false;
			return model->accel(this, next );
		}
	}*/
	// Try to find another route
	/*
	if( sim->hasFlag(Simulator::RouteChanges) && nextLane && !nextLane->canEnter(this) ){
			const LaneObject* obj = nextLane->firstObject();
			while( obj && obj->getObjectType() != VehicleT ) obj = obj->getNextObject();
			if( obj && obj->getObjectType() == VehicleT ) {
				const Vehicle* v = dynamic_cast<const Vehicle*> (obj);
				if( sim->time > v->timeEntryRoad + 120 ){
					tryToFindOtherPath(sim);
			}
		}
	}*/
	// Always stop at the intersection
	/*if( hasPriority>0 && getTrafficLightColor() == vns::NoLight && getFrontPosition() < lane->getLaneEnd()-5 ){
		if( next ){
			return model->accel(this, min(next->lanePos,lane->getLaneEnd()) );
		}
		return model->accel(this, lane->getLaneEnd() );
	}*/

	/*if( getTrafficLightColor() != vns::RedLight && (nextLane && lane->getMaximumSpeed()<20 && !nextLane->canEnter(this)) ){
		tryToOptimizeGridLocks();
	}*/

	yield = false;
	bool stopAtEnd = false;

	// If the traffic color is RED or I cannot enter Lane!
	if( getTrafficLightColor() == vns::RedLight ){// || (nextLane && lane->getMaximumSpeed()<20 && !nextLane->canEnter(this)) ){
		//yield = true;
		crossing = false;
		stopAtEnd = true;
	}
	//
	/*
	if( willBlockIntersection() ){
		yield = true;
		crossing = false;
		return getAccelToEnd();
	}*/

	/*if( crossing == true){
		return getAccelToCross();
	}*/

	float acc = model->accel(this);

	int8 nr = junction->getNumberOfRoads();
	for (int8 i = 0; i < nr; i++) {
		Road* road = junction->getRoad(i);
		int8 nl = road->getNumberOfLanes();
		for (int8 l = 0; l < nl; l++) {
			Lane* ln = road->getLane(l);
			if( ln == lane ) continue;
			if( ln->getEndJunction() == junction ){
				/* cars approaching the junction */
				if( crossing==true ) continue;
				Vehicle* v = ln->getLastVehicle();
				if(v==0) continue;
				if( v->getFrontPosition() > ln->getLaneEnd()-JUNCTION_LOOKUP ) {
					if( hasTrajectoryConflictWith(v) ){
						if( v->isGivingWay() || v->getTrafficLightColor() == vns::RedLight ){
							continue;
						}
						if (v->isCrossing()
								|| v->getJunctionEnteringTime()
										< getJunctionEnteringTime()) {
							stopAtEnd = true;
						} else {
							if (v->getJunctionEnteringTime()
									== getJunctionEnteringTime()) {
								if (road->getID() < lane->road->getID()) {
									stopAtEnd = true;
								}
								if (road->getID() == lane->road->getID()) {
									if (ln->getID() < lane->getID()) {
										stopAtEnd = true;
									}
								}
							}
						}
					}
				}
			}else{
				/* cars leaving the junction */
				const Vehicle* v = ln->getFirstVehicle();
				if(v==0) continue;
				if( v->getRearPosition() < ln->getLaneStart()-5 ){
					if( hasTrajectoryConflictWithInside(v) ){
						if( v->getRearPosition() < ln->getLaneStart()-15 ){
							stopAtEnd = true;
						}
						else{
							float df = pos.distanceTo(v->getPosition())-v->getLength();
							acc = vns_MIN(acc,model->accel( this, lanePos+df, v->getSpeed(), v->getAccel() ));
						}

					}
				}
			}
		}
	}
	if(stopAtEnd){
		acc = vns_MIN(acc,model->accel( this, lane->getLaneEnd()));
	}else{
		crossing=true;
	}
	return acc;
	//double stopPos = lane->getLaneEnd()+4+30*((180-abs(dirToNextLane))/180.0);
	//return model->accel( this , stopPos );
}

/*************************************************************************/

void Vehicle::changeToLane(Simulator* sim,Lane* destLane){
	// Fast Lane Change
	currentLaneDecision = vns::DriverModel::None;
	laneStopPosition = vns::MAX_FLOAT;
/*  prevLane = lane;
	sim->addObjectToLane(this, destLane, lanePos);
	enterLane(sim);
	return;*/

	float laneEnd = destLane->getLaneEnd();//-LANE_CHANGE_GAP;
	float oldPos = lanePos;
	float endPos;

	if(lanePos>laneEnd-10 && !lane->hasFlag(vns::Lane::ParkingLane)){
		fprintf(stderr,"lanePos>laneEnd-10 %lf %lf - %lf\n",lanePos,laneEnd-10,laneEnd-10-lanePos);
	}
	if( speed > 3.f ) {
		endPos = vns_MIN(oldPos + 2.f*speed, laneEnd-5);
	} else {
		endPos = vns_MIN(oldPos + 5, laneEnd-5);
	}

	if(ghost->getLane()) sim->removeObjectFromLane(ghost);

	Vec npt, ndir, nperp;
	destLane->getPositionPoint(npt, ndir, nperp, endPos);

	lane->getPositionPoint(pos,dir,nperp,lanePos);
	trajectory.create(pos, dir, npt, ndir);
	usingTrajectory = true;
	trajEnd = endPos;

	prevLane = lane;
	sim->addObjectToLane(this, destLane, endPos-trajectory.length());
	sim->addObjectToLane(ghost, prevLane,endPos-trajectory.length());// oldPos );

	/* NEW LANE*/
	enterLane(sim);
	state = Vehicle::ChangingLane;
	stateFunction = &Vehicle::changingLane;
	accelFunction = &Vehicle::acc_changingLane;
	checkFunction = &Vehicle::check_changingLane;
	//changingLane(sim);
}


void Vehicle::changeWay(Simulator* sim,Lane* newLane){
	float destPos = (newLane->getRoad()->getLength()-lanePos);
	Vec dpt,ddir,dperp;
	newLane->getPositionPoint(dpt,ddir,dperp,destPos);
	trajectory.create(getPosition(),getDirection(),dpt,ddir);
	usingTrajectory = true;
	trajEnd = destPos;
	prevLane = lane;
	float oldPos = lanePos;
	sim->addObjectToLane(this, newLane, destPos-trajectory.length());
	sim->addObjectToLane(ghost, prevLane, oldPos );

	path.next();
	enterRoad(sim);
	state = Vehicle::ChangingWay;
	stateFunction = &Vehicle::changingWay;
	accelFunction = &Vehicle::acc_changingWay;
	checkFunction = &Vehicle::check_changingWay;
	//changingWay( sim );
}

void Vehicle::crossToRoad(Simulator* sim,Lane* newLane){
	Vec perp;
	lane->getPositionPoint(pos,dir,perp,lanePos);
	trajectory.create(pos,dir,newLane->getFirstPoint(),newLane->getFirstPointDirection());
	usingTrajectory = true;
	trajEnd = newLane->getLaneStart();
	prevLane = lane;
	//float oldPos = lanePos;
	lane->exitVehicle = this;
	sim->addObjectToLane(this, newLane, newLane->getLaneStart()-trajectory.length());


	path.next();
	enterRoad(sim);
	state = Vehicle::MovingInJunction;
	stateFunction = &Vehicle::movingInJunction;
	accelFunction = &Vehicle::acc_movingInJunction;
	checkFunction = &Vehicle::check_movingInJunction;
	//movingInJunction( sim );
}



float Vehicle::acceltoVehicle( const Vehicle* v ) const {
	/* mutual intersection */
	float acc = vns::MAX_FLOAT;
	bool priority = false;
	if (v->getJunctionEnteringTime() < getJunctionEnteringTime() ||
				(v->getJunctionEnteringTime()== getJunctionEnteringTime() && v->lane->road->getID() < lane->road->getID()) ||
				(v->getJunctionEnteringTime()== getJunctionEnteringTime() && v->lane->road->getID() == lane->road->getID() && v->lane->getID() < lane->getID()) ) {
			priority = true;
		}

	Vec mypos = pos+dir*getLength()*0.5;
	const Vec& vd = v->getDirection();
	const Vec& vp = v->getPosition();
	float vlen = v->getLength()*0.5;
	float halfw = getWidth()*0.5;

	Vec vp1 = vp-vd*(vlen+halfw);
	Vec vp2 = vp+vd*(vlen+halfw);
	Vec ip;
	if( intersects(mypos,lane->getFirstPoint(), vp1,vp2,ip) ){
		if (priority) {
			return vns::MAX_FLOAT;
		}
		float df = pos.distanceTo(ip)-vlen;
		return model->accel( this, lanePos+df );
	}
	if( intersects(mypos,lane->getFirstPoint(), vp2,v->lane->getFirstPoint(),ip ) ){
		if (priority) {
			return vns::MAX_FLOAT;
		}
		float df = pos.distanceTo(ip)-vlen;
		return model->accel( this, lanePos+df );
	}
	return acc;
}






















float Vehicle::facilitateLaneChanges(){
	/*if(currentLaneDecision!=vns::DriverModel::MandatoryChangeToLeft && currentLaneDecision!=vns::DriverModel::MandatoryChangeToRight){
		return next;
	}*/
	float gapMin = 5.0;
	Lane* left = lane->getSameWayDrivingLaneOnLeft();
	Vehicle* v1 = 0;
	Vehicle* v2 = 0;
	if(left){
		LaneObject* prevLeft;
		LaneObject* nextLeft;
		left->objectsAround(getLanePosition(), prevLeft, nextLeft);
		if(nextLeft && getFrontPosition()>nextLeft->getRearPosition()){
			nextLeft = nextLeft->getNextObject();
		}
		Vehicle* v = Vehicle::fromObject( nextLeft );
		if(v && v->currentLaneDecision==vns::DriverModel::MandatoryChangeToRight){
			v1 = v;
		}
	}
	Lane* right = lane->getSameWayDrivingLaneOnRight();
	if(right){
		LaneObject* prevRight;
		LaneObject* nextRight;
		right->objectsAround(getLanePosition(), prevRight, nextRight);
		if(nextRight && getFrontPosition()>nextRight->getRearPosition()){
			nextRight = nextRight->getNextObject();
		}
		Vehicle* v = Vehicle::fromObject( nextRight );
		if(v && v->currentLaneDecision==vns::DriverModel::MandatoryChangeToLeft){
			v2 = v;
		}
	}
	Vehicle* ret = 0;
	if(v1 && v2){
		ret = v1->getRearPosition()<v2->getRearPosition()?v1:v2;
	}else{
		if(v1) ret = v1;
		if(v2) ret = v2;
	}
	if(ret){
		return ret->getRearPosition()-gapMin;
	}
	return vns::MAX_FLOAT;
}

void Vehicle::enterRoad( Simulator* sim ){
	yield = false;
	crossing = false;
	for(vns::List<Observer*>::Iterator obs=sim->observers.begin();obs!=sim->observers.end();obs++){
		(*obs)->onRoadEnter(sim, this );
	}
	if( path.isValid() ){
		stops = path.getStopsIterator();
	}
	enterLane( sim );
}

void Vehicle::enterLane(Simulator* sim) {
	laneToChange = 0;
    laneStopPosition = vns::MAX_FLOAT;
    currentLaneDecision = DriverModel::None;
    invertWay = false;
	INeedNewRoute = false;
	hasPriority = 0;

    // Actualizar sensores da Lane
    sensorIterator = lane->getSensorIterator();
    while( sensorIterator.isValid() && sensorIterator.getPosition() < lanePos ){
    	sensorIterator.next();
    }

    model->setSpeedLimit( lane->getMaximumSpeed() );

    nextLane = 0;
    if( path.hasNext() ) {
    	Road* nRoad = path.getNextRoad();
    	nextLane = lane->getNextLane(nRoad);
        if( path.getRoad() == nRoad ){
        	if( path.getNextRoadWay() != path.getWay() ){
        		invertWay = true;
        	}
        }
    }

	if(nextLane){
		dirToNextLane = getLane()->getLastPointDirection().angleDiffTo( nextLane->getFirstPointDirection() );
		hasPriority = getPriority();
	}

	if(getFrontPosition()<lane->getLaneEnd()-10){
		currentLaneDecision = model->checkForMandatoryLaneChanges(this);
	}

	for(vns::List<Observer*>::Iterator obs=sim->observers.begin();obs!=sim->observers.end();obs++){
		(*obs)->onLaneEnter(sim, this);
	}
}


vns::Lane* Vehicle::getBestNextLane() const {
	Road* road = lane->getRoad();
	int8 nl = lane->getRoad()->getNumberOfLanes();
	int8 i;
	for(i=lane->getID()+1;i<nl;i++){
		Lane* ln = road->getLane(i);
		if( ln==lane || ln->hasFlag(vns::Lane::NoTrafficLane) ) continue;
		vns::Lane* nLane = ln->getNextLane(path.getNextRoad());
		if(nLane){
			return nLane;
		}
	}
	for(i=lane->getID()-1;i>=0;i--){
		Lane* ln = road->getLane(i);
		if( ln==lane || ln->hasFlag(vns::Lane::NoTrafficLane) ) continue;
		vns::Lane* nLane = ln->getNextLane(path.getNextRoad());
		if(nLane){
			return nLane;
		}
	}
	return 0;
}




void Vehicle::tryToFindOtherPath( Simulator* sim ){
	vns::Junction* junction = lane->getEndJunction();
	int32 nr = junction->getNumberOfRoads();
	for(int32 i=0;i<nr;i++){
		vns::Road* road = junction->getRoad(i);
		if(road != nextLane->road && road != lane->road && lane->road->hasConnectionTo(road)){
			int32 nl = road->getNumberOfLanes();
			Lane* nextln = 0;
			for(int32 l=0;l<nl;l++){
				vns::Lane* ln = road->getLane(l);
				if( !ln->hasFlags(Lane::NoTrafficLane) && ln->getStartJunction()==junction && ln->canEnter(this) ){
					nextln = ln;
				}
			}
			if(nextln == 0) continue;

			Way fromWay = (road->getStartJunction()==junction?Forward:Backward);
			Path* newpath = sim->calculatePath(road,fromWay,path.getLastRoad(),path.getLastRoadWay());
			if(newpath){
				if(tmpPath != 0){
					delete tmpPath;
					stops = PathNode::StopsIterator();
				}
				tmpPath = newpath;
				newpath->prependNode(lane->road,lane->getWay());
				path = newpath->getFirstNodeIterator();
				for(vns::List<Observer*>::Iterator obs=sim->observers.begin();obs!=sim->observers.end();obs++){
					(*obs)->onRouteChanged(sim,this);
				}
				nextLane = nextln;
				dirToNextLane = getLane()->getLastPointDirection().angleDiffTo( nextLane->getFirstPointDirection() );
			}
		}
	}
}




int Vehicle::getPriority(){
	if( lane->road->getType() == vns::Road::Motorway ) return 0;
	if( nextLane && nextLane->road->getType() == vns::Road::Motorway ) return 0;

	//if( lane->hasFlag(vns::Lane::StopSign) ) return 2;
	//if( lane->hasFlag(vns::Lane::YieldSign) ) return 1;

	Junction* junction = lane->getEndJunction();
	int32 nr = junction->getNumberOfRoads();
	for (int32 i = 0; i < nr; i++) {
		Road* road = junction->getRoad(i);
		if(road == lane->getRoad()) continue;
		int32 nl = road->getNumberOfLanes();
		for (int32 l = 0; l < nl; l++) {
			Lane* ln = road->getLane(l);
			if( ln->getEndJunction() == junction && ln->hasConnectionTo(nextLane->getRoad())){
				return 1;
				if( ln->hasFlag(vns::Lane::StopSign) || ln->hasFlag(vns::Lane::YieldSign) ) continue;
				if( lane->getMaximumSpeed() < ln->getMaximumSpeed() ) return 1;
				if( lane->getLastPointDirection().angleDiffTo( -ln->getLastPointDirection() ) > dirToNextLane){
					return 1;
				}
			}
		}
	}
	return 0;
}

bool Vehicle::lossPriorityAgainst( const Vehicle* v ) const {
	if(lane->road == v->lane->road){
		float myd = lane->getLastPoint().distanceTo(nextLane->getFirstPoint());
		float vd = v->lane->getLastPoint().distanceTo(v->nextLane->getFirstPoint());
		if( vd < myd ) return true;
		if( vd == myd ){
			if(lane->getWay() == Forward) return (lane->getID()>v->lane->getID());
			else return (lane->getID()<v->lane->getID());
		}
		return false;
	}
	/*
	if(lane->hasFlag(vns::Lane::StopSign) || lane->hasFlag(vns::Lane::YieldSign)){
		if( (!v->lane->hasFlag(vns::Lane::StopSign) && !v->lane->hasFlag(vns::Lane::YieldSign))){
			return true;
		}
	}else{
		if(v->lane->hasFlag(vns::Lane::StopSign) || v->lane->hasFlag(vns::Lane::YieldSign)){
			return false;
		}
	}*/
	if(lane->getLastPointDirection().angleDiffTo(-v->lane->getLastPointDirection()) > dirToNextLane){
		return true;
	}
	return false;
}






vns::Lane* Vehicle::considerLaneChanges(Simulator* sim){
	if( next==0 ){
		return 0;
	}
	// So existe uma lane
	if( lane->getRoad()->getNumberOfLanes() < 2 ) return 0;
	// Se ja estiver a mudar de faixa
	if( state == Vehicle::ChangingLane ) return 0;

	// se nao estiver a mudar de faixa, determinar se outra faixa é mais atractiva e se é possivel mudar

	currentLaneDecision = model->makeLaneChangeDecision(this,sim);
	switch( currentLaneDecision ){
	case DriverModel::OptionalChangeToLeft:{
		return lane->getSameWayDrivingLaneOnLeft();
	}
	case DriverModel::OptionalChangeToRight:{
		return lane->getSameWayDrivingLaneOnRight();
	}
	case DriverModel::StayInLane: return 0;
	case DriverModel::None: return 0;
	default: return 0;
	}
	return 0;
}

bool Vehicle::hasTrajectoryConflictWith( const Vehicle* v ) const {
	if( nextLane == 0 || v->nextLane==0 ) return false;
	if( nextLane == v->nextLane ) return true;
	if( intersects(lane->getLastPoint(), nextLane->getFirstPoint(), v->lane->getLastPoint(), v->nextLane->getFirstPoint()) ){
		return true;
	}
	return false;
}

bool Vehicle::hasTrajectoryConflictWithInside( const Vehicle* v ) const {
	//if( nextLane == 0 || v->prevLane == 0 ) return false;
	if( nextLane==0 ) return false;
	if( nextLane == v->lane ) return true;

	const Vec& vd = v->getDirection();
	const Vec& vp = v->getPosition();
	float vlen = v->getLength()*0.5;
	float halfw = getWidth()*0.5;

	Vec vp1 = vp-vd*(vlen+halfw);
	Vec vp2 = vp+vd*(vlen+halfw);
	if( intersects(lane->getLastPoint(), nextLane->getFirstPoint(), vp1, vp2) ){
		return true;
	}
	if( intersects(lane->getLastPoint(), nextLane->getFirstPoint(), vp2, v->lane->getFirstPoint()) ){
		return true;
	}
	return false;
}

Vehicle* Vehicle::fromObject(Object* obj){
	if( obj == 0 ) return 0;
	if(obj->getObjectType()==Object::VehicleT){
		return dynamic_cast<Vehicle*>(obj);
	}
	return 0;
}

Vehicle* Vehicle::getVehicleInFront(){
	LaneObject* obj = getNextObject();
	while( obj ){
		if( obj->getObjectType() == VehicleT ){
			return dynamic_cast<Vehicle*> (obj);
		}
		obj = obj->getNextObject();
	}
	return 0;
}

Vehicle* Vehicle::getVehicleBehind(){
	LaneObject* obj = getPreviousObject();
	while( obj ){
		if( obj->getObjectType() == VehicleT ){
			return dynamic_cast<Vehicle*> (obj);
		}
		obj = obj->getPreviousObject();
	}
	return 0;
}

const Vehicle* Vehicle::getVehicleInFront() const {
	const LaneObject* obj = getNextObject();
	while( obj ){
		if( obj->getObjectType() == VehicleT ){
			return dynamic_cast<const Vehicle*> (obj);
		}
		obj = obj->getNextObject();
	}
	return 0;
}

const Vehicle* Vehicle::getVehicleBehind() const {
	const LaneObject* obj = getPreviousObject();
	while( obj ){
		if( obj->getObjectType() == VehicleT ){
			return dynamic_cast<const Vehicle*> (obj);
		}
		obj = obj->getPreviousObject();
	}
	return 0;
}


bool Vehicle::updatePath(Path* newpath){
	Path::Iterator it = newpath->getFirstNodeIterator();
	if(it.getRoad() != lane->getRoad() || it.getWay() != lane->getWay()){
		return false;
	}
	if(tmpPath!=0){
		delete tmpPath;
	}
	tmpPath = newpath;
	path = it;
	if( path.isValid() ){
		stops = path.getStopsIterator();
	}
	return true;
}

bool Vehicle::willBlockIntersection() const {
	if(nextLane==0) return false;
	const LaneObject* v = nextLane->getFirstObject();
	if(v==0) return false;
	float pos = nextLane->getLaneStart()+getLength()+2.0;
	float rear = v->getRearPosition();
	if( rear > pos ) return false;
	float t = (pos-rear)/v->getSpeed();
	if(t>=6){
		return false;
	}
	return true;
}

Road* Vehicle::getNextRoad(){
	if(path.hasNext()){
		return path.getNextRoad();
	}
	return 0;
}

const Road* Vehicle::getNextRoad() const {
	if(path.hasNext()){
		return path.getNextRoad();
	}
	return 0;
}

const PathStop* Vehicle::getRoadStop() const {
	if( stops.isValid() ){
		return stops.get();
	}
	return 0;
}

vns::Light Vehicle::getTrafficLightColor() const {
	return lane->getTrafficLightColor();
}

float Vehicle::getAccelTo(const LaneObject* obj) const {
	if(obj) return model->accel( this, obj);
	return model->accel( this );
}

float Vehicle::getAccelToEnd() const {
	float end = lane->getLaneEnd();
	Vehicle* v = lane->exitVehicle;
	if(v){
		float df = pos.distanceTo(v->getPosition())-v->getLength()*0.5;
		if(end+df<end){
			return model->accel( this, end+df, v->getSpeed(), v->getAccel() );
		}
	}
	return model->accel( this, end );
}

float Vehicle::getAccelToCross() const {
	/*Vehicle* v = lane->exitVehicle;
	if(v){
		float df = pos.distanceTo(v->getPosition())-v->getLength()*0.5;
		return model->accel( this, lanePos+df, v->getSpeed(), v->getAccel() );
	}
	if(nextLane){
		LaneObject* obj = nextLane->getFirstObject();
		if(obj){
			float df = pos.distanceTo(obj->getPosition())-obj->getLength()*0.5;
			return model->accel( this, lanePos+df,obj->getSpeed(),obj->getAccel());
		}
	}*/

	float acc = model->accel(this);

	const Junction* junction = lane->getEndJunction();
	int8 nr = junction->getNumberOfRoads();
	for (int8 i = 0; i < nr; i++) {
		const Road* road = junction->getRoad(i);
		int8 nl = road->getNumberOfLanes();
		for (int8 l = 0; l < nl; l++) {
			const Lane* ln = road->getLane(l);
			if( ln == lane ) continue;
			if( ln->getEndJunction() == junction ){
				/* cars approaching the junction */

			}else{
				/* cars leaving the junction */

				const Vehicle* v = ln->getFirstVehicle();
				if(v==0) continue;
				if( v->getRearPosition() < ln->getLaneStart()-5 ){
					if( hasTrajectoryConflictWithInside(v) ){
						float df = pos.distanceTo(v->getPosition())-v->getLength()*0.5;
						float macc = model->accel( this, lanePos+df, v->getSpeed(), v->getAccel() );
						if(macc<acc){
							acc = macc;
						}
					}
				}
			}
		}
	}
	return acc;
}

void Vehicle::installModule(const Simulator* sim,VehicleModule* module){
	modules.append(module);
	module->onInstall(sim,this);
}

void Vehicle::uninstallAllModules(const Simulator* sim){
	while(!modules.isEmpty()){
		VehicleModule* module = modules.takeFirst();
		module->onUninstall(sim,this);
		delete module;
	}
}

void Vehicle::runModules(const Simulator* sim){
	/* Run modules */
	for(vns::List<VehicleModule*>::iterator it = modules.begin();it!=modules.end();it++){
		(*it)->onSimulationStep(sim,this,vns::Simulator::DT);
	}
}


}
