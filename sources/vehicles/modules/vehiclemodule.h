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

#ifndef VNS_VEHICLEMODULE_H_
#define VNS_VEHICLEMODULE_H_

namespace vns {

class Simulator;
class Vehicle;

class VehicleModule {
public:
	virtual ~VehicleModule(){};
	virtual void onInstall(const Simulator* simulator,const Vehicle* vehicle) = 0;
	virtual void onSimulationStep(const Simulator* simulator,const Vehicle* vehicle,double dt) = 0;
	virtual void onUninstall(const Simulator* simulator,const Vehicle* vehicle) = 0;
};

}
#endif
