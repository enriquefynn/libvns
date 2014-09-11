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

#include <omp.h>

#include "parallelsimulator.h"

namespace vns {

ParallelSimulator::ParallelSimulator() : Simulator() {

}

ParallelSimulator::ParallelSimulator(vns::RoadNetwork* network) : Simulator(network) {

}

ParallelSimulator::~ParallelSimulator(){

}

void ParallelSimulator::onSimulationStart(){
    int32 nr = network->getNumberOfRoads();
	#pragma omp parallel for
    for(int32 i=0;i<nr;i++){
    	vns::Road* road = network->getRoad(i);
    	vns::setRandomSeed(&(road->seed));
    	road->simulationStart(this);
    }

    initializeTrafficLights();

	trafficGenerationModel->onSimulationStart(this);
}

void ParallelSimulator::onSimulationStep(){
	vns::setRandomSeed(&seed);
	//double tStart = omp_get_wtime();

    scheduler->invokeEvents(time,DT);

    //double tEnd = omp_get_wtime();
    //printf("Time taken: %lfs\n", tEnd - tStart);

    trafficGenerationModel->onSimulationStep( this );

    int32 nr = network->getNumberOfRoads();
    uint32 sum = 0;
    uint32 sumQueue= 0;
	#pragma omp parallel for reduction(+:sum,sumQueue)
    for(int32 i=0;i<nr;i++){
    	vns::Road* road = network->getRoad(i);
    	vns::setRandomSeed(&(road->seed));
    	sum += road->simulationStep(this);
    	if(road->getNumberOfCarsInQueue()>1)
    		sumQueue += road->getNumberOfCarsInQueue();
    }
    nVehiclesInQueue = sumQueue;
    nVehicles = sum;

	#pragma omp parallel for
    for(int32 i=0;i<nr;i++){
    	vns::Road* road = network->getRoad(i);
    	vns::setRandomSeed(&(road->seed));
    	road->updateStep(this);
    }
    vns::setRandomSeed(&seed);
}


}
