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

#ifndef VNS_SCENEVIEWER_H_
#define VNS_SCENEVIEWER_H_

#include <QtOpenGL/QtOpenGL>
#include <QtOpenGL/QGLWidget>
#include "vec.h"
#include "painter.h"
#include "inputeventhandler.h"
#include "laneobject.h"

namespace vns {

class LaneObject;

class SceneViewer : public QGLWidget {
public:
    SceneViewer( QWidget * parent = 0, const QGLWidget * shareWidget = 0);
    virtual ~SceneViewer();

    virtual const Vec& getCenter() const = 0;
    virtual void setPainter( Painter* painter);
    virtual void setCenter( double x, double y);
    virtual double getScale() const = 0;
    virtual void setScale(double s) = 0;
    virtual vns::Vec mouseToRealWorldCoordinates( int x, int y ) = 0;
    virtual void setInputEventHandler( InputEventHandler* handler );

    virtual void onLaneObjectCreated(const vns::LaneObject* obj);
    virtual void onLaneObjectRemoved(const vns::LaneObject* obj);
};

}

#endif /* VNS_SCENEVIEWER_H_ */
