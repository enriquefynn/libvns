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

#include "osgmapviewer.h"
#include "osgvehicle.h"
#include <osgGA/TrackballManipulator>

namespace vns {

OsgMapViewer::OsgMapViewer(const vns::Simulator* sim, QWidget *parent) : OsgGLWidget(parent) {
	simulator = sim;

	const vns::RoadNetwork* network = sim->getRoadNetwork();
    root = osgScenario.generateRoadNetwork(network);
    setSceneData(root);
    setCameraManipulator(new osgGA::TrackballManipulator);
    getCamera()->setClearColor( osg::Vec4(0.9,0.9,0.9,1.0) );
    getCamera()->setSmallFeatureCullingPixelSize(5);
}

OsgMapViewer::~OsgMapViewer() {

}

void OsgMapViewer::onLaneObjectCreated(const vns::LaneObject* obj){
	const vns::Vehicle* vehicle = dynamic_cast<const vns::Vehicle*>(obj);
	if(obj==0) return;
	OsgVehicle* osgnode = osgFactory.createVehicle( vehicle );
	osgVehicles.insert( vehicle->getID() , osgnode);
	root->addChild( osgnode );
}

void OsgMapViewer::onLaneObjectRemoved(const vns::LaneObject* obj){
	const vns::Vehicle* vehicle = dynamic_cast<const vns::Vehicle*>(obj);
	if(obj==0) return;
	OsgVehicle* osgnode = osgVehicles.value( vehicle->getID() );
	osgVehicles.remove( vehicle->getID() );
	root->removeChild( osgnode );
}


}
