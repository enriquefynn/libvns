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

#ifndef VNS_ROUTESFILE_H_
#define VNS_ROUTESFILE_H_

namespace vns {

class Routes;
class RoadNetwork;

class RoutesFile {
public:
	RoutesFile(){};
	virtual inline ~RoutesFile(){};
	virtual const char* getFileType(){ return "Binary Routes (RTS)"; };
	virtual const char* getFileExtension(){ return "Binary Routes (*.rts)"; };
	virtual bool load(Routes* routes, const char* fileName);
	virtual bool save(const Routes* routes, const char* fileName);
	Routes* createRoutes(const char* fileName,RoadNetwork* network);
};

}

#endif
