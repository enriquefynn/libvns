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


#include "painter.h"
#include "mapviewer.h"
#include "panhandler.h"

namespace vns {

MapViewer::MapViewer(const Simulator* sim, QWidget *parent) : SceneViewer(parent) {
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	painter = defaultPainter = new Painter();
	scale = -1.0;
	Lod = 1;
	simulator = sim;

	eventHandler = defaultEventHandler = new PanHandler();

	QGLFormat fmt = QGLFormat::defaultFormat();
	fmt.setSampleBuffers(true);
	QGLFormat::setDefaultFormat(fmt);
	setFormat(fmt);
}

MapViewer::~MapViewer(){
	makeCurrent();
	delete defaultPainter;
	delete defaultEventHandler;
}

void MapViewer::setInputEventHandler(InputEventHandler* handler){
	if(handler){
		eventHandler = handler;
	}else{
		eventHandler = defaultEventHandler;
	}
	eventHandler->initialize(this);
}

void MapViewer::setPainter(Painter* ptr){
	if(ptr){
		painter = ptr;
	}else{
		painter = defaultPainter;
	}
	updateGL();
}

void MapViewer::initializeGL() {

	glClearColor(1, 1, 1, 1);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable antialiasing
	glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);

	painter->initialize(this);

	const RoadNetwork* network = simulator->getRoadNetwork();
	if(!network){
		return;
	}

    allscenarioList = glGenLists(1);
	glNewList(allscenarioList, GL_COMPILE);
	glColor3f(0.45f, 0.45f, 0.45f);
	glLineWidth(1.0);
	int32 nr = network->getNumberOfRoads();
	for (int32 i = 0; i < nr; i++) {
		const Road* road = network->getRoad(i);
		int32 np = road->getNumberOfPoints();
		glBegin( GL_LINE_STRIP);
		for (int32 j = 0; j < np; j++) {
			const Vec& p = road->getPoint(j);
			glVertex2d(p.getX(), p.getY());
		}
		glEnd();
	}
	glEndList();

    int32 nj = network->getNumberOfJunctions();
    junctions.resize(nj);
	for (int32 i = 0; i < nj; i++) {
		const Junction* junction = network->getJunction(i);
		GLuint list = glGenLists(1);
		glNewList( list , GL_COMPILE );
		painter->drawJunctionAsphalt(this,junction,4);
		painter->drawJunctionMarkers(this,junction,4);
		glEndList();
		junctions[i] = list;
	}
    nr = network->getNumberOfRoads();
    roads.resize(nr);
	for (int32 i = 0; i < nr; i++) {
		const Road* road = network->getRoad(i);
		GLuint list = glGenLists(1);
		glNewList( list , GL_COMPILE );
		painter->drawRoadAsphalt(this,road,4);
		painter->drawRoadMarkers(this,road,4);
		glEndList();
		roads[i] = list;
	}
}


void MapViewer::resizeGL(int w, int h) {
	glLoadIdentity();
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(center.getX() - scale * w * 0.5, center.getX() + scale * w * 0.5, center.getY() - scale * h * 0.5, center.getY() + scale * h * 0.5,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MapViewer::paintGL() {
	int w = width();
	int h = height();

	makeCurrent();

	glLoadIdentity();
	glViewport(0, 0, (GLint) w, (GLint) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	const RoadNetwork* network = simulator->getRoadNetwork();
	if(!network){
		return;
	}

	const BBox& bb = network->getExtents();
	if (scale < 0) {
		scale = (bb.getMaxX() - bb.getMinX()) / w;
		center.setX( (bb.getMaxX()+ bb.getMinX())*0.5 );
		center.setY( (bb.getMaxY() + bb.getMinY())*0.5 );
	}

	if (scale < 0.2)
		Lod = 4;
	else if (scale < 1)
		Lod = 3;
	else if (scale < 5)
		Lod = 2;
	else if (scale < 10)
		Lod = 1;

	glOrtho(center.getX() - scale * w * 0.5, center.getX() + scale * w * 0.5, center.getY() - scale * h * 0.5, center.getY() + scale * h * 0.5,-1,1);
	BBox viewBB(center.getX() - scale * w / 1.8, center.getY() - scale * h / 1.8, center.getX()+ scale * w / 1.8, center.getY() + scale * h / 1.8);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT);

	if(painter->isDrawRoadsEnabled()){
		if( Lod <= 2){
			glCallList(allscenarioList);
		}else{
			int32 nj = network->getNumberOfJunctions();
			for (int32 i = 0; i < nj; i++) {
				const Junction* junction = network->getJunction(i);
				if (junction && viewBB.intersects(junction->getBBox())){
					glCallList( junctions[i] );
				}
			}
			int32 nr = network->getNumberOfRoads();
			for (int32 i = 0; i < nr; i++) {
				const Road* road = network->getRoad(i);
				if (road && viewBB.intersects(road->getBBox())){
					glCallList( roads[i] );
					int32 nl = road->getNumberOfLanes();
					for(int32 l=0;l<nl;l++){
						const Lane* lane = road->getLane(l);
						if( simulator->hasFlag(Simulator::TrafficLights) && lane->hasTrafficLight() ){
							painter->drawLights(lane);
						}
					}
				}
			}
		}


    int32 nr = network->getNumberOfRoads();
	for (int32 i = 0; i < nr; i++) {
		const Road* road = network->getRoad(i);
		if(viewBB.intersects(road->getBBox())){
			int32 nl = road->getNumberOfLanes();
			for(int32 l=0;l<nl;l++){
				const Lane* lane = road->getLane(l);
				Lane::ObjectIterator it = lane->getFirstObjectIterator();
				while( it.isValid() ) {
					const LaneObject* obj = it.getLaneObject();
					if (obj->getObjectType() == LaneObject::VehicleT) {
						const Vehicle* vehicle = dynamic_cast<const Vehicle*> (obj);
						painter->drawVehicle( this, vehicle, Lod);
					}
					it.next();
				}
			}
			/*
			 * PARKING LANES
			 */

			if (road->getRightParking()) {
				const Lane* lane = road->getRightParking();
				Lane::ObjectIterator it = lane->getFirstObjectIterator();
				while (it.isValid()) {
					const LaneObject* obj = it.getLaneObject();
					if (obj->getObjectType() == LaneObject::VehicleT) {
						const Vehicle* vehicle = dynamic_cast<const Vehicle*> (obj);
						painter->drawVehicle(this, vehicle, Lod);
					}
					it.next();
				}
			}
			if (road->getLeftParking()) {
				const Lane* lane = road->getLeftParking();
				Lane::ObjectIterator it = lane->getFirstObjectIterator();
				while (it.isValid()) {
					const LaneObject* obj = it.getLaneObject();
					if (obj->getObjectType() == LaneObject::VehicleT) {
						const Vehicle* vehicle = dynamic_cast<const Vehicle*> (obj);
						painter->drawVehicle(this, vehicle, Lod);
					}
					it.next();
				}
			}

		}
	}
	}


	List<Vehicle*>::ConstIterator it = simulator->getExternalVehicles().constBegin();
	List<Vehicle*>::ConstIterator end = simulator->getExternalVehicles().constEnd();
	while (it != end) {
		painter->drawVehicle(this, *it, Lod);
		it++;
	}

	painter->draw(this,viewBB,Lod);
}

void MapViewer::mousePressEvent(QMouseEvent *event) {
	eventHandler->mousePressEvent(this,event);
}

void MapViewer::mouseReleaseEvent(QMouseEvent* event) {
	eventHandler->mouseReleaseEvent(this,event);
}

void MapViewer::mouseMoveEvent(QMouseEvent *event) {
	eventHandler->mouseMoveEvent(this,event);
}

void MapViewer::wheelEvent(QWheelEvent* event) {
	eventHandler->wheelEvent(this,event);
}

void MapViewer::keyPressEvent(QKeyEvent* event){
	eventHandler->keyPressEvent(this,event);
}

void MapViewer::keyReleaseEvent(QKeyEvent* event){
	eventHandler->keyReleaseEvent(this,event);
}

void MapViewer::setCenter(double x, double y) {
	center.setX(x);
	center.setY(y);
	updateGL();
}

void MapViewer::setCenter(const vns::Vec& c) {
	center = c;
	updateGL();
}

void MapViewer::setScale(double s){
	scale = s;
	updateGL();
}

Vec MapViewer::mouseToRealWorldCoordinates(int x, int y) {
	return Vec(center.getX() + (x - width() / 2) * scale, center.getY() - (y - height() / 2) * scale);
}

}

