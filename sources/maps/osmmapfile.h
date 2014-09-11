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

#ifndef VNS_OSMMAPFILE_H_
#define VNS_OSMMAPFILE_H_

#include "mapfile.h"

namespace vns {

class OsmMapFile : public MapFile {
public:
	OsmMapFile();
	virtual ~OsmMapFile();
	const char* getFileType() const { return "OSM File"; };
	const char* getFileExtension() const { return "OSM File (*.osm)"; };
	bool load(vns::RoadNetwork* network, const char* fileName);
	bool save(const vns::RoadNetwork* network, const char* fileName);

};

}

#endif
