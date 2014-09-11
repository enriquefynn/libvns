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

#include "panhandler.h"
#include "sceneviewer.h"
#include <QtGui/QtGui>

namespace vns {

PanHandler::PanHandler():InputEventHandler(){

}

PanHandler::~PanHandler(){

}

void PanHandler::initialize(SceneViewer* viewer){
	viewer->setCursor(Qt::OpenHandCursor);
}

void PanHandler::mousePressEvent(SceneViewer* viewer, QMouseEvent* event ){
	if (event->buttons() & Qt::LeftButton) {
		viewer->setCursor(Qt::ClosedHandCursor);
	}
}

void PanHandler::mouseReleaseEvent(SceneViewer* viewer, QMouseEvent* ){
	viewer->setCursor(Qt::OpenHandCursor);
}

void PanHandler::mouseMoveEvent(SceneViewer* viewer, QMouseEvent* event){
	if (event->buttons() & Qt::LeftButton) {
		int x = event->x(), y = event->y();
		double scale = viewer->getScale();
		vns::Vec c = viewer->getCenter();
		double dx = (x - lastX) * scale, dy = (y - lastY) * scale;
		viewer->setCenter( c.getX()-dx, c.getY()+dy );
	}
	lastX = event->pos().x();
	lastY = event->pos().y();
}

void PanHandler::wheelEvent(SceneViewer* viewer, QWheelEvent* event ){
	viewer->setScale( viewer->getScale() * (1 + 0.0005 * event->delta()) );
}

}
