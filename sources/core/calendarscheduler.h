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

#ifndef VNS_CALENDARSCHEDULER_H_
#define VNS_CALENDARSCHEDULER_H_

#include "eventscheduler.h"

namespace vns {

class CalendarScheduler : public EventScheduler {
private:
	/*! \cond PRIVATE */
	struct Bucket {
		Event* event;
	    Bucket* next;
	    Bucket(Event* e,Bucket* next=0) : event(e),next(next) {};
	};
	/*! \endcond */
    unsigned int mtableSize;
    Bucket** mtable;
    float dt;
    int msize;

public:
	CalendarScheduler(double dt,unsigned int tableSize);
	virtual ~CalendarScheduler();

	void print();
	void schedule(Event* event);
	void invokeEvents(double start,double dt);
	inline int getNumberOfEvents() const { return msize; }
	inline bool isEmpty() const { return msize==0; }
	void clearEvents();
};



}

#endif
