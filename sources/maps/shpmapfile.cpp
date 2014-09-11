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

#include "shpmapfile.h"
#include "roadnetwork.h"
#include "mercartor.h"

namespace vns {

ShpMapFile::ShpMapFile() : vns::MapFile() {

}

ShpMapFile::~ShpMapFile() {

}

bool ShpMapFile::loadRoad(DBFHandle&, int, vns::RoadNetwork* network, vns::PolyLine* line){
	network->createRoad(line,1,0);
	return true;
}

bool ShpMapFile::hasValidNumberOfFields(int){
	return true;
}

vns::BBox ShpMapFile::getExtents(double xmin, double ymin, double xmax, double ymax){
	vns::Vec vmin = vns::Mercartor::toXY(xmin,ymin);
	vns::Vec vmax = vns::Mercartor::toXY(xmax,ymax);
	return vns::BBox( vmin, vmax );
}

void ShpMapFile::initializeLoad(vns::RoadNetwork*){

}

void ShpMapFile::finalizeLoad(vns::RoadNetwork* network){
	network->generateRoadsConnectivity();
	network->setAutomaticConnectivityGeneration(true);
}

vns::Vec ShpMapFile::loadPoint(double x, double y){
	return vns::Mercartor::toXY(x,y);
}

vns::Vec ShpMapFile::savePoint(double x,double y){
	return vns::Mercartor::toLonLat(x,y);
}

bool ShpMapFile::load(vns::RoadNetwork* network, const char* fileName){
	SHPHandle shpHandle = SHPOpen(fileName, "rb");
	if (shpHandle == 0) {
		return false;
	}
	DBFHandle dbfHandle = DBFOpen(fileName, "rb");
	if (dbfHandle == 0) {
		SHPClose(shpHandle);
		return false;
	}

    int nEntities;
    int shapeType;
    double minc[4];
    double maxc[4];
	SHPGetInfo(shpHandle, &nEntities, &shapeType, minc, maxc);
	if( shapeType != SHPT_ARC && shapeType != SHPT_ARCZ ) {
		SHPClose(shpHandle);
		DBFClose(dbfHandle);
		return false;
	}
	if( !hasValidNumberOfFields( DBFGetFieldCount(dbfHandle) ) ) {
		SHPClose(shpHandle);
		DBFClose(dbfHandle);
		return 0;
	}

	vns::BBox bb = getExtents(minc[0],minc[1],maxc[0],maxc[1]);
	vns::Vec offset( (bb.getMaxX()+bb.getMinX())*0.5, (bb.getMaxY()+bb.getMinY())*0.5 );
	network->setOffset( offset );
	network->setAutomaticConnectivityGeneration(false);

	initializeLoad(network);

	for(int id=0;id<nEntities;id++){
		SHPObject* obj = SHPReadObject(shpHandle, id);
		for(int v=0;v<obj->nVertices;v++){
			vns::Vec p = loadPoint(obj->padfX[v],obj->padfY[v]) - offset;
			obj->padfX[v] = p.getX();
			obj->padfY[v] = p.getY();
			obj->padfZ[v] = obj->padfZ[v];
		}
		if (obj->nVertices >= 2) {
			vns::PolyLine* line = new vns::PolyLine(obj->nVertices, obj->padfX, obj->padfY,obj->padfZ);
			if( !loadRoad(dbfHandle,id,network,line) ){
				SHPDestroyObject(obj);
				SHPClose(shpHandle);
				DBFClose(dbfHandle);
				return false;
			}
		}
		SHPDestroyObject(obj);
	}
	SHPClose(shpHandle);
	DBFClose(dbfHandle);

	finalizeLoad(network);

	return true;
}


void ShpMapFile::createDBFFields(DBFHandle&){
}

void ShpMapFile::saveDBFRoad(DBFHandle&,int,const vns::Road*){
}

bool ShpMapFile::save(const vns::RoadNetwork* network, const char* filename){
	SHPHandle shp = SHPCreate(filename,SHPT_ARC);
	DBFHandle dbf = DBFCreate(filename);

	createDBFFields(dbf);

	vns::Vec offset = network->getOffset();
	for(int i=0;i<network->getNumberOfRoads();i++){
		const vns::Road* road = network->getRoad(i);
		const vns::PolyLine* line = road->getPolyLine();
		double* x = new double[line->size()];
		double* y = new double[line->size()];
		double* z = new double[line->size()];
		for(int j=0;j<line->size();j++){
			vns::Vec p = line->getPoint(j);
			vns::Vec np = savePoint(p.getX()+offset.getX(),p.getY()+offset.getY());
			x[j] = np.getX();
			y[j] = np.getY();
			z[j] = p.getZ();
		}
		SHPObject* object = SHPCreateSimpleObject(SHPT_ARC,line->size(),x,y,z);
		int id = SHPWriteObject( shp, -1 , object );
		SHPDestroyObject( object );
		delete[] x;delete[] y;delete[] z;
		saveDBFRoad(dbf,id,road);
	}
	SHPClose( shp );
	DBFClose( dbf );

	return true;
}

}
