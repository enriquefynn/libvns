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

#include "modulesfactory.h"
#include "vehiclemodule.h"
#include "vehicle.h"
#include "simulator.h"

namespace vns {

ModulesFactory::ModulesFactory() {
	simulator = 0;
}

ModulesFactory::~ModulesFactory() {

}

void ModulesFactory::setSimulator(Simulator* sim){
	simulator = sim;
}

void ModulesFactory::install(Vehicle* vehicle,VehicleModule* module){
	vehicle->installModule(simulator,module);
}


}
