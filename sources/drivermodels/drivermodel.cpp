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

#include "drivermodel.h"
#include "lane.h"
#include "vehicle.h"
#include "driverinfo.h"
#include "drivers.h"
#include <cstring>

#define OPTIONAL_LANECHANGE_MIN_DISTANCE 30.0

namespace vns {

DriverModel::DriverModel() {
	/* Memory */
    mtau = 1.0;
    mresignationMaxAlphaT = 1.0;
    mresignationMinAlphaV0 = 1.0;
    mresignationMinAlphaA = 1.0;
    malphaA = 1.0;
    malphaV0 = 1.0;
    malphaT = 1.0;

    yield = randomValue(0.5,1.0);
    patience = randomValue(0.2,0.5); // 0.0 never waits and 1.0 waits forever

    // MOBIL change lane Model
    gapMin = 0.2;
    politeness = 0.1;
    bSafe = 10.0;
    biasRight = 0.3;
    threshold = 0.2;
    speedLimit = 120/3.6;
}


DriverModel::DriverModel(const DriverInfo* d) {
	/* Memory */
    mtau = randomValue(d->mtau_min,d->mtau_max);
    mresignationMaxAlphaT = randomValue(d->malphaT_min,d->malphaT_max);
    mresignationMinAlphaV0 = randomValue(d->malphaV0_min,d->malphaV0_max);
    mresignationMinAlphaA = randomValue(d->malphaA_min,d->malphaA_max);
    malphaA = 1.0;
    malphaV0 = 1.0;
    malphaT = 1.0;

    yield = randomValue(0.5,1.0);
    patience = randomValue(0.2,0.5); // 0.0 never waits and 1.0 waits forever

    // MOBIL change lane Model
    gapMin = 0.5;
    politeness = 0.1;//randomValue(d->politiness_min,d->politiness_max); // politiness
    //deltab = randomValue(d->deltab_min,d->deltab_max);
    bSafe = 4.0;//randomValue(d->bsafe_min,d->bsave_max);
    biasRight = 0.3;//randomValue(d->athr_min,d->athr_max);
    threshold = 0.2;
    speedLimit = 120/3.6;
}

DriverModel::~DriverModel(){}


void DriverModel::updateMemory(double dt,float v,float v0){
    // exponential moving average
	float gamma = exp(-dt / mtau);

    // level of service function
    float vRel = v / v0;

    // integration of alpha-factors
    malphaT = gamma * malphaT + (1.0 - gamma) * (mresignationMaxAlphaT + vRel * (1.0 - mresignationMaxAlphaT));
    malphaV0 = gamma * malphaV0 + (1.0 - gamma) * (mresignationMinAlphaV0 + vRel * (1.0 - mresignationMinAlphaV0));
    malphaA = gamma * malphaA + (1.0 - gamma) * (mresignationMinAlphaA + vRel * (1.0 - mresignationMinAlphaA));
}

bool DriverModel::canInvertWay(LaneObject* ,Lane* ){
	int r = rand()%100;
	if( r > (int)patience*100 ){
		return true;
	}
	return false;
}

float DriverModel::laneChangeAccelerationBalance(Vehicle* me, Lane* newLane ) {
	// is valid lane?
	// with this we can avoid entrance lanes, lanes without access, etc
	Road* nRoad = me->getNextRoad();
    if( nRoad != 0 && !newLane->hasConnectionTo(nRoad) ){
        // never change lane into an entrance lane
        return vns::MIN_FLOAT;
    }


	Vehicle* oldBack = Vehicle::fromObject( me->getPreviousObject() );
	Vehicle* oldFront = Vehicle::fromObject( me->getNextObject() );
	if(oldFront){
		// TODO: so se estiver perto de mim
		if( oldFront->isInProcessOfLaneChange() ){
			return vns::MIN_FLOAT;
		}
	}

    LaneObject* prevNew;
    LaneObject* nextNew;
    newLane->objectsAround(me->getLanePosition(), prevNew, nextNew);

    Vehicle* newFront = Vehicle::fromObject( nextNew );
	if(newFront){
		// TODO: so se estiver perto de mim
		if( newFront->isInProcessOfLaneChange() ){//&& nextNewV->laneChangeDestination() == lane ){
			return vns::MIN_FLOAT;
		}
	    if( newFront->getRearPosition() - me->getFrontPosition() < gapMin ) {
	        return vns::MIN_FLOAT;
	    }
	}

    Vehicle* newBack = Vehicle::fromObject( prevNew );
	if(newBack){
		// TODO: so se estiver perto de mim
		if( newBack->isInProcessOfLaneChange() ){
			return vns::MIN_FLOAT;
		}
	    if( me->getRearPosition() - newBack->getFrontPosition() < gapMin ) {
	        return vns::MIN_FLOAT;
	    }
	}

    float newBackNewAcc = (newBack != 0 ? newBack->getAccelTo( me ) : 0.0);
    if( newBackNewAcc <= -bSafe ) {
        return vns::MIN_FLOAT;
    }
    // check now incentive criterion
    // consider three vehicles: me, oldBack, newBack

    // incentive criterion (always model of BACK vehicle used!!)
    // works also for on-ramp: on-ramp has 1 lane with index 0
    // === LEFT on main road -> strong desired to change =
    // large positive biasRight for lcModel of ramp vehicles

   	float meNewAcc = me->getAccelTo( newFront );
    float meOldAcc = me->getAccelTo( me->getNextObject() );

    float oldBackOldAcc = ( oldBack!=0 ? oldBack->getAccelTo( oldBack->getNextObject() ) : 0.0);
    float newBackOldAcc = ( newBack!=0 ? newBack->getAccelTo( newBack->getNextObject() ) : 0.0);

    float oldBackNewAcc = ( oldBack!=0 && me->getNextObject()!=0 ? oldBack->getAccelTo( me->getNextObject() ) : 0.0);

    // MOBIL trade-off for driver and neighborhood

    float oldBackDiffAcc = oldBackNewAcc - oldBackOldAcc;
    float newBackDiffAcc = newBackNewAcc - newBackOldAcc;
    float meDiffAcc = meNewAcc - meOldAcc;

    // MOBIL's incentive formula
    // change left = -1
    int biasSign = 1;
    if( me->getLane()->getWay() == Forward )
    	biasSign = (newLane->getID()<me->getLane()->getID()) ? 1 : -1;
    else
    	biasSign = (newLane->getID()>me->getLane()->getID()) ? 1 : -1;

    return meDiffAcc + politeness * (oldBackDiffAcc + newBackDiffAcc) - threshold - biasSign * biasRight;
}


bool DriverModel::isSafeToChangeLane(Vehicle* me, Lane* newLane ) {
    LaneObject* prevNew;
    LaneObject* nextNew;
    newLane->objectsAround(me->getLanePosition(), prevNew, nextNew);

	Vehicle* newBack = Vehicle::fromObject( prevNew );
	Vehicle* newFront = Vehicle::fromObject( nextNew );
	if(newFront){
		// check distance to front vehicle
		if( newFront->isInProcessOfLaneChange() && newFront->getNextLane() == me->getLane()){
			return false;
		}
		/*
		if(me->getSpeed()<3.0 && newFront->getSpeed() && me->getFrontPosition()>newLane->getLaneEnd()-30){

		}*/
		if( newFront->getRearPosition() - me->getFrontPosition() < gapMin ){
			return false;
		}
	}

	if(newBack){
		// check distance to vehicle behind
		//if( newBack->isInProcessOfLaneChange() ) return false;
		/*if(me->getFrontPosition()>newLane->getLaneEnd()-30){
			float p = vns::randomValue(0.f,1.f);
			float df = (newLane->getLaneEnd()-me->getFrontPosition())/30.0;
			//Debug << df << "\n";
			if( p>df ){
				return true;
			}
			return false;
		}*/
		if( me->getRearPosition() - newBack->getFrontPosition() < gapMin ){
			return false;
		}
		// DO HERE
		// check acceleration of back vehicle
		/*
		if(me->getFrontPosition()>newLane->getLaneEnd()-30){
			int r = vns::random()%100;
			if( r > (int)patience*100 ){
				return true;
			}
			return false;
		}*/
		float backNewAcc = newBack->getAccelTo(me);
		if( backNewAcc <= -bSafe) {
			return false;
		}
	}
	/*
	if(me->getFrontPosition()>newLane->getLaneEnd()-100){
		int r = vns::randomInt()%100;
		if( r > (int)patience*100 ){
			return true;
		}
		return false;
	}*/
	float meNewAcc = me->getAccelTo(newFront);
    if( meNewAcc >= -bSafe ) {
        return true;
    }
    return false;
}

DriverModel::LaneChangeDecision DriverModel::makeDecisionForEnteringVehicles(Vehicle* me){

	Lane* lane = me->getLane();
	if( lane->getRoad()->getNumberOfLanes()<=2 ) return None;

	Lane* left = lane->getSameWayDrivingLaneOnLeft();
	Lane* right = lane->getSameWayDrivingLaneOnRight();
	// if it is an entry road;
	if(left && left->getNumberOfConnections() == 0){
	    LaneObject* frontEntry;
	    LaneObject* backEntry;
	    left->objectsAround(me->getLanePosition(), backEntry, frontEntry);
	    if( frontEntry == 0 || frontEntry->getObjectType() != Object::VehicleT ){
	    	return None;
	    }
	    float accToFront = me->getAccelTo(frontEntry);
	    if( accToFront < -bSafe ){
	    	// if does not exist or if can change lane (not an entry lane)
	    	if( right == 0 || right->getNumberOfConnections() == 0) return None;
		    LaneObject* fOther;
		    LaneObject* bOther;
		    right->objectsAround(me->getLanePosition(), bOther, fOther);
		    Vehicle* newFront = Vehicle::fromObject(fOther);
		    Vehicle* newBack = Vehicle::fromObject(bOther);
		    if(newFront){
		    	if(newFront->isInProcessOfLaneChange()) return None;
		    	if(newFront->getRearPosition() - me->getFrontPosition() < gapMin ) return None;
		    }
		    if(newBack){
		    	if(newBack->isInProcessOfLaneChange()) return None;
		    	if(me->getRearPosition() - newBack->getFrontPosition() < gapMin ) return None;
		    }
		    float newBackNewAcc = ( newBack != 0 ? newBack->getAccelTo(me) : 0.0 );
		    float meNewAcc = me->getAccelTo(newFront);
		    if( newBackNewAcc <= -bSafe || meNewAcc <= -bSafe ) return None;
		    return OptionalChangeToRight;
	    }
	}
	// if it is an entry road;
	if(right && right->getNumberOfConnections() == 0){
	    LaneObject* frontEntry;
	    LaneObject* backEntry;
	    right->objectsAround(me->getLanePosition(), backEntry, frontEntry);
	    if( frontEntry == 0 || frontEntry->getObjectType() != Object::VehicleT ){
	    	return None;
	    }
	    float accToFront = me->getAccelTo(frontEntry);
	    if( accToFront < -bSafe ){
	    	// if does not exist or if can change lane (not an entry lane)
	    	if( left == 0 || left->getNumberOfConnections() == 0) return None;
		    LaneObject* fOther;
		    LaneObject* bOther;
		    left->objectsAround(me->getLanePosition(), bOther, fOther);
		    Vehicle* newFront = Vehicle::fromObject(fOther);
		    Vehicle* newBack = Vehicle::fromObject(bOther);
		    if(newFront){
		    	if(newFront->isInProcessOfLaneChange()) return None;
		    	if(newFront->getRearPosition() - me->getFrontPosition() < gapMin ) return None;
		    }
		    if(newBack){
		    	if(newBack->isInProcessOfLaneChange()) return None;
		    	if(me->getRearPosition() - newBack->getFrontPosition() < gapMin ) return None;
		    }
		    float newBackNewAcc = ( newBack != 0 ? newBack->getAccelTo(me) : 0.0 );
		    float meNewAcc = me->getAccelTo(newFront);
		    if( newBackNewAcc <= -bSafe || meNewAcc <= -bSafe ) return None;
		    return OptionalChangeToLeft;
	    }
	}
	return None;
}

DriverModel::LaneChangeDecision DriverModel::checkForMandatoryLaneChanges(Vehicle* me) {
	if( me->getVehicleType() == vns::TRAIN ) return DriverModel::None;

	Lane* lane = me->getLane();

	/* Stops */
	const vns::PathStop* stop = me->getRoadStop();
	if( stop ){
		switch(stop->getType()){
		case vns::PathStop::RightTaxiStand:
		case vns::PathStop::RightParking:{
			Lane* r = lane->getDrivingLaneOnRight();
			if( r!=0 ){
				if( r->getWay()==lane->getWay() && r->allowsVehicle(me->getVehicleType()) ) return DriverModel::MandatoryChangeToRight;
			}else{
				Lane* rpark = lane->getParkingLaneOnRight();
				if( rpark ) return DriverModel::MandatoryChangeToRight;
			}
			return DriverModel::MandatoryToStayInLane;
		}
		case vns::PathStop::LeftTaxiStand:
		case vns::PathStop::LeftParking:{
			Lane* l = lane->getDrivingLaneOnLeft();
			if( l!=0 ){
				if( l->getWay()==lane->getWay() && l->allowsVehicle(me->getVehicleType()) ) return DriverModel::MandatoryChangeToLeft;
			}else{
				Lane* lpark = lane->getParkingLaneOnLeft();
				if( lpark ) return DriverModel::MandatoryChangeToLeft;
			}
			return DriverModel::MandatoryToStayInLane;
		}
		case vns::PathStop::LeftMostLane:{
			Lane* l = lane->getSameWayDrivingLaneOnLeft();
			if( l && l->allowsVehicle(me->getVehicleType()) ) return DriverModel::MandatoryChangeToLeft;
			return DriverModel::MandatoryToStayInLane;
		}
		case vns::PathStop::RightMostLane:{
			Lane* r = lane->getSameWayDrivingLaneOnRight();
			if( r && r->allowsVehicle(me->getVehicleType()) ) return DriverModel::MandatoryChangeToRight;
			return DriverModel::MandatoryToStayInLane;
		}
		}
	}

	/* Change Way */
	/*if( me->requiresChangeWay() ){
		// European Way
		Lane* left = lane->getDrivingLaneOnLeft();
		VNS_ASSERT( left ,"Change Way","Road does not have any reverse lane.");
		if(left->getWay() == lane->getWay()){
			return DriverModel::MandatoryChangeToLeft;
		}
		return DriverModel::None;
	}*/

	Road* nRoad = me->getNextRoad();
	if(nRoad == 0){
		if(lane->hasFlag(vns::Lane::ParkingLane)) return DriverModel::MandatoryToStayInLane;
		vns::Junction* endJunction = lane->getEndJunction();
		if(endJunction){
			vns::Way meway = lane->getWay();
			if(lane->getRoad()->hasRightParking(meway)){
				return DriverModel::MandatoryChangeToRight;
			}else{
				if(lane->getRoad()->hasLeftParking(meway)){
					return DriverModel::MandatoryChangeToLeft;
				}
			}
		}
		return DriverModel::None;
	}

	if( lane->hasConnectionTo(nRoad) && me->getFrontPosition()>lane->getLaneEnd()-OPTIONAL_LANECHANGE_MIN_DISTANCE ){
		return DriverModel::MandatoryToStayInLane;
	}


	// if road does not have connections - entry lane, parking lanes
	if( lane->getNumberOfConnections()==0 || lane->hasConnectionTo(nRoad) == false ){
		// Testar a rua da direita
		Lane* right = lane->getSameWayDrivingLaneOnRight();
		while( right ) {
			if( right->hasConnectionTo(nRoad) ){
				return DriverModel::MandatoryChangeToRight;
			}
			right = right->getSameWayDrivingLaneOnRight();
		}
		// Testar a rua da esquerda
		Lane* left = lane->getSameWayDrivingLaneOnLeft();
		while( left ) {
			if( left->hasConnectionTo(nRoad) ){
				return DriverModel::MandatoryChangeToLeft;
			}
			left = left->getSameWayDrivingLaneOnLeft();
		}
	}
	// consider mandatory lane-change to exit on next road segment ahead
	// consider change to other lane

	const Lane* nextLane = me->getNextLane();
	if(nextLane){
		if(nextLane->getLength()>50) return DriverModel::None;
		const vns::Path::Iterator it = me->getPathIterator();
		vns::PathNode* nd = it.getNext();
		if(nd->hasNext()){
			Road* nnRoad = nd->getNext()->getRoad();
			if(nextLane->hasConnectionTo(nnRoad)) return DriverModel::None;
			Lane* right = lane->getSameWayDrivingLaneOnRight();
			while( right ) {
				Lane* nright = right->getNextLane(nRoad);
				if( nright && nright->hasConnectionTo(nnRoad) ){
					return DriverModel::MandatoryChangeToRight;
				}
				right = right->getSameWayDrivingLaneOnRight();
			}
			// Testar a rua da esquerda
			Lane* left = lane->getSameWayDrivingLaneOnLeft();
			while( left ) {
				Lane* nleft = left->getNextLane(nRoad);
				if( nleft && nleft->hasConnectionTo(nnRoad) ){
					return DriverModel::MandatoryChangeToLeft;
				}
				left = left->getSameWayDrivingLaneOnLeft();
			}
		}
	}

	return DriverModel::None;
}

DriverModel::LaneChangeDecision DriverModel::checkForOptionalLaneChanges(Vehicle* me){

	Lane* lane = me->getLane();
    float accToLeft = vns::MIN_FLOAT;
    float accToRight = vns::MIN_FLOAT;
    // consider lane-changing to right-hand side lane (decreasing lane index)

	if( lane->hasFlags(Lane::ChangeLaneToRight) ){
		Lane* right = lane->getSameWayDrivingLaneOnRight();
		if( right ){
			//possibleLanes.contains(right->getID()) ){
			// only consider lane changes into traffic lanes
			// se estiver á distancia ou se for uma das lanes possiveis
			accToRight = laneChangeAccelerationBalance(me,right);
		}
	}

	if( lane->hasFlags(Lane::ChangeLaneToLeft) ){
		Lane* left = lane->getSameWayDrivingLaneOnLeft();
		if( left ) {
			// && possibleLanes.contains(left->getID())
			// only consider lane changes into traffic lanes
			// se estiver á distancia ou se for uma das lanes possiveis
			accToLeft = laneChangeAccelerationBalance(me,left);
		}
	}

    if( (accToRight > 0) || (accToLeft > 0) ) {
        if( accToRight > accToLeft ){
        	return OptionalChangeToRight;
        }
        return OptionalChangeToLeft;
    }
	return None;
}


DriverModel::LaneChangeDecision DriverModel::makeLaneChangeDecision( Vehicle* me, Simulator* ){
	LaneChangeDecision decision;

	// check for mandatory lane changes to reach exit lane

	//decision = checkForMandatoryLaneChanges(me);
	//if( decision != None ) return decision;

	// check for mandatory lane changes to reach entrance lane


	// check for mandatory lane changes for entering vehicle

	decision = makeDecisionForEnteringVehicles(me);
	if(decision != None) return decision;

	// check other possible lane changes

	return checkForOptionalLaneChanges(me);
}


DriverModel* DriverModel::newDriverModel( const DriverInfo* info ){
	switch(info->model){
	case DriverModel::IDM: return new vns::IDM(info);
	case DriverModel::EIDM: return new vns::EIDM(info);
	case DriverModel::GIPPS: return new vns::Gipps(info);
	case DriverModel::KRAUSS: return new vns::Krauss(info);
	case DriverModel::NEWELL: return new vns::Newell(info);
	case DriverModel::OVM: return new vns::OVM(info);
	case DriverModel::FVDM: return new vns::FVDM(info);
	case DriverModel::KKW: return new vns::KKW(info);
	case DriverModel::NAGEL: return new vns::Nagel(info);
	case DriverModel::PIPES: return new vns::Pipes(info);
	default: return 0;
	}
	return 0;
}

const char* DriverModel::getDriverModelName(vns::DriverModel::Type type){
	switch(type){
	case DriverModel::IDM: return "idm";
	case DriverModel::GIPPS: return "gipps";
	case DriverModel::KKW: return "kkw";
	case DriverModel::KRAUSS: return "krauss";
	case DriverModel::NEWELL: return "newell";
	case DriverModel::OVM: return "ovm";
	case DriverModel::EIDM: return "eidm";
	case DriverModel::NAGEL: return "nagel";
	case DriverModel::FVDM: return "fvdm";
	case DriverModel::PIPES: return "pipes";
	default: return "invalid";
	}
	return "invalid";
}

DriverModel::Type DriverModel::getDriverModel(const char* modelName){
	if( strcmp(modelName,"idm") == 0 ) {
		return DriverModel::IDM;
	} else if( strcmp(modelName,"gipps") == 0 ) {
		return DriverModel::GIPPS;
	} else if( strcmp(modelName,"kkw") == 0 ) {
		return DriverModel::KKW;
	} else if( strcmp(modelName,"krauss") == 0 ) {
		return DriverModel::KRAUSS;
	} else if( strcmp(modelName,"newell") == 0 ) {
		return DriverModel::NEWELL;
	} else if( strcmp(modelName,"ovm") == 0 ) {
		return DriverModel::OVM;
	} else if( strcmp(modelName,"eidm") == 0 ) {
		return DriverModel::EIDM;
	} else if( strcmp(modelName,"nagel") == 0 ) {
		return DriverModel::NAGEL;
	}else if( strcmp(modelName,"fvdm") == 0 ) {
		return DriverModel::FVDM;
	}else if( strcmp(modelName,"pipes") == 0 ) {
		return DriverModel::PIPES;
	}
	return DriverModel::INVALID;
}

}
