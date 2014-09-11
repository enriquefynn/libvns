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

#ifndef VNS_LANEOBJECT_H
#define VNS_LANEOBJECT_H

#include "core.h"
#include "lock.h"
#include "vec.h"
#include "object.h"

namespace vns {

class Lane;
class Simulator;

class LaneObject : public Object {

public:

    LaneObject(Object::Type type, float width=1.0, float length=1.0);
    inline virtual ~LaneObject(){};
    virtual float getAccel() const { return 0; };
    virtual float getAccelTo(const LaneObject*) const { return 0; };
    inline float gapTo(LaneObject *fo) const { return fo->lanePos-lanePos-(length+fo->length)*0.5; };
    inline float gapTo(float pos, LaneObject *fo) const { return fo->lanePos-pos-(getLength()+fo->getLength())*0.5; };
    inline float gapToNextObject() const { return next->lanePos-lanePos-(length+next->length)*0.5; };
    inline float getLanePosition() const { return lanePos; };
    inline float getFrontPosition() const { return lanePos+length*0.5; };
    inline float getRearPosition() const { return lanePos-length*0.5; };
    inline LaneObject* getNextObject(){ return next; };
    inline const LaneObject* getNextObject() const { return next; };
    inline LaneObject* getPreviousObject(){ return prev; };
    inline const LaneObject* getPreviousObject() const { return prev; };
    inline Lane* getLane(){ return lane; };
    inline const Lane* getLane() const { return lane; };
    inline Lane* getNextLane(){ return nextLane; };
    inline const Lane* getNextLane() const { return nextLane; };
    inline Lane* getPreviousLane() { return prevLane; };
    inline const Lane* getPreviousLane() const { return prevLane; };

    inline void testLock(){ mlock.testLock(); };
    inline void lock(){ mlock.lock(); };
    inline void unlock(){ mlock.unlock(); };

protected:
    virtual void simulationStep(Simulator*){};
    virtual void updateStep(Simulator*){};
    float lanePos;
    Lane* lane;
    Lane* nextLane;
    Lane* prevLane;
    LaneObject* next;
    LaneObject* prev;
    unsigned int seed;

    uint32 simstep;

private:
    Lock mlock;

    friend class Lane;
    friend class Road;
    friend class Simulator;
    friend class ParallelSimulator;
};

}

#endif
