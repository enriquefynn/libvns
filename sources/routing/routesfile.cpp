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

#include "routesfile.h"
#include "routes.h"
#include "roadnetwork.h"
#include <fstream>

namespace vns {

Routes* RoutesFile::createRoutes(const char* fileName,RoadNetwork* network){
	Routes* routes = new Routes(network);
	if(load(routes,fileName)){
		return routes;
	}
	delete routes;
	return 0;
}

bool RoutesFile::load(Routes* routes, const char* fileName){
	std::fstream file;
	file.open(fileName, std::fstream::in | std::fstream::binary );
	if(file.is_open()){
		routes->clear();
    	DataStream in(&file);
    	in>>routes;
        file.close();
        return true;
    }
    return false;
}

bool RoutesFile::save(const Routes* routes, const char* fileName){
	std::fstream file;
	file.open(fileName, std::fstream::out | std::fstream::binary | std::fstream::trunc );
	if(file.is_open()){
    	DataStream out(&file);
    	out<<routes;
    	file.flush();
    	file.close();
    	return true;
    }
    return false;
}

}
