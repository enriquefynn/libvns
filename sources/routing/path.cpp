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

#include "path.h"
#include "road.h"
#include "routes.h"

namespace vns {

Path::Path(){
	id = -1;
	first = 0;
	last = 0;
	size = 0;
	loop = false;
}

Path::Path(Road* road,Way way){
	id = -1;
	first = 0;
	last = 0;
	size = 0;
	loop = false;
	appendNode(road,way);
}

Path::~Path(){
	PathNode* tmp;
	while(first){
		tmp = first;
		first = first->next;
		delete tmp;
	}
	first = 0;
	last = 0;
	size = 0;
}

void Path::write(DataStream& f,const Path* path){
	f << path->loop;
	f << path->size;
	PathNode* curr = path->first;
	while(curr){
		PathNode::write(f,curr);
		curr = curr->next;
	}
}

vns::Path* Path::read(DataStream& f, Routes* routes){
	Path* path = new Path();
	f >> path->loop;
	f >> path->size;
	for(int32 i=0;i<path->size;i++){
		PathNode* node = PathNode::read(f,routes);
		if(path->first == 0){
			path->first = path->last = node;
		}else{
			path->last->next = node;
			node->prev = path->last;
			path->last = node;
		}
		path->last = node;
	}
	return path;
}


void Path::printPath(){
    Path::Iterator it = getFirstNodeIterator();
    while(it.isValid()){
    	fprintf(stderr,"%d %s [",it.getRoad()->getID(),(it.getWay()==vns::Forward?"->":"<-"));
    	PathNode::StopsIterator st = it.getStopsIterator();
    	while(st.isValid()){
    		fprintf(stderr,"%f",st.getPosition());
    		st.next();
    	}
    	fprintf(stderr,"]");
    	it.next();
    }
    fprintf(stderr,"::\n");
}

bool Path::isValidPath(const Path* path) {
    if( path->size == 0 ) return false;
    Path::ConstIterator i = path->getFirstNodeConstIterator();
    while( i.hasNext() ){
    	const Road* road = i.getRoad();
    	const Road* nextRoad = i.getNextRoad();
        if( road==0 || nextRoad == 0){
        	return false;
        }
        if(!road->hasConnectionTo(nextRoad)){
        	return false;
        }
        i.next();
    }
    return true;
}

bool Path::equalPaths(const Path* path1, const Path* path2) {
    if (path1->size != path2->size){
    	return false;
    }
    Path::ConstIterator i1 = path1->getFirstNodeConstIterator();
    Path::ConstIterator i2 = path2->getFirstNodeConstIterator();
    while(i1.isValid()){
    	if((i1.getRoad() != i2.getRoad()) || (i1.getWay() != i2.getWay())){
    		return false;
    	}
    	i1.next();
    	i2.next();
    }
    return true;
}


PathNode* Path::appendNode(Road* road, Way way){
	PathNode* curr = new PathNode(road,way);
	size++;
	if(first == 0){
		first = last = curr;
	}else{
		last->next = curr;
		curr->prev = last;
		last = curr;
	}
	return curr;
}

PathNode* Path::prependNode(Road* road, Way way){
	PathNode* curr = new PathNode(road,way);
	size++;
	if(first == 0){
		first = last = curr;
	}else{
		first->prev = curr;
		curr->next = first;
		first = curr;
	}
	return curr;
}

Path* Path::clone() {
	Path* path = new Path();
	Path::Iterator it = this->getFirstNodeIterator();
	while(it.isValid()){
		PathNode* curr = it.get();
		PathNode* node = path->appendNode(curr->getRoad(),curr->getWay());
		for(int i=0;i<curr->stops.size();i++){
			PathStop* st = curr->stops.at(i);
			node->addStop(st->getType(),st->getPosition(),st->getDuration());
		}
		it.next();
	}
	return path;
}

bool Path::append(Path* path){
	Path::Iterator it = path->getFirstNodeIterator();
	if(last!=0){
		if(!it.isValid() || last->getRoad()!=it.getRoad() || last->getWay() != it.getWay()){
			return false;
		}
		it.next();
		if( !it.isValid() || last->getRoad()==0 || it.getRoad() == 0){
			return false;
		}
		if(!last->getRoad()->hasConnectionTo(it.getRoad())){
			return false;
		}
	}
	while(it.isValid()){
		PathNode* curr = it.get();
		PathNode* node = appendNode(curr->getRoad(),curr->getWay());
		for(int i=0;i<curr->stops.size();i++){
			PathStop* st = curr->stops.at(i);
			node->addStop(st->getType(),st->getPosition(),st->getDuration());
		}
		it.next();
	}
	return true;
}

void Path::join(Path* path){
	if(last){
		if(path->first){
			last->next = path->first;
			path->first->prev = last;
			last = path->last;
			size += path->size;
			loop = false;
		}
	}else{
		if(path->first){
			first = path->first;
			last = path->last;
			size = path->size;
			loop = path->loop;
		}
	}

	path->first = 0;
	path->last = 0;
	path->size = 0;
	path->loop = false;
	delete path;
}

void Path::setLoop(bool v){
	loop = v;
	if(loop){
		last->next = first;
		first->prev = last;
	}else{
		last->next = 0;
		first->prev = 0;
	}
}




}
