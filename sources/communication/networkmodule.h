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


#ifndef VNS_NETWORKMODULE_H_
#define VNS_NETWORKMODULE_H_

#include "core.h"

namespace vns {

class Simulator;
class Vehicle;

class NetworkModule {

public:
	NetworkModule(){};
	virtual ~NetworkModule(){};

public:
	virtual void onSimulationStart(const Simulator*){};
	virtual void onSimulationStop(const Simulator*){};
	virtual void onSimulationStep(const Simulator*){};
	virtual void onVehicleCreated(const Simulator*,Vehicle*){};
	virtual void onVehicleRemoved(const Simulator*,Vehicle*){};

	friend class Simulator;
};

}

#endif /* VNS_NETWORKMODULE_H_ */
