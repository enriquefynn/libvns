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

#ifndef VNS_OSGFACTORY_H_
#define VNS_OSGFACTORY_H_

#include "vector.h"
#include "osgvehicle.h"

namespace osg{
	class Node;
}

namespace vns {

class OsgFactory {
public:
	OsgFactory();
	virtual ~OsgFactory();
	OsgVehicle* createVehicle(const Vehicle* vehicle);

	osg::Node* newNode(const char* filename, double dir,double scale);
	Vector< Vector<osg::Node*> > vehicles;
};

}
#endif /* VNS_OSGFACTORY_H_ */
