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

#ifndef VNS_TRAVELSTATSMODULE_H_
#define VNS_TRAVELSTATSMODULE_H_

#include "vehiclemodule.h"
#include "vec.h"

namespace vns {

class Vec;

class TravelStatsModule : public VehicleModule {
public:
	TravelStatsModule();
	virtual ~TravelStatsModule();
	void onInstall(const Simulator* simulator,const Vehicle* vehicle);
	void onSimulationStep(const Simulator* simulator,const Vehicle* vehicle,double dt);
	void onUninstall(const Simulator* simulator,const Vehicle* vehicle);

private:
	vns::Vec lastPos;
	double distanceTravelled;
	double timeTravelled;
};

}

#endif
