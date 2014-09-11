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

#include "sceneviewer.h"

namespace vns {

SceneViewer::SceneViewer( QWidget * parent, const QGLWidget * shareWidget ):
	QGLWidget(parent,shareWidget){

}

SceneViewer::~SceneViewer(){

}

void SceneViewer::setPainter( Painter* ){}
void SceneViewer::setCenter( double , double ){}
void SceneViewer::setInputEventHandler(InputEventHandler* ){}
void SceneViewer::onLaneObjectCreated(const vns::LaneObject* ){}
void SceneViewer::onLaneObjectRemoved(const vns::LaneObject* ){}

}



