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

#include "junction.h"
#include "list.h"

namespace vns {


Junction::Junction(RoadNetwork* rnw,int32 i,const Vec& pos) {
	roadNetwork = rnw;
	id = i;
	location = pos;
	bb.addPoint(pos);
}

Junction::~Junction(){
	int32 nr=roads.size();
	for(int32 i=0;i<nr;i++){
		Road* road = roads[i];
		if( road ){
			if( road->startJunction == this ){
				road->startJunction = 0;
				road->clearConnections( Backward );
			}else{
				if( road->endJunction == this ){
					road->endJunction = 0;
					road->clearConnections( Forward );
				}
			}
			road->update();
		}
	}
	roads.clear();
	roadNetwork->junctionsHash.remove( location );
	bb.clear();
	roadNetwork = 0;
}

bool Junction::isEntryRoad(const Road* road) const {
	if(road->endJunction == this){
		return road->hasLanes( Forward );
	}else{
		if(road->startJunction == this){
			return road->hasLanes( Backward );
		}
	}
	return false;
}

bool Junction::isExitRoad( const Road* road ) const {
	if(road->startJunction == this){
		return road->hasLanes( Forward );
	}else{
		if(road->endJunction == this){
			return road->hasLanes( Backward );
		}
	}
	return false;
}

int8 Junction::getRoadIndex(const Road* road) const {
	for(int i=0;i<roads.size();i++){
		if( roads.at(i)==road ) return i;
	}
	return -1;
}


bool Junction::addRoad(Road* road){
	if(road->startJunction == this || road->endJunction == this) return false;
	if( location == road->getFirstPoint() && road->startJunction == 0 ){
		road->startJunction = this;
		roads.append( road );
	}else{
		if( location == road->getLastPoint() && road->endJunction == 0 ){
			road->endJunction = this;
			roads.append( road );
		}else{
			return false;
		}

	}
	road->update();
	updateConnections();
    return true;
}

void Junction::removeRoad(Road* road){
	int8 i = getRoadIndex(road);
	removeRoadByIndex( i );
}

void Junction::removeRoadByIndex(int8 i){
	if(i<0 || i>=roads.size() ) return;
	Road* road = roads.at(i);
	if( road && road->startJunction == this ){
		road->startJunction = 0;
		road->clearConnections( Backward );
		roads.remove(i);
		updateJunction();
		updateConnections();
	} else {
		if ( road && road->endJunction == this) {
			road->endJunction = 0;
			road->clearConnections( Forward );
			roads.remove(i);
			updateJunction();
			updateConnections();
		}
	}
}


void Junction::checkRoadsOrder(){
    bool swapped = true;
    int32 j=0;
    Road* tmp;
    int32 nr = roads.size();
    while(swapped){
    	swapped = false;
    	j++;
        for (int32 i=0; i<nr-j; i++){
        	Road* r1 = roads[i];
        	Vec a = (this == r1->endJunction? -r1->getPolyLine()->getLastPointDirection() : r1->getPolyLine()->getFirstPointDirection());
        	Road* r2 = roads[i+1];
        	Vec b = (this == r2->endJunction? -r2->getPolyLine()->getLastPointDirection() : r2->getPolyLine()->getFirstPointDirection());
            if (a.dir_rad() > b.dir_rad()){
                  tmp = roads[i];
                  roads[i] = roads[i+1];
                  roads[i+1] = tmp;
                  swapped = true;
             }
        }
    }
}


Road* Junction::getLeftRoad(const Road* road){
	int8 ri = getRoadIndex(road);
    int8 nr = roads.size();
    if(ri<0 || nr<2) return 0;
    ri = (ri+nr-1) % nr;
    return roads.at(ri);
}

Road* Junction::getRightRoad(const Road* road){
	int8 ri = getRoadIndex(road);
    int8 nr = roads.size();
    if(ri<0 || nr<2) return 0;
    ri = (ri+nr+1) % nr;
    return roads.at(ri);
}

Road* Junction::getLeftEntryRoad(const Road* road){
	int8 ri = getRoadIndex(road);
	int8 nr = roads.size();
    if(ri<0 || nr<2) return 0;
    int8 oldri = ri;
    ri = (ri+nr-1) % nr;
    while( oldri != ri ){
    	vns::Road* rr = roads.at(ri);
    	if( isEntryRoad(rr) ) return rr;
    	ri = (ri+nr-1) % nr;
    }
    return 0;
}

Road* Junction::getRightEntryRoad(const Road* road) {
	int8 ri = getRoadIndex(road);
	int8 nr = roads.size();
    if(ri<0 || nr<2) return 0;
    int8 oldri = ri;
    ri = (ri+nr+1) % nr;
    while( oldri != ri ){
    	vns::Road* rr = roads.at(ri);
    	if( isEntryRoad(rr) ) return rr;
    	ri = (ri+nr+1) % nr;
    }
    return 0;
}

void Junction::updateJunction(){
	checkRoadsOrder();
	bb.clear();
	bb.addPoint( Vec(location.getX()-2,location.getY()-2));
	bb.addPoint( Vec(location.getX()+2,location.getY()+2));
	int32 nr = roads.size();
	Vec pos,dir,perp;
	Vec p,n1,d1;
	bool forward;
	float len, halfWidth,halfWidth2,roadStart,roadEnd;
	Road* leftRoad;
	Road* rightRoad;

	for(int32 i=0;i<nr;i++){
		Road* road = roads.at(i);
		forward = (road->endJunction==this);
		len = road->getPolyLine()->getLength();
		halfWidth = road->getWidth()*0.5+ROAD_MARGINSGAP;
		roadStart = 0.0;
		roadEnd = road->getPolyLine()->getLength();
		if( forward ){
			n1 = road->getPolyLine()->getLastPointNormal();
			d1 = road->getPolyLine()->getLastPointDirection();
			p = road->getPolyLine()->getLastPoint()+road->offset*n1;
		}else{
	    	n1 = -road->getPolyLine()->getFirstPointNormal();
	    	d1 = -road->getPolyLine()->getFirstPointDirection();
	    	p = road->getPolyLine()->getFirstPoint()-road->offset*n1;
		}

		if (nr == 2) {
			leftRoad = getLeftRoad(road);
			if (leftRoad) {
				float dd = abs(leftRoad->getWidth()-road->getWidth())+ROAD_ENDGAP;
				if (road->endJunction == this) {
					roadEnd = len-dd;
				} else {
					roadStart = dd;
				}
			}
		} else {
			leftRoad = getLeftRoad(road);
			if (leftRoad) {
				Vec n2, d2;
				if (leftRoad->endJunction == this) {
					n2 = -leftRoad->getPolyLine()->getLastPointNormal();
					d2 = -leftRoad->getPolyLine()->getLastPointDirection();
				} else {
					n2 = leftRoad->getPolyLine()->getFirstPointNormal();
					d2 = leftRoad->getPolyLine()->getFirstPointDirection();
				}
				halfWidth2 = leftRoad->getWidth() * 0.5 + ROAD_MARGINSGAP;
				Vec dp = (1.0 / (1.0 + n1 * n2)) * (n1 + n2);
				Vec pp;
				float w;
				if (halfWidth > halfWidth2) {
					pp = p - n2 * (halfWidth - halfWidth2);
					w = halfWidth;
				} else {
					pp = p - n1 * (halfWidth2 - halfWidth);
					w = halfWidth2;
				}
				pos = pp + w * dp;
				float dd = (pp - (pos - w * n1)).norm()+ROAD_ENDGAP;
				if (forward) {
					if (len - dd < roadEnd) {
						roadEnd = len - dd;
					}
				} else {
					if (dd > roadStart) {
						roadStart = dd;
					}
				}
			}
			rightRoad = getRightRoad(road);
			if (rightRoad) {
				Vec n2, d2;
				if (rightRoad->endJunction == this) {
					n2 = -rightRoad->getPolyLine()->getLastPointNormal();
					d2 = -rightRoad->getPolyLine()->getLastPointDirection();
				} else {
					n2 = rightRoad->getPolyLine()->getFirstPointNormal();
					d2 = rightRoad->getPolyLine()->getFirstPointDirection();
				}
				halfWidth2 = rightRoad->getWidth() * 0.5 + ROAD_MARGINSGAP;
				Vec dp = (1.0 / (1.0 + n1 * n2)) * (n1 + n2);
				Vec pp;
				float w;
				if (halfWidth > halfWidth2) {
					pp = p + n2 * (halfWidth - halfWidth2);
					w = halfWidth;
				} else {
					pp = p + n1 * (halfWidth2 - halfWidth);
					w = halfWidth2;
				}
				pos = (pp - w * dp);
				float dd = (pp - (pos + w * n1)).norm()+ROAD_ENDGAP;

				if (forward) {
					if (len - dd < roadEnd) {
						roadEnd = len - dd;
					}
				} else {
					if (dd > roadStart) {
						roadStart = dd;
					}
				}
			}
		}

		if(forward){
			road->setRoadEnd( roadEnd );
			road->getPolyLine()->pointDirFromFirstPoint(pos,dir,perp, road->getRoadEnd() );
			p = pos + perp*halfWidth;
			bb.addPoint(p);
			p = pos - perp*halfWidth;
			bb.addPoint(p);
		}else{
			road->setRoadStart( roadStart );
			road->getPolyLine()->pointDirFromFirstPoint(pos,dir,perp, road->getRoadStart() );
			p = pos + perp*halfWidth;
			bb.addPoint(p);
			p = pos - perp*halfWidth;
			bb.addPoint(p);
		}
	}
}


void Junction::updateConnections(){
	clearConnections();
	int32 nr = roads.size();
	if(nr < 2){
		updateNextLanes();
		return;
	}

	vns::List<Lane*> mylanes;
	vns::List<Road*> left;
	vns::List<Road*> right;
	Road* center;
	bool hasLeft = false;
	bool hasRight = false;
	double ad;

	for (int32 k = 0; k < nr; k++) {
		Road* road = roads[k];
		mylanes.clear();
		left.clear();
		right.clear();
		center = 0;
		hasLeft = false;
		hasRight = false;
		/*
		 * Get road lanes from left to right
		 * 0 1 2 3 4 5
		 */
		int32 nl = road->getNumberOfLanes();
		if (road->getEndJunction(Forward) == this) {
			for (int32 l = 0; l < nl; l++) {
				Lane* myln = road->getLane(l);
				if (myln->getEndJunction() == this) {
					mylanes.push_front( myln );
				}
			}
		}else{
			for (int32 l = 0; l < nl; l++) {
				Lane* myln = road->getLane(l);
				if (myln->getEndJunction() == this) {
					mylanes.push_back( myln );
				}
			}
		}
		if( mylanes.size() == 0 ){
			// There are no lanes approaching the junction
			continue;
		}
		/*
		 * get destination Roads
		 */
		Vec roaddir = road->getLastPointDirection( this );
		double minD = vns::MAX_DOUBLE;
		int32 nrr = roads.size();
		for (int32 i = 0; i < nrr; i++) {
			Road* rd = roads.at(i);
			if(road == rd ) continue;
			if (isExitRoad(rd)) {
				ad = roaddir.angleDiffTo( rd->getFirstPointDirection( this ) );
				if (ad <= 0) {
					right.append( rd );
				} else {
					left.append( rd );
				}
				if( abs(ad) < minD ){
					center = rd;
					minD = abs(ad);
				}
			}
		}
		if( center == 0) continue;
		/*
		 * Number of lanes in front
		 */
		nl = center->getNumberOfLanes();
		int32 nlf = 0;
		for(int32 l=0;l<nl;l++){
			Lane* lf = center->getLane(l);
			if( lf->getStartJunction() == this ){
				nlf++;
			}
		}
		/*
		 * Calculate connections
		 */
		vns::List<Road*>::iterator it;
		for(it=right.begin();it != right.end();it++){
			Road* rt = *it;
			if( rt != center ){
				mylanes.last()->addConnectionTo( rt );
				hasRight = true;
			}
		}
		for(it=left.begin();it != left.end();it++){
			Road* rt = *it;
			if( rt != center ){
				mylanes.first()->addConnectionTo( rt );
				hasLeft = true;
			}
		}
		/*
		 * center
		 */
		if( hasRight &&  mylanes.size() > nlf ){
			mylanes.removeLast();
		}
		if( hasLeft &&  mylanes.size() > nlf ){
			mylanes.removeFirst();
		}
		vns::List<Lane*>::iterator itt;
		for(itt=mylanes.begin();itt != mylanes.end();itt++){
			Lane* lf = *itt;
			lf->addConnectionTo( center );
		}
		road->updateConnectionsTODO();
	}
	updateNextLanes();
}

void Junction::clearConnections(){
	int32 nr = roads.size();
	for(int32 i=0;i<nr;i++){
		Road* road = roads[i];
		road->clearConnections( (road->endJunction == this?Forward:Backward) );
	}
}

void Junction::updateCenter(double x,double y,double z){
	int32 nr = roads.size();
	for(int32 i=0;i<nr;i++){
		Road* road = roads[i];
		if(road->endJunction == this){
			road->getPolyLine()->updatePoint(road->getNumberOfPoints()-1,x,y,z);
		}else{
			road->getPolyLine()->updatePoint(0,x,y,z);
		}
		road->update();
	}
	roadNetwork->junctionsHash.remove( location );
	location.setX(x);
	location.setY(y);
	location.setZ(z);
	while(roadNetwork->junctionsHash.contains(location)){
		location.setX(location.getX()+0.001);
	}
	roadNetwork->junctionsHash.insert( location, this );
	updateJunction();
}

void Junction::updateNextLanes(){
	int32 nr = roads.size();
	for(int a=0;a<nr;a++){
		Road* roadA = roads.at(a);
		for(int b=0;b<nr;b++){
			if(a==b) continue;
			Road* roadB = roads.at(b);
			roadNetwork->computeLaneConnections(this,roadA,roadB);
		}
	}
}


DataStream& operator <<(DataStream& f, const Junction* junction) {
	f<<junction->type;
	f<<junction->id;
	f<<junction->bb;
    f<<junction->location;
    return f;
}

DataStream& operator >>(DataStream& f, Junction* junction) {
	f>>junction->type;
	f>>junction->id;
	f>>junction->bb;
    f>>junction->location;
    return f;
}

}
