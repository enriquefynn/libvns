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

#ifndef VNS_ROAD_H
#define VNS_ROAD_H

#include "core.h"
#include "vec.h"
#include "bbox.h"
#include "datastream.h"
#include "vector.h"
#include "list.h"
#include "polyline.h"
#include "lane.h"
#include "bitset.h"
#include "eventscheduler.h"

#define VNS_ROAD_CROSSWALKWIDTH 3.0
#define ROAD_ENDGAP 2.0
#define ROAD_MARGINSGAP 0.2
#define VSN_MIN_ROAD_LENGTH_FOR_PARKING 10.0
/*
 * TODO: remove len, add fwidth and bwidth to write
 */

namespace vns {

class RoadNetwork;
class Junction;
class Route;
class ScheduledRoute;

class Road {

public:
	enum Type {
		Undefined = 0, Motorway,MotorwayLink,Street,Trunk,TrunkLink,Primary,Residential,
		BusGuideway,Service,Parking,Fuel,Toll,Footway,Cicleway,Steps,Railway,Monorail
	};

	unsigned int seed;

protected:
    Road(RoadNetwork* network = 0);
    Road(RoadNetwork* network, vns::Road::Type type, int32 id, PolyLine* p);
    ~Road();

private:

    BBox bb;
    RoadNetwork *roadNetwork;
    vns::Road::Type type;
    int32 id;
    Junction* startJunction;
    Junction* endJunction;
    float width;
    float offset;
    float roadStart;
    float roadEnd;

    Lane* leftParkingLane;
    Lane* rightParkingLane;
    bool startCrossWalk;
    bool endCrossWalk;

    vns::Vector<Lane*> lanes;
    PolyLine* polyline;

    void* userdata;
    
    vns::List<Vehicle*> forwardQueue;
    vns::List<Vehicle*> backwardQueue;
    uint32 maximumForwardQueue;
    uint32 maximumBackwardQueue;

    // Traffic demand
	EventScheduler* scheduler;
	uint32 nextVehicleID;
public:

	EventScheduler* getEventScheduler(){ return scheduler; };
	static const char* fromType(vns::Road::Type type);
	static vns::Road::Type fromTypeName(const char* name);

	inline void setType(vns::Road::Type type){ this->type = type; };
	inline vns::Road::Type getType() const { return type; };
    inline const BBox& getBBox() const { return bb; };
    inline int32 getID() const { return id; };
    inline float getWidth() const { return width; };
    float getWidth(vns::Way way) const;
    inline void setUserData(void* data){ userdata = data; };
    inline void* getUserData() const { return userdata; };
    inline float getLength() const { return polyline->getLength(); };
    inline int32 getNumberOfPoints() const { return polyline->size(); };
    inline const Vec& getPoint(int32 i) const { return polyline->getPoint(i); };
    inline const Vec& getNormal(int32 i) const { return polyline->getNormal(i); };
    inline float getLengthUntilPoint(int32 i) const { return polyline->getT(i); };
    inline Vec getDirection(int32 i) const { return polyline->getDirection(i); };
    Vec getLastPointDirection(Way way = Forward) const;
    Vec getFirstPointDirection(Way way = Forward) const;
    Vec getLastPointDirection(const Junction* junction) const;
    Vec getFirstPointDirection(const Junction* junction) const;
    const Vec& getFirstPoint(Way way = Forward) const;
    const Vec& getLastPoint(Way way = Forward) const;
    inline RoadNetwork* getRoadNetwork(){ return roadNetwork; };
    inline const RoadNetwork* getRoadNetwork() const { return roadNetwork; };
    inline PolyLine* getPolyLine(){ return polyline; };
    inline const PolyLine* getPolyLine() const { return polyline; };
    bool contains(const Vec& pt) const;
    inline void setRoadOffset(float offset){ this->offset=offset; update(); };
    inline float getRoadOffset() const { return offset; };

    inline Lane* getLane(int8 laneID){ return lanes.at(laneID); };
    inline const Lane* getLane(int8 laneID) const { return lanes.at(laneID); };
    inline int8 getNumberOfLanes() const { return lanes.size(); };
    int8 getNumberOfForwardLanes() const;
    int8 getNumberOfBackwardLanes() const;

    bool isSourceRoad(Way way) const;
    bool isSinkRoad(Way way) const;
    void setRightParking(bool rightParking);
    void setLeftParking(bool leftParking);
    void setEndCrossWalk(bool crossWalk);
    void setStartCrossWalk(bool crossWalk);
    void setMaximumSpeed(float speed);
    void setMinimumSpeed(float speed);
    
    bool hasParking(vns::Way way);

    Lane* createLane(Way way = Forward, float width=3.0);
    void removeLane(int8 laneID);

    bool allowsVehicle(const VehicleType& type,const vns::Way& way=vns::Forward) const;

    BitSet getLanesInWay(Way way) const;
    BitSet getLanesConnectingTo(const Road* nextRoad) const;
    
    inline Junction* getEndJunction(Way way = Forward){return (way==Forward?endJunction:startJunction);};
    inline Junction* getStartJunction(Way way = Forward){return (way==Forward?startJunction:endJunction);};
    inline const Junction* getEndJunction(Way way = Forward) const {return (way==Forward?endJunction:startJunction);};
    inline const Junction* getStartJunction(Way way = Forward) const {return (way==Forward?startJunction:endJunction);};
    inline float getRoadStart(Way way = Forward) const { return (way==Forward?roadStart:polyline->getLength()-roadEnd); };
    inline float getRoadEnd(Way way = Forward) const { return (way==Forward?roadEnd:polyline->getLength()-roadStart); };

    inline bool hasRightParking(Way way = Forward) const { return (way==Forward?rightParkingLane!=0:leftParkingLane!=0); };
    inline bool hasLeftParking(Way way = Forward) const { return (way==Forward?leftParkingLane!=0:rightParkingLane!=0); };
    inline Lane* getRightParking(Way way = Forward) const { return (way==Forward?rightParkingLane:leftParkingLane); };
    inline Lane* getLeftParking(Way way = Forward) const { return (way==Forward?leftParkingLane:rightParkingLane); };

    inline bool hasStartCrossWalk(Way way = Forward) const { return (way==Forward?startCrossWalk:endCrossWalk); };
    inline bool hasEndCrossWalk(Way way = Forward) const { return (way==Forward?endCrossWalk:startCrossWalk); };


    void setPoint(int32 i, const Vec& point);
    bool insertPoint(int32 i, Vec point);
    bool removePoint(int32 i);

    bool hasLanes(Way way=Forward) const { return (way==Forward?lanes[lanes.size()-1]->way==way:lanes[0]->way == way); };


    bool hasConnectionTo(const Road* road) const;
    bool hasConnectionTo(vns::Way way,const Road* road) const;
    bool hasConnectionTo(const Lane* lane) const;
    void clearConnections(Way way);
    void clearAllConnections();


    void getStartPoint(Vec& pt, Vec& dir, Vec& perp, Way way = Forward) const ;
    void getEndPoint(Vec& pt, Vec& dir, Vec& perp, Way way = Forward) const ;
    void getPositionPoint(Vec& pt, Vec& dir, Vec& perp, double pos, Way way = Forward) const;

    void setMaximumCarsInQueue(vns::Way, uint32 v);
    uint32 getNumberOfCarsInQueue(vns::Way way) const { return (way==Forward?forwardQueue.size():backwardQueue.size()); };
    uint32 getNumberOfCarsInQueue() const { return forwardQueue.size()+backwardQueue.size(); };
    inline uint32 getMaximumCarsInQueue(vns::Way way) const { return (way==Forward?maximumForwardQueue:maximumBackwardQueue); };

    bool canInvertWay(vns::Way way) const;

    void updateConnectionsTODO();
private:

    void simulationStart(vns::Simulator* simulator);
    uint32 simulationStep(vns::Simulator* simulator);
    void updateStep(vns::Simulator* simulator);
    Lane* chooseLaneForVehicle(Way way, Vehicle* vehicle, double& lanePos);

    bool hasRoomInQueue(Way way);
    void addVehicleToQueue(Way way, Vehicle* vehicle);
    void clearQueuedVehicles();

    Road* clone() const;
    void updateConnections();

    void update();

    void setPoints(const Vector<Vec>& points);
    void setRoadEnd(float end);
    void setRoadStart(float start);

    friend class Simulator;
    friend class ParallelSimulator;
    friend class RoadNetwork;
    friend class Lane;
    friend class Junction;
    friend DataStream& operator <<(DataStream&f, const Road* road);
    friend DataStream& operator >>(DataStream& f, Road* road);
    friend DataStream& operator <<(DataStream&f, const RoadNetwork* network);
    friend DataStream& operator >>(DataStream& f, RoadNetwork* network);
};

}

#endif
