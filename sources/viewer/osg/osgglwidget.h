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

#ifndef VNS_OSGGLWIDGET_H_
#define VNS_OSGGLWIDGET_H_


#include <osgViewer/Viewer>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include "sceneviewer.h"

namespace vns {

class OsgGLWidget : public osgViewer::Viewer, public SceneViewer {
    public:

	OsgGLWidget(QWidget * parent = 0, const QGLWidget * shareWidget = 0);
	virtual ~OsgGLWidget();

    osgViewer::GraphicsWindow* getGraphicsWindow() { return _gw.get(); }
    const osgViewer::GraphicsWindow* getGraphicsWindow() const { return _gw.get(); }

    protected:
        osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> _gw;

        virtual void resizeGL( int width, int height );
        virtual void keyPressEvent( QKeyEvent* event );
        virtual void keyReleaseEvent( QKeyEvent* event );
        virtual void mousePressEvent( QMouseEvent* event );
        virtual void mouseReleaseEvent( QMouseEvent* event );
        virtual void mouseMoveEvent( QMouseEvent* event );
        virtual void paintGL();
};

}
#endif /* VNS_OSGGLWIDGET_H_ */
