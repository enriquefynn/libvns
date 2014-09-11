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

#ifndef VNS_OSGSCENARIO_H_
#define VNS_OSGSCENARIO_H_

#include <osgTerrain/TerrainTile>
#include <osgTerrain/Terrain>
#include <osg/LightSource>
#include <osg/Node>
#include <osg/Group>
#include <osg/LOD>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/AlphaFunc>

#include "roadnetwork.h"
#include "trajectory.h"
#include "srtm.h"

namespace vns {

class OSGScenario {
public:
	OSGScenario();
	virtual ~OSGScenario();

	osg::Group* generateRoadNetwork(const vns::RoadNetwork* network);

	osg::Node* createRoad(const vns::Road* road);
	osg::Node* createRoadMarkers(const vns::Road* road);
	osg::Node* createJunction(const vns::Junction* junction);
	osg::Node* createJunctionMarkers(const vns::Junction* junction);
	void addContinuousLine(osg::Geode* geode, const PolyLine* line, double offset, double start, double end, double lineWidth=0.12);
	void addDiscontinuousLine(osg::Geode* geode,const PolyLine* line, double offset, double start, double end, double lineWidth=0.12,double spacing=2.0);
	osg::Geometry* createTrajectoryMarkers(const vns::Vec& p1,const vns::Vec& d1,const vns::Vec& n1, const vns::Vec& p4,const vns::Vec& d4,const vns::Vec& n4,double lineWidth=0.12);
	osg::Group* createHeightField(const vns::RoadNetwork* network);

private:
	vns::Trajectory trajectory;
	osg::StateSet* asphaltStateSet;
	osg::StateSet* markersStateSet;
	osg::StateSet* grassStateSet;
};

}

#endif /* OSGSCENARIO_H_ */
