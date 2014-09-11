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

#include <QtOpenGL/QtOpenGL>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtGui/QImage>
#include "painter.h"
#include "sceneviewer.h"
#include "vehiclefactory.h"

namespace vns {

Painter::Painter() {
	circleList = 0;
	lineWidth = 0.12;
	draw_junctionAsphalt = true;
	draw_junctionMarkers = true;
	draw_roadAsphalt = true;
	draw_roadMarkers = true;
	draw_roadObjects = true;
	draw_roads = true;

	int32 vTypes = VehicleFactory::getNumberOfVehicleTypes();
	vehiclesIMG.resize(vTypes);

	vehiclesIMG[vns::CAR].append(":/0_0.png");
	vehiclesIMG[vns::CAR].append(":/0_1.png");
	vehiclesIMG[vns::CAR].append(":/0_2.png");
	vehiclesIMG[vns::CAR].append(":/0_3.png");
	vehiclesIMG[vns::CAR].append(":/0_4.png");
	vehiclesIMG[vns::CAR].append(":/0_5.png");
	vehiclesIMG[vns::CAR].append(":/0_6.png");
	vehiclesIMG[vns::CAR].append(":/0_7.png");
	vehiclesIMG[vns::CAR].append(":/0_8.png");
	vehiclesIMG[vns::CAR].append(":/0_9.png");
	vehiclesIMG[vns::CAR].append(":/0_10.png");
	vehiclesIMG[vns::CAR].append(":/0_11.png");
	vehiclesIMG[vns::CAR].append(":/0_12.png");
	vehiclesIMG[vns::CAR].append(":/0_13.png");
	vehiclesIMG[vns::CAR].append(":/0_14.png");
	vehiclesIMG[vns::CAR].append(":/0_15.png");

	vehiclesIMG[vns::BIKE].append(":/1_0.png");
	vehiclesIMG[vns::BIKE].append(":/1_1.png");
	vehiclesIMG[vns::BIKE].append(":/1_2.png");


	vehiclesIMG[vns::TAXI].append(":/2_0.png");

	vehiclesIMG[vns::BUS].append(":/3_0.png");

	vehiclesIMG[vns::TRUCK].append(":/4_0.png");
	vehiclesIMG[vns::TRUCK].append(":/4_1.png");

	vehiclesIMG[vns::TRAIN].append(":/5_0.png");

}

Painter::~Painter(){

}

void Painter::setColor3f(float r,float g,float b){
	glColor3f(r,g,b);
}

void Painter::setColor4f(float r,float g,float b,float a){
	glColor4f(r,g,b,a);
}

void Painter::setPointSize(float s){
	glPointSize(s);
}

void Painter::setLineWidth(float w){
	glLineWidth(w);
}

void Painter::drawPoint(double x, double y, double z){
	glBegin(GL_POINTS);
	glVertex3d(x,y,z);
	glEnd();
}

void Painter::drawPoint(const vns::Vec& p){
	glBegin(GL_POINTS);
	glVertex2d(p.getX(),p.getY());
	glEnd();
}

void Painter::drawBBox(const BBox& bb){
	glBegin(GL_LINE_LOOP);
	glVertex2d(bb.getMinX(),bb.getMinY());
	glVertex2d(bb.getMinX(),bb.getMaxY());
	glVertex2d(bb.getMaxX(),bb.getMaxY());
	glVertex2d(bb.getMaxX(),bb.getMinY());
	glEnd();
}

void Painter::initialize(SceneViewer* w){

	/*
	 * LOAD TEXTURES
	 */
	stopTID = w->bindTexture( QImage(":/stop_lane.png") );
	yieldTID = w->bindTexture( QImage(":/yield_lane.png") );
	busTID = w->bindTexture( QImage(":/bus_lane.png") );
	redLightTID = w->bindTexture( QImage(":/tl_red.png") );
	greenLightTID = w->bindTexture( QImage(":/tl_green.png") );
	yellowLightTID = w->bindTexture( QImage(":/tl_yellow.png") );
	offLightTID = w->bindTexture( QImage(":/tl_off.png") );

	vehicleTIDs.resize(vehiclesIMG.size());
	for(int32 i=0;i<vehiclesIMG.size();i++){
		int32 nm = vehiclesIMG.at(i).size();
		vehicleTIDs[i].resize(nm);
		for(int32 j=0;j<nm;j++){
			QImage img(vehiclesIMG.at(i).at(j));
			vehicleTIDs[i][j] = w->bindTexture( img );
		}
	}

    if (circleList==0) {
        circleList = glGenLists(1);
        glNewList(circleList,GL_COMPILE);
        int n = 50;
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2d(0,0);
        for (int i=0; i<=n; ++i) {
            double a = -2*M_PI*i/n;
            double x = cos(a);
            double y = sin(a);
            glVertex2d(x,y);
        }
        glEnd();
        glEndList();
    }

}

void Painter::drawLights(const Lane* lane){
	Vec dir = lane->getLastPointDirection();
	Vec p = lane->getLastPoint() + dir * 0.6;

	glPushMatrix();
	glTranslated(p.getX(), p.getY(), 0);
	glRotated(dir.dir_deg(), 0, 0, 1);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable( GL_TEXTURE_2D);
	switch (lane->getTrafficLightColor()) {
	case RedLight:
		glBindTexture(GL_TEXTURE_2D, redLightTID);
		break;
	case GreenLight:
		glBindTexture(GL_TEXTURE_2D, greenLightTID);
		break;
	case YellowLight:
		glBindTexture(GL_TEXTURE_2D, yellowLightTID);
		break;
	default:
		glBindTexture(GL_TEXTURE_2D, offLightTID);
		break;
	}

	glBegin( GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex2d(-0.4, 1.0);
	glTexCoord2f(0.0, 1.0);
	glVertex2d(0.4, 1.0);
	glTexCoord2f(1.0, 1.0);
	glVertex2d(0.4, -1.0);
	glTexCoord2f(1.0, 0.0);
	glVertex2d(-0.4, -1.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

/*
 *  l + (nl-l)/2
 *  2 + (4-2)/2
 */

void Painter::drawLaneConnections(SceneViewer* , const Lane* lane, uint8 ){
	Vec p1 = lane->getLastPoint();
	Vec d1 = lane->getLastPointDirection();
	Vec p2,d2;
	double step = 0.1;
	Vec pos,dir,perp,p;

	int32 nc = lane->getNumberOfConnections();
	for(int32 i=0;i<nc;i++){
		const Road* droad = lane->getConnection(i);
		if( lane->getEndJunction() == droad->getStartJunction(Forward) ){
			droad->getStartPoint(p2,d2,perp,Forward);
		}else{
			droad->getStartPoint(p2,d2,perp,Backward);
		}
		glLineWidth(2.0);
		glColor4d(1.0,1.0,0.0,0.9);
		glBegin(GL_LINE_STRIP);
		trajectory.create(p1,d1,p2,d2);
		for(int32 k=0;k<=10;k++){
			trajectory.interpolate(k*step,pos,dir);
		    glVertex2d(pos.getX(),pos.getY());
		}
		glEnd();
		//perp = d2.perpendicular().normalize();
		glBegin(GL_TRIANGLES);
		p = pos - perp * 0.5;
		glVertex2d(p.getX(), p.getY());
		p = (pos + d2 * 0.5);
		glVertex2d(p.getX(), p.getY());
		p = pos + perp * 0.5;
		glVertex2d(p.getX(), p.getY());
		glEnd();
	}
}


void Painter::drawLaneNextLanes(SceneViewer* , const Lane* lane, uint8 ){
	Vec p1 = lane->getLastPoint();
	Vec d1 = lane->getLastPointDirection();
	Vec p2,d2;
	double step = 0.1;
	Vec pos,dir,perp,p;

	const Junction* junction = lane->getEndJunction();
	if(junction==0) return;
	int32 nr = junction->getNumberOfRoads();
	for(int32 i=0;i<nr;i++){
		const Road* droad = junction->getRoad(i);
		if( !junction->isExitRoad(droad) ) continue;
		const Lane* ln = lane->getNextLane(droad);
		if(ln==0) continue;
		p2 = ln->getFirstPoint();
		d2 = ln->getFirstPointDirection();
		perp = ln->getFirstPointNormal();
		glLineWidth(2.0);
		glColor4d(1.0,1.0,0.0,0.9);
		glBegin(GL_LINE_STRIP);
		trajectory.create(p1,d1,p2,d2);
		for(int32 k=0;k<=10;k++){
			trajectory.interpolate(k*step,pos,dir);
			glVertex2d(pos.getX(),pos.getY());
		}
		glEnd();
		//perp = d2.perpendicular().normalize();
		glBegin(GL_TRIANGLES);
		p = pos - perp * 0.5;
		glVertex2d(p.getX(), p.getY());
		p = (pos + d2 * 0.5);
		glVertex2d(p.getX(), p.getY());
		p = pos + perp * 0.5;
		glVertex2d(p.getX(), p.getY());
		glEnd();
	}
}

void Painter::drawNumbersOfJunctionExitLanes(SceneViewer* w, const Junction* junction, uint8 ){
	glColor4f(1.0,0.9,0.0,1.0);
	Vec p,d,n;
	if(junction){
    	int32 nr = junction->getNumberOfRoads();
    	for(qint32 i=0;i<nr;i++){
    		const Road* road = junction->getRoad(i);
    		if( junction == road->getStartJunction(Forward) ){
    			road->getStartPoint(p,d,n,Forward);
    		}else{
    			road->getStartPoint(p,d,n,Backward);
    		}
			p = p+d*2.0;
			glPushMatrix();
			glTranslated(p.getX(), p.getY(), 0);
			w->renderText(0.0,0.0,0.0,QString("%1").arg(road->getID()) );
			glPopMatrix();
    	}
    }
}


void Painter::drawVehicle(SceneViewer*, const Vehicle* vehicle, uint8 lod){
	glColor3f(1, 1, 1);
	//if (vehicle->state == VehicleStoped)
	//	return;

	const Vec& pos = vehicle->getPosition();
	const Vec& dir = vehicle->getDirection();

	if(lod<=2){
		glPointSize(3.0);
		/*
		if(vehicle->getVehicleType()==3){
			glColor3f(1.0,0.0,0.0);
		}else{
			glColor3f(0.2,0.5,1.0);
		}*/
		glColor3f(0.2,0.5,1.0);
		glBegin(GL_POINTS);
		glVertex2d(pos.getX(), pos.getY());
		glEnd();
		return;
	}

	glPushMatrix();
	glTranslated(pos.getX(), pos.getY(), 0);

	glRotated(dir.dir_deg(), 0, 0, 1);


	VehicleType vType = vehicle->getVehicleType();
	VehicleModel vModel = vehicle->getVehicleModel()%vehicleTIDs.at(vType).size();
	uint32 vTID = vehicleTIDs.at(vType).at(vModel);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable( GL_TEXTURE_2D );
	glBindTexture(GL_TEXTURE_2D,vTID);
	glBegin( GL_QUADS );
	glTexCoord2f(0.0, 0.0);
	glVertex2d(-vehicle->getLength() * 0.5, vehicle->getWidth() * 0.5);
	glTexCoord2f(0.0, 1.0);
	glVertex2d(vehicle->getLength() * 0.5, vehicle->getWidth() * 0.5);
	glTexCoord2f(1.0, 1.0);
	glVertex2d(vehicle->getLength() * 0.5, -vehicle->getWidth() * 0.5);
	glTexCoord2f(1.0, 0.0);
	glVertex2d(-vehicle->getLength() * 0.5, -vehicle->getWidth() * 0.5);
	glEnd();
	glDisable(GL_TEXTURE_2D);


	/*
	if( vehicle->isRightBlinking() ){
		glColor3f(1, 0.9, 0);
		drawCircularPoint(vehicle->getLength()*0.4, vehicle->getWidth()*0.45,0.1);
		drawCircularPoint(-vehicle->getLength()*0.4, vehicle->getWidth()*0.45,0.1);
	}

	if( vehicle->isLeftBlinking() ){
		glColor3f(1, 0.9, 0);
		drawCircularPoint(vehicle->getLength()*0.4, -vehicle->getWidth()*0.45,0.1);
		drawCircularPoint(-vehicle->getLength()*0.4, -vehicle->getWidth()*0.45,0.1);
	}*/
	if( vehicle->isBraking() ){
		glColor3f(1, 0, 0);
		drawCircularPoint(-vehicle->getLength()*0.45, vehicle->getWidth()*0.3,0.15);
		drawCircularPoint(-vehicle->getLength()*0.45, -vehicle->getWidth()*0.3,0.15);
	}

	glPopMatrix();


/*
	if(vehicle->state == Vehicle::ChangingLane){
		glColor3f(0.0,0.0,1.0);
		drawCircularPoint(pos,1.0);
	}

	if(vehicle->state == Vehicle::MovingInLane){
		if(vehicle->currentLaneDecision == vns::DriverModel::MandatoryChangeToLeft){
			glColor3f(0.0,0.0,0.0);
			drawCircularPoint(pos,1.0);
		}else
		if(vehicle->currentLaneDecision == vns::DriverModel::MandatoryChangeToRight){
			glColor3f(0.0,0.0,0.0);
			drawCircularPoint(pos,1.0);
		}else{
			glColor3f(0.0,1.0,0.0);
			drawCircularPoint(pos,1.0);
		}
		if(vehicle->getFrontPosition()>vehicle->getLane()->getLaneEnd()-20){
			glColor3f(0.0,1.0,1.0);
			drawCircularPoint(pos,1.0);
		}
	}*/

	/*if(vehicle->getObjectType() != vns::Object::VehicleT){
		glColor3f(1.0,1.0,1.0);
		drawCircularPoint(pos,1.0);
	}*/



	/*




	glPushMatrix();
	glTranslated(pos.getX(),pos.getY(), 0);
	glRotated(dir.dir_deg(), 0, 0, 1);
	glColor3f(0.0,0.0,0.0);
	w->renderText(0.0,0.0,0.0,QString("%1").arg(vehicle->getLanePosition()) );
	w->renderText(5.0,5.0,0.0,QString("%1").arg(vehicle->ghost->getLanePosition()) );
	glPopMatrix();


	if(vehicle->hasPermissionToCross == false){
		glColor3f(0.0,0.0,1.0);
		drawCircularPoint(pos,1.0);
	}
	if(vehicle->hasPermissionToCross == true){
		glColor3f(0.0,1.0,0.0);
		drawCircularPoint(pos,1.0);
	}
	if(vehicle->getCrossPermission() == Vehicle::CannotCross){
		glColor3f(1.0,0.0,0.0);
		drawCircularPoint(pos,1.0);
	}
	glColor3f(1.0,0.0,0.0);
	if( vehicle->getNextLane() && vehicle->getLanePosition() > vehicle->getLane()->getLaneEnd()-10 ){
		glLineWidth(2.0);
		glColor3f(1.0,0.0,0.0);
		glBegin( GL_LINES);
		Vec p = pos + vehicle->getDirection()*(vehicle->getLength()*0.5);
		glVertex2d(p.getX(), p.getY());
		Vec pos2 = vehicle->getNextLane()->getFirstPoint();
		glVertex2d(pos2.getX(), pos2.getY());
		glEnd();
	}

	glPushMatrix();
	glTranslated(pos.getX(), pos.getY(), 0);
	w->renderText(0.0,0.0,0.0,QString("%1").arg(vehicle->getID()) );
	glPopMatrix();
	*/

/*
	if(vehicle->isLaneChangeRequired == false){
		glColor3f(0.0,1.0,0.0);
		drawCircularPoint(pos,1.0);
	}

	if(vehicle->isLaneChangeRequired == true){
		glColor3f(1.0,0.0,0.0);
		drawCircularPoint(pos,1.0);
	}

	if(vehicle->state == Vehicle::ChangingLane){
		glColor3f(0.0,0.0,1.0);
		drawCircularPoint(pos,1.0);
	}
	*/


	/*

	if(vehicle->state == CrossingJunction){
		glColor3f(0.0,1.0,0.0);
		drawCircularPoint(pos,1.0);
	}
	if(vehicle->state == WaitingToCross){
		glColor3f(1.0,0.0,0.0);
		drawCircularPoint(pos,1.0);
	}
	if(vehicle->state == PrepareToCrossJunction){
		glColor3f(0.0,0.0,1.0);
		drawCircularPoint(pos,1.0);
	}

	if(vehicle->conflict){
		glColor3f(1.0,1.0,0.0);
		drawCircularPoint(pos,0.25);
	}
	*/
/*




	*/

	/*
	 	glColor3f(1, 1, 1);
	glBegin(GL_POLYGON);
	glVertex2d(-vehicle->getLength()*0.5, -vehicle->getWidth()*0.5);
	glVertex2d(-vehicle->getLength()*0.5, vehicle->getWidth()*0.5);
	glVertex2d(vehicle->getLength()*0.5 - 0.3, vehicle->getWidth()*0.5);
	glVertex2d(vehicle->getLength()*0.5, 0);
	glVertex2d(vehicle->getLength()*0.5 - 0.3, -vehicle->getWidth()*0.5);
	glEnd();

	if( vehicle->isBraking() ){
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		glVertex2d(-vehicle->getLength()*0.5, vehicle->getWidth()*0.2);
		glVertex2d(-vehicle->getLength()*0.5, -vehicle->getWidth()*0.2);
		glEnd();
	}
	 * */



}


void Painter::drawHighlightedRoad(const Road* road, uint8 ){
	qint32 np = road->getNumberOfPoints();
	glColor4f(0.3,0.7,1.0,0.3);
	Vec p,pt,perp;
	double d = road->getWidth()*0.5;
	glBegin(GL_TRIANGLE_STRIP);
	for(qint32 i=0;i<np;i++){
		pt = road->getPoint(i);
		perp = road->getNormal(i);
	    p = pt+perp*d;
	    glVertex2d(p.getX(),p.getY());
	    p = pt-perp*d;
	    glVertex2d(p.getX(),p.getY());
	}
    glEnd();
}


void Painter::drawRoadPoints(SceneViewer*, const Road* road, uint8){
	glLineWidth(5.0);
	const PolyLine* line = road->getPolyLine();
	int32 n = line->size();
	glBegin(GL_LINE_STRIP);
	glColor3f(1.0f, 0.0f, 1.0f);
	for(int32 i=0;i<n;i++){
		Vec pt = line->getPoint(i);
		glVertex2d(pt.getX(),pt.getY());
	}
	glEnd();
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 1.0f);
	for(int32 i=0;i<n;i++){
		Vec pt = line->getPoint(i);
		Vec pt1 = pt + line->getNormal(i)*road->getWidth()*0.5;
		glVertex2d(pt.getX(),pt.getY());
		glVertex2d(pt1.getX(),pt1.getY());
	}
	glEnd();
	glPointSize(10.0);
	for(int32 i=0;i<n;i++){
		glBegin(GL_POINTS);
		if(i==n-1){
			glColor3f(1.0f, 0.0f, 0.0f);
		}else{
			glColor3f(0.0f, 0.0f, 1.0f);
		}
		Vec pt = line->getPoint(i);
		glVertex2d(pt.getX(),pt.getY());
		glEnd();
	}
}



void Painter::drawRoadAsphalt(SceneViewer*, const Road* road, uint8 lod){
	glColor3f(0.45f, 0.45f, 0.45f);
	const PolyLine* line = road->getPolyLine();
	int32 n = line->size();
	if(lod <= 2){
		glLineWidth(1.0);
	    glBegin(GL_LINE_STRIP);
	    for(qint32 i=0;i<n;i++){
	    	Vec p = line->getPoint(i);
	    	glVertex2d(p.getX(),p.getY());
	    }
	    glEnd();
	    return;
	}
	Vec pt;
	Vec d1, p1, n1;
	Vec dx1, px1, nx1;
	Vec d2, p2, n2;
	Vec dx2, px2, nx2;
	double gapParking = 1.0;
	double gw = lineWidth*0.5;
	double rwm = road->getWidth()*0.5+ROAD_MARGINSGAP;
	double rw = road->getWidth()*0.5;
	double start = road->getRoadStart();
	double end = road->getRoadEnd();
	double offset = road->getRoadOffset();

    int32 i=0;
    while(i<n && line->getT(i) <= start){
    	i++;
    }
    road->getStartPoint(p1,d1,n1);
    road->getEndPoint(p2,d2,n2);

    glBegin(GL_TRIANGLE_STRIP);
    pt = p1-n1*rwm;
    glVertex2d(pt.getX(),pt.getY());
    pt = p1+n1*rwm;
    glVertex2d(pt.getX(),pt.getY());
    while(i<n && line->getT(i) < end){
        pt = line->getPoint(i)+line->getNormal(i)*(offset-rwm);
        glVertex2d(pt.getX(),pt.getY());
        pt = line->getPoint(i)+line->getNormal(i)*(offset+rwm);
        glVertex2d(pt.getX(),pt.getY());
    	i++;
    }
    pt = p2-n2*rwm;
    glVertex2d(pt.getX(),pt.getY());
    pt = p2+n2*rwm;
    glVertex2d(pt.getX(),pt.getY());
    glEnd();

    /*
     * Left Parking Asphalt
     */
    if(road->hasLeftParking()){
    	const Lane* lp = road->getLeftParking();
    	if(lp->getWay()==Forward){
    		start = lp->getLaneStart();
    		end = lp->getLaneEnd();
    	}else{
    		start = road->getLength()-lp->getLaneEnd();
    		end = road->getLength()-lp->getLaneStart();
    	}

        int32 id=0;
        while(id<n && line->getT(id) <= start+gapParking){
        	id++;
        }
    	road->getPositionPoint(p1,d1,n1,start);
    	road->getPositionPoint(px1,dx1,nx1,start+gapParking);
        road->getPositionPoint(p2,d2,n2,end);
        road->getPositionPoint(px2,dx2,nx2,end-gapParking);

        glColor3f(0.45f, 0.45f, 0.45f);
        glBegin(GL_TRIANGLE_STRIP);
        pt = p1+n1*rwm;
        glVertex2d(pt.getX(),pt.getY());
        pt = px1+nx1*(rwm+lp->getWidth());
        glVertex2d(pt.getX(),pt.getY());
        i = id;
        while(i<n && line->getT(i) < end-gapParking){
            pt = line->getPoint(i)+line->getNormal(i)*(offset+rwm);
            glVertex2d(pt.getX(),pt.getY());
            pt = line->getPoint(i)+line->getNormal(i)*(offset+(rwm+lp->getWidth()));
            glVertex2d(pt.getX(),pt.getY());
        	i++;
        }
        pt = p2+n2*rwm;
        glVertex2d(pt.getX(),pt.getY());
        pt = px2+nx2*(rwm+lp->getWidth());
        glVertex2d(pt.getX(),pt.getY());
        glEnd();

        // Side lane markers
        glColor3f(1.f, 1.f, 1.f);
        drawContinuousLine(road->getPolyLine(), rw+offset, road->getRoadStart(), start);

        glBegin(GL_TRIANGLE_STRIP);
        pt = p1+n1*(rw+gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = p1+n1*(rw-gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = px1+nx1*(rw+lp->getWidth()+gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = px1+nx1*(rw+lp->getWidth()-gw);
        glVertex2d(pt.getX(),pt.getY());
        i = id;
        while(i<n && line->getT(i) < end-gapParking){
            pt = line->getPoint(i)+line->getNormal(i)*(offset+(rw+lp->getWidth())+gw);
            glVertex2d(pt.getX(),pt.getY());
            pt = line->getPoint(i)+line->getNormal(i)*(offset+(rw+lp->getWidth())-gw);
            glVertex2d(pt.getX(),pt.getY());
        	i++;
        }
        pt = px2+nx2*(rw+lp->getWidth()+gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = px2+nx2*(rw+lp->getWidth()-gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = p2+n2*(rw+gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = p2+n2*(rw-gw);
        glVertex2d(pt.getX(),pt.getY());
        glEnd();
        drawContinuousLine(road->getPolyLine(), rw+offset, road->getRoadEnd(), end);
        drawDiscontinuousLine(line,rw+offset,start,end,lineWidth,1.0);
    }else{
    	glColor3f(1.f, 1.f, 1.f);
    	drawContinuousLine(road->getPolyLine(), rw+offset, road->getRoadStart(), road->getRoadEnd());
    }

    /*
     * Right Parking Asphalt
     */
    if(road->hasRightParking()){
    	const Lane* lp = road->getRightParking();
    	if(lp->getWay()==Forward){
    		start = lp->getLaneStart();
    		end = lp->getLaneEnd();
    	}else{
    		start = road->getLength()-lp->getLaneEnd();
    		end = road->getLength()-lp->getLaneStart();
    	}

        int32 id=0;
        while(id<n && line->getT(id) <= start+gapParking){
        	id++;
        }
    	road->getPositionPoint(p1,d1,n1,start);
    	road->getPositionPoint(px1,dx1,nx1,start+gapParking);
        road->getPositionPoint(p2,d2,n2,end);
        road->getPositionPoint(px2,dx2,nx2,end-gapParking);

        /*
         * 		Vec nc,nn;

		for(int32 k=0;k<=10;k++){
			trajectory.interpolate(k*step,pos,dir);
			nc = dir.perpendicular().normalize();
			nn = (1.0/(1.0+np*nc))*(np+nc);
		    p = pos+nn*lw;
		    glVertex2d(p.getX(),p.getY());
		    p = pos-nn*lw;
		    glVertex2d(p.getX(),p.getY());
		}
	    p = p4+n4*lw;
	    glVertex2d(p.getX(),p.getY());
	    p = p4-n4*lw;
	    glVertex2d(p.getX(),p.getY());
		glEnd();
         */

        glColor3f(0.45f, 0.45f, 0.45f);
        glBegin(GL_TRIANGLE_STRIP);
        pt = p1-n1*rwm;
        glVertex2d(pt.getX(),pt.getY());
        pt = px1-nx1*(rwm+lp->getWidth());
        glVertex2d(pt.getX(),pt.getY());
        i = id;
        while(i<n && line->getT(i) < end-gapParking){
            pt = line->getPoint(i)+line->getNormal(i)*(offset-rwm);
            glVertex2d(pt.getX(),pt.getY());
            pt = line->getPoint(i)+line->getNormal(i)*(offset-(rwm+lp->getWidth()));
            glVertex2d(pt.getX(),pt.getY());
        	i++;
        }
        pt = p2-n2*rwm;
        glVertex2d(pt.getX(),pt.getY());
        pt = px2-nx2*(rwm+lp->getWidth());
        glVertex2d(pt.getX(),pt.getY());
        glEnd();

        // Side lane markers
        glColor3f(1.f, 1.f, 1.f);
        drawContinuousLine(road->getPolyLine(), -rw+offset, road->getRoadStart(), start);

        glBegin(GL_TRIANGLE_STRIP);
        pt = p1-n1*(rw+gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = p1-n1*(rw-gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = px1-nx1*(rw+lp->getWidth()+gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = px1-nx1*(rw+lp->getWidth()-gw);
        glVertex2d(pt.getX(),pt.getY());
        i = id;
        while(i<n && line->getT(i) < end-gapParking){
            pt = line->getPoint(i)+line->getNormal(i)*(offset-(rw+lp->getWidth())+gw);
            glVertex2d(pt.getX(),pt.getY());
            pt = line->getPoint(i)+line->getNormal(i)*(offset-(rw+lp->getWidth())-gw);
            glVertex2d(pt.getX(),pt.getY());
        	i++;
        }
        pt = px2-nx2*(rw+lp->getWidth()+gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = px2-nx2*(rw+lp->getWidth()-gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = p2-n2*(rw+gw);
        glVertex2d(pt.getX(),pt.getY());
        pt = p2-n2*(rw-gw);
        glVertex2d(pt.getX(),pt.getY());
        glEnd();
        drawContinuousLine(road->getPolyLine(), -rw+offset, road->getRoadEnd(), end);
        drawDiscontinuousLine(line,-rw+offset,start,end,lineWidth,1.0);
    }else{
    	glColor3f(1.f, 1.f, 1.f);
    	drawContinuousLine(road->getPolyLine(), -rw+offset, road->getRoadStart(), road->getRoadEnd());
    }

}

void Painter::drawDebugRoad(const Road* road){
	const PolyLine* line = road->getPolyLine();
	int32 n = line->size();
	Vec dp,dir, pt, perp;

	double start = road->getRoadStart();
	double end = road->getRoadEnd();

    int32 i=0;
    while(i<n && line->getT(i) <= start){
    	i++;
    }

    road->getStartPoint(pt,dir,perp);
    drawDebugArrows(pt,dir,perp);

    while(i<n && line->getT(i) < end){
        dir = line->getDirection(i);
        pt = line->getPoint(i);
        perp = line->getNormal(i);
        drawDebugArrows(pt,dir,perp);
    	i++;
    }

    road->getEndPoint(pt,dir,perp);
    drawDebugArrows(pt,dir,perp);
}

void Painter::drawRoadInfo(SceneViewer* viewer,const Road* road){
	const PolyLine* line = road->getPolyLine();
	int32 n = line->size();
	Vec dp,dir, pt, perp;

	double start = road->getRoadStart();
	double end = road->getRoadEnd();

    int32 i=0;
    while(i<n && line->getT(i) <= start){
    	i++;
    }

    road->getStartPoint(pt,dir,perp);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2d(pt.getX(),pt.getY());
	Vec ptf = pt + dir*2.0;
	glVertex2d(ptf.getX(),ptf.getY());
	glEnd();
	Vec normal = dir.perpendicular().normalized();
	glBegin(GL_TRIANGLES);
	Vec p = ptf - normal * 0.5;
	glVertex2d(p.getX(), p.getY());
	p = (ptf + dir * 0.5);
	glVertex2d(p.getX(), p.getY());
	p = ptf + normal * 0.5;
	glVertex2d(p.getX(), p.getY());
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);
	viewer->renderText(pt.getX(),pt.getY(),0,QString("%1").arg(road->getID()));

    while(i<n && line->getT(i) < end){
        dir = line->getDirection(i);
        pt = line->getPoint(i);
        perp = line->getNormal(i);
    	glColor3f(1.0f, 0.0f, 0.0f);
    	glBegin(GL_LINES);
    	glVertex2d(pt.getX(),pt.getY());
    	ptf = pt + dir*2.0;
    	glVertex2d(ptf.getX(),ptf.getY());
    	glEnd();
		normal = dir.perpendicular().normalized();
		glBegin(GL_TRIANGLES);
		p = ptf - normal * 0.5;
		glVertex2d(p.getX(), p.getY());
		p = (ptf + dir * 0.5);
		glVertex2d(p.getX(), p.getY());
		p = ptf + normal * 0.5;
		glVertex2d(p.getX(), p.getY());
		glEnd();

		glColor3f(0.0f, 0.0f, 1.0f);
		viewer->renderText(pt.getX(),pt.getY(),0,QString("%1").arg(road->getID()));

    	i++;
    }

    road->getEndPoint(pt,dir,perp);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2d(pt.getX(),pt.getY());
	ptf = pt + dir*2.0;
	glVertex2d(ptf.getX(),ptf.getY());
	glEnd();
	normal = dir.perpendicular().normalized();
	glBegin(GL_TRIANGLES);
	p = ptf - normal * 0.5;
	glVertex2d(p.getX(), p.getY());
	p = (ptf + dir * 0.5);
	glVertex2d(p.getX(), p.getY());
	p = ptf + normal * 0.5;
	glVertex2d(p.getX(), p.getY());
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);
	viewer->renderText(pt.getX(),pt.getY(),0,QString("%1").arg(road->getID()));
}



void Painter::drawDebugArrows(const Vec& pt, const Vec& dir, const Vec& perp){
    // DIR ARROW
    	glColor3f(1.0f, 0.0f, 0.0f);
    	glBegin(GL_LINES);
    	glVertex2d(pt.getX(),pt.getY());
    	Vec ptf = pt + dir*2.0;
    	glVertex2d(ptf.getX(),ptf.getY());
    	glEnd();
		Vec normal = dir.perpendicular().normalized();
		glBegin(GL_TRIANGLES);
		Vec p = ptf - normal * 0.5;
		glVertex2d(p.getX(), p.getY());
		p = (ptf + dir * 0.5);
		glVertex2d(p.getX(), p.getY());
		p = ptf + normal * 0.5;
		glVertex2d(p.getX(), p.getY());
		glEnd();
	// NORMAL ARROW
    	glColor3f(0.0f, 0.0f, 1.0f);
    	glBegin(GL_LINES);
    	glVertex2d(pt.getX(),pt.getY());
    	ptf = pt + perp*2.0;
    	glVertex2d(ptf.getX(),ptf.getY());
    	glEnd();
		normal = perp.perpendicular().normalized();
		glBegin(GL_TRIANGLES);
		p = ptf - normal * 0.5;
		glVertex2d(p.getX(), p.getY());
		p = (ptf + perp * 0.5);
		glVertex2d(p.getX(), p.getY());
		p = ptf + normal * 0.5;
		glVertex2d(p.getX(), p.getY());
		glEnd();

}

void Painter::drawDebugJunction(const Junction* junction){
	int32 nr = junction->getNumberOfRoads();
	Vec c = junction->getPosition();
	Vec p,p1,pa,pb,p4,n1,n2,n3,n4,d1,d4;
	Vec pos,dir;
	const Road* road1;
	const Road* road2;

	for(qint32 i=0; i<nr;i++){
		road1 = junction->getRoad(i);
		double w1 = road1->getWidth()*0.5+ROAD_MARGINSGAP;
		if(road1->getEndJunction() == junction){
			road1->getEndPoint(p1,d1,n1,Forward);
		}else{
			road1->getStartPoint(p1,d1,n1,Backward);
			d1 = -d1;
			n1 = -n1;
		}
		this->drawDebugArrows(p1,d1,n1);
		if(i==nr-1){
			road2 = junction->getRoad(0);
		}else{
			road2 = junction->getRoad(i+1);
		}
		double w4 = road2->getWidth()*0.5+ROAD_MARGINSGAP;
		if( road2->getStartJunction() == junction ){
			road2->getStartPoint(p4,d4,n4,Forward);
		}else{
			road2->getEndPoint(p4,d4,n4,Backward);
			d4 = -d4;
			n4 = -n4;
		}
		this->drawDebugArrows(p4,d4,n4);
		pa = p1-n1*w1;
		pb = p4-n4*w4;
	}
}

void Painter::drawJunctionAsphalt(SceneViewer*,const Junction* junction,uint8 lod){
	if( lod <= 2 ) return;
	glColor3f(0.45f, 0.45f, 0.45f);
	int32 nr = junction->getNumberOfRoads();
	Vec c = junction->getPosition();
	Vec p,p1,pa,pb,p4,n1,n2,n3,n4,d1,d4;
	Vec pos,dir;
	const Road* road1;
	const Road* road2;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(c.getX(),c.getY());
	for(int32 i=0; i<nr;i++){
		road1 = junction->getRoad(i);
		double w1 = road1->getWidth()*0.5+ROAD_MARGINSGAP;
		if(road1->getEndJunction() == junction){
			road1->getEndPoint(p1,d1,n1,Forward);
		}else{
			road1->getEndPoint(p1,d1,n1,Backward);
		}
		if(i==nr-1){
			road2 = junction->getRoad(0);
		}else{
			road2 = junction->getRoad(i+1);
		}
		double w4 = road2->getWidth()*0.5+ROAD_MARGINSGAP;
		if( road2->getStartJunction() == junction ){
			road2->getStartPoint(p4,d4,n4,Forward);
		}else{
			road2->getStartPoint(p4,d4,n4,Backward);
		}
		pa = p1-n1*w1;
		pb = p4-n4*w4;

		trajectory.create(pa,d1,pb,d4);
		double step = 0.1;
		for(int32 k=0;k<=10;k++){
			trajectory.interpolate(k*step,pos,dir);
		    glVertex2d(pos.getX(),pos.getY());
		}

		if(i==nr-1){
			pos = p4+n4*w4;
			glVertex2d(pos.getX(),pos.getY());
		}
	}
	glEnd();

}


void Painter::drawRoadMarkers(SceneViewer* w, const Road* road, uint8 lod){
	int32 nl = road->getNumberOfLanes();
	for (int32 i = 0; i < nl; ++i) {
		const Lane* lane = road->getLane(i);
		drawLane( w, lane , lod );
	}


	/*********************************/
	// draw Cross Walks
	//road->setEndCrossWalk(true);
	if(road->hasEndCrossWalk()){
		drawCrossWalk(road,Forward);
	}
	if(road->hasStartCrossWalk()){
		drawCrossWalk(road,Backward);
	}
	/*****************************/

	double start,end,offset,width;
	const Lane* l1;
	const Lane* l2;
	bool left,right;
	nl = road->getNumberOfLanes();
	for(int32 i = 1; i < nl; i++){
		l1 = road->getLane(i);
		l2 = road->getLane(i-1);
		offset = l1->getRoadCenterOffset();
		width = l1->getWidth();
		start = (l1->getWay()==Forward?l1->getLaneStart():road->getPolyLine()->getLength()-l1->getLaneEnd());
		end = (l1->getWay()==Forward?l1->getLaneEnd():road->getPolyLine()->getLength()-l1->getLaneStart());
		right = (l2->getWay()==Forward?l2->hasFlag(Lane::ChangeLaneToRight):l2->hasFlag(Lane::ChangeLaneToLeft));
		left = (l1->getWay()==Forward?l1->hasFlag(Lane::ChangeLaneToLeft):l1->hasFlag(Lane::ChangeLaneToRight));
		glColor3f(1, 1, 1);
		if(right && left){
			drawDiscontinuousLine(road->getPolyLine(), offset+width*0.5, start, end);
		}else{
			if(!right && !left){
				drawContinuousLine(road->getPolyLine(), offset+width*0.5, start, end);
			}else{
				if(right && !left){
					drawContinuousLine(road->getPolyLine(), offset+width*0.46, start, end);
					drawDiscontinuousLine(road->getPolyLine(), offset+width*0.54, start, end);
				}else{
					drawDiscontinuousLine(road->getPolyLine(), offset+width*0.46, start, end);
					drawContinuousLine(road->getPolyLine(), offset+width*0.54, start, end);
				}
			}
		}

	}

	for(int l=0;l<nl;l++){
		l1 = road->getLane(l);
		if(l1->hasFlag(vns::Lane::Rails)){
			glColor3f(0.2, 0.2, 0.2);
			drawContinuousLine(l1->getRoad()->getPolyLine(),l1->getRoadCenterOffset()-0.8,l1->getRoad()->getRoadStart(),l1->getRoad()->getRoadEnd(),0.1);
			drawContinuousLine(l1->getRoad()->getPolyLine(),l1->getRoadCenterOffset()+0.8,l1->getRoad()->getRoadStart(),l1->getRoad()->getRoadEnd(),0.1);
		}
	}



	/*
	Road* road = lane->getRoad();
	glLineStipple(2, 0x00FF);
	glEnable( GL_LINE_STIPPLE);
	if( lane->Forward ){
		drawPolyLine( w, road->getPolyLine(), (lane->getRoadCenterOffset() + lane->getWidth() * 0.5), lane->getLaneStart(), lane->getLaneEnd());
	}else{
		drawPolyLine( w, road->getPolyLine(), lane->getRoadCenterOffset() + lane->getWidth() * 0.5, road->getLength() - lane->getLaneEnd(), road->getLength() - lane->getLaneStart());
	}
	glDisable(GL_LINE_STIPPLE);
*/
	/*
	Vec pt, pt1, dir, perp;
	road->getPolyLine()->pointDirFromFirstPoint(pt, dir, perp, 0.0, road->getRoadStart());

	glColor3f(1, 0, 0);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	glVertex2d(pt.getX(),pt.getY());
	glEnd();
	road->getPolyLine()->pointDirFromFirstPoint(pt, dir, perp, 0.0, road->getRoadEnd());
	glColor3f(0, 0, 1);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	glVertex2d(pt.getX(),pt.getY());
	glEnd();
*/

	// Road BBOX
	/*glColor3f(1, 1, 0);
	glBegin(GL_LINE_LOOP);
	BBox& bb = road->getBBox();
	glVertex2d(bb.getMinX(),bb.getMinY());
	glVertex2d(bb.getMinX(),bb.getMaxY());
	glVertex2d(bb.getMaxX(),bb.getMaxY());
	glVertex2d(bb.getMaxX(),bb.getMinY());
	glEnd();
	*/


}

void Painter::drawJunctionMarkers(SceneViewer* ,const Junction* junction,uint8 ){
	int32 nr = junction->getNumberOfRoads();

	Vec p,p1,p2,p3,p4,n1,n2,n3,n4,d1,d4;
	glColor3f(1, 1, 0);
	glPointSize(5.0);
	double lw = lineWidth*0.5;
	glColor3f(1, 1, 1);
	const Road* road1;
	const Road* road2;
	for(int32 i=0; i<nr;i++){
		road1 = junction->getRoad(i);
		double w1 = road1->getWidth()*0.5;
		if(road1->getEndJunction() == junction){
			road1->getEndPoint(p1,d1,n1,Forward);
		}else{
			road1->getEndPoint(p1,d1,n1,Backward);
		}
		if(i==nr-1){
			road2 = junction->getRoad(0);
		}else{
			road2 = junction->getRoad(i+1);
		}
		double w4 = road2->getWidth()*0.5;
		if( road2->getStartJunction() == junction ){
			road2->getStartPoint(p4,d4,n4,Forward);
		}else{
			road2->getStartPoint(p4,d4,n4,Backward);
		}
		glBegin(GL_TRIANGLE_STRIP);
		p1 = p1-n1*w1;
		p2 = p1+d1*ROAD_MARGINSGAP;
		p4 = p4-n4*w4;
		p3 = p4-d4*ROAD_MARGINSGAP;

	    p = p1+n1*lw;
	    glVertex2d(p.getX(),p.getY());
	    p = p1-n1*lw;
	    glVertex2d(p.getX(),p.getY());

		trajectory.create(p2,d1,p3,d4);
		double step = 0.1;
		Vec pos,dir;
		Vec np = n1;
		Vec nc,nn;

		for(int32 k=0;k<=10;k++){
			trajectory.interpolate(k*step,pos,dir);
			nc = dir.perpendicular().normalized();
			nn = (1.0/(1.0+np*nc))*(np+nc);
		    p = pos+nn*lw;
		    glVertex2d(p.getX(),p.getY());
		    p = pos-nn*lw;
		    glVertex2d(p.getX(),p.getY());
		}
	    p = p4+n4*lw;
	    glVertex2d(p.getX(),p.getY());
	    p = p4-n4*lw;
	    glVertex2d(p.getX(),p.getY());
		glEnd();
	}

	/* Draw rails */
	for(int32 i=0; i<nr;i++){
		const vns::Road* road = junction->getRoad(i);
		for(int8 l=0;l<road->getNumberOfLanes();l++){
			const vns::Lane* lane = road->getLane(l);
			if(lane->hasFlag(vns::Lane::Rails)){
				for(int32 c=0;c<lane->getNumberOfConnections();c++){
					const vns::Road* nroad = lane->getConnection(c);
					const vns::Lane* nlane = lane->getNextLane(nroad);
					if(nlane && nlane->hasFlag(vns::Lane::Rails)){
						vns::Vec p1 = lane->getLastPoint();
						vns::Vec p2 = nlane->getFirstPoint();
						vns::Vec d1 = lane->getLastPointDirection();
						vns::Vec d2 = nlane->getFirstPointDirection();
						vns::Vec n1 = lane->getLastPointNormal();
						vns::Vec n2 = nlane->getFirstPointNormal();
						glColor3f(0.2, 0.2, 0.2);
						drawTrajectory(p1+n1*0.8,d1,p2+n2*0.8,d2,0.1);
						drawTrajectory(p1-n1*0.8,d1,p2-n2*0.8,d2,0.1);
					}
				}
			}
		}
	}
}

void Painter::drawTrajectory(const vns::Vec& p1,const vns::Vec& d1,const vns::Vec& p2,const vns::Vec& d2,float width){
	double lw = width*0.5;
	trajectory.create(p1,d1,p2,d2);
	double step = 0.1;
	Vec p,pos,dir;
	Vec np = d1.perpendicular().normalized();
	Vec nc,nn;

	glBegin(GL_TRIANGLE_STRIP);

	/*
    p = p1+n1*lw;
    glVertex2d(p.getX(),p.getY());
    p = p1-n1*lw;
    glVertex2d(p.getX(),p.getY());*/

	for(int32 k=0;k<=10;k++){
		trajectory.interpolate(k*step,pos,dir);
		nn = dir.perpendicular().normalized();
		//nn = (1.0/(1.0+np*nc))*(np+nc);
	    p = pos+nn*lw;
	    glVertex2d(p.getX(),p.getY());
	    p = pos-nn*lw;
	    glVertex2d(p.getX(),p.getY());
	}
	glEnd();
}

void Painter::drawJunctionBorder(const Vec& p1,const Vec& p2, const Vec& p3, const Vec& p4,double offset){
	double lw = lineWidth*0.5;
	Vec n1,n2,n3,n4,p;
	n4 = (p4-p3).perpendicular().normalized();
	n3 = (p3-p2).perpendicular().normalized();
	n2 = (p2-p1).perpendicular().normalized();
	n1 = n2;
	n3 = (1.0/(1.0+n3*n4))*(n3+n4);
	n2 = (1.0/(1.0+n2*n3))*(n2+n3);

	glBegin(GL_TRIANGLE_STRIP);
    p = p1+n1*(offset-lw);
    glVertex2d(p.getX(),p.getY());
    p = p1+n1*(offset+lw);
    glVertex2d(p.getX(),p.getY());
    p = p2+n2*(offset-lw);
    glVertex2d(p.getX(),p.getY());
    p = p2+n2*(offset+lw);
    glVertex2d(p.getX(),p.getY());
    p = p3+n3*(offset-lw);
    glVertex2d(p.getX(),p.getY());
    p = p3+n3*(offset+lw);
    glVertex2d(p.getX(),p.getY());
    p = p4+n4*(offset-lw);
    glVertex2d(p.getX(),p.getY());
    p = p4+n4*(offset+lw);
    glVertex2d(p.getX(),p.getY());
	glEnd();
}



void Painter::drawContinuousLine(const PolyLine* line, double offset, double start, double end, double lineWidth) {

	int32 n = line->size();
	double d = lineWidth*0.5;

	Vec pt,dir,perp;
    int32 i=0;
    while(i<n && line->getT(i) <= start){ i++; }
    glBegin(GL_TRIANGLE_STRIP);

    line->pointDirFromFirstPoint(pt,dir,perp,start);
    Vec pt1 = pt+perp*(offset-d);
    Vec pt2 = pt+perp*(offset+d);
    glVertex2d(pt1.getX(),pt1.getY());
    glVertex2d(pt2.getX(),pt2.getY());

    while(i<n && line->getT(i) < end){
        pt1 = line->getPoint(i)+line->getNormal(i)*(offset-d);
        pt2 = line->getPoint(i)+line->getNormal(i)*(offset+d);
        glVertex2d(pt1.getX(),pt1.getY());
        glVertex2d(pt2.getX(),pt2.getY());
    	i++;
    }

    line->pointDirFromFirstPoint(pt,dir,perp,end);
    pt1 = pt+perp*(offset-d);
    pt2 = pt+perp*(offset+d);
    glVertex2d(pt1.getX(),pt1.getY());
    glVertex2d(pt2.getX(),pt2.getY());

    glEnd();
}

void Painter::drawDiscontinuousLine(const PolyLine* line, double offset, double start, double end,double lineWidth,double spacing) {
	double lw = lineWidth*0.5;
	double lineLength = 1.0;
	int32 n = line->size();

    Vec pt,pt1,pt2,p1,p2,perp,dir;
    int32 i=0;
    while( i < n-1 && line->getT(i+1) < start ){
    	i++;
    }
    double d = start - line->getT(i);
    bool quit = false;
    while( i < n-1){
        p1 = line->getPoint(i)+line->getNormal(i)*offset;
        dir = line->getDirection(i);
        perp = line->getPerpendicular(i);
        if( line->getT(i+1) > end ){
        	p2 = line->getPoint(i)+dir*(end-line->getT(i))+line->getNormal(i+1)*offset;
            quit = true;
        }else{
            p2 = line->getPoint(i+1)+line->getNormal(i+1)*offset;
        }
        double dp = (p2-p1).norm();

        while(d < dp){
            glBegin(GL_TRIANGLE_STRIP);
            pt1 = (p1+dir*d)-perp*lw;
            pt2 = (p1+dir*d)+perp*lw;
            glVertex2d(pt1.getX(),pt1.getY());
            glVertex2d(pt2.getX(),pt2.getY());
            if(d+lineLength > dp){
            	double dif = (d+lineLength)-dp;
                pt1 = pt1+dir*dif;
                pt2 = pt2+dir*dif;
                glVertex2d(pt1.getX(),pt1.getY());
                glVertex2d(pt2.getX(),pt2.getY());


                glEnd();
            }else{
                pt1 = pt1+dir*lineLength;
                pt2 = pt2+dir*lineLength;
                glVertex2d(pt1.getX(),pt1.getY());
                glVertex2d(pt2.getX(),pt2.getY());
                glEnd();
            }
            d += lineLength+spacing;
        }
        if(quit){
        	break;
        }
        d = d-dp;
        i++;
    }
}

void Painter::drawCrossWalk( const Road* road, Way way, float width, float interval) {
	const PolyLine* line = road->getPolyLine();
	int32 n = line->size();
	float len = road->getWidth();
	if(len<2.0) return;
	float s = fmod(len,interval);
	if(s<0.1){
		len -= s+interval;
	}else{
		len -= s;
	}
	float iw = -len*0.5;
	float fw = len*0.5;

	Vec pt1,dir1,perp1,pt2,dir2,perp2,pt,dir,perp;

	double start;
	double end;
	if(way == Forward){
		end = road->getRoadEnd();
		start = end - width;
	}else{
		start = road->getRoadStart();
		end = start + width;
	}

	road->getPositionPoint(pt1, dir1, perp1, start );
	road->getPositionPoint(pt2, dir2, perp2, end );

	int32 id = 0;
    while( id < n && line->getT(id) < start ){ id++; }

	Vec p;
	while(iw<fw){
		glBegin(GL_TRIANGLE_STRIP);
		p = pt1+perp1*iw;
		glVertex2d(p.getX(), p.getY());
		p = pt1+perp1*(iw+interval);
		glVertex2d(p.getX(), p.getY());
		int32 i = id;
		while( i < n && line->getT(i) < end ){
			pt = line->getPoint(i);
			dir = line->getDirection(i);
			perp = line->getNormal(i);
			p = pt+perp*iw;
			glVertex2d(p.getX(), p.getY());
			p = pt+perp*(iw+interval);
			glVertex2d(p.getX(), p.getY());
			i++;
		}
		p = pt2+perp2*iw;
		glVertex2d(p.getX(), p.getY());
		p = pt2+perp2*(iw+interval);
		glVertex2d(p.getX(), p.getY());
		glEnd();

		iw += interval*2;
	}
}


void Painter::drawLane(SceneViewer*, const Lane* lane,uint8) {

	glColor3f(1, 1, 1);

	if(lane->hasFlag(Lane::NoTrafficLane)){
		if(lane->getWay()==Forward){
			drawContinuousLine(lane->getRoad()->getPolyLine(),lane->getRoadCenterOffset(),lane->getLaneStart(),lane->getLaneEnd(),lane->getWidth());
		}else{
			drawContinuousLine(lane->getRoad()->getPolyLine(),lane->getRoadCenterOffset(),lane->getRoad()->getLength()-lane->getLaneEnd(),lane->getRoad()->getLength()-lane->getLaneStart(),lane->getWidth());

		}
		return;
	}



	/*********************************/
	// Lane END MARKER
	Vec pt = lane->getLastPoint();
	Vec dir = lane->getLastPointDirection();
	Vec perp = lane->getLastPointNormal();
	double d = lane->getWidth() * 0.4;
	Vec p1 = (pt - dir * 0.20) + perp * d;
	Vec p2 = pt + perp * d;
	Vec p3 = pt - perp * d;
	Vec p4 = (pt - dir * 0.20) - perp * d;
	glColor3f(1, 1, 1);
	glBegin( GL_POLYGON);
	glVertex2d(p1.getX(), p1.getY());
	glVertex2d(p2.getX(), p2.getY());
	glVertex2d(p3.getX(), p3.getY());
	glVertex2d(p4.getX(), p4.getY());
	glEnd();
	/*****************************/

	Vec p = pt;
	// Stop Sign
	if(lane->hasFlag(Lane::StopSign)){
		p = pt - dir*0.6;
		drawStopSign( p , dir, perp );
	}else{
		if(lane->hasFlag(Lane::YieldSign)){
			drawYieldSign( lane );
		}
	}

	p = p - dir*2.0;
	drawLaneArrows(p, dir, perp, lane);

	/*
	 * Draw lane triggers
	 */
	Lane::SensorIterator i = lane->getSensorIterator();
	while(i.isValid()){
		lane->getPositionPoint(pt,dir,perp,i.getPosition());
		float len = i.getTrigger()->getLength();
		Vec p1 = (pt - dir * len*0.5) + perp * lane->getWidth()*0.5;
		Vec p2 = pt + perp * lane->getWidth()*0.5;
		Vec p3 = pt - perp * lane->getWidth()*0.5;
		Vec p4 = (pt - dir * len*0.5) - perp * lane->getWidth()*0.5;
		glColor4f(1.0, 1.0, 0.0, 0.9);
		glBegin( GL_POLYGON);
		glVertex2d(p1.getX(), p1.getY());
		glVertex2d(p2.getX(), p2.getY());
		glVertex2d(p3.getX(), p3.getY());
		glVertex2d(p4.getX(), p4.getY());
		glEnd();
		i.next();
	}
}

void Painter::drawStopSign(const Vec& pt, const Vec& dir, const Vec& perp){

    //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, stopTID);
	double d = 1.1;
	double h = 0.8;
	Vec p1 = (pt - dir * h) + perp * d;
	Vec p2 = pt + perp * d;
	Vec p3 = pt - perp * d;
	Vec p4 = (pt - dir * h) - perp * d;
	glBegin( GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2d(p1.getX(), p1.getY());
    glTexCoord2f(0.0, 1.0); glVertex2d(p2.getX(), p2.getY());
    glTexCoord2f(1.0, 1.0); glVertex2d(p3.getX(), p3.getY());
    glTexCoord2f(1.0, 0.0); glVertex2d(p4.getX(), p4.getY());
	glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Painter::drawYieldSign( const Lane* lane){
	Vec pt = lane->getLastPoint();
	Vec dir = lane->getLastPointDirection();
	Vec perp = lane->getLastPointNormal();
	Vec p;
    // Asphalt color
	float w = 2.2;
	float h = 2.5;
	float lw = 0.25;
	glColor3f(1.f,1.f,1.f);
    glBegin(GL_TRIANGLES);
    p = pt - dir*h;
    glVertex2d(p.getX(), p.getY());
    p = pt - dir*0.5-perp*w*0.5;
    glVertex2d(p.getX(), p.getY());
    p = pt - dir*0.5+perp*w*0.5;
    glVertex2d(p.getX(), p.getY());
    glEnd();
    glColor3f(0.45f, 0.45f, 0.45f);
    glBegin(GL_TRIANGLES);
    p = pt - dir*(h-lw);
    glVertex2d(p.getX(), p.getY());
    p = pt - dir*(0.5+lw)-perp*(w*0.5-lw);
    glVertex2d(p.getX(), p.getY());
    p = pt - dir*(0.5+lw)+perp*(w*0.5-lw);
    glVertex2d(p.getX(), p.getY());
    glEnd();
}


void Painter::drawLaneArrows(const Vec& pt, const Vec& dir, const Vec& perp, const Lane* lane){
	bool front = false;
	bool left = false;
	bool right = false;
	double ad;
	Vec d1 = lane->getLastPointDirection();
	const Junction* endJunction = lane->getEndJunction();
	int32 nc = lane->getNumberOfConnections();
	for(int32 c=0;c<nc;c++){
		const Road* road = lane->getConnection(c);
		if( endJunction == road->getStartJunction(Forward) ){
			ad = d1.angleDiffTo( road->getFirstPointDirection(Forward) );
		}else{
			ad = d1.angleDiffTo( road->getFirstPointDirection(Backward) );
		}
		if( ad > 10 ){
			right = true;
		}else{
			if( ad < -10 ){
				left = true;
			}else{
				front = true;
			}
		}
	}
	/************************************/


	if (front) {
		// FRONT ARROW
		glColor3f(1, 1, 1);
		glBegin( GL_TRIANGLES);
		Vec p = pt;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 0.7) - perp * 0.3;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 0.7) + perp * 0.3;
		glVertex2d(p.getX(), p.getY());
		glEnd();

		glBegin( GL_POLYGON);
		p = (pt - dir * 0.7) - perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 0.7) + perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.0) + perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.0) - perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		glEnd();
	}

	if (left) {
		glBegin(GL_TRIANGLES);
		Vec p = (pt - dir * 0.8) + perp * 0.4;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.1) + perp * 1.0;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.4) + perp * 0.4;
		glVertex2d(p.getX(), p.getY());
		glEnd();

		glBegin( GL_POLYGON);
		p = (pt - dir * 1.0) + perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.0) + perp * 0.4;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.2) + perp * 0.4;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.2) + perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		glEnd();
	}

	if (right) {
		glBegin( GL_TRIANGLES);
		Vec p = (pt - dir * 0.8) - perp * 0.4;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.1) - perp * 1.0;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.4) - perp * 0.4;
		glVertex2d(p.getX(), p.getY());
		glEnd();

		glBegin( GL_POLYGON);
		p = (pt - dir * 1.0) - perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.0) - perp * 0.4;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.2) - perp * 0.4;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.2) - perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		glEnd();
	}

	if (front || left || right) {
		// ARROW TAIL
		glBegin( GL_POLYGON);
		Vec p = (pt - dir * 1.0) - perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 1.0) + perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 2.2) + perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		p = (pt - dir * 2.2) - perp * 0.1;
		glVertex2d(p.getX(), p.getY());
		glEnd();
	}

}

void Painter::drawCircularPoint(const Vec& pos, const double radius){
	glPushMatrix();
	glTranslated(pos.getX(), pos.getY(), 0);
	glScaled(radius,radius,radius);
	glCallList(circleList);
	glPopMatrix();
}

void Painter::drawCircularPoint(double x,double y, const double radius){
	glPushMatrix();
	glTranslated(x, y, 0);
	glScaled(radius,radius,radius);
	glCallList(circleList);
	glPopMatrix();
}


void Painter::drawRoutePath(SceneViewer*,const Path* path){
	Vec p,dir,n,p1,p2;
	glColor4f(1.0,0.0,0.0,1.0);
	glLineWidth(3.0);
	float arrowHeight = 7.0;
	float arrowWidth = 5.0;
	Path::ConstIterator it = path->getFirstNodeConstIterator();
	while( it.isValid() ){
		const Road* road = it.getRoad();
		qint32 np = road->getNumberOfPoints();
		if( it.getWay() == Forward ){
			glColor4f(1.0,0.0,0.0,1.0);
			glBegin(GL_LINE_STRIP);
			for(qint32 i=0;i<np;i++){
				p = road->getPoint(i);
				glVertex2d(p.getX(),p.getY());
			}
			glEnd();
			glColor4f(0.0,0.0,1.0,1.0);
			dir = road->getDirection(np-1);
			n = dir.perpendicular().normalized();
			glBegin(GL_LINE_STRIP);
			p1 = (p-dir*arrowHeight)+n*arrowWidth;
			glVertex2d(p1.getX(),p1.getY());
			glVertex2d(p.getX(),p.getY());
			p2 = (p-dir*arrowHeight)-n*arrowWidth;
			glVertex2d(p2.getX(),p2.getY());
			glEnd();
		}else{
			glColor4f(1.0,0.0,0.0,1.0);
			glBegin(GL_LINE_STRIP);
			for(qint32 i=np-1;i>=0;i--){
				p = road->getPoint(i);
				glVertex2d(p.getX(),p.getY());
			}
			glEnd();
			glColor4f(0.0,0.0,1.0,1.0);
			dir = -road->getDirection(0);
			n = dir.perpendicular().normalized();
			glBegin(GL_LINE_STRIP);
			p1 = (p-dir*arrowHeight)+n*arrowWidth;
			glVertex2d(p1.getX(),p1.getY());
			glVertex2d(p.getX(),p.getY());
			p2 = (p-dir*arrowHeight)-n*arrowWidth;
			glVertex2d(p2.getX(),p2.getY());
			glEnd();
		}
		it.next();
	}
}

void Painter::drawLine(const vns::Vec& p1,const vns::Vec& p2){
	glBegin(GL_LINES);
	glVertex2d(p1.getX(), p1.getY());
	glVertex2d(p2.getX(), p2.getY());
	glEnd();
}

void Painter::drawRailAsphalt(SceneViewer*, const Road* road, uint8){
	glColor3f(1.0f, 0.4f, 0.3f);
	drawDiscontinuousLine(road->getPolyLine(),0.0,road->getRoadStart(),road->getRoadEnd(),road->getWidth(),2.0);
}


}
