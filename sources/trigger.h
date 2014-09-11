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

#ifndef VNS_TRIGGER_H_
#define VNS_TRIGGER_H_

#include "core.h"

namespace vns {

class Vehicle;

class Trigger {
public:
	virtual ~Trigger(){};
	virtual void enter( Vehicle* vehicle, double simtime ) = 0;
	virtual void leave( Vehicle* vehicle, double simtime ) = 0;
	virtual int32 read( double simtime ) = 0;
	virtual float getLength() = 0;
};

}

#endif /* VNS_TRIGGER_H_ */
