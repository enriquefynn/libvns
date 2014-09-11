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

#ifndef VNS_MAPVIEWER_H
#define VNS_MAPVIEWER_H

#include "sceneviewer.h"
#include "simulator.h"
#include "vector.h"

namespace vns {

class MapViewer: public SceneViewer {

    Q_OBJECT

public:
    MapViewer( const Simulator* sim, QWidget *parent = 0 );
    ~MapViewer();
    void setPainter( Painter* painter );
    void setCenter( const vns::Vec& center );
    void setCenter( double x, double y );
    inline double getScale() const { return scale; };
    void setScale(double s);
    inline const Vec& getCenter() const { return center; };
    vns::Vec mouseToRealWorldCoordinates( int x, int y );
    void setInputEventHandler(InputEventHandler* handler);


private:
    InputEventHandler* eventHandler;
    InputEventHandler* defaultEventHandler;
    Painter* painter;
    Painter* defaultPainter;
    const Simulator* simulator;

    double scale;
    vns::Vec center;
    int Lod;

    int allscenarioList;
    vns::Vector<uint32> roads;
    vns::Vector<uint32> junctions;

protected:

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
};

}

#endif
