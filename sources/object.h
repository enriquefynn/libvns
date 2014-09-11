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

#ifndef VNS_OBJECT_H_
#define VNS_OBJECT_H_

#include "core.h"
#include "vec.h"

namespace vns {

class Simulator;

class Object {
public:
	enum Type {
	    ObstacleT,
	    VehicleT,
	    GhostT
	};

    Object(Object::Type type, float width, float length){
    	this->type = type;
    	this->width = width;
    	this->length = length;
    };

    virtual inline ~Object(){};
    inline Object::Type getObjectType() const { return type; };
    virtual const Vec& getPosition() const { return pos; };
    virtual const Vec& getDirection() const { return dir; };
    virtual float getSpeed() const { return 0; };
    inline float getWidth() const { return width; };
    inline float getLength() const { return length; };

protected:
    virtual void simulationStep(Simulator* simulator) = 0;
    virtual void updateStep(Simulator* simulator) = 0;

    Vec pos;
    Vec dir;
    float length;
    float width;
    Object::Type type;
};

}

#endif /* VNS_OBJECT_H_ */
