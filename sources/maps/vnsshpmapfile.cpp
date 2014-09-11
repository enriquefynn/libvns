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

#include "vnsshpmapfile.h"
#include "roadnetwork.h"
#include "mercartor.h"
#include <sstream>

#define MAXLANES 10

namespace vns {

std::vector<int> VNSShpMapFile::getFlagsList(const char* text){
	std::vector<int> v;
    std::istringstream f(text);
    std::string s;
    while( std::getline(f, s, ',') ){
    	int flags = 0;
    	if(s.at(0)!='0') flags |= vns::Lane::Buses;
    	if(s.at(1)!='0') flags |= vns::Lane::Cars;
    	if(s.at(2)!='0') flags |= vns::Lane::Bikes;
    	if(s.at(3)!='0') flags |= vns::Lane::Trucks;
    	if(s.at(4)!='0') flags |= vns::Lane::Rails;
    	if(s.at(5)!='0') flags |= vns::Lane::Pedestrians;
    	if(s.at(6)!='0') flags |= vns::Lane::StopSign;
    	if(s.at(7)!='0') flags |= vns::Lane::YieldSign;
    	if(s.at(8)!='0') flags |= vns::Lane::ChangeLaneToLeft;
    	if(s.at(9)!='0') flags |= vns::Lane::ChangeLaneToRight;
    	if(s.at(10)=='0') flags |= vns::Lane::NoTrafficLane;
        v.push_back( flags );
    }
    return v;
}

std::vector< std::vector<int> > VNSShpMapFile::getConnectionsList(const char* text,unsigned int nlanes){
	std::vector< std::vector<int> > v(nlanes);
    std::istringstream f(text);
    std::string s;
    int i = 0;
    while( std::getline(f, s, ',') ){
        std::istringstream ff(s.c_str());
        std::string ss;
        while( std::getline(ff, ss, '|') ){
        	int rid = atoi(ss.c_str());
            v.at(i).push_back( rid );
        }
        i++;
    }
    return v;
}

std::vector<bool> VNSShpMapFile::getBoolList(const char* text){
	std::vector<bool> v;
    std::istringstream f(text);
    std::string s;
    while( std::getline(f, s, ',') ){
    	bool b = bool(atoi(s.c_str()));
        v.push_back( b );
    }
    return v;
}

std::vector<double> VNSShpMapFile::getDoubleList(const char* text){
	std::vector<double> v;
    std::istringstream f(text);
    std::string s;
    while( std::getline(f, s, ',') ){
    	double b = atof(s.c_str());
        v.push_back( b );
    }
    return v;
}


VNSShpMapFile::VNSShpMapFile() : vns::ShpMapFile() {

}

VNSShpMapFile::~VNSShpMapFile() {

}

bool VNSShpMapFile::hasValidNumberOfFields(int numberOfFields){
	return numberOfFields==18;
}

void VNSShpMapFile::initializeLoad(vns::RoadNetwork*){
	roadConnections.clear();
}

void VNSShpMapFile::finalizeLoad(vns::RoadNetwork* network){

	network->generateRoadsConnectivity();

	for(int i=0;i<network->getNumberOfRoads();i++){
		vns::Road* road = network->getRoad(i);
		if(roadConnections.count(road->getID())>0){
			std::string text = roadConnections[road->getID()];
			//std::cout << text << std::endl;
			std::vector< std::vector<int32> > connections = getConnectionsList(text.c_str(),road->getNumberOfLanes());
			VNS_ASSERT( connections.size()==(unsigned int)road->getNumberOfLanes() ,"SHP Error","Invalid number of lanes in 'LANESCONNECTIONS'.");

			for(int l=0;l<road->getNumberOfLanes();l++){
				vns::Lane* lane = road->getLane(l);
				lane->clearConnections();
				for(unsigned int c=0;c<connections.at(l).size();c++){
					int32 rid = connections.at(l).at(c);
					if(rid>=0 && rid<network->getNumberOfRoads()){
						vns::Road* nroad = network->getRoad(rid);
						lane->addConnectionTo(nroad);
					}
				}
			}
		}
	}
	network->setAutomaticConnectivityGeneration(true);
}


bool VNSShpMapFile::loadRoad(DBFHandle& dbfHandle, int id, vns::RoadNetwork* network, vns::PolyLine* line) {
	int32 roadID = DBFReadIntegerAttribute(dbfHandle, id, 0);

	const char* sroadType = DBFReadStringAttribute(dbfHandle, id, 1);
	vns::Road::Type roadType = vns::Road::fromTypeName(sroadType);
	double offset = DBFReadDoubleAttribute(dbfHandle, id, 2);
	bool leftparking = bool(DBFReadIntegerAttribute(dbfHandle, id, 3));
	bool rightparking = bool(DBFReadIntegerAttribute(dbfHandle, id, 4));
	bool startcrosswalk = bool(DBFReadIntegerAttribute(dbfHandle, id, 5));
	bool endcrosswalk = bool(DBFReadIntegerAttribute(dbfHandle, id, 6));

	int forwardbacklog = DBFReadIntegerAttribute(dbfHandle, id, 7);
	int backwardbacklog = DBFReadIntegerAttribute(dbfHandle, id, 8);
	unsigned int nlanes = DBFReadIntegerAttribute(dbfHandle, id, 9);

	/*
	 bool forwardsource = bool(DBFReadIntegerAttribute(dbfHandle, id, 9));
	 bool forwardsink = bool(DBFReadIntegerAttribute(dbfHandle, id, 10));
	 bool backwardsource = bool(DBFReadIntegerAttribute(dbfHandle, id, 11));
	 bool backwardsink = bool(DBFReadIntegerAttribute(dbfHandle, id, 12));
	 */

	const char* slanes = DBFReadStringAttribute(dbfHandle, id, 10);
	std::vector<bool> waylanes = getBoolList(slanes);
	int flanes = 0;
	int blanes = 0;
	for (unsigned int l = 0; l < waylanes.size(); l++) {
		bool way = waylanes.at(l);
		if (way)
			flanes++;
		else
			blanes++;
	}

	const char* slanesmaxspeed = DBFReadStringAttribute(dbfHandle, id, 11);
	std::vector<double> maxspeed = getDoubleList(slanesmaxspeed);

	VNS_ASSERT( maxspeed.size()==nlanes ,"SHP Error","Invalid number of lanes in 'LANESMAXSPEED'.");

	const char* slanesminspeed = DBFReadStringAttribute(dbfHandle, id, 12);
	std::vector<double> minspeed = getDoubleList(slanesminspeed);

	VNS_ASSERT( minspeed.size()==nlanes ,"SHP Error","Invalid number of lanes in 'LANESMINSPEED'.");

	const char* slaneswidth = DBFReadStringAttribute(dbfHandle, id, 13);
	std::vector<double> width = getDoubleList(slaneswidth);

	VNS_ASSERT( width.size()==nlanes ,"SHP Error","Invalid number of lanes in 'LANESWIDTH'.");

	const char* slanesstartoffset = DBFReadStringAttribute(dbfHandle, id, 14);
	std::vector<double> startoffset = getDoubleList(slanesstartoffset);

	VNS_ASSERT( startoffset.size()==nlanes ,"SHP Error","Invalid number of lanes in 'LANESSTARTOFFSET'.");

	const char* slanesendoffset = DBFReadStringAttribute(dbfHandle, id, 15);
	std::vector<double> endoffset = getDoubleList(slanesendoffset);

	VNS_ASSERT( endoffset.size()==nlanes ,"SHP Error","Invalid number of lanes in 'LANESSTARTOFFSET'.");

	const char* slanesflags = DBFReadStringAttribute(dbfHandle, id, 16);
	std::vector<int32> flags = getFlagsList(slanesflags);

	VNS_ASSERT( flags.size()==nlanes ,"SHP Error","Invalid number of lanes in 'LANESFLAGS'.");

	const char* slanesconnections = DBFReadStringAttribute(dbfHandle, id, 17);
	roadConnections[roadID] = std::string(slanesconnections);

	/*
	 bool oneway = false;
	 if( way == "P" ) {
	 return true;
	 }
	 if (way == "F"){
	 oneway = true;
	 }
	 if (way == "B") {
	 oneway = true;
	 line->reverse();
	 }

	 int flanes = 1;
	 int blanes = 0;
	 if( oneway==false ){
	 blanes = 1;
	 }*/

	vns::Road* road = network->createRoad(line, flanes, blanes);
	road->setType(roadType);
	road->setRoadOffset(offset);
	road->setLeftParking(leftparking);
	road->setRightParking(rightparking);
	road->setStartCrossWalk(startcrosswalk);
	road->setEndCrossWalk(endcrosswalk);
	road->setMaximumCarsInQueue(vns::Forward, forwardbacklog);
	road->setMaximumCarsInQueue(vns::Backward, backwardbacklog);
	for (int32 l = 0; l < road->getNumberOfLanes(); l++) {
		vns::Lane* lane = road->getLane(l);
		lane->setMaximumSpeed(maxspeed.at(l) / 3.6);
		lane->setMinimumSpeed(minspeed.at(l) / 3.6);
		lane->setWidth(width.at(l));
		lane->setLaneStartOffset(startoffset.at(l));
		lane->setLaneEndOffset(endoffset.at(l));
		lane->setFlags(flags.at(l));
	}
	return true;
}


void VNSShpMapFile::createDBFFields(DBFHandle& dbfout){
	DBFAddField(dbfout,"ID",FTInteger,10,0);
	DBFAddField(dbfout,"TYPE",FTString,15,0);
	DBFAddField(dbfout,"OFFSET",FTDouble,10,3);
	DBFAddField(dbfout,"LEFTPARKING",FTInteger,1,0);
	DBFAddField(dbfout,"RIGHTPARKING",FTInteger,1,0);
	DBFAddField(dbfout,"STARTCROSSWALK",FTInteger,1,0);
	DBFAddField(dbfout,"ENDCROSSWALK",FTInteger,1,0);
	DBFAddField(dbfout,"FORWARDBACKLOG",FTInteger,10,0);
	DBFAddField(dbfout,"BACKWARDBACKLOG",FTInteger,10,0);
	DBFAddField(dbfout,"LANES",FTInteger,5,0);
	DBFAddField(dbfout,"LANESFORWARD",FTString,MAXLANES*2,0);
	DBFAddField(dbfout,"LANESMAXSPEED",FTString,MAXLANES*15,0);
	DBFAddField(dbfout,"LANESMINSPEED",FTString,MAXLANES*15,0);
	DBFAddField(dbfout,"LANESWIDTH",FTString,MAXLANES*15,0);
	DBFAddField(dbfout,"LANESSTARTOFFSET",FTString,MAXLANES*15,0);
	DBFAddField(dbfout,"LANESENDOFFSET",FTString,MAXLANES*15,0);
	DBFAddField(dbfout,"LANESFLAGS",FTString,MAXLANES*15,0);
	DBFAddField(dbfout,"LANESCONNECTIONS",FTString,MAXLANES*100,0);
}

void VNSShpMapFile::saveDBFRoad(DBFHandle& dbfout,int id,const vns::Road* road){
	DBFWriteIntegerAttribute(dbfout,id,0,road->getID());
	DBFWriteStringAttribute(dbfout,id,1,vns::Road::fromType(road->getType()));
	DBFWriteDoubleAttribute(dbfout,id,2,road->getRoadOffset());
	DBFWriteIntegerAttribute(dbfout,id,3,(int)road->hasLeftParking());
	DBFWriteIntegerAttribute(dbfout,id,4,(int)road->hasRightParking());
	DBFWriteIntegerAttribute(dbfout,id,5,(int)road->hasStartCrossWalk());
	DBFWriteIntegerAttribute(dbfout,id,6,(int)road->hasEndCrossWalk());
	DBFWriteIntegerAttribute(dbfout,id,7,road->getNumberOfCarsInQueue(vns::Forward));
	DBFWriteIntegerAttribute(dbfout,id,8,road->getNumberOfCarsInQueue(vns::Backward));
	DBFWriteIntegerAttribute(dbfout,id,9,road->getNumberOfLanes());
	std::stringstream lanes;
	std::stringstream maxspeed;
	std::stringstream minspeed;
	std::stringstream width;
	std::stringstream start;
	std::stringstream end;
	std::stringstream flags;
	std::stringstream conns;

	int nl = road->getNumberOfLanes();
	if(nl>MAXLANES){
		fprintf(stderr,"The maximum number of allowed lanes is %d and not %d\n",MAXLANES,nl);
		exit(-1);
	}
	for(int i=0;i<nl;i++){
		const vns::Lane* lane = road->getLane(i);
		lanes << (lane->getWay()==vns::Forward?'1':'0');
		maxspeed << lane->getMaximumSpeed()*3.6;
		minspeed << lane->getMinimumSpeed()*3.6;
		width << vns::round(lane->getWidth(),8);
		start << vns::round(lane->getLaneStartOffset(),8);
		end << vns::round(lane->getLaneEndOffset(),8);
		flags << lane->hasFlag(Lane::Buses) << lane->hasFlag(Lane::Cars) << lane->hasFlag(Lane::Bikes)
				<< lane->hasFlag(Lane::Trucks) << lane->hasFlag(Lane::Rails) << lane->hasFlag(Lane::Pedestrians)
				<< lane->hasFlag(Lane::StopSign) << lane->hasFlag(Lane::YieldSign) << lane->hasFlag(Lane::ChangeLaneToLeft)
				<< lane->hasFlag(Lane::ChangeLaneToRight) << (lane->hasFlag(Lane::NoTrafficLane)?0:1);

		for(int c=0;c<lane->getNumberOfConnections();c++){
			if (c > 0){
				conns << '|';
			}
			const Road* nroad = lane->getConnection(c);
			conns << (int)nroad->getID();
		}
		if(i<nl-1){
			lanes << ',';
			maxspeed << ',';
			minspeed << ',';
			width << ',';
			start << ',';
			end << ',';
			flags << ',';
			conns << ',';
		}
	}
	DBFWriteStringAttribute(dbfout,id,10,lanes.str().c_str());
	DBFWriteStringAttribute(dbfout,id,11,maxspeed.str().c_str());
	DBFWriteStringAttribute(dbfout,id,12,minspeed.str().c_str());
	DBFWriteStringAttribute(dbfout,id,13,width.str().c_str());
	DBFWriteStringAttribute(dbfout,id,14,start.str().c_str());
	DBFWriteStringAttribute(dbfout,id,15,end.str().c_str());
	DBFWriteStringAttribute(dbfout,id,16,flags.str().c_str());
	DBFWriteStringAttribute(dbfout,id,17,conns.str().c_str());
}

}
