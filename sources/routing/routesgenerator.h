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

#ifndef VNS_ROUTESGENERATOR_H_
#define VNS_ROUTESGENERATOR_H_

#include "roadnetwork.h"
#include "list.h"
#include "pathfinderhandler.h"
#include "routes.h"

namespace vns {

class RoutesGenerator {
private:
	class Node {
	public:
		Node(){road=0;};
		Node(vns::Road* road,vns::Way way):road(road),way(way){};
		virtual ~Node(){};
		vns::Road* road;
		vns::Way way;
	};
public:
	typedef enum {ALL, PARKING, NOJUNCTION, PARKING_OR_NOJUNCTION} Type;



	RoutesGenerator(vns::RoadNetwork* network);
	virtual ~RoutesGenerator();

	void setGenerationInterval(float interval);
	void setVehicleType(const VehicleType& type);
	void setSourceRoads(vns::RoutesGenerator::Type type);
	void setSinkRoads(vns::RoutesGenerator::Type type);
	void setHandler(vns::PathFinderHandler* finderHandler);
	void setRandomSeed(unsigned int rseed){ randomSeed = rseed; };
	void setDriver(const vns::DriverInfo& dr){ driver = dr; };

	vns::Routes* generate(int numberOfRoutes);
	void generate(vns::Routes* routes,int numberOfRoutes);

private:

	void updateSourcesAndSinks();
	vns::RoadNetwork* network;
	vns::WeightFunction* function;
	vns::PathFinderHandler* handler;
	vns::PathFinderHandler* defaultHandler;
	float interval;
	VehicleType vehicleType;
	vns::DriverInfo driver;
	vns::RoutesGenerator::Type sourceType;
	vns::RoutesGenerator::Type sinkType;
	vns::Vector<vns::RoutesGenerator::Node> sources;
	vns::Vector<vns::RoutesGenerator::Node> sinks;
	unsigned int randomSeed;
};

}

#endif
