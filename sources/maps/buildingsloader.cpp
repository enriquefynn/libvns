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

#include "buildingsloader.h"
#include "shapefil.h"

namespace vns {

vns::Buildings* BuildingsLoader::fromShapefile(const char* filename, const vns::Vec& offset){

	SHPHandle shpHandle = SHPOpen(filename, "rb");
	if (shpHandle == 0) {
		return 0;
	}
	DBFHandle dbfHandle = DBFOpen(filename, "rb");
	if (dbfHandle == 0) {
		SHPClose(shpHandle);
		return 0;
	}
	int nEntities;
	int shapeType;
    double minc[4];
    double maxc[4];
	SHPGetInfo(shpHandle, &nEntities, &shapeType, minc, maxc);

	if (shapeType != SHPT_POLYGON) {
		SHPClose(shpHandle);
		DBFClose(dbfHandle);
		return false;
	}

	vns::Buildings* buildings = new vns::Buildings();

	for(int i=0;i<nEntities;i++){
		SHPObject* obj = SHPReadObject(shpHandle, i);
		for(int i=0;i<obj->nVertices;i++){
			obj->padfX[i] = obj->padfX[i] - offset.getX();
			obj->padfY[i] = obj->padfY[i] - offset.getY();
			obj->padfZ[i] = obj->padfZ[i] - offset.getZ();
		}
		vns::Building* building = new vns::Building(obj->nVertices,obj->padfX,obj->padfY,obj->padfZ);
		buildings->addBuilding( building );
		SHPDestroyObject(obj);
	}

	SHPClose(shpHandle);
	DBFClose(dbfHandle);

	return buildings;
}

}
