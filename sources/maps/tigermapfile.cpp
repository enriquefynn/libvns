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

#include "tigermapfile.h"
#include "roadnetwork.h"
#include "mercartor.h"
#include <cstring>

class TigerRoad {
public:
	TigerRoad(vns::PolyLine* l,vns::Road::Type t,int fl=1,int bl=0){
		line = l;
		type = t;
		flanes = fl;
		blanes = bl;
	};
	vns::PolyLine* line;
	vns::Road::Type type;
	int flanes;
	int blanes;
};

namespace vns {

TigerMapFile::TigerMapFile() : vns::ShpMapFile() {

}

TigerMapFile::~TigerMapFile() {

}

bool TigerMapFile::save(const vns::RoadNetwork*, const char*){
	VNS_ASSERT( false ,"TigerMapFile::save","Not yet Implemented!");
}


void TigerMapFile::initializeLoad(vns::RoadNetwork*){
	pointsRoads.clear();

	vns::List<TigerRoad*>::Iterator it=tigerRoads.begin();
    while(it!=tigerRoads.end()){
    	TigerRoad* tiger = *it;
    	it = tigerRoads.erase(it);
    	delete tiger;
    }
}

void TigerMapFile::finalizeLoad(vns::RoadNetwork* network){
	vns::List<TigerRoad*>::Iterator i = tigerRoads.begin();
	while( i!=tigerRoads.end() ){
		TigerRoad* tiger = *i;
		vns::List< vns::PolyLine* > polylines;
		vns::Vector<vns::Vec> np;
		for(int pi=0;pi<tiger->line->size();pi++){
			vns::Vec p = tiger->line->getPoint(pi);
			np.append(p);
			if(pi>0 && pointsRoads.value(p)>1){
				// divide
				polylines.append( new vns::PolyLine(np) );
				np.clear();
				if(pi<tiger->line->size()-1) np.append(p);
			}
		}
		if(np.size()>1){
			polylines.append( new vns::PolyLine(np) );
		}

		for(vns::List< vns::PolyLine* >::Iterator it=polylines.begin();it!=polylines.end();it++){
			vns::PolyLine* line = *it;
			vns::Road* road = network->createRoad(line, tiger->flanes, tiger->blanes);
			road->setType(tiger->type);
		}
		i = tigerRoads.erase(i);
		delete tiger->line;
		delete tiger;
	}

	network->generateRoadsConnectivity();
	network->setAutomaticConnectivityGeneration(true);
}

bool TigerMapFile::hasValidNumberOfFields(int numberOfFields){
	return numberOfFields==4;
}

bool TigerMapFile::loadRoad(DBFHandle& dbfHandle, int id, vns::RoadNetwork*, vns::PolyLine* line) {
	vns::Road::Type roadType = vns::Road::Undefined;
	int flanes = 1;
	int blanes = 0;
	const char* sroadtype = DBFReadStringAttribute(dbfHandle, id, 3);
	if(strcmp(sroadtype,"R1011")==0){
		// Railroad feature, Main spur or yard
		return true;
	}
	if(strcmp(sroadtype,"R1051")==0){
		// Car line, monorail, tram
		return true;
	}
	if(strcmp(sroadtype,"R1052")==0){
		// Cog Rail Line
		return true;
	}
	if(strcmp(sroadtype,"S1100")==0){
		// Primary Road
		roadType = vns::Road::Motorway;
		flanes = 3;
		blanes = 3;
	}
	if(strcmp(sroadtype,"S1200")==0){
		// Secondary Road
		roadType = vns::Road::Primary;
		flanes = 2;
		blanes = 2;
	}
	if(strcmp(sroadtype,"S1400")==0){
		// Rural Road, City Road
		roadType = vns::Road::Street;
		flanes = 1;
		blanes = 1;
	}
	if(strcmp(sroadtype,"S1500")==0){
		// Vehicular trail 4WD
		return true;
	}
	if(strcmp(sroadtype,"S1630")==0){
		roadType = vns::Road::MotorwayLink;
	}
	if(strcmp(sroadtype,"S1640")==0){
		// Service Drive usually along a limited access highway
		roadType = vns::Road::Service;
	}
	if(strcmp(sroadtype,"S1710")==0){
		// Pedestrian trail
		roadType = vns::Road::Footway;
		return true;
	}
	if(strcmp(sroadtype,"S1720")==0){
		// Stairway
		roadType = vns::Road::Steps;
		return true;
	}
	if(strcmp(sroadtype,"S1730")==0){
		// Alley
		return true;
	}
	if(strcmp(sroadtype,"S1740")==0){
		// Private Road for service vehicles,logging, oil fields, ranches, etc
		//roadType = vns::Road::Private;
		return true;
	}
	if(strcmp(sroadtype,"S1780")==0){
		// Parking Lot Road
		roadType = vns::Road::Parking;
	}
	if(strcmp(sroadtype,"S1820")==0){
		// bike path or trail
		roadType = vns::Road::Cicleway;
		return true;
	}
	if(strcmp(sroadtype,"S1830")==0){
		// bridle path
		return true;
	}

	for(int i=0;i<line->size();i++){
		vns::Vec p = line->getPoint(i);
		int n = pointsRoads.value(p,0);
		pointsRoads.insert(p, n+1);
	}

	tigerRoads.push_back( new TigerRoad(line,roadType,flanes,blanes) );

	return true;
}

/*
void TigerMapFile::createDBFFields(DBFHandle& dbfout){
	DBFAddField(dbfout,"STATEFP",FTString,2,0);
	DBFAddField(dbfout,"COUNTYFP",FTString,3,0);
	DBFAddField(dbfout,"LINEARID",FTString,22,3);
	DBFAddField(dbfout,"FULLNAME",FTString,100,0);
	DBFAddField(dbfout,"RTTYP",FTInteger,1,0);
	DBFAddField(dbfout,"MTFCC",FTInteger,5,0);
}

void TigerMapFile::saveDBFRoad(DBFHandle&,int,const vns::Road*){

}*/

}
