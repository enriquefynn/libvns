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

#ifndef VNS_OSGVEHICLE_H_
#define VNS_OSGVEHICLE_H_

#include <osg/PositionAttitudeTransform>
#include "vehicle.h"

namespace vns {

class OsgVehicleUpdateCallback : public osg::NodeCallback {
private:
	const vns::Vehicle* vehicle;
	vns::Vec pos,dir;
public:
	OsgVehicleUpdateCallback(const Vehicle* v){
		vehicle = v;
	}

	virtual void operator()(osg::Node * node, osg::NodeVisitor* nv){
		osg::PositionAttitudeTransform* pat = dynamic_cast <osg::PositionAttitudeTransform*> (node);
		if (vehicle && pat){
			pos = vehicle->getPosition();
			dir = vehicle->getDirection();
			pat->setPosition(osg::Vec3(pos.getX(),pos.getY(),pos.getZ()));
			pat->setAttitude(osg::Quat(dir.dir_rad(),osg::Vec3(0,0,1)));
		}
		traverse(node, nv);
	}


};

class OsgVehicle : public osg::PositionAttitudeTransform {

public:
	OsgVehicle(osg::Node* node, const Vehicle* vehicle){
		addChild( node );
		updateCallback = new OsgVehicleUpdateCallback(vehicle);
		setUpdateCallback( updateCallback );
	};

	virtual ~OsgVehicle(){  };

private:
	OsgVehicleUpdateCallback* updateCallback;
};

}

#endif /* VNS_OSGVEHICLE_H_ */
