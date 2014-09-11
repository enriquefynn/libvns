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

#include <osg/Node>
#include <osgDB/ReadFile>
#include <osg/Drawable>
#include <sstream>
#include "vehiclefactory.h"
#include "osgfactory.h"
#include <QtCore>

#define MODELS_DIR "/usr/rjf/libvns/vehicles3d/"

namespace vns {

OsgFactory::OsgFactory() {
	QDir dir(MODELS_DIR);
	int ntypes = vns::VehicleFactory::getNumberOfVehicleTypes();
	vehicles.resize(ntypes);
	for(int i=0;i<ntypes;i++){
		dir.cd(QString("%1").arg(i));
		dir.setFilter(QDir::Files | QDir::NoSymLinks);
		QFileInfoList list = dir.entryInfoList();
		vehicles[i].resize(list.size());
		std::cout << "     Bytes Filename" << std::endl;
		for(int f = 0; f < list.size(); ++f) {
	         QFileInfo fileInfo = list.at(f);
	         vehicles[i][f] = newNode(fileInfo.fileName().toAscii().data(),osg::DegreesToRadians(90.0),0.3);
	         std::cout << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10)
	                                                 .arg(fileInfo.fileName()));
	         std::cout << std::endl;
		}
		dir.cdUp();
	}
}

OsgFactory::~OsgFactory() {
	for(int i=0;i<vehicles.size();i++){
		int nmodels = vehicles[i].size();
		for(int j=0;j<nmodels;j++){
			//delete vehicles[i][j];
		}
		vehicles[i].clear();
	}
	vehicles.clear();
}

OsgVehicle* OsgFactory::createVehicle(const vns::Vehicle* vehicle){
	int t = vehicle->getVehicleType();
	int m = vehicle->getVehicleModel();
	return new OsgVehicle( dynamic_cast<osg::Node*>(vehicles[t][m]->clone(osg::CopyOp::SHALLOW_COPY)) , vehicle );
}

osg::Node* OsgFactory::newNode(const char* filename, double dir, double scale){
	osg::Node* node = osgDB::readNodeFile(filename);
	osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform();
	pat->setScale(osg::Vec3(scale, scale, scale));
	pat->setAttitude(osg::Quat(dir,osg::Vec3(0,0,1)));
	pat->addChild( node );
	return pat;
}

}
