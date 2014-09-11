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

#include "simulator.h"
#include "drivers.h"
#include "calendarscheduler.h"
#include <ctime>

namespace vns {

double Simulator::DT = 0.033;

Simulator::Simulator() {
	network = 0;
	randomSeed = 0;

	flags = 0;
	flags |= Simulator::SmoothLaneChanges;
	flags |= Simulator::TrafficLights;
	flags |= Simulator::RouteChanges;

    vehicleFactory = 0;
    modulesFactory = 0;
    networkModule = 0;
	trafficGenerationModel = 0;

    astarfunction = new TravelTimeFunction();
    astar = new AStarHandler( astarfunction );

    stepTime = DT;
    scheduler = new CalendarScheduler(DT,100*int(1.0/DT));
    running = false;
    nVehicles = 0;
    nVehiclesInQueue = 0;
    time = 0;
    finishTime = 0.f;
    dateTime = 0;
}

Simulator::Simulator(vns::RoadNetwork* net){
	network = net;
	randomSeed = 0;

	flags = 0;
	flags |= Simulator::SmoothLaneChanges;
	flags |= Simulator::TrafficLights;
	flags |= Simulator::RouteChanges;

    vehicleFactory = 0;
    modulesFactory = 0;
    networkModule = 0;
	trafficGenerationModel = 0;

    astarfunction = new TravelTimeFunction();
    astar = new AStarHandler( astarfunction );

    stepTime = DT;
    scheduler = new CalendarScheduler(DT,100*int(1.0/DT));
    running = false;
    nVehicles = 0;
    nVehiclesInQueue = 0;
    time = 0;
    finishTime = 0.f;
    dateTime = 0;
}


Simulator::~Simulator(){
	observers.clear();
	//delete vehicleFactory;
	//delete trafficGenerationModel;
	delete astarfunction;
	delete astar;
	delete scheduler;
}

Path* Simulator::calculatePath( Road* from, Way wayFrom, Road* to, Way wayTo) const{
	return astar->calculatePath(network,from,wayFrom,to,wayTo);
}

void Simulator::setRoadNetwork(RoadNetwork* net){
	VNS_ASSERT( net >= 0 ,"Simulator::setRoadNetwork","Invalid RoadNetwork!");
	network = net;
}

void Simulator::setFinishTime(double endTime){
	VNS_ASSERT( endTime >= 0 ,"Simulator::setFinishTime","finishTime >= 0");
	finishTime = endTime;
}

void Simulator::setStepTime(double st){
	VNS_ASSERT( st >= DT ,"Simulator::setStepTime","stepTime should be >= 0.033");
	stepTime = st;
}

void Simulator::addObserver(Observer* obs){
	VNS_ASSERT( obs != 0 ,"Simulator::addObserver","Null instance!");
	VNS_ASSERT( !observers.contains(obs) ,"Simulator::addObserver","This observer is already registered!");
	observers.append(obs);
}

void Simulator::setNetworkModule(NetworkModule* netmodule){
	VNS_ASSERT( netmodule != 0 ,"Simulator::setNetworkModule","Null instance!");
	networkModule = netmodule;
}

void Simulator::setVehicleFactory(VehicleFactory* factory){
	VNS_ASSERT( factory != 0 ,"Simulator::setVehicleFactory","Null instance!");
	vehicleFactory = factory;
}

void Simulator::setModulesFactory(ModulesFactory* factory){
	if(modulesFactory){
		modulesFactory->setSimulator(0);
	}
	modulesFactory = factory;
	if(modulesFactory) modulesFactory->setSimulator(this);
}

void Simulator::setTrafficGenerationModel(TrafficGenerationModel* model){
	VNS_ASSERT( model != 0 ,"Simulator::setTrafficGenerationModel","Null instance!");
	trafficGenerationModel = model;
}

void Simulator::simulationStep() {


}

void Simulator::addVehicle(Vehicle *vehicle, Lane* lane, double position){
	addObjectToLane( vehicle , lane, position );
	if(vehicle->getVehicleType()==vns::TRAIN){
		vns::Train* train = dynamic_cast<vns::Train*>(vehicle);
		double pos = position-(train->getLength()*0.5)-0.3;
		for(int i=0;i<train->getNumberOfCars();i++){
			vns::TrainCar* car = train->getCar(i);
			pos -= car->getLength()*0.5+0.1;
			addObjectToLane( car , lane, pos );
			pos -= (car->getLength()*0.5);
		}
	}
	//vehicle->getLane()->getPositionPoint(obj->pos,obj->dir,perp,obj->lanePos);
	vehicle->initialize(this);

    //vehicle->state = vns::Vehicle::Initialising;
    //vehicle->stateFunction = &vns::Vehicle::initialize;

    if(modulesFactory) modulesFactory->installModules(vehicle);
    if(networkModule) networkModule->onVehicleCreated(this,vehicle);
}

void Simulator::addObjectToLane(LaneObject *obj, Lane* lane, double position) {
	if(obj->lane != 0){
		obj->lane->removeObject(obj);
	}
    lane->addObject(obj, position);
}

void Simulator::removeObjectFromLane(LaneObject *obj){
	if(obj->lane != 0){
		obj->lane->removeObject(obj);
	}
}

void Simulator::deleteVehicle(Vehicle *vehicle){
    for(vns::List<Observer*>::Iterator obs=observers.begin();obs!=observers.end();obs++){(*obs)->onVehicleRemoved(this,vehicle);}
    vehicle->uninstallAllModules(this);
    if( networkModule ) networkModule->onVehicleRemoved(this,vehicle);
	if(vehicle->lane != 0){
		vehicle->lane->removeObject(vehicle);
	}
	delete vehicle;
}

void Simulator::run(){
	if( !running ){
		start();
	}
	while(finishTime==0 || time < finishTime ){
		step();
	}
	stop();
}

void Simulator::start() {
    if(running) {
    	return;
    }
    VNS_ASSERT( trafficGenerationModel != 0 ,"Simulator::setTrafficGenerationModel","Null Instance!");
    VNS_ASSERT( network != 0 ,"Simulator::setRoadNetwork","Null Instance!");
    VNS_ASSERT( vehicleFactory != 0 ,"Simulator::setVehicleFactory","Null instance!");

    if( randomSeed == 0 ){
    	seed = ::time(0);
    }else{
    	seed = randomSeed;
    }

    for(int i=0;i<network->getNumberOfRoads();i++){
    	vns::Road* road = network->getRoad(i);
    	road->seed = seed;
    }

	time = 0;
	simstep = 0;
    running = true;

    for(vns::List<Observer*>::Iterator obs=observers.begin();obs!=observers.end();obs++){(*obs)->onSimulationStart(this);}

    //Initialize Network Module
	if( networkModule ) networkModule->onSimulationStart(this);

    onSimulationStart();

}

void Simulator::step(){
	if( running ){
		//clock_t tStart = clock();
		double stopStep = time + stepTime;
		while( time < stopStep ){
		    simstep++;
		    time += DT;
		    onSimulationStep();
		}
	    if(networkModule){
	    	networkModule->onSimulationStep(this);
	    }
		for(vns::List<Observer*>::Iterator obs=observers.begin();obs!=observers.end();obs++){ (*obs)->onSimulationStep(this); }
		//stepExcutionTime = (double)(clock() - tStart)/CLOCKS_PER_SEC;
	}
}


void Simulator::stop() {
    if(!running){
    	return;
    }
    running = false;
    for(vns::List<Observer*>::Iterator obs=observers.begin();obs!=observers.end();obs++){(*obs)->onSimulationStop(this);}
    // Finalize Network Module
    if( networkModule ) networkModule->onSimulationStop(this);

    onSimulationStop();

	time = 0;
	simstep = 0;
	nVehiclesInQueue = 0;
}

void Simulator::onSimulationStart(){
    int32 nr = network->getNumberOfRoads();
    for(int32 i=0;i<nr;i++){
    	vns::Road* road = network->getRoad(i);
    	vns::setRandomSeed(&(road->seed));
    	road->simulationStart(this);
    }
    initializeTrafficLights();

	trafficGenerationModel->onSimulationStart(this);
}

void Simulator::initializeTrafficLights(){
    vns::setRandomSeed(&seed);
	if( hasFlag(Simulator::TrafficLights) ){
		int32 ntl = network->getNumberOfTrafficLights();
		for(int32 i=0 ; i < ntl ; i++){
			TrafficLightController* controller = network->getTrafficLightController(i);
			controller->doInitialize( this );
		}
	}
}

void Simulator::onSimulationStep(){
	vns::setRandomSeed(&seed);
	//double tStart = omp_get_wtime();

    scheduler->invokeEvents(time,DT);

    trafficGenerationModel->onSimulationStep( this );
    //double tEnd = omp_get_wtime();
    //printf("Time taken: %lfs\n", tEnd - tStart);

    nVehiclesInQueue = 0;
    nVehicles = 0;
    int32 nr = network->getNumberOfRoads();
    for(int32 i=0;i<nr;i++){
    	vns::Road* road = network->getRoad(i);
    	vns::setRandomSeed(&(road->seed));
    	nVehicles+=road->simulationStep(this);
    	if(road->getNumberOfCarsInQueue()>1)
    		nVehiclesInQueue += road->getNumberOfCarsInQueue();
    }

    for(int32 i=0;i<nr;i++){
    	vns::Road* road = network->getRoad(i);
    	vns::setRandomSeed(&(road->seed));
    	road->updateStep(this);
    }
    vns::setRandomSeed(&seed);
    vns::List<Vehicle*>::Iterator it = externalVehicles.begin();
    while(it!=externalVehicles.end()){
    	Vehicle* vehicle = *it;
    	vehicle->simulationStep(this);
    	it++;
    }
}

void Simulator::onSimulationStop(){

    //Finalize Traffic Generation Model
    trafficGenerationModel->onSimulationFinished( this );

    int32 nr = network->getNumberOfRoads();
    for(int32 i=0;i<nr;i++){
    	vns::Road* road = network->getRoad(i);
    	//road->scheduler->clearEvents();
    	road->clearQueuedVehicles();
    	int8 nl = road->getNumberOfLanes();
    	for(int8 l=0;l<nl;l++){
    		vns::Lane* lane = road->getLane(l);
    		lane->exitVehicle = 0;
            LaneObject* obj = lane->getLastObject();
            while( obj ){
				Vehicle* vobj = Vehicle::fromObject(obj);
				obj = obj->getPreviousObject();
				if(vobj){
					vobj->lane->removeObject(vobj);
					delete vobj;
				}
            }
    	}
		if(road->leftParkingLane){
			road->leftParkingLane->exitVehicle = 0;
			LaneObject* obj = road->leftParkingLane->getLastObject();
			while( obj ){
				Vehicle* vobj = Vehicle::fromObject(obj);
				obj = obj->getPreviousObject();
				if(vobj){
					vobj->lane->removeObject(vobj);
					delete vobj;
				}
			}
		}
		if(road->rightParkingLane){
			road->rightParkingLane->exitVehicle = 0;
			LaneObject* obj = road->rightParkingLane->getLastObject();
			while( obj ){
				Vehicle* vobj = Vehicle::fromObject(obj);
				obj = obj->getPreviousObject();
				if(vobj){
					vobj->lane->removeObject(vobj);
					delete vobj;
				}
			}
		}
    }
}

Vehicle* Simulator::createVehicle(VehicleType vehicleType, const DriverInfo* driver, const Path::Iterator& it){
	const Road* rd = it.getRoad();
    Road* road = network->getRoad(rd->getID());
    Way way = it.getWay();
    if( road->hasRoomInQueue(way) ){
    	uint64 vid = ((uint64)road->getID() << 32) | (uint64)road->nextVehicleID;
    	Vehicle* vehicle = vehicleFactory->newVehicle(vid,vehicleType,driver,it);
    	if( vehicle ){
    		road->nextVehicleID++;
        	road->addVehicleToQueue(way,vehicle);
    	    for(vns::List<Observer*>::Iterator obs=observers.begin();obs!=observers.end();obs++){
    		    (*obs)->onVehicleCreated(this,vehicle);
    	    }
    	    return vehicle;
    	}
    }
	return 0;
}


void Simulator::addExternalVehicle(Vehicle* vehicle){
	externalVehicles.append(vehicle);
	vehicle->initialize(this);
	if(networkModule) networkModule->onVehicleCreated(this,vehicle);
    for(vns::List<Observer*>::Iterator obs=observers.begin();obs!=observers.end();obs++){
	    //if(modulesFactory) modulesFactory->installModules(vehicle);
	    (*obs)->onVehicleCreated(this,vehicle);
	    (*obs)->onVehicleInjected(this,vehicle);
    }
}

const List<Vehicle*>& Simulator::getExternalVehicles() const{
	return externalVehicles;
}

}
