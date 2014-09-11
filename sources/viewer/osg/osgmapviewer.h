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

#ifndef VNS_OSGMAPVIEWER_H_
#define VNS_OSGMAPVIEWER_H_

#include "osgglwidget.h"
#include "vec.h"
#include "roadnetwork.h"
#include "simulator.h"
#include "painter.h"
#include "inputeventhandler.h"
#include "osgscenario.h"
#include "osgvehicle.h"
#include "osgfactory.h"

namespace vns {

class OsgMapViewer : public vns::OsgGLWidget {
public:
	OsgMapViewer(const vns::Simulator* sim, QWidget *parent = 0);
	virtual ~OsgMapViewer();

    void onLaneObjectCreated(const vns::LaneObject* obj);
    void onLaneObjectRemoved(const vns::LaneObject* obj);

    inline const Vec& getCenter() const { return center; };
    inline double getScale() const { return 0; };
    inline void setScale(double){};
    inline vns::Vec mouseToRealWorldCoordinates( int, int){ return vns::Vec(); };

private:
    const Simulator* simulator;

    OSGScenario osgScenario;
    osg::Group* root;

    OsgFactory osgFactory;

    QHash<int32,OsgVehicle*> osgVehicles;

    vns::Vec center;
};

}

#endif /* VNS_OSGMAPVIEWER_H_ */
