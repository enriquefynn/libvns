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

#ifndef VNS_OBSERVER_H_
#define VNS_OBSERVER_H_

namespace vns {

class Simulator;
class Vehicle;
class LaneObject;

class Observer {
public:
	virtual ~Observer(){};
	virtual inline void onVehicleCreated(const Simulator* , Vehicle* ){};
	virtual inline void onVehicleRemoved(const Simulator* , Vehicle* ){};
	virtual inline void onVehicleInjected(const Simulator* , Vehicle* ){};
	virtual inline void onRouteChanged(const Simulator* , Vehicle* ){};
	virtual inline void onEnterRouteStop(const Simulator* , Vehicle* ){};
	virtual inline void onLeaveRouteStop(const Simulator* , Vehicle* ){};
	virtual inline void onEnterTaxiStand(const Simulator* , Vehicle* ){};
	virtual inline void onLeaveTaxiStand(const Simulator* , Vehicle* ){};
	virtual inline void onEnterParkingLane(const Simulator* , Vehicle* ){};
	virtual inline void onLeaveParkingLane(const Simulator* , Vehicle* ){};
	virtual inline void onLaneEnter(const Simulator* , Vehicle* ){};
	virtual inline void onRoadEnter(const Simulator* , Vehicle* ){};
	virtual inline void onSimulationStart(const Simulator* ){};
	virtual inline void onSimulationStop(const Simulator* ){};
	virtual inline void onSimulationStep(const Simulator* ){};
	virtual inline void onLaneObjectSimulationStep(const Simulator* ,LaneObject*){};
};

}

#endif /* VNS_OBSERVER_H_ */
