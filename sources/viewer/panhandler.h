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

#ifndef VNS_PANHANDLER_H_
#define VNS_PANHANDLER_H_

#include "inputeventhandler.h"

namespace vns {

/*! \cond PRIVATE */

class PanHandler: public InputEventHandler {

private:
    int lastX;
    int lastY;

public:
    PanHandler();
	virtual ~PanHandler();

	virtual void initialize(SceneViewer* viewer);
    virtual void mousePressEvent(SceneViewer* viewer, QMouseEvent* event );
    virtual void mouseReleaseEvent(SceneViewer* viewer, QMouseEvent* event );
    virtual void mouseMoveEvent(SceneViewer* viewer, QMouseEvent* event );
    virtual void wheelEvent(SceneViewer* viewer, QWheelEvent* event );
};

/*! \endcond */

}


#endif /* PANHANDLER_H_ */
