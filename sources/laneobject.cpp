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

#include "laneobject.h"

namespace vns {

LaneObject::LaneObject(Object::Type type, float width, float length) : Object(type,width,length){
	lanePos = 0;
	lane = 0;
	prev = 0;
	next = 0;
	nextLane = 0;
	prevLane = 0;
}

}

