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

#include "calendarscheduler.h"
#include <iostream>

namespace vns {

CalendarScheduler::CalendarScheduler(double ddt,unsigned int tableSize) : EventScheduler() {
	dt = ddt;
    mtable = new Bucket*[tableSize];
    for(unsigned int i=0;i<tableSize;i++){
    	mtable[i] = 0;
    }
    mtableSize = tableSize;
    msize = 0;
}

CalendarScheduler::~CalendarScheduler() {
	clearEvents();
	delete[] mtable;
}

void CalendarScheduler::schedule(Event* event){
	int h = int(event->getTime()/dt) % mtableSize;
	Bucket* bucket = mtable[h];
	Bucket* prev = 0;
	while(bucket){
		if( event->getTime() <= bucket->event->getTime() ){
			if(prev==0){
				mtable[h] = new Bucket(event,bucket);
			}else{
				prev->next = new Bucket(event,bucket);
			}
			msize++;
			return;
		}
		prev = bucket;
		bucket = bucket->next;
	}
	if(prev==0){
		mtable[h] = new Bucket(event,bucket);
	}else{
		prev->next = new Bucket(event,bucket);
	}
	msize++;
}

void CalendarScheduler::invokeEvents(double start, double interval){
	double end = start+interval;
	int s = int(start/dt) % mtableSize;
	int e = (int(end/dt)+1) % mtableSize;
	while( s != e ){
		while( mtable[s] && mtable[s]->event->getTime() <= end ){
			Bucket* bucket = mtable[s];
			mtable[s] = mtable[s]->next;
			msize--;
			bucket->event->invoke();
			delete bucket->event;
			delete bucket;
		}
		s = (s+1)%mtableSize;
	}
}

void CalendarScheduler::clearEvents(){
	unsigned int i = 0;
	while( i < mtableSize ){
		Bucket* bucket = mtable[i];
		while( bucket ){
			Bucket* d = bucket;
			bucket = bucket->next;
			delete d->event;
			delete d;
		}
		mtable[i] = 0;
		i++;
	}
	msize = 0;
}

void CalendarScheduler::print(){
	for(unsigned int i=0;i<mtableSize;i++){
		if(mtable[i]){
			std::cout << i << " - ";
			Bucket* b = mtable[i];
			while(b){
				std::cout << b->event->getTime() << " , ";
				b = b->next;
			}
		}else{
			std::cout << i << " - null";
		}
		std::cout << "\n";
	}
}


}
