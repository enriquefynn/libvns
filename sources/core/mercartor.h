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

#ifndef VNS_MERCARTOR_H_
#define VNS_MERCARTOR_H_

#include "vec.h"

namespace vns {

/**
 *  Spherical Mercator Projection
 */

class RoadNetwork;
class PolyLine;

class Mercartor {
public:
	static Vec toXY(double lon,double lat);
	static Vec toXY(const Vec& p);
	static Vec toLonLat(double x, double y);
	static Vec toLonLat(const Vec& p);
	static void toLonLat(RoadNetwork* network);
	static void toLonLat(PolyLine* line);
	static void toXY(RoadNetwork* network);
	static void toXY(PolyLine* line);
};

}
#endif /* VNS_MERCARTOR_H_ */
