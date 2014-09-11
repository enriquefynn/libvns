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

#ifndef PRETIMEDTRAFFICLIGHTCONTROLLER_H_
#define PRETIMEDTRAFFICLIGHTCONTROLLER_H_

#include "datastream.h"
#include "trafficlightcontroller.h"
#include "vector.h"

namespace vns {

class PreTimedTrafficLightController;

class PreTimedState {
private:
	double time;
	Vector< TrafficLightState* > states;
	int32 currentState;
    double comuteTime;
    PreTimedTrafficLightController* controller;

	PreTimedState(PreTimedTrafficLightController* controller);
	PreTimedState(double scheduleTime,PreTimedTrafficLightController* controller);
	~PreTimedState();
    void update(Simulator* sim);
    void initialize(Simulator* sim);

	friend class PreTimedTrafficLightController;
    friend DataStream& operator <<(DataStream& f, const PreTimedState* state);
    friend DataStream& operator >>(DataStream& f, PreTimedState* state);
    friend DataStream& operator <<(DataStream& f, PreTimedTrafficLightController* controller);
    friend DataStream& operator >>(DataStream& f, PreTimedTrafficLightController* controller);

public:
	inline int8 getNumberOfStates(){ return states.size(); };
	int8 addState(float duration, const Vector<Light>& lights);
	inline double getScheduledTime(){ return time; };
};

class PreTimedTrafficLightController : public TrafficLightController {
private:
	int32 currentState;
    Vector< PreTimedState* > preTimedStates;
	Vector<Lane*> lanes;

public:
	inline int32 getNumberOfPreTimedStates() const { return preTimedStates.size(); };
	inline PreTimedState* getPreTimedState(int32 i) const { return preTimedStates.at(i); };
	PreTimedState* newPreTimedState( double scheduledTimed );
	void removePreTimedState(PreTimedState* state);
	void removePreTimedState(int32 i);

    inline int32 getNumberOfLights() const { return lanes.size(); };
    inline Lane* getLane(int32 lightID) { return lanes.at(lightID); };
    inline const Lane* getLane(int32 lightID) const { return lanes.at(lightID); };

    bool installLight(Lane* lane);
	void installLights(const Vector<Lane*>& lanes);
    bool hasLightInstalled(Lane* lane);
    void uninstallLight(Lane* lane);
    void uninstallLights();

protected:
	PreTimedTrafficLightController(int32 id, RoadNetwork* network);
	virtual ~PreTimedTrafficLightController();

    void initialize(Simulator* sim);
    void update(Simulator* sim);

    friend class RoadNetwork;
    friend class PreTimedState;
    friend class TrafficLightController;
    friend DataStream& operator <<(DataStream& f, PreTimedTrafficLightController* controller);
    friend DataStream& operator >>(DataStream& f, PreTimedTrafficLightController* controller);
};

}

#endif /* PRETIMEDTRAFFICLIGHTCONTROLLER_H_ */
