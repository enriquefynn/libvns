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

#include "buildings.h"
#include <time.h>

namespace vns {

Buildings* Buildings::staticInstance = 0;

Buildings::Buildings() {

	staticInstance = this;
}

Buildings::~Buildings() {
	clear();
}

void Buildings::addBuilding(vns::Building* building){
	buildings.append( building );
	extents.addBBox( building->getBBox() );
	rtree.addItem( building );
}

void Buildings::clear(){
	for(int i=0;i<buildings.size();i++){
		delete buildings[i];
	}
	buildings.clear();
	extents.clear();
}

bool Buildings::intersects(const vns::Vec& p1,const vns::Vec& p2){
	//clock_t tStart = clock();
	/*for(int i=0;i<buildings.size();i++){
		vns::Building* b = buildings.at(i);
		if(b->intersects(p1,p2)){
			return true;
		}
	}
	//printf("Time taken: %.6fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
	return false;
	*/

	vns::Buildings::IntersectionVisitor v(p1,p2);
	rtree.applyVisitor(v);

	//printf("Time taken: %.6fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
	return v.hasIntersection();


}

}
