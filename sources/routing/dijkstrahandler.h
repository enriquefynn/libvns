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

#ifndef VNS_DIJKSTRAHANDLER_H_
#define VNS_DIJKSTRAHANDLER_H_

#include "core.h"
#include "path.h"
#include "pathfinderhandler.h"
#include "vector.h"

namespace vns {

class DijkstraHandler : public PathFinderHandler {

private:
	WeightFunction* function;

    Path* createPath(RoadNetwork* network,int32 id);

    int32 NR;

	Vector<double> dist;
	Vector<int32> prev;

public:
	DijkstraHandler(WeightFunction* function);
	virtual ~DijkstraHandler();

	Path* calculatePath(RoadNetwork* network, Road* from, Way wayFrom, Road* to, Way wayTo, const VehicleType& vehicleType=vns::CAR);

private:
	/*! \cond PRIVATE */
    class Node {
      public:
        Node(){ road=0; };
        Node(Road* road, Way way, double w) :
			road(road), way(way), w(w) {
		};
        ~Node(){};
        Road* road;
        Way way;
        double w;

        bool operator<(const Node& node) const{
        	return w > node.w;
        };

        friend class Comp;
    };

    struct Comp {
        bool operator()( const DijkstraHandler::Node n1, const DijkstraHandler::Node n2 ) const {
            return n1.w > n2.w;
       }
    };
    /*! \endcond */
};

}

#endif /* DIJKSTRAHANDLER_H_ */
