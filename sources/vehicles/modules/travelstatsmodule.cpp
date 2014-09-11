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

#include <stdio.h>
#include "travelstatsmodule.h"
#include "simulator.h"

namespace vns {

TravelStatsModule::TravelStatsModule() {
	// TODO Auto-generated constructor stub

}

TravelStatsModule::~TravelStatsModule() {
	// TODO Auto-generated destructor stub
}

void TravelStatsModule::onInstall(const Simulator*,const Vehicle* vehicle){
	distanceTravelled = 0.0;
	timeTravelled = 0.0;
	lastPos = vehicle->getPosition();
}

void TravelStatsModule::onSimulationStep(const Simulator*,const Vehicle* vehicle,double dt){
	vns::Vec p = vehicle->getPosition();
	distanceTravelled += lastPos.distanceTo(p);
	lastPos = p;
	timeTravelled += dt;
}

void TravelStatsModule::onUninstall(const Simulator*,const Vehicle* vehicle){
	printf("TRAVEL_STATS: %d,%lf,%lf\n",vehicle->getID(),timeTravelled,distanceTravelled);
}

}
