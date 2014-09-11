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

#ifndef VNS_LISTSCHEDULER_H_
#define VNS_LISTSCHEDULER_H_

#include "eventscheduler.h"
#include "list.h"

namespace vns {

class ListScheduler : public EventScheduler {
private:
	List<Event*> events;

public:
	ListScheduler();
	virtual ~ListScheduler();

	void schedule(Event* event);
	void invokeEvents(double start,double dt);
	inline int getNumberOfEvents() const { return events.size(); }
	inline bool isEmpty() const { return events.isEmpty(); }
	void clearEvents();
	void print();
};



}

#endif
