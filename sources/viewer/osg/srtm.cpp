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

#include "srtm.h"

#include <QtCore/QtCore>
//#include "proj.h"

namespace vns {

SRTM::SRTM(const char* filename) {

	QFile file(filename);
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);    // read the data serialized from the file
	in >> xmin;
	in >> ymin;
	in >> xres;
	in >> yres;
	in >> w;
	in >> h;

	data = new float[w*h];
	float z;
	for(int row=0;row<h;row++){
		for(int col=0;col<w;col++){
			in >> z;
			data[row*h+col] = z;
		}
	}

	qDebug() << get(0,0) << get(0,1) << get(0,2);
	/*
	xmin = -50715.46;
	ymin = 182316.592;
	xres = 7.0;
	yres = 7.0;*/
	xmax = xmin+(xres*w);
	ymax = ymin+(yres*h);
}

SRTM::~SRTM() {
	// TODO Auto-generated destructor stub
}

bool SRTM::contains(const vns::Vec& p) const {
	return p.getX()>=xmin && p.getX()<=xmax && p.getY()>=ymin && p.getY()<=ymax;
}


float SRTM::getZ(const vns::Vec& p){
	if(p.getX()<xmin || p.getX()>xmax || p.getY()<ymin || p.getY()>ymax){
		return 0.0;
	}
	int x = int((p.getX()-xmin)/xres);
	int y = int((p.getY()-ymin)/yres);
	return data[y*h+x];
}



void SRTM::applyRoadNetwork(vns::RoadNetwork* network){
	int nr = network->getNumberOfRoads();
	vns::Vec o = network->getOffset();
	qDebug() << o.getX() << o.getY() << o.getZ();
	for(int i=0;i<nr;i++){
		vns::Road* road = network->getRoad(i);
		int np = road->getNumberOfPoints();
		for(int j=0;j<np;j++){
			vns::Vec p = road->getPoint(j);
			p.setX( p.getX()+o.getX() );
			p.setY( p.getY()+o.getY() );
			float z = getZ(p);
			qDebug() << z;
			road->setPoint(j,vns::Vec(p.getX()-o.getX(),p.getY()-o.getY(),z));
		}
	}
}

vns::Vec SRTM::getNormal(int x,int y){
	vns::Vec sum(0.0f, 0.0f, 0.0f);

	vns::Vec out;
	if (y > 0) {
		out = vns::Vec(0.0, data[(y-1)*h+x] - data[(y)*h+x], -1.0);
	}
	vns::Vec in;
	if (y < h - 1) {
		in = vns::Vec(0.0, data[(y+1)*h+x] - data[(y)*h+x], 1.0);
	}
	vns::Vec left;
	if (x > 0) {
		left = vns::Vec(-1.0, data[y*h+x-1] - data[(y)*h+x], 0.0);
	}
	vns::Vec right;
	if (x < w - 1) {
		right = vns::Vec(1.0, data[y*h+x+1] - data[(y)*h+x], 0.0);
	}

	if (x > 0 && y > 0) {
		sum = sum + out.cross(left).normalized();
	}
	if (x > 0 && y < h - 1) {
		sum = sum + left.cross(in).normalized();
	}
	if (x < w - 1 && y < h - 1) {
		sum = sum + in.cross(right).normalized();
	}
	if (x < w - 1 && y > 0) {
		sum = sum + right.cross(out).normalized();
	}
	return sum;
}
}
