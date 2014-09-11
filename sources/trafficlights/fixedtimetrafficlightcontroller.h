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

#ifndef VNS_FIXEDTIMETRAFFICLIGHTCONTROLLER_H_
#define VNS_FIXEDTIMETRAFFICLIGHTCONTROLLER_H_

#include "trafficlightcontroller.h"
#include "vector.h"

namespace vns {

class Lane;

class FixedTimeTrafficLightController : public TrafficLightController {

private:
	Vector<Lane*> lanes;
	int8 currentState;
	Vector< TrafficLightState* > states;
	double timeOffset;

public:

	inline void setTimeOffset(double offset){ timeOffset = offset; };
	inline double getTimeOffset() const { return timeOffset; };
	inline int32 getNumberOfStates() const { return states.size(); };

	int32 addState(float duration, const Vector<Light>& colors);
	void removeState(int32 i);
	void clearStates();
	TrafficLightState* getState(int32 i) { return states.at(i); };
	const TrafficLightState* getState(int32 i) const { return states.at(i); };

    int32 getNumberOfLights() const { return lanes.size(); };
    Lane* getLane(int32 lightID) { return lanes.at(lightID); };
    const Lane* getLane(int32 lightID) const { return lanes.at(lightID); };
    bool installLight(Lane* lane);
    bool hasLightInstalled(Lane* lane) const;
    void uninstallLight(Lane* lane);
    void uninstallLights();

protected:
	FixedTimeTrafficLightController(int32 id, RoadNetwork* network);
	virtual ~FixedTimeTrafficLightController();

    void initialize(Simulator* sim);
    void update(Simulator* sim);

    friend class RoadNetwork;
    friend class TrafficLightController;
    friend DataStream& operator <<(DataStream& f, FixedTimeTrafficLightController* controller);
    friend DataStream& operator >>(DataStream& f, FixedTimeTrafficLightController* controller);
};

}

#endif /* FIXEDTIMETRAFFICLIGHTCONTROLLER_H_ */
