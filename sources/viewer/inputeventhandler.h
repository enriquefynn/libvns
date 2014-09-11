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

#ifndef VNS_INPUTEVENTHANDLER_H_
#define VNS_INPUTEVENTHANDLER_H_

#include "core.h"

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

namespace vns {

class SceneViewer;

/*! \cond PRIVATE */

class InputEventHandler {
public:
	virtual ~InputEventHandler(){};

	virtual void initialize( SceneViewer* ){};
    virtual void mousePressEvent(SceneViewer*, QMouseEvent* ){};
    virtual void mouseReleaseEvent(SceneViewer*, QMouseEvent*){};
    virtual void mouseMoveEvent(SceneViewer*, QMouseEvent*){};
    virtual void wheelEvent(SceneViewer*, QWheelEvent*){};
    virtual void keyPressEvent(SceneViewer*, QKeyEvent*){};
    virtual void keyReleaseEvent(SceneViewer*, QKeyEvent*){};
};

/*! \endcond */

}



#endif /* INPUTEVENTHANDLER_H_ */
