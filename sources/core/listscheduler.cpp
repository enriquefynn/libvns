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

#include "listscheduler.h"
#include <iostream>

namespace vns {

ListScheduler::ListScheduler() : EventScheduler() {

}

ListScheduler::~ListScheduler() {
	clearEvents();
}

void ListScheduler::schedule(Event* event){
	List<Event*>::Iterator it=events.begin();
	while( it != events.end() ){
		Event* e = *it;
		if( event->getTime() <= e->getTime() ){
			events.insert(it,event);
			return;
		}
		it++;
	}
	events.append(event);
}

void ListScheduler::invokeEvents(double start,double dt){
	double end = start+dt;
	while( !events.isEmpty() ){
		Event* e = events.first();
		std::cout << "exec: "<< e->getTime() << "\n";
		if( e->getTime()>end ){
			return;
		}
		e->invoke();
		events.removeFirst();
		delete e;
	}
}

void ListScheduler::clearEvents(){
	List<Event*>::Iterator it=events.begin();
	while( it != events.end() ){
		Event* e= *it;
		it = events.erase(it);
		delete e;
	}
}

void ListScheduler::print(){
	List<Event*>::Iterator it=events.begin();
	while( it != events.end() ){
		Event* e = *it;
		std::cout << e->getTime() << " , ";
		it++;
	}
	std::cout << "\n";
}


}
