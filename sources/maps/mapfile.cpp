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

#include "mapfile.h"
#include "roadnetwork.h"
#include <fstream>

namespace vns {

vns::RoadNetwork* MapFile::createRoadNetwork(const char* fileName){
	vns::RoadNetwork* network = new vns::RoadNetwork();
	if(load(network,fileName)){
		return network;
	}
	delete network;
	return 0;
}

bool MapFile::load(vns::RoadNetwork* network, const char* fileName) {
	std::fstream file;
	file.open(fileName, std::fstream::in | std::fstream::binary );
	if(file.is_open()){
		network->clear();
		DataStream in(&file);
		in >> network;
		file.close();
		return true;
	}
	return false;
}

bool MapFile::save(const vns::RoadNetwork* network, const char* fileName) {
	std::fstream file;
	file.open(fileName, std::fstream::out | std::fstream::binary | std::fstream::trunc );
    if(file.is_open()){
    	DataStream out(&file);
    	out << network;
    	file.flush();
    	file.close();
    	return true;
    }
    return false;
}



}
