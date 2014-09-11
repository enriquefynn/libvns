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

#ifndef VNS_TRAFFICLIGHTCONTROLLER_H_
#define VNS_TRAFFICLIGHTCONTROLLER_H_

#include "datastream.h"
#include "core.h"
#include "vector.h"

namespace vns {


class Simulator;
class RoadNetwork;
class Lane;

class TrafficLightState {
private:
	Vector<Light> lights;
	float duration;

public:
	TrafficLightState();
	TrafficLightState(float t, const Vector<Light>& colors);
	virtual ~TrafficLightState();

	Light getLight(int8 i) const;
	void setLight(int8 i,Light light);
	inline float getDuration() const { return duration; };
	inline void setDuration(float t){ duration = t; };
	int8 getNumberOfLights() const;

    friend DataStream& operator <<(DataStream& f, const TrafficLightState* state);
    friend DataStream& operator >>(DataStream& f, TrafficLightState* state);

    friend class TrafficLightController;
    friend class FixedTimeTrafficLightController;
    friend class PreTimedTrafficLightController;
};


class TrafficLightController {

public:
	typedef enum { FixedTime, PreTimed} Type;

	inline int32 getID() const { return id; };
    inline TrafficLightController::Type getType() const { return type; };
    inline RoadNetwork* getRoadNetwork(){ return network; };
    inline const RoadNetwork* getRoadNetwork() const { return network; };

    virtual int32 getNumberOfLights() const = 0;
    virtual Lane* getLane(int32 lightID) = 0;
    virtual const Lane* getLane(int32 lightID) const = 0;
    virtual void uninstallLights() = 0;

protected:
    TrafficLightController(int32 id, TrafficLightController::Type type, RoadNetwork* network);
	virtual ~TrafficLightController();

    virtual void initialize(Simulator* sim) = 0;
    virtual void update(Simulator* sim) = 0;

private:
    TrafficLightController::Type type;
	int32 id;
	RoadNetwork* network;

	static void write(DataStream& f,TrafficLightController* controller);
	static TrafficLightController* read(DataStream& f,RoadNetwork* network);

    inline void doInitialize(Simulator* sim) { initialize(sim); };
    inline void doUpdate(Simulator* sim) { update(sim); };

    friend class RoadNetwork;
    friend class Simulator;
    friend DataStream& operator <<(DataStream& f, const Light& color);
    friend DataStream& operator >>(DataStream& f, Light& color);
    friend DataStream& operator <<(DataStream&f, const RoadNetwork* network);
    friend DataStream& operator >>(DataStream& f, RoadNetwork* network);
};

}

#endif /* TRAFFICLIGHTCONTROLLER_H_ */
