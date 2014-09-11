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

#ifndef VNS_LANE_H
#define VNS_LANE_H

#include "core.h"
#include "datastream.h"
#include "lock.h"
#include "polyline.h"
#include "laneobject.h"
#include "trafficlightcontroller.h"
#include "trigger.h"
#include "vector.h"
#include "hashtable.h"

// TODO: add removeItem in Vector

namespace vns {

#define LANE_LIMIT_MAXSPEED 255
#define DISTANCE_FROM_JUNCTION 6.0

class RoadNetwork;
class MapWindow;
class Junction;
class Road;
class TrafficLightController;

class Lane {
public:
	static const int32 NoFlags = 0;
	static const int32 ParkingLane = 1<<0;
	static const int32 BusLane = 1<<1;
	static const int32 StopSign = 1<<2;
	static const int32 YieldSign = 1<<3;
	static const int32 ChangeLaneToLeft = 1<<4;
	static const int32 ChangeLaneToRight = 1<<5;
	static const int32 NoTrafficLane = 1<<6;
	static const int32 Cars = 1<<7;
	static const int32 Trucks = 1<<8;
	static const int32 Buses = 1<<9;
	static const int32 Rails = 1<<10;
	static const int32 Bikes = 1<<11;
	static const int32 Pedestrians = 1<<12;
	static const int32 TaxiStand = 1<<13;

	inline void setFlags(int32 flgs){ flags = flgs; };
	inline void addFlags(int32 flgs){ flags |= flgs; };
	inline void removeFlags(int32 flgs){ flags &= (~flgs); };
	inline bool hasFlag(int32 flg) const { return flags & flg; };
	inline bool hasFlags(int32 flgs) const { return (flags & flgs) == flgs; };
	inline int32 getFlags() const { return flags; };
    
    inline void testLock(){ mlock.testLock(); };
    inline void lock(){ mlock.lock(); };
    inline void unlock(){ mlock.unlock(); };

protected:
    Lane();
    Lane(Road* road, int8 id=0, Way way=Forward, int32 flags=NoFlags, float width=3.0);
    ~Lane();

    class LaneSensor {
    public:
    	LaneSensor(Trigger* trig, float pos){ trigger = trig; lanePos = pos; };
    	~LaneSensor(){};
    	Trigger* trigger;
    	float lanePos;
    	LaneSensor* next;

    	friend class SensorIterator;
    };
private:
    int8 id;
    Road* road;
    int32 flags;
    Way way;
    float maximumSpeed;
    float minimumSpeed;
    float width;
    float offset;
    float laneStart;
    float laneEnd;
    Vec firstPoint;
    Vec firstPointDir;
    Vec firstPointNormal;
    Vec lastPoint;
    Vec lastPointDir;
    Vec lastPointNormal;
    vns::Vector<const Road*> connections;
    vns::HashTable<const Road*,Lane*>* nextLanes;

    vns::Lock mlock;
    Light lightColor;
    LaneSensor* laneSensors;
    void* userdata;
    LaneObject* first;
    LaneObject* last;
    int32 nObjects;

    Vehicle* exitVehicle;


    Lane* clone() const;
    void setRoadCenterOffset(float offset);

    friend class Road;
    friend class RoadNetwork;
    friend class Vehicle;
    friend class Simulator;
    friend class ParallelSimulator;
    friend DataStream& operator <<(DataStream&f, const Lane* lane);
    friend DataStream& operator >>(DataStream& f, Lane* lane);
    friend DataStream& operator <<(DataStream&f, const Road* road);
    friend DataStream& operator >>(DataStream& f, Road* road);
    friend DataStream& operator <<(DataStream&f, const RoadNetwork* network);
    friend DataStream& operator >>(DataStream& f, RoadNetwork* network);

public:
    class SensorIterator {
    private:
        LaneSensor* i;
    public:
        inline SensorIterator() : i(0){};
        inline SensorIterator(LaneSensor* sensor) : i(sensor){};
        inline Trigger* getTrigger(){ return i->trigger; };
        inline float getPosition() const { return i->lanePos; };
        inline bool isValid() const { return i!=0; };
        inline void next(){ i = i->next; };
    };

    void addLaneSensor(Trigger* trigger, float pos);
    void removeLaneSensor(Trigger* trigger);
    void clearLaneSensors();
    inline Lane::SensorIterator getSensorIterator() const { return Lane::SensorIterator(laneSensors); };

    inline Lane* getNextLane(const Road* nextRoad){ return nextLanes->value(nextRoad,0); };
    inline const Lane* getNextLane(const Road* nextRoad) const { return nextLanes->value(nextRoad,0); };

    inline int32 getNumberOfConnections() const { return connections.size(); };
    inline const Road* getConnection(int32 i) const { return connections.at(i); };
    bool addConnectionTo(const Road* road);
    bool removeConnectionTo(const Road* road);
    bool hasConnectionTo(const Lane* lane) const;
    bool hasConnectionTo(const Road* road) const;
    void clearConnections();
    bool allowsVehicle(const VehicleType& type) const;

    inline int8 getID() const { return id; };
    inline Road* getRoad() { return road; };
    inline const Road* getRoad() const { return road; };
    inline float getRoadCenterOffset() const { return offset; };
    inline float getWidth() const { return width; };
    inline float getLength() const { return laneEnd-laneStart; };
    inline float getLaneEnd() const { return laneEnd; };
    inline float getLaneStart() const { return laneStart; };
    inline const Vec& getFirstPoint() const { return firstPoint; };
    inline const Vec& getLastPoint() const { return lastPoint; };
    inline const Vec& getFirstPointDirection() const { return firstPointDir; };
    inline const Vec& getLastPointDirection() const { return lastPointDir; };
    inline const Vec& getFirstPointNormal() const { return firstPointNormal; };
    inline const Vec& getLastPointNormal() const { return lastPointNormal; };
    inline float getMaximumSpeed() const { return maximumSpeed; };
    inline float getMinimumSpeed() const { return minimumSpeed; };
    inline void setMaximumSpeed(float maxSpeed){ maximumSpeed = vns_MAX(maxSpeed,minimumSpeed); };
    inline void setMinimumSpeed(float minSpeed){ minimumSpeed = vns_MAX(0,vns_MIN(minSpeed,maximumSpeed)); };
    inline Way getWay() const { return way; };
    inline bool hasTrafficLight() const { return lightColor != NoLight; };
    inline Light getTrafficLightColor() const { return lightColor; };
    inline void setTrafficLightColor(Light color){ lightColor = color; };
    inline void setUserData(void* data){ userdata = data; };
    inline void* getUserData() const { return userdata; };

    Junction* getStartJunction();
    const Junction* getStartJunction() const;
    Junction* getEndJunction();
    const Junction* getEndJunction() const;
    RoadNetwork* getRoadNetwork();
    const RoadNetwork* getRoadNetwork() const;
    Lane* getDrivingLaneOnLeft();
    Lane* getDrivingLaneOnRight();
    Lane* getSameWayDrivingLaneOnLeft();
    Lane* getSameWayDrivingLaneOnRight();
    Lane* getParkingLaneOnLeft();
    Lane* getParkingLaneOnRight();
    Lane* getNextLaneTo(const Lane* destLane);

    void getPositionPoint(Vec& pt, Vec& dir, Vec& perp, float pos) const;

    void setWidth(float width);
    void setLaneStart(float pos);
    void setLaneEnd(float pos);
    void setLaneStartOffset(float pos);
    void setLaneEndOffset(float pos);
    float getLaneEndOffset() const;
    float getLaneStartOffset() const;

    /*
     * LaneObjects
     */

    Vehicle* getFirstVehicle();
    Vehicle* getLastVehicle();
    const Vehicle* getFirstVehicle() const;
    const Vehicle* getLastVehicle() const;
    inline bool isFirstObject(const LaneObject *obj) const { return obj==first; };
    inline bool isLastObject(const LaneObject *obj) const { return obj==last; };
    inline LaneObject* firstObject() { return first; };
    inline LaneObject* lastObject() { return last; };
    inline const LaneObject* firstObject() const { return first; };
    inline const LaneObject* lastObject() const { return last; };
    inline const LaneObject* getFirstObject() const { return first; };
    inline const LaneObject* getLastObject() const { return last; };
    inline LaneObject* getFirstObject() { return first; };
    inline LaneObject* getLastObject() { return last; };
    inline int32 getNumberOfObjects() const { return nObjects; };

    void objectsAround(float position, LaneObject*& previous, LaneObject*& next) const;

    bool canEnter(const LaneObject* obj) const;
    bool canEnter(float vehicleLength) const;
    //void updateObjectPosition(LaneObject* obj,float position);

private:
    uint32 simulationStep(Simulator* simulator);
    void updateStep(Simulator* simulator);
    void addObject(LaneObject* obj, float position);
    void removeObject(LaneObject* obj);


    double getFreeSpacePosition(float len) const;

public:
    class ObjectIterator {
    private:
        LaneObject* i;
    public:
        inline ObjectIterator():i(0){};
        inline ObjectIterator(LaneObject *object):i(object){};
        inline bool isValid() const { return i!=0; };
        inline void next(){ i = i->next; };
        inline void previous(){ i = i->prev; };
        inline LaneObject* getLaneObject(){ return i; };
    };

    inline Lane::ObjectIterator getFirstObjectIterator() const { return Lane::ObjectIterator(first); };
    inline Lane::ObjectIterator getLastObjectIterator() const { return Lane::ObjectIterator(last); };

};

}

#endif
