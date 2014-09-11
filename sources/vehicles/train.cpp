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

#include "train.h"
#include "simulator.h"
#include "traincarmodel.h"

namespace vns {

TrainCar::TrainCar(vns::Train* train)
	: Vehicle(0,vns::TRAIN,train->getVehicleModel(),train->getPathIterator(),train->getWidth(),train->getLength(),new vns::TrainCarModel()){
	leader = train;

	nextThinkTime = 0;
    nextLane = 0;
    state = Vehicle::MovingInLane;
    stateFunction = &TrainCar::movingInLane;
}

TrainCar::~TrainCar() {

}

void TrainCar::updateLanePosition(Simulator*){
	accel = leader->getAccel();
	speed = leader->getSpeed();
	double newPos = lanePos + vns::Simulator::DT*(speed + 0.5*accel*vns::Simulator::DT);
	if(usingTrajectory) {
	        lanePos = newPos;
	        double t = 1.0-((trajEnd-lanePos)/trajectory.length());
	        if(t<0.f) t = 0.f;
	        if(t>1.f) t = 1.f;
	        trajectory.interpolate(t, pos, dir);
	        dir.normalize();
	        if(lanePos>trajEnd){
	        	pos = pos+dir*(lanePos-trajEnd);
	        }
	} else {
    	Vec perp;
    	lanePos = newPos;
    	lane->getPositionPoint(pos,dir,perp,lanePos);
		//lane->updateObjectPosition(this,newPos);
	}
}

void TrainCar::initialize(Simulator*){
	nextThinkTime = 0;
    nextLane = 0;
    if( path.hasNext() ) {
    	PathNode* n = path.getNext();
    	Road* nRoad = n->getRoad();
    	if(nRoad) nextLane = lane->getNextLane(nRoad);
    }
    state = Vehicle::MovingInLane;
    stateFunction = &TrainCar::movingInLane;
}


void TrainCar::movingInLane(Simulator* sim){
	if (nextLane) {
		if( getFrontPosition() >= lane->getLaneEnd() ) {
			crossToRoad(sim,nextLane);
		}
	}else{
		if( lanePos >= lane->getLaneEnd() ){
			if(path.hasNext()){
				VNS_Debug << "OUT----" << getID()<<"\n";
			}
			state = Removed;
		}
	}
}

void TrainCar::crossToRoad(Simulator* sim,Lane* newLane){
	trajectory.create(getPosition(),getDirection(),newLane->getFirstPoint(),newLane->getFirstPointDirection());
	usingTrajectory = true;
	trajEnd = newLane->getLaneStart();
	prevLane = lane;

	sim->addObjectToLane(this, newLane, newLane->getLaneStart()-trajectory.length());

	path.next();

    nextLane = 0;
    if( path.hasNext() ) {
    	PathNode* n = path.getNext();
    	Road* nRoad = n->getRoad();
    	if(nRoad) nextLane = lane->getNextLane(nRoad);
    }
	state = Vehicle::MovingInJunction;
	stateFunction = &TrainCar::movingInJunction;
}

void TrainCar::movingInJunction(Simulator*) {
    if( getLanePosition() >= getLane()->getLaneStart() ) {
        usingTrajectory = false;
        state = Vehicle::MovingInLane;
        stateFunction = &TrainCar::movingInLane;
    }
}

void TrainCar::simulationStep( Simulator* ) {

}

void TrainCar::updateStep( Simulator* sim ) {
	if(state!=Removed){
		updateLanePosition( sim );
		(this->*stateFunction)( sim );
	}
}

TrainCar* TrainCar::fromVehicle(Vehicle* vehicle){
	return dynamic_cast<TrainCar*>(vehicle);
}


Train::Train(uint64 id,int nCars, int32 model,const Path::Iterator& path,float width, float length,DriverModel* driver) : Vehicle(id,vns::TRAIN,model,path,width,length,driver){
	cars.resize(nCars);
	for(int i=0;i<cars.size();i++){
		cars[i] = new TrainCar(this);
	}
}

Train::~Train() {

}

void Train::simulationStep( Simulator* sim ) {
	Vehicle::simulationStep(sim);
}

void Train::updateStep(Simulator* sim){
	Vehicle::updateStep(sim);
	if( state == Removed ){
		for(int i=0;i<cars.size();i++){
			cars.at(i)->state = Removed;
		}
	}
}

void Train::initialize( Simulator* simulator ) {
	Vehicle::initialize( simulator );
	for(int i=0;i<cars.size();i++){
		cars[i]->initialize(simulator);
	}
}

Train* Train::fromVehicle(Vehicle* vehicle){
	return dynamic_cast<Train*>(vehicle);
}

}
