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

#ifndef VNS_ROADNETWORK_H
#define VNS_ROADNETWORK_H

#include "core.h"
#include "datastream.h"
#include "bbox.h"
#include "vec.h"
#include "vector.h"
#include "hashtable.h"
#include "junction.h"
#include "road.h"
#include "lane.h"
#include "trafficlightcontroller.h"
#include "fixedtimetrafficlightcontroller.h"
#include "pretimedtrafficlightcontroller.h"

#define ROADNETWORK_VERSION "1.0"

namespace vns {

class Trigger;

class RoadNetwork {
private:
    BBox extents;
    Vec offset;
    vns::Vector<Junction*> junctions;
    vns::Vector<Road*> roads;
    vns::Vector<TrafficLightController*> trafficLights;
    vns::HashTable<Vec,Junction*> junctionsHash;
    bool automaticConnectivityGeneration;

public:

    RoadNetwork();
    RoadNetwork(const char* file);
    ~RoadNetwork();

    const char* getVersion() const { return ROADNETWORK_VERSION; };

    TrafficLightController* createTrafficLightController(TrafficLightController::Type type);
    inline int32 getNumberOfTrafficLights() const { return trafficLights.size(); };
    const TrafficLightController* getTrafficLightController(int32 id) const;
    TrafficLightController* getTrafficLightController(int32 id);
    void removeTrafficLightController(int32 id);
    void removeTrafficLightController(TrafficLightController* controller);
    void clearTrafficLights();

    bool save(const char* file);
    bool load(const char* file);
    void clear();

    bool join(Junction* junctionA, Junction* junctionB);
    bool join(Road* roadA, Road* roadB);
    bool join(Road* road, Junction* junction);
    void disconnect(Road* road, Junction* junction);
    void disconnectAll(Junction* junction);
    void moveToFront(Road* road);
    Junction* splitRoad(Road* road, int32 splitPoint);
    Road* mergeRoads(Junction* junction);

    inline const BBox& getExtents() const { return extents; };

    Road* getNearestRoad( const Vec& pt, int8 roadType=-1);
    Road* getPointAndRoadWithin(Vec& pt, double radius, int32& pointIndex, int32 exceptRoad = -1);

    inline Road* getRoad(int32 rid){ return roads.at(rid); };
    inline const Road* getRoad(int32 rid) const { return roads.at(rid); };
    Road* getRoad(const vns::Vec& pt);
    inline int32 getNumberOfRoads() const { return roads.size(); };
    Lane* getLane(int32 roadID,int8 laneID);

    Junction* getJunction(const Vec& point);
    Junction* getJunction(const BBox& bb);
    inline Junction* getJunction(int32 jid){ return junctions.at(jid); };
    inline const Junction* getJunction(int32 jid) const { return junctions.at(jid); };
    inline int32 getNumberOfJunctions() const { return junctions.size(); };

    Road* createRoad(PolyLine* polyline, int32 forwardLanes=1, int32 reverseLanes=0);
    Road* createRoad(const Vector<Vec>& points, Road* road);
    bool isValidRoad(const Road* road);
    bool isValidJunction(const Junction* junction);
    bool isValidLane(const Lane* lane);
    void deleteRoad(Road* road);
    void deleteRoad(int32 roadID);
    void deleteRoads(Vector<int32>& roadIDs);

    inline void setAutomaticConnectivityGeneration(bool value){ automaticConnectivityGeneration = value; };
    void generateRoadsConnectivity();
    inline void setOffset(const Vec& offset){ this->offset = offset;};
    inline const Vec& getOffset() const { return offset; };

    void balance();

private:
    void generateConnectivityOnRoadCreation(Road* road);

    bool hasJunction(const Vec& p);

    void computeLaneConnections(Junction* junction, Road* roadA, Road* roadB);
    void computeRoadJunctions(Road* road);
    Junction* newJunction(const Vec& pos);
    void deleteJunction(int32 i);
    void deleteJunction(Junction* junction);

    friend DataStream& operator <<(DataStream& f, const RoadNetwork* network);
    friend DataStream& operator >>(DataStream& f, RoadNetwork* network);
    friend class Simulator;
    friend class Road;
    friend class Junction;

};

}

#endif
