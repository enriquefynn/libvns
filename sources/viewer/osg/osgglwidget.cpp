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

#include "osgglwidget.h"

namespace vns {

OsgGLWidget::OsgGLWidget(QWidget * parent, const QGLWidget * shareWidget):osgViewer::Viewer(), SceneViewer(parent,shareWidget){
    _gw = new osgViewer::GraphicsWindowEmbedded(0,0,width(),height());
    setFocusPolicy(Qt::ClickFocus);
	getCamera()->setViewport(new osg::Viewport(0,0,width(),height()));
    getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(width())/static_cast<double>(height()), 1.0f, 10000.0f);
    getCamera()->setGraphicsContext(getGraphicsWindow());
    setThreadingModel(osgViewer::Viewer::SingleThreaded);
    osg::StateSet* stateset = getCamera()->getOrCreateStateSet();
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
	QGLFormat fmt = QGLFormat::defaultFormat();
	fmt.setSampleBuffers(true);
	QGLFormat::setDefaultFormat(fmt);
	setFormat(fmt);
}


OsgGLWidget::~OsgGLWidget() {

}

void OsgGLWidget::resizeGL( int width, int height ){
    _gw->getEventQueue()->windowResize(0, 0, width, height );
    _gw->resized(0,0,width,height);
}

void OsgGLWidget::keyPressEvent( QKeyEvent* event ){
	 _gw->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
}

void OsgGLWidget::keyReleaseEvent( QKeyEvent* event ){
	_gw->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
}

void OsgGLWidget::mousePressEvent( QMouseEvent* event ){
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    _gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
}

void OsgGLWidget::mouseReleaseEvent( QMouseEvent* event ){
    int button = 0;
    switch(event->button())
    {
        case(Qt::LeftButton): button = 1; break;
        case(Qt::MidButton): button = 2; break;
        case(Qt::RightButton): button = 3; break;
        case(Qt::NoButton): button = 0; break;
        default: button = 0; break;
    }
    _gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
}

void OsgGLWidget::mouseMoveEvent( QMouseEvent* event ){
    _gw->getEventQueue()->mouseMotion(event->x(), event->y());
}

void OsgGLWidget::paintGL(){
    frame();
}

}

