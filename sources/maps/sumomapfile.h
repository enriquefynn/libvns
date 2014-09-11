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

#ifndef VNS_SUMOMAPFILE_H_
#define VNS_SUMOMAPFILE_H_

#include "mapfile.h"

namespace vns {

class SUMOMapFile : public MapFile {
public:
	SUMOMapFile();
	virtual ~SUMOMapFile();
	const char* getFileType() const { return "SUMO File"; };
	const char* getFileExtension() const { return "SUMO File (*.xml)"; };
	bool load(vns::RoadNetwork* network, const char* fileName);
	bool save(const vns::RoadNetwork* network, const char* fileName);
};

}

#endif
