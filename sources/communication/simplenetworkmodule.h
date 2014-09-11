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

#ifndef VNS_SIMPLENETWORKMODULE_H_
#define VNS_SIMPLENETWORKMODULE_H_

#include "core.h"
#include "lock.h"
#include "networkmodule.h"
#include "hashtable.h"

namespace vns {

class Vehicle;
class EventScheduler;


class SimpleNetwork {
public:
	SimpleNetwork();
	virtual ~SimpleNetwork();
	void setCommunicationRange(double range);
	double getCommunicationRange();

protected:
	inline double getSimulationTime(){ return time; };
	inline vns::EventScheduler* getScheduler(){ return scheduler; };

	virtual void send(Vehicle* sender,void* data);
	virtual void onInitialize(const Simulator*,Vehicle*){};
	virtual void onDataReceived(Vehicle*,Vehicle*, void*){};
	virtual void onSendFinish(void*){};

private:

	void onSimulationStart(const Simulator* simulator);
	void onSimulationStep(const Simulator* simulator);
	void onSimulationStop(const Simulator* simulator);
	void onVehicleCreated(const Simulator* simulator,Vehicle* vehicle);
	void onVehicleRemoved(const Simulator* simulator,Vehicle* vehicle);

	vns::HashTable<uint64,Vehicle*>* onBoardUnits;
	vns::EventScheduler* scheduler;
	double communicationRange;
	double time;
	friend class SimpleNetworkModule;
};

class SimpleNetworkModule : public NetworkModule {

public:

	SimpleNetworkModule();
	virtual ~SimpleNetworkModule();

	void onSimulationStart(const Simulator* simulator);
	void onSimulationStep(const Simulator* simulator);
	void onSimulationStop(const Simulator* simulator);
	void onVehicleCreated(const Simulator* simulator,Vehicle* vehicle);
	void onVehicleRemoved(const Simulator* simulator,Vehicle* vehicle);

	void setNetwork(SimpleNetwork* network);

private:

	vns::Lock lock;
	SimpleNetwork* network;
};

}

#endif /* VNS_SIMPLENETWORKMODULE_H_ */
