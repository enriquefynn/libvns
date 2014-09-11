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

#ifndef VNS_ASTARHANDLER_H_
#define VNS_ASTARHANDLER_H_

#include "core.h"
#include "path.h"
#include "pathfinderhandler.h"
#include "vector.h"

namespace vns {

class AStarHandler : public PathFinderHandler {


public:
	AStarHandler(WeightFunction* f);
	virtual ~AStarHandler();

	Path* calculatePath(RoadNetwork* network, Road* from, Way wayFrom, Road* to, Way wayTo, const VehicleType& vType=vns::CAR);

private:

	WeightFunction* function;

    int32 NR;

	Vector<bool> visited;
	/*! \cond PRIVATE */
    class Node {
      public:
        Node(){ road=0; parent = 0; };
        Node(Road* road, Way way, double g, double h, Node* parent=0) :
			road(road), way(way), g(g), h(h), parent(parent) {
			f = g + h;
		};
        ~Node(){};
        Road* road;
        Way way;
        double f;
        double g;
        double h;
        Node* parent;

        bool operator<(const Node& node) const{
        	return f > node.f;
        };

        friend class Comp;
        friend class AStarHandler;
    };

    struct Comp {
        bool operator()( const AStarHandler::Node* n1, const AStarHandler::Node* n2 ) const {
            return n1->f > n2->f;
       }
    };
    /*! \endcond */

    Path* createPath(const AStarHandler::Node* node);


};

}

#endif /* ASTARHANDLER_H_ */
