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

#ifndef VNS_JUNCTION_H
#define VNS_JUNCTION_H

#include "datastream.h"
#include "vec.h"
#include "road.h"
#include "roadnetwork.h"
#include "vector.h"

namespace vns {

class Junction {

private:
    RoadNetwork *roadNetwork;
    int8 type;
    int32 id;
    BBox bb;
    Vec location;
    vns::Vector<Road*> roads;

    enum JunctionType {InvalidJunction, ConnectionJunction, IntersectionJunction};

protected:

    Junction(RoadNetwork* roadNetwork, int32 id=-1,const Vec& position = Vec());
    ~Junction();

public:
    inline const BBox& getBBox() const { return bb; };
	inline int32 getID() const { return id; };
	inline RoadNetwork* getRoadNetwork(){ return roadNetwork; };
	inline const RoadNetwork* getRoadNetwork() const { return roadNetwork; };
	inline const Vec& getPosition() const { return location; };
    inline void setPosition(const Vec& p){ updateCenter(p.getX(),p.getY(),p.getZ()); };

	inline int8 getNumberOfRoads() const { return roads.size(); };
	inline Road* getRoad(int8 i){ return roads.at(i); };
	inline const Road* getRoad(int8 i) const { return roads.at(i); };
	int8 getRoadIndex(const Road* road) const;
    bool isEntryRoad(const Road* road) const;
    bool isExitRoad(const Road* road) const;

    Road* getLeftRoad(const Road* road);
    Road* getRightRoad(const Road* road);
    Road* getLeftEntryRoad(const Road* road);
    Road* getRightEntryRoad(const Road* road);

private:
    void updateNextLanes();
    void updateJunction();
    void updateCenter(double x,double y,double z);
    void updateConnections();
    void checkRoadsOrder();
    bool addRoad(Road* road);
    void removeRoad(Road* road);
    void removeRoadByIndex(int8 i);
    void clearConnections();

    friend class RoadNetwork;
    friend class Road;
    friend DataStream& operator <<(DataStream&f, const RoadNetwork* network);
    friend DataStream& operator >>(DataStream&f, RoadNetwork* network);
    friend DataStream& operator <<(DataStream&f, const Junction* junction);
    friend DataStream& operator >>(DataStream&f, Junction* junction);
    friend DataStream& operator <<(DataStream&f, const Road* road);
    friend DataStream& operator >>(DataStream&f, Road* road);
};

}

#endif
