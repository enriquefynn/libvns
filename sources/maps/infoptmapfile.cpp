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

#include "infoptmapfile.h"
#include "roadnetwork.h"
#include "mercartor.h"
#include <cstring>

namespace vns {

InfoPTMapFile::InfoPTMapFile() : vns::ShpMapFile() {

}

InfoPTMapFile::~InfoPTMapFile() {

}

bool InfoPTMapFile::hasValidNumberOfFields(int numberOfFields){
	return numberOfFields==4;
}

bool InfoPTMapFile::loadRoad(DBFHandle& dbfHandle, int id, vns::RoadNetwork* network, vns::PolyLine* line) {
	vns::Road::Type roadType = vns::Road::Undefined;
	bool leftParking = false;
	bool rightParking = false;

	int32 type = DBFReadIntegerAttribute(dbfHandle, id, 2);
	if( type == 1 ){
		roadType = vns::Road::Motorway;
	}
	if( type == 2 ){
		roadType = vns::Road::Trunk;
	}
	if( type == 1 ){
		roadType = vns::Road::MotorwayLink;
	}
	if( type == 3 ){
		roadType = vns::Road::Primary;
	}
	if( type == 4 || type == 5){
		roadType = vns::Road::Street;
	}
	if( type >= 6 ){
		roadType = vns::Road::Residential;
		leftParking = true;
		rightParking = true;
	}

	//const char* name = DBFReadStringAttribute(dbfHandle, id, 7);
	int32 nlanes = DBFReadIntegerAttribute(dbfHandle, id, 8);
	const char* way = DBFReadStringAttribute(dbfHandle, id, 10);
	//int32 vms = DBFReadIntegerAttribute(dbfHandle, id, 30);
	int32 vmax = DBFReadIntegerAttribute(dbfHandle, id, 31);
	bool oneway = false;

	if( strcmp(way,"N")==0 ){
		return true;
	}
	if( strcmp(way,"TF")==0 ){
		oneway = true;
		line->reverse();
	}
	if( strcmp(way,"FT")==0 ){
		oneway = true;
	}

	vns::Road* road;
	if(oneway){
		road = network->createRoad(line,nlanes,0);
	}else{
		int32 flanes = nlanes/2+nlanes%2;
		int32 blanes = nlanes/2;
		road = network->createRoad(line,flanes,blanes);
	}
	if(road){
		road->setType(roadType);
		road->setLeftParking(leftParking);
		road->setRightParking(rightParking);
		for(int l=0;l<road->getNumberOfLanes();l++){
			vns::Lane* lane = road->getLane(l);
			lane->setMaximumSpeed(vmax/3.6);
			lane->setMinimumSpeed(0);
		}
	}

	return true;
}

void InfoPTMapFile::createDBFFields(DBFHandle& dbfout){
	DBFAddField(dbfout,"ID",FTString,2,0);
	DBFAddField(dbfout,"CONCELHO",FTString,3,0);
	DBFAddField(dbfout,"CRITERIO",FTString,22,3);
	DBFAddField(dbfout,"L_A_FROM",FTString,100,0);
	DBFAddField(dbfout,"L_A_TO",FTInteger,1,0);
	DBFAddField(dbfout,"L_IRREG",FTInteger,5,0);
	DBFAddField(dbfout,"L_STRUC",FTInteger,5,0);
	DBFAddField(dbfout,"NOME",FTInteger,5,0);
	DBFAddField(dbfout,"N_FAIXAS",FTInteger,5,0);
	DBFAddField(dbfout,"OBS",FTInteger,5,0);
	DBFAddField(dbfout,"ONEWAY",FTInteger,5,0);
	DBFAddField(dbfout,"RESTRI",FTInteger,5,0);
	DBFAddField(dbfout,"R_A_FROM",FTInteger,5,0);
	DBFAddField(dbfout,"R_IRREG",FTInteger,5,0);
	DBFAddField(dbfout,"R_STRUC",FTInteger,5,0);
	DBFAddField(dbfout,"CRIT_CID",FTInteger,5,0);
	DBFAddField(dbfout,"FOW",FTInteger,5,0);
	DBFAddField(dbfout,"BIFURC",FTInteger,5,0);
	DBFAddField(dbfout,"PORTAGEM",FTInteger,5,0);
	DBFAddField(dbfout,"NET2CLASS",FTInteger,5,0);
	DBFAddField(dbfout,"ROUTENUM",FTInteger,5,0);
	DBFAddField(dbfout,"PRENM",FTInteger,5,0);
	DBFAddField(dbfout,"DESIGNM",FTInteger,5,0);
	DBFAddField(dbfout,"ALTERNM",FTInteger,5,0);
	DBFAddField(dbfout,"L_PC",FTInteger,5,0);
	DBFAddField(dbfout,"R_PC",FTInteger,5,0);
	DBFAddField(dbfout,"L_FREG",FTInteger,5,0);
	DBFAddField(dbfout,"R_FREG",FTInteger,5,0);
	DBFAddField(dbfout,"V_M_S",FTInteger,5,0);
	DBFAddField(dbfout,"V_MAX",FTInteger,5,0);
	DBFAddField(dbfout,"COD_ART",FTInteger,5,0);
	DBFAddField(dbfout,"FREG",FTInteger,5,0);
}

void InfoPTMapFile::saveDBFRoad(DBFHandle& ,int ,const vns::Road*){
	VNS_ASSERT( false ,"InfoPTMapFile::saveDBFRoad","Not yet Implemented!");
}

}
