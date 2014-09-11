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

#include "mercartor.h"
#include "maths.h"
#include "vec.h"
#include "polyline.h"
#include "roadnetwork.h"

#define R_MAJOR 6378137.0
#define R_MINOR 6356752.3142
#define PI 3.141592653589793
#define PI2 1.5707963267948966
#define PI4 0.7853981633974483

namespace vns {

Vec Mercartor::toXY(double lon,double lat)  {
	double x = (PI/180.0) * lon * R_MAJOR;
	double a = PI4+((lat*PI)/180.0)*0.5;
	double y = log(tan( a )) * R_MINOR;
	return vns::Vec( x , y );
}

Vec Mercartor::toXY(const Vec& p)  {
	return toXY(p.getX(),p.getY());
}

Vec Mercartor::toLonLat(double x, double y)  {
	double lon = ((180.0/PI)*x)/R_MAJOR;
	double a = y/R_MINOR;
	double lat = (180.0/PI)*(2.0*atan(exp(a))-PI2);
    return vns::Vec( lon , lat );
}

Vec Mercartor::toLonLat(const Vec& p)  {
	return toLonLat(p.getX(),p.getY());
}

void Mercartor::toLonLat(RoadNetwork* network)  {
	for (int i = 0; i < network->getNumberOfRoads(); i++) {
		Road* road = network->getRoad(i);
		toLonLat(road->getPolyLine());
	}
}

void Mercartor::toLonLat(PolyLine* line)  {
		for(int j=0;j<line->size();j++){
			Vec p = line->getPoint(j);
			Vec np = toLonLat(p);
			line->updatePoint(j,np.getX(),np.getY(),p.getZ());
		}
}

void Mercartor::toXY(RoadNetwork* network)  {
		for(int i=0;i<network->getNumberOfRoads();i++){
			vns::Road* road = network->getRoad(i);
			toXY(road->getPolyLine());
		}
}

void Mercartor::toXY(PolyLine* line)  {
		for(int j=0;j<line->size();j++){
			vns::Vec p = line->getPoint(j);
			vns::Vec np = toXY(p);
			line->updatePoint(j,np.getX(),np.getY(),p.getZ());
		}
}

}
